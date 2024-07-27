#pragma once

#include "qcircuit.h"
#include "qhipster.h"

/**
 * @brief Conduct the state vector simulation for a quantum circuit
 *
 * @param sv the state vector
 * @param qc a quantum circuit
 */
void SVSim(Matrix<DTYPE>& sv, QCircuit& qc);

//
// Utility functions
//

/**
 * @brief [TODO] Conduct the state vector simulation for a quantum gate
 * 
 * @param sv   the state vector
 * @param gate the processing gate
 * @param myRank the MPI rank
 */
void svsimForGate(Matrix<DTYPE>& sv, QGate& gate, int myRank=0);

/**
 * @brief [TODO] Check if the index of an amplitude is a legal control pattern of the gate
 * 
 * @param ampidx the amplitude index
 * @param gate   the processing gate
 * @return true  ampidx is a legal control pattern
 * @return false ampidx is an illegal control pattern
 */
bool isLegalControlPattern(ll ampidx, QGate& gate);