#pragma once

#include "qcircuit.h"

/**
 * @brief State vector simulation of a quantum circuit on the state vector
 *
 * @param sv the state vector
 * @param qc a quantum circuit
 */
void SVSim(Matrix<DTYPE>& sv, QCircuit& qc);

//
// Utility functions
//

/**
 * @brief [TODO] State vector simulation for a quantum gate
 * 
 * @param sv   the state vector
 * @param gate the processing gate
 */
void svsimForGate(Matrix<DTYPE>& sv, QGate& gate);

/**
 * @brief [TODO] Check if the index of an amplitude is a legal control pattern of the gate
 * 
 * @param amp  the amplitude index
 * @param gate the processing gate
 * @return true  amp is a legal control pattern
 * @return false amp is an illegal control pattern
 */
bool isLegalControlPattern(ll ampidx, QGate& gate);