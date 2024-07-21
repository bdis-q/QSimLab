#include "quanpath.h"

// Generate a separable quantum circuit
QCircuit separableqc() {
    int numQubits = 11;
    QCircuit qc(numQubits, "separable");
    qc.cx(numQubits-1, 0); // one CX gate
    for (int i = 1; i < numQubits; i += 2) { // one level of CY gates
        qc.cy(i, i - 1);
    }
    for (int i = 1; i < numQubits; i += 2) { // one level of CZ gates
        qc.cz(i, i - 1);
    }
    for (int i = 0; i < numQubits; ++ i) { // one level of RX gates
        qc.rx((double)i / numQubits, i);
    }
    for (int i = 0; i < numQubits; ++ i) { // one level of RY gates
        qc.ry((double)i / numQubits, i);
    }
    for (int i = 0; i < numQubits; ++ i) { // one level of RZ gates
        qc.rz((double)i / numQubits, i);
    }
    qc.swap(0, 2);
    qc.swap(1, 3);

    return qc;
}

int main() {
    // Initialize a quantum circuit
    QCircuit qc = separableqc();

    // Initialize MPI
    int myRank; // the rank of the current process
    int numWorkers; // #distributed processes
    MPI_Init(nullptr, nullptr);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numWorkers);
    assert((numWorkers & (numWorkers - 1)) == 0); // numWorkers should be a power of 2
    if (myRank == 0) {
        qc.print();
    }

    // Initialize distributed settings for QuanPath
    int numHighQubits = log2(numWorkers); // #high-order qubits
    int numLowQubits = qc.numQubits - numHighQubits; // #low-order qubits
    ll  lenlocalSv = (1 << numLowQubits); // the length of the local state vector

    // Initialize the local state vector for each process
    // [NOTE] The initial state vector is not |00..0>
    Matrix<DTYPE> localSv(lenlocalSv, 1);
    localSv.data[myRank][0] = 1.0 / sqrt(numWorkers);

    // //////////////////////////////////////////////////////////////////
    // Step 1. Calculate the high-order operation matrix using a thread
    // //////////////////////////////////////////////////////////////////
    pthread_t omsim;
    HighOpmatThreadArgs args = {qc, numHighQubits};
    int ret = pthread_create(&omsim, nullptr, highOMSim, (void*)&args);
    assert(ret == 0);

    // /////////////////////////////////////////////////////////////
    // Step 2. Local SVSim for gates on low-order qubits and merge
    // /////////////////////////////////////////////////////////////
    lowSVSim(localSv, qc, myRank);

    // /////////////////////////////////////////////////
    // Step 3. Final merge that requires communication
    // /////////////////////////////////////////////////
    // Wait for the thread to get the high-order operation matrix
    void* result;
    ret = pthread_join(omsim, &result);
    assert(ret == 0);
    Matrix<DTYPE>* ptrOpmat = (Matrix<DTYPE>*)result;

    // Merge
    merge(localSv, ptrOpmat, numWorkers, myRank);
    delete ptrOpmat;

    // //////////////////////////////////////////
    // Step 4. Collect the simulation results
    // //////////////////////////////////////////
    stringstream filenameStream;
    ofstream file;
    ifstream infile;
    filenameStream.str(""); // clear the stream
    filenameStream << myRank << ".txt";
    file.open(filenameStream.str());
    for (ll i = 0; i < lenlocalSv; ++ i) {
        file << fixed << setprecision(4) << localSv.data[i][0] << endl; // output the localSv
    }
    file.close();

    MPI_Barrier(MPI_COMM_WORLD); // wait for all processes
    if (myRank == 0) { // collect the simulation results
        file.open("output.txt");
        for (int rank = 0; rank < numWorkers; ++ rank) {
            filenameStream.str("");
            filenameStream << rank << ".txt";
            infile.open(filenameStream.str());
            string line;
            while (getline(infile, line)) {
                file << line << endl;
            }
            infile.close();
            assert(remove(filenameStream.str().c_str()) == 0);
        }
        file.close();
        cout << "[INFO] Done. " << endl;
    }

    MPI_Finalize();
    return 0;
}