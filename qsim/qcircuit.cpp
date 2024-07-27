#include "qcircuit.h"

QCircuit::QCircuit() {}

/**
 * @brief Construct an n-qubit 1-level quantum circuit object
 * 
 * @param numQubits_ #Qubits
 * @param numDepths_ #Depths
 */
QCircuit::QCircuit(int numQubits_, string name_){
    numQubits = numQubits_;
    numDepths = 0;
    add_level(); // numDepths += 1
    name = name_;
}

// 
// Single-qubit gates
// 

/**
 * @brief Apply an H gate to qubit[qid]
 * 
 * @param qid   qubit id
 */
void QCircuit::h(int qid) {
    if (! gates[numDepths-1][qid].isIDE()) {
        add_level();
    }
    gates[numDepths-1][qid] = QGate("H", {}, {qid});
}

/**
 * @brief Apply an X gate to qubit[qid]
 * 
 * @param qid   qubit id
 */
void QCircuit::x(int qid) {
    if (! gates[numDepths-1][qid].isIDE()) {
        add_level();
    }
    gates[numDepths-1][qid] = QGate("X", {}, {qid});
}

/**
 * @brief Apply a Y gate to qubit[qid]
 * 
 */
void QCircuit::y(int qid) {
    if (! gates[numDepths-1][qid].isIDE()) {
        add_level();
    }
    gates[numDepths-1][qid] = QGate("Y", {}, {qid});
}

/**
 * @brief Apply a Z gate to qubit[qid]
 * 
 * @param qid   qubit id
 */
void QCircuit::z(int qid) {
    if (! gates[numDepths-1][qid].isIDE()) {
        add_level();
    }
    gates[numDepths-1][qid] = QGate("Z", {}, {qid});
}

/**
 * @brief Apply an RX gate to qubit[qid]
 * 
 * @param param the gate parameter
 * @param qid   qubit id
 */
void QCircuit::rx(double theta, int qid) {
    if (! gates[numDepths-1][qid].isIDE()) {
        add_level();
    }
    gates[numDepths-1][qid] = QGate("RX", {}, {qid}, theta);
}

/**
 * @brief Apply an RY gate to qubit[qid]
 * 
 * @param param the gate parameter
 * @param qid   qubit id
 */
void QCircuit::ry(double theta, int qid) {
    if (! gates[numDepths-1][qid].isIDE()) {
        add_level();
    }
    gates[numDepths-1][qid] = QGate("RY", {}, {qid}, theta);
}

/**
 * @brief Apply an RZ gate to qubit[qid]
 * 
 * @param param the gate parameter
 * @param qid   qubit id
 */
void QCircuit::rz(double theta, int qid) {
    if (! gates[numDepths-1][qid].isIDE()) {
        add_level();
    }
    gates[numDepths-1][qid] = QGate("RZ", {}, {qid}, theta);
}

// 
// 2-qubit gates
// 

/**
 * @brief Apply a CX gate to qubit[ctrl] and qubit[targ]
 * 
 * @param ctrl  control qubit id
 * @param targ  target qubit id
 */
void QCircuit::cx(int ctrl, int targ) {
    int start = min(ctrl, targ);
    int end = max(ctrl, targ);
    for (int i = start; i <= end; ++ i) {
        if (! gates[numDepths-1][i].isIDE()) {
            add_level();
            break;
        }
    }
    for (int i = start; i <= end; ++ i) {
        gates[numDepths-1][i] = QGate("MARK", {ctrl}, {targ});
    }
    gates[numDepths-1][targ] = QGate("CX", {ctrl}, {targ});
}

/**
 * @brief Apply a CY gate to qubit[ctrl] and qubit[targ]
 * 
 * @param ctrl  control qubit id
 * @param targ  target qubit id
 */
void QCircuit::cy(int ctrl, int targ) {
    int start = min(ctrl, targ);
    int end = max(ctrl, targ);
    for (int i = start; i <= end; ++ i) {
        if (! gates[numDepths-1][i].isIDE()) {
            add_level();
            break;
        }
    }
    for (int i = start; i <= end; ++i) {
        gates[numDepths-1][i] = QGate("MARK", {ctrl}, {targ});
    }
    gates[numDepths-1][targ] = QGate("CY", {ctrl}, {targ});
}

/**
 * @brief Apply a CZ gate to qubit[ctrl] and qubit[targ]
 * 
 * @param ctrl  control qubit id
 * @param targ  target qubit id
 */
void QCircuit::cz(int ctrl, int targ) {
    int start = min(ctrl, targ);
    int end = max(ctrl, targ);
    for (int i = start; i <= end; ++ i) {
        if (! gates[numDepths-1][i].isIDE()) {
            add_level();
            break;
        }
    }
    for (int i = start; i <= end; ++i) {
        gates[numDepths-1][i] = QGate("MARK", {ctrl}, {targ});
    }
    gates[numDepths-1][targ] = QGate("CZ", {ctrl}, {targ});
}

/**
 * @brief Apply a SWAP gate to qubit[qid1] and qubit[qid2]
 * 
 * @param qid1  qubit id 1
 * @param qid2  qubit id 2
 */
void QCircuit::swap(int qid1, int qid2) {
    int start = min(qid1, qid2);
    int end = max(qid1, qid2);
    for (int i = start; i <= end; ++ i) {
        if (! gates[numDepths-1][i].isIDE()) {
            add_level();
            break;
        }
    }
    for (int i = start; i <= end; ++ i) {
        gates[numDepths-1][i] = QGate("MARK", {}, {start, end});
    }
    gates[numDepths-1][end] = QGate("SWAP", {}, {start, end});
}

/**
 * @brief Add a barrier to the quantum circuit
 */
void QCircuit::barrier() {
    add_level();
}

/**
 * @brief Set the circuit depth to numDepths_
 * 
 * @param numDepths_  the target circuit depth
 */
void QCircuit::setDepths(int numDepths_) {
    int range = numDepths_ - numDepths;
    for (int i = 0; i < range; ++ i){
        add_level();
    }
}

/**
 * @brief Print the structure of the quantum circuit
 */
void QCircuit::print() {
    printInfo();
    int start = 0;
    if (numQubits >= 6) {
        start = numQubits - 6;
    }
    for (int i = numQubits - 1; i >= start; -- i) {
        cout << "q[" << i << "]\t";
        for (int j = 0; j < numDepths; ++ j) {
            if (j > 10) {
                cout << "...";
                break;
            }
            if (gates[j][i].isControlQubit(i)) {
                cout << "C";
            } else if (gates[j][i].isTargetQubit(i)) {
                cout << "T";
            }
            cout << gates[j][i].gname << "\t"; 
        }
        cout << endl;
    }
}

/**
 * @brief Print the information of the quantum circuit
 */
void QCircuit::printInfo() {
    cout << "[INFO] [" << name << "] numQubits: [" << numQubits << "] numDepths: [" << numDepths << "]" << endl;
}

/**
 * @brief Add a new level full of IDE gates to the circuit
 */
void QCircuit::add_level() {
    vector<QGate> level;
    for (int i = 0; i < numQubits; ++ i) {
        level.push_back(QGate("IDE", {}, {i}));
    }
    gates.push_back(level);
    numDepths ++;
}
