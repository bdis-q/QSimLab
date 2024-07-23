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
    qc.swap(0, 3);
    qc.swap(numQubits-1, numQubits-2);

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

    qHiPSTER(qc, numWorkers, myRank);
    QuanPath(qc, numWorkers, myRank);

    // Check simulation results
    if (myRank == 0) {
        Matrix<DTYPE> sv((1 << qc.numQubits), 1);
        for (int i = 0; i < numWorkers; ++ i) {
            sv.data[i * ((1 << qc.numQubits) / numWorkers) + i][0] = 1.0 / sqrt(numWorkers);
        }
        SVSim(sv, qc);
        printSimResults(sv, "SVSim.txt", 1, 0);
    }

    MPI_Finalize();
    return 0;
}