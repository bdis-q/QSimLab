#include "quanpath.h"

/**
 * @brief QuanPath
 *
 * @param qc a quantum circuit
 * @param numWorkers the number of distributed working processes
 * @param myRank the MPI rank of the current process
 */
void QuanPath(QCircuit& qc, int numWorkers, int myRank) {
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
    for (int lev = 0; lev < qc.numDepths; ++ lev) {
        localSVSimForOneLevel(localSv, qc, lev, myRank);
    }

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
    printSimResults(localSv, "QuanPath.txt", numWorkers, myRank);
}

/**
 * @brief [TODO] Conduct OMSim for high-order qubits using a thread
 *
 * @param arg HighOpmatThreadArgs including qc and numHighQubits
 */
void* highOMSim(void* arg) {
    // Get arguments
    HighOpmatThreadArgs* args = (HighOpmatThreadArgs*)arg;
    QCircuit& qc = args->qc;
    int numHighQubits = args->numHighQubits;
    int numLowQubits  = qc.numQubits - numHighQubits;

    Matrix<DTYPE> opmat, levelmat;
    opmat.identity(1 << numHighQubits);
    levelmat.identity(2);

    for (int j = 0; j < qc.numDepths; ++ j) {
        int qid = qc.numQubits - 1;

        // get the highest gate matrix
        while (qc.gates[j][qid].isMARK() && qc.gates[j][qid].targetQubits[0] >= numLowQubits) {
            // Skip the pseudo placeholder MARK gates placed at control positions
            // when the target gate is applied to high-order qubits
            // If the target gate is applied to low-order qubits, MARK should be regarded as IDE
            -- qid;
        }
        // [TODO] Calculate the operation matrix for gates applied to high-order qubits
        // [HINT] We have modified getCompleteMatrix to deal with MARK
        //        In this assignment, MARK is associated with an identity matrix
        cout << "[TODO] Calculate the operation matrix for gates applied to high-order qubits" << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);









        // ///////////////////////////////////////////////////////////////////////////
    }

    Matrix<DTYPE>* ptrOpmat = new Matrix<DTYPE>(opmat); // prevent being destructed
    pthread_exit((void*)ptrOpmat);
}

/**
 * @brief [TODO] Conduct the final merge operation in QuanPath
 * 
 * @param localSv the local state vector
 * @param ptrOpmat the pointer to the high-order operation matrix
 * @param numWorkers #distributed processes
 * @param myRank the MPI rank of the current process
 */
void merge(Matrix<DTYPE>& localSv, Matrix<DTYPE>* ptrOpmat, int numWorkers, int myRank) {
    ll sendcnt = localSv.row / numWorkers; // #communication units

    // 2D array -> consecutive 1D array
    DTYPE* sendbuf = new DTYPE[localSv.row];
    DTYPE* recvbuf = new DTYPE[localSv.row];
    assert(sendbuf != nullptr);
    assert(recvbuf != nullptr);
    for (ll k = 0; k < localSv.row; ++ k) {
        memcpy(sendbuf + k, localSv.data[k], sizeof(DTYPE));
    }

    // [TODO] Conduct the final merge operation
    cout << "[TODO] Conduct the final merge operation. " << endl;
    MPI_Abort(MPI_COMM_WORLD, 1);
    // all-to-all communication 1


    // local merge computation








    // all-to-all communication 2


    // update localSv



    // ///////////////////////////////////////////////////////////////////////////

    delete [] sendbuf;
    delete [] recvbuf;

    return;
}