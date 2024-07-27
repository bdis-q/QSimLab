#include "qhipster.h"

/**
 * @brief qHiPSTER, a basic distributed SVSim implementation
 *
 * @param qc a quantum circuit
 * @param numWorkers the number of distributed working processes
 * @param myRank the MPI rank of the current process
 */
void qHiPSTER(QCircuit& qc, int numWorkers, int myRank) {
    // Initialize distributed settings for qHiPSTER
    int numHighQubits = log2(numWorkers); // #high-order qubits
    int numLowQubits = qc.numQubits - numHighQubits; // #low-order qubits
    ll  lenlocalSv = (1 << numLowQubits); // the length of the local state vector

    // Initialize the local state vector for each process
    // [NOTE] The initial state vector is not |00..0>
    Matrix<DTYPE> localSv(lenlocalSv, 1);
    localSv.data[myRank][0] = 1.0 / sqrt(numWorkers);

    for (int lev = 0; lev < qc.numDepths; ++ lev) {
        // one level of local computation for low-order qubits
        localSVSimForOneLevel(localSv, qc, lev, myRank);
        // computation for high-order qubits that requires communication
        distributedSVSimForOneLevel(localSv, qc, lev, myRank);
    }

    // Collect simulation results
    printSimResults(localSv, "qHiPSTER.txt", numWorkers, myRank);
}

/**
 * @brief Conduct SVSim for one level of gates on low-order qubits
 *
 * @param localSv the local state vector
 * @param qc the original quantum circuit
 * @param level the processing level
 * @param myRank the MPI rank of the current process
 */
void localSVSimForOneLevel(Matrix<DTYPE>& localSv, QCircuit& qc, int level, int myRank) {
    int numLowQubits = log2(localSv.row);
    for (int qid = 0; qid < numLowQubits; ++ qid) {
        QGate& gate = qc.gates[level][qid];
        if (gate.isIDE() || gate.isMARK()) {
            continue;
        }
        svsimForGate(localSv, gate, myRank);
    }
}

/**
 * @brief [TODO] Check if ampidx or myRank is a legal control pattern of the gate
 * 
 * @param ampidx the amplitude index
 * @param gate the processing gate
 * @param numLowQubits #low-order qubits in QuanPath
 * @param myRank the MPI rank of the current process
 * @return true  ampidx or myRank is a legal control pattern
 * @return false ampidx or myRank is an illegal control pattern
 */
bool isDistributedLegalControlPattern(ll ampidx, QGate& gate, int numLowQubits, int myRank) {
    int ctrl;
    ll ctrlmask;
    for (int i = 0; i < gate.numControls(); ++ i) {
        // [HINT] If the control qubits is low-order, mask with ampidx
        //        Otherwise, mask with myRank
        cout << "[TODO] Check the control qubits of the gate." << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);












        // ///////////////////////////////////////////////////////////////////////////
    }
    return true;
}

/**
 * @brief Conduct SVSim for one level of gates on high-order qubits
 *
 * @param localSv the local state vector
 * @param qc the original quantum circuit
 * @param level the processing level
 * @param myRank the MPI rank of the current process
 */
