#pragma once

#include <mpi.h>

#include "omsim.h"
#include "svsim.h"

#define MPI_DTYPE MPI_DOUBLE_COMPLEX

/**
 * @brief qHiPSTER, a basic distributed SVSim implementation
 *
 * @param qc a quantum circuit
 * @param numWorkers the number of distributed working processes
 * @param myRank the MPI rank of the current process
 */
void qHiPSTER(QCircuit& qc, int numWorkers, int myRank);

/**
 * @brief Conduct SVSim for one level of gates on low-order qubits
 *
 * @param localSv the local state vector
 * @param qc the original quantum circuit
 * @param level the processing level
 * @param myRank the MPI rank of the current process
 */
void localSVSimForOneLevel(Matrix<DTYPE>& localSv, QCircuit& qc, int level, int myRank);

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
bool isDistributedLegalControlPattern(ll ampidx, QGate& gate, int numLowQubits, int myRank);

/**
 * @brief Conduct SVSim for one level of gates on high-order qubits
 *
 * @param localSv the local state vector
 * @param qc the original quantum circuit
 * @param level the processing level
 * @param myRank the MPI rank of the current process
 */
void distributedSVSimForOneLevel(Matrix<DTYPE>& localSv, QCircuit& qc, int level, int myRank);

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
MPI_Comm getPeerComm(int numLowQubits, int numHighQubits, QGate& gate, int myRank);


pair<vector<ll>, vector<ll>> compactLocalAmp(vector<DTYPE>& sendbuf, QGate& gate);
void svsimOnCompactedAmp(vector<DTYPE>& compbuf, int numPeers, QGate& gate, vector<ll>& heads);
void reorderLocalAmp(vector<DTYPE>& compbuf, pair<vector<ll>, vector<ll>>& ampOrder);


void printSimResults(Matrix<DTYPE>& localSv, string fname, int numWorkers, int myRank);