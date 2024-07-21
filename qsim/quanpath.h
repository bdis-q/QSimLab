#pragma once

#include <mpi.h>

#include "omsim.h"
#include "svsim.h"

typedef struct {
    QCircuit& qc;
    int numHighQubits;
} HighOpmatThreadArgs;

/**
 * @brief [TODO] Conduct OMSim for high-order qubits using a thread
 *
 * @param arg HighOpmatThreadArgs including qc and numHighQubits
 */
void* highOMSim(void* arg);

/**
 * @brief Conduct SVSim for low-order qubits
 *
 * @param localSv the local state vector
 * @param qc the original quantum circuit
 * @param myRank the MPI rank
 */
void lowSVSim(Matrix<DTYPE>& localSv, QCircuit& qc, int myRank);

/**
 * @brief [TODO] Check if the index of an amplitude is a legal control pattern of the gate
 * 
 * @param ampidx the amplitude index
 * @param gate the processing gate
 * @param numLowQubits #low-order qubits in QuanPath
 * @param myRank the MPI rank
 * @return true  ampidx is a legal control pattern
 * @return false ampidx is an illegal control pattern
 */
bool isQuanPathLegalControlPattern(ll ampidx, QGate& gate, int numLowQubits, int myRank);

/**
 * @brief [TODO] Conduct the final merge operation in QuanPath
 * 
 * @param localSv the local state vector
 * @param ptrOpmat the pointer to the high-order operation matrix
 * @param numWorkers #distributed processes
 * @param myRank the MPI rank
 */
void merge(Matrix<DTYPE>& localSv, Matrix<DTYPE>* ptrOpmat, int numWorkers, int myRank);