void distributedSVSimForOneLevel(Matrix<DTYPE>& localSv, QCircuit& qc, int level, int myRank) {
    int numLowQubits = log2(localSv.row);
    int numHighQubits = qc.numQubits - numLowQubits;
    vector<int> peerRanks;
    vector<DTYPE> sendbuf;
    vector<DTYPE> recvbuf(localSv.row);

    for (ll idx = 0; idx < localSv.row; ++ idx) {
        sendbuf.push_back(localSv.data[idx][0]); // initialize sendbuf
    }

    for (int qid = numLowQubits; qid < qc.numQubits; ++ qid) {
        QGate& gate = qc.gates[level][qid];
        if (gate.isIDE() || gate.isMARK()) {
            continue;
        }
        // ////////////////////////////////////////
        // Step 1. Create sub communication group
        // ////////////////////////////////////////
        MPI_Comm peerComm = getPeerComm(numLowQubits, numHighQubits, gate, myRank);
        if (peerComm == MPI_COMM_NULL) {
            continue; // skip this group
        }

        // ///////////////////////////////////////////////
        // Step 2. Send/receive amplitudes to/from peers
        // ///////////////////////////////////////////////
        int numPeers; // the number of processes in peerComm
        MPI_Comm_size(peerComm, &numPeers);
        ll sendcnt = localSv.row / numPeers;

        // Step 2.1. compact involved amplitudes to construct sendbuf
        auto ampOrder = compactLocalAmp(sendbuf, gate);

        // Step 2.2. all-to-all communication to obtain amplitudes from the other processes
        int ret = MPI_Alltoall(sendbuf.data(), sendcnt, MPI_DTYPE, 
                               recvbuf.data(), sendcnt, MPI_DTYPE, peerComm);
        assert(ret == MPI_SUCCESS);

        // ///////////////////////////////
        // Step 3. Conduct SVSim locally
        // ///////////////////////////////
        svsimOnCompactedAmp(recvbuf, numPeers, gate, ampOrder.first);

        // ///////////////////////////////////////////////
        // Step 4. Send/receive amplitudes to/from peers
        // ///////////////////////////////////////////////
        // Step 4.1. all-to-all communication to write back amplitudes
        ret = MPI_Alltoall(recvbuf.data(), sendcnt, MPI_DTYPE, 
                           sendbuf.data(), sendcnt, MPI_DTYPE, peerComm);
        assert(ret == MPI_SUCCESS);
        MPI_Comm_free(&peerComm);

        // Step 4.2. Recover the local amplitudes to their original places
        recoverLocalAmp(sendbuf, ampOrder);
    }
    // update localSv
    for (ll idx = 0; idx < localSv.row; ++ idx) {
        localSv.data[idx][0] = sendbuf[idx];
    }
}

/**
 * @brief [TODO] Get a communication group including the current working process
 *
 * @param numLowQubits #low-order qubits
 * @param numHighQubits #high-order qubits
 * @param gate the processing gate
 * @param myRank the MPI rank of the current process
 * @return MPI_COMM_NULL if myRank is not a valid control pattern
 * @return the MPI communicator of processes that require communication
 */
MPI_Comm getPeerComm(int numLowQubits, int numHighQubits, QGate& gate, int myRank) {
    vector<int> peerRanks; // the communication targets
    vector<int> highTargs; // high-order target qubits
    int peerBase = 0; // the minimum rank of the processes in the communication group
    int numPeers = 1; // the number of processes in the communication group

    /* [TODO] Check if myRank is a legal control pattern of gate and
              find a group of processes whose ranks differ only in the (target_qid - numLowQubits)-th bits

       [HINT] Step 1. Check the high-order qubit 'qid' one by one
              Let 'idx' be 'qid-numLowQubits'
                Case 1. If qid is a control qubit but the idx-th bit of myRank is 0 (not a legal control pattern)
                        this group of workers do not need to perform SVSim
                        Set numPeers to 0 and break
                Case 2. If qid is a target qubit, double numPeers and push idx into highTargs
                        <e.g.> 1 high-order target: 2 processes should exchange data
                               2 high-order targets: 4 processes should exchange data
                               1 high-order and 1 low-order targets: still 2 processes
                Case 3. If qid is not a target qubit, the idx-th bit of myRank is the idx-th bit of peerBase

              Step 2. Let 'peerIdx' range from [0, numPeers)
                      Calculate the 'peerStride' from 'peerBase' using highTargs
                      Push 'peerBase + peerStride' into peerRanks

       <e.g.> Given an 8-qubit circuit and 16 workers, gate has q7 controlling q5 and q3
                  high  │   low
        qid:    7 6 5 4 │ 3 2 1 0
        idx:    3 2 1 0
        myRank: 1_0_1_0 (10)
                │ │ │ └ Case 3. peerBase <- 0_0_0_0 (0)
                │ │ └── Case 2. numPeers <- 2, highTargs <- {1}
                │ └──── Case 3. peerBase <- 0_0_0_0 (0)
                └────── Case 3. peerBase <- 1_0_0_0 (8)

        peerIdx = 0: peerStride = 0, peerRank = 1_0_0_0 (8)
        peerIdx = 1: peerStride = (1 << highTargs[0]) = 1_0 (2), peerRank = 1_0_1_0 (10)
        peerRanks = {1_0_0_0 (8), 1_0_1_0 (10)}
    */
    cout << "[TODO] Build peerRanks. " << endl;
    MPI_Abort(MPI_COMM_WORLD, 1);





















    // ////////////////////////////////////////////////////////////////////////
    MPI_Group worldGroup, peerGroup;
    MPI_Comm  peerComm;
    /* [TODO] Create a new MPI_Comm communicator 'peerComm' based on peerRanks
       [HINT] Useful functions: MPI_Comm_group, MPI_Group_incl, MPI_Comm_create
       [NOTE] If you use MPI_Comm_create, all processes should call it
              Otherwise, this function will be blocked
              When peerRanks is empty, it can safely set peerComm to MPI_COMM_NULL
    */
    cout << "[TODO] Create a new MPI_Comm communicator 'peerComm' based on peerRanks. " << endl;
    MPI_Abort(MPI_COMM_WORLD, 1);




    // ////////////////////////////////////////////////////////////////////////
    MPI_Group_free(&peerGroup);
    MPI_Group_free(&worldGroup);
    return peerComm;
}

