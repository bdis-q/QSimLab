#pragma once

#include <mpi.h>

#include "omsim.h"
#include "svsim.h"

/**
 * @brief QuanPath
 *
 * @param qc a quantum circuit
 * @param numWorkers the number of distributed working processes
 * @param myRank the MPI rank of the current process
 */
void QuanPath(QCircuit& qc, int numWorkers, int myRank);

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
 * @brief [TODO] Conduct the final merge operation in QuanPath
 * 
 * @param localSv the local state vector
 * @param ptrOpmat the pointer to the high-order operation matrix
 * @param numWorkers #distributed processes
 * @param myRank the MPI rank of the current process
 */
void merge(Matrix<DTYPE>& localSv, Matrix<DTYPE>* ptrOpmat, int numWorkers, int myRank);