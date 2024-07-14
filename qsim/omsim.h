#pragma once

#include "qcircuit.h"

/**
 * @brief [TODO] Conduct operation matrix simulation of a quantum circuit
 * 
 * @param sv the state vector
 * @param qc a quantum circuit
 * @return Matrix<DTYPE> the operation matrix
 */
Matrix<DTYPE> OMSim(Matrix<DTYPE>& sv, QCircuit& qc);

//
// Utility functions
//

/**
 * @brief [TODO] Get a complete gate matrix according to the applied qubits
 * 
 * @param gate the processing gate
 * @return Matrix<DTYPE> a complete gate matrix
 */
Matrix<DTYPE> getCompleteMatrix(QGate& gate);

/**
 * @brief [TODO] Generate the gate matrix of a controlled gate
 *
 * @param gate the processing gate
 * @return Matrix<DTYPE> a complete gate matrix
*/
Matrix<DTYPE> genControlledGateMatrix(QGate& gate);

/**
 * @brief Generate the gate matrix of a SWAP gate
 * 
 * @param gate the processing SWAP gate
 * @return Matrix<DTYPE> a complete gate matrix
 */
Matrix<DTYPE> genSwapGateMatrix(QGate& gate);

/**
 * @brief Swap two rows of a gate matrix
 * 
 * @param r1   row index 1
 * @param r2   row index 2
 * @param gate return value
 */
void swapRow(ll r1, ll r2, Matrix<DTYPE>& gate);