/**
 * @brief [TODO] Reorder local amplitudes to construct a sendbuf for MPI_Alltoall
 *
 * @param sendbuf the input and the output buffer of local amplitudes
 * @param gate the processing gate
 * @return pair.first   the index of the first amplitude within each group originally
 * @return pair.second  the strides for locally involved amplitudes
 */
pair<vector<ll>, vector<ll>> compactLocalAmp(vector<DTYPE>& sendbuf, QGate& gate) {
    /* [TODO] Compact involved amplitudes to construct sendbuf
       [HINT] This is necessary when simulating multi-target gates
              whose targets include both high-order and low-order qubits.
       <e.g.> Given a 4-qubit circuit and 4 workers, consider a 2-target gate on q1 and q2. 
              Since sendbuf will be partitioned evenly and sent to different workers,
              local amplitudes should be placed consecutively. 
              Take worker 00 and 01 as examples. 
                +- a_0000                       +- a_0000             A0             +- a_0000             A0
                |          +- a_0001            +- a_0010  _ _ _ _ _ _ _             +- a_0010  _ _ _ _ _ _ _
    Worker 00   +- a_0010  |                    |          +- a_0001                 |
                |          +- a_0011            |          +- a_0011  A1             +- a_0100             B0
    ____________|__________|________            |__________|____________             +- a_0110_______________
                |          |         -compact-> |          |             -alltoall->            +- a_0001
                +- a_0100  |                    +- a_0100  |          B0                        +- a_0011  A1
    Worker 01   |          +- a_0101            +- a_0110  | _ _ _ _ _ _                        | _ _ _ _ _ _
                +- a_0110  |                               +- a_0101                            +- a_0101
                           +- a_0111                       +- a_0111  B1                        +- a_0111  B1
    */
    //
    // Step 1. Record low-order target qubits
    //
    int numLowQubits = log2(sendbuf.size());
    vector<int> lowTargs;   // low-order target qubits
    for (const auto& targ : gate.targetQubits) {
        if (targ < numLowQubits) {
            lowTargs.push_back(targ);
        } else {
            break; // gate.targetQubits is sorted
        }
    }
    // 
    // [TODO] Step 2. Calculate the strides for locally involved amplitudes
    // <e.g.> For worker 00, the stride between a_0010 and a_0000 is 2
    // 
    vector<ll> strides;
    cout << "[TODO] Calculate the strides for locally involved amplitudes. " << endl;
    MPI_Abort(MPI_COMM_WORLD, 1);









    // ///////////////////////////////////////////////////////////

    // 
    // [TODO] Step 3. Compact groups of amplitudes
    // 
    vector<ll> heads; // the index of the first amplitude within each group on each worker
                      // <e.g.> a_0000 and a_0001 for worker 00
                      //        a_0100 and a_0101 for worker 01
    vector<DTYPE> compbuf; // save the amplitudes after reordering
    bool isAccessed[sendbuf.size()];
    memset(isAccessed, 0, sendbuf.size()*sizeof(bool));
    cout << "[TODO] Construct heads and compbuf. " << endl;
    MPI_Abort(MPI_COMM_WORLD, 1);








    // ///////////////////////////////////////////////////////////

    // 
    // Step 4. Update sendbuf
    // 
    for (size_t i = 0; i < compbuf.size(); ++ i) {
        sendbuf[i] = compbuf[i];
    }

    // 
    // Step 5. Return information about amplitude positions
    // 
    return make_pair(heads, strides);
}

