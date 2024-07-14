#pragma once

#include "matrix.h"

class QGate {
public:
    string gname; // gate name
    vector<int> controlQubits; // the control qubits of the gate
    vector<int> targetQubits; // the target qubits of the gate
    shared_ptr<Matrix<DTYPE>> gmat; // the gate matrix
    
    QGate();
    QGate(string gname_, vector<int> controls_, vector<int> targets_);
    QGate(string gname_, vector<int> controls_, vector<int> targets_, double theta);
    QGate(const QGate& other);

    QGate& operator=(const QGate& other);

    int numQubits(); // the number of input/output qubits of the gate
    int numControls(); // the number of control qubits of the gate
    int numTargets(); // the number of target qubits of the gate

    bool isIDE(); // check if the gate is an identity gate
    bool isMARK(); // check if the gate is a placeholder gate
    bool isSingle(); // check if the gate is a single-qubit gate
    bool is2QubitControlled(); // check if the gate is a 2-qubit controlled gate
    
    bool isControlQubit(int qid); // check if qubit[qid] is a control qubit of the gate
    bool isTargetQubit(int qid); // check if qubit[qid] is a target qubit of the gate
    
    void print(); // print the gate information

    ~QGate();
};

//
// Utility functions
//

// Compare two integers by their absolute values
// Control qubits can be negative to denote 0-controlled
bool compareByAbsoluteValue(int a, int b);