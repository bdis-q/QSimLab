#include "quanpath.h"

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
        // cout << "[TODO] Calculate the operation matrix for gates applied to high-order qubits" << endl;
        // exit(1);
        levelmat = move(getCompleteMatrix(qc.gates[j][qid]));
        for (int i = qid - 1; i >= numLowQubits; -- i) {
            if (qc.gates[j][i].isMARK() && qc.gates[j][i].targetQubits[0] >= numLowQubits) {
                continue;
            }
            Matrix<DTYPE> tmpmat = move(getCompleteMatrix(qc.gates[j][i]));
            levelmat = move(levelmat.tensorProduct(tmpmat));
        }
        opmat = move(levelmat * opmat);
        // ///////////////////////////////////////////////////////////////////////////
    }

    Matrix<DTYPE>* ptrOpmat = new Matrix<DTYPE>(opmat); // prevent being destructed
    pthread_exit((void*)ptrOpmat);
}

/**
 * @brief [TODO] Conduct SVSim for low-order qubits
 *
 * @param localSv the local state vector
 * @param qc the original quantum circuit
 * @param myRank the MPI rank
 */
void lowSVSim(Matrix<DTYPE>& localSv, QCircuit& qc, int myRank) {
    for (int j = 0; j < qc.numDepths; ++ j) {
        for (int qid = 0; qid < log2(localSv.row); ++ qid) {
            QGate& gate = qc.gates[j][qid];
            if (gate.isIDE() || gate.isMARK()) {
                continue;
            }
            svsimForGate(localSv, gate, myRank);
        }
    }
}

/**
 * @brief [TODO] Check if the index of an amplitude is a legal control pattern of the gate
 * 
 * @param ampidx the amplitude index
 * @param gate   the processing gate
 * @return true  ampidx is a legal control pattern
 * @return false ampidx is an illegal control pattern
 */
bool isQuanPathLegalControlPattern(ll ampidx, QGate& gate, int numLowQubits, int myRank) {
    int ctrl;
    ll ctrlmask;
    for (int i = 0; i < gate.numControls(); ++ i) {
        // [HINT] If the control qubits is low-order, mask with ampidx
        //        Otherwise, mask with myRank
        // cout << "[TODO] Check the control qubits of the gate." << endl;
        // exit(1);
        ctrl = gate.controlQubits[i];
        if (ctrl < numLowQubits) {
            ctrlmask = (1 << ctrl);
            if ((ampidx & ctrlmask) == 0) {
                return false;
            }
        } else {
            ctrlmask = (1 << (ctrl - numLowQubits));
            if ((myRank & ctrlmask) == 0) {
                return false;
            }
        }
        // ///////////////////////////////////////////////////////////////////////////
    }
    return true;
}

/**
 * @brief [TODO] Conduct the final merge operation in QuanPath
 * 
 * @param localSv the local state vector
 * @param ptrOpmat the pointer to the high-order operation matrix
 * @param numWorkers #distributed processes
 * @param myRank the MPI rank
 */
void merge(Matrix<DTYPE>& localSv, Matrix<DTYPE>* ptrOpmat, int numWorkers, int myRank) {
    ll group = localSv.row / numWorkers; // #communication unit
    
    // 2D array -> consecutive 1D array
    DTYPE* sendbuf = new DTYPE[localSv.row];
    DTYPE* recvbuf = new DTYPE[localSv.row];
    assert(sendbuf != nullptr);
    assert(recvbuf != nullptr);
    for (ll k = 0; k < localSv.row; ++ k) {
        memcpy(sendbuf + k, localSv.data[k], sizeof(DTYPE));
    }

    // [TODO] Conduct the final merge operation
    // cout << "[TODO] Conduct the final merge operation. " << endl;
    // exit(1);
    // all-to-all communication 1
    int ret = MPI_Alltoall(sendbuf, group, MPI_DOUBLE_COMPLEX, recvbuf, group, MPI_DOUBLE_COMPLEX, MPI_COMM_WORLD);
    assert(ret == MPI_SUCCESS);

    // local merge computation
    for (ll k = 0; k < localSv.row; ++ k) {
        sendbuf[k] = 0;
    }
    ll iStart, jStart;
    for (int i = 0; i < numWorkers; ++ i) {
        iStart = i * group; // the start point of each group
        for (int j = 0; j < numWorkers; ++ j) {
            jStart = j * group; // the offset within each group
            for (ll k = 0; k < group; ++ k) {
                sendbuf[iStart + k] += ptrOpmat->data[i][j] * recvbuf[jStart + k];
            }
        }
    }

    // all-to-all communication 2
    ret = MPI_Alltoall(sendbuf, group, MPI_DOUBLE_COMPLEX, recvbuf, group, MPI_DOUBLE_COMPLEX, MPI_COMM_WORLD);
    assert(ret == MPI_SUCCESS);

    // update localSv
    for (ll k = 0; k < localSv.row; ++ k) {
        memcpy(localSv.data[k], recvbuf + k, sizeof(DTYPE));
    }
    // ///////////////////////////////////////////////////////////////////////////

    delete [] sendbuf;
    delete [] recvbuf;

    return;
}