/**
 * @brief Conduct SVSim on the recvbuf after MPI_Alltoall with compacted amplitudes
 *
 * @param compbuf the local state vector after MPI_Alltoall
 * @param numPeers the number of peer processes involved in the communication
 * @param gate the processing gate
 * @param heads the index of the first amplitude within each group originally
 */
void svsimOnCompactedAmp(vector<DTYPE>& compbuf, int numPeers, QGate& gate, vector<ll>& heads) {
    ll numAmps = (1 << gate.numTargets());
    Matrix<DTYPE> amps_vec(numAmps, 1);
    ll localcnt = numAmps / numPeers;
    ll sendcnt = compbuf.size() / numPeers;

    for (size_t iter = 0; iter < compbuf.size() / numAmps; ++ iter) {
        // check low-order control qubits
        if (! isDistributedLegalControlPattern(heads[iter], gate, log2(compbuf.size()), ~0)) {
            continue;
        }

        // get a group of amplitudes
        ll idx = 0;
        for (int peerIdx = 0; peerIdx < numPeers; ++ peerIdx) {
            for (ll offset = 0; offset < localcnt; ++ offset) {
                amps_vec.data[idx ++][0] = compbuf[peerIdx * sendcnt + iter * localcnt + offset];
            }
        }

        // apply the gate matrix and update the state vector in place
        amps_vec = (*gate.gmat) * amps_vec;

        // update a group of amplitudes
        idx = 0;
        for (int peerIdx = 0; peerIdx < numPeers; ++ peerIdx) {
            for (ll offset = 0; offset < localcnt; ++ offset) {
                compbuf[peerIdx * sendcnt + iter * localcnt + offset] = amps_vec.data[idx ++][0];
            }
        }
    }
}

/**
 * @brief Recover the local amplitudes to their original places
 *
 * @param compbuf the compacted local state vector
 * @param ampOrder information about amplitude positions
 */
void recoverLocalAmp(vector<DTYPE>& compbuf, pair<vector<ll>, vector<ll>>& ampOrder) {
    vector<ll> heads = ampOrder.first;
    vector<ll> strides = ampOrder.second;
    vector<DTYPE> original(compbuf.size());

    // recover the positions of local amplitudes based on ampOrder
    size_t idx = 0;
    for (const auto& head : heads) {
        for (const auto& stride : strides) {
            original[head + stride] = compbuf[idx ++];
        }
    }

    // update compbuf
    for (size_t i = 0; i < original.size(); ++ i) {
        compbuf[i] = original[i];
    }
}

/**
 * @brief Print the simulation results of all processes
 *
 * @param localSv the local state vector for each process
 * @param fname the name of the output file
 * @param numWorkers the total number of processes
 * @param myRank the MPI rank of the current process
 */
void printSimResults(Matrix<DTYPE>& localSv, string fname, int numWorkers, int myRank) {
    stringstream filenameStream;
    ofstream file;
    ifstream infile;
    filenameStream.str(""); // clear the stream
    filenameStream << myRank << ".txt";
    file.open(filenameStream.str());
    for (ll i = 0; i < localSv.row; ++ i) {
        file << fixed << setprecision(4) << localSv.data[i][0] << endl; // output the localSv
    }
    file.close();

    if (numWorkers > 1) {
        MPI_Barrier(MPI_COMM_WORLD); // wait for all processes
    }
    if (myRank == 0) { // collect the simulation results
        file.open(fname);
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
}