#include "svsim.h"

/**
 * @brief State vector simulation of a quantum circuit on the state vector
 * 
 * @param sv    the initial state vector
 * @param qc    a quantum circuit
 */
void SVSim(Matrix<DTYPE>& sv, QCircuit& qc) {
    for (int j = 0; j < qc.numDepths; ++ j) {
        for (int qid = 0; qid < qc.numQubits; ++ qid) {
            QGate& gate = qc.gates[j][qid];
            if (gate.isIDE() || gate.isMARK()) {
                continue;
            }
            svsimForGate(sv, gate);
        }
    }
}

/**
 * @brief [TODO] State vector simulation for a quantum gate
 * 
 * @param sv    the state vector
 * @param gate  the processing gate
 */
void svsimForGate(Matrix<DTYPE>& sv, QGate& gate) {
    bool isAccessed[sv.row];
    memset(isAccessed, 0, sv.row*sizeof(bool));
    
    ll numAmps = (1 << gate.numTargets()); // the number of amplitudes involved in matrix-vector multiplication
    Matrix<DTYPE> amps_vec(numAmps, 1); // save the involved amplitudes

    // [TODO] 1. Calculate the strides for the involved amplitudes
    /*  <e.g.>
        (1) If there is only one target qubit, two amplitudes are involved. 
            If the target qubit is q[k], strides = {0, 2^k}. 
        (2) If there are two target qubits, four amplitudes are involved. 
                      c  t
            idx[0] |..0..0..> -+ strides[0] = 0 --+------------------+
                               |                  |                  |
            idx[1] |..0..1..> -+ strides[1] = 2^t |                  |
                                                  |                  |
            idx[2] |..1..0..> --------------------+ strides[2] = 2^c |
                                                                     |
            idx[3] |..1..1..> ---------------------------------------+ strides[3] = 2^c + 2^t
        (3) If there are 'gate.numTargets()' target qubits, there are 'numAmps' amplitudes. 
    */
    vector<ll> strides;
    // cout << "[TODO] Calculate the strides for the involved amplitudes." << endl;
    // exit(1);
    for (ll idx = 0; idx < numAmps; ++ idx) {
        ll stride = 0;
        for (int j = 0; j < gate.numTargets(); ++ j) {
            if (idx & (1 << j)) { // if the j-th bit of idx is 1
                stride += (1 << gate.targetQubits[j]);
            }
        }
        strides.push_back(stride);
    }
    // ///////////////////////////////////////////////////////////

    // 2. Iterate over all amplitudes
    for (ll ampidx = 0; ampidx < sv.row; ++ ampidx) {
        // 2.1. Skip the amplitude if it is already accessed
        if (isAccessed[ampidx]) continue;

        // [TODO] 2.2. Save the involved amplitudes to amps_vec and mark them as accessed
        // cout << "[TODO] Save the involved amplitudes to amps_vec and mark them as accessed." << endl;
        // exit(1);
        for (ll idx = 0; idx < numAmps; ++ idx) {
            if (ampidx + strides[idx] >= sv.row) {
                cout << "[ERROR] Exceed the length of the state vector." << endl;
                exit(1);
            }
            amps_vec.data[idx][0] = sv.data[ampidx + strides[idx]][0];
            isAccessed[ampidx + strides[idx]] = true;
        }
        // ///////////////////////////////////////////////////////////

        // 3. Check the control bits of the current amplitude
        //    If the control bits are not satisfied, skip this amplitude group
        if (! isLegalControlPattern(ampidx, gate)) {
            continue;
        }

        // 4. Apply the gate matrix and update the state vector in place
        amps_vec = (*gate.gmat) * amps_vec;
        for (ll idx = 0; idx < numAmps; ++ idx) {
            sv.data[ampidx + strides[idx]][0] = amps_vec.data[idx][0];
        }
    }
}

/**
 * @brief [TODO] Check if the index of an amplitude is a legal control pattern of the gate
 * 
 * @param amp  the amplitude index
 * @param gate the processing gate
 * @return true  amp is a legal control pattern
 * @return false amp is an illegal control pattern
 */
bool isLegalControlPattern(ll ampidx, QGate& gate) {
    int ctrl;
    ll ctrlmask = 0;
    for (int i = 0; i < gate.numControls(); ++ i) {
        // [TODO] Check the control qubits of the gate ////////////////
        // [HINT] If the i-th bit of amp is 0 and q_i is a |1> control qubit of gate, return false. 
        // cout << "[TODO] Check the control qubits of the gate." << endl;
        // exit(1);
        ctrl = gate.controlQubits[i];
        ctrlmask = (1 << ctrl);
        
        // suppose a negative ctrl means 0-controlled and a positive ctrl means 1-controlled
        // 0-controlled and the control qubit of amp is 1
        if (ctrl < 0 && (ampidx & ctrlmask) == 1) {
            return false;
        }

        // 1-controlled and the control qubit of amp is 0
        if (ctrl > 0 && (ampidx & ctrlmask) == 0) {
            return false;
        }
        // ///////////////////////////////////////////////////////////
    }
    return true;
}