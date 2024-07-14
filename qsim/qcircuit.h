#pragma once

#include "qgate.h"

class QCircuit {
public:
    int numQubits;
    int numDepths;
    vector<vector<QGate>> gates;
    string name;

    QCircuit();
    QCircuit(int numQubits_, string name_="qcircuit");

    // 
    // Single-qubit gates
    // 
    void h(int qid);
    void x(int qid);
    void y(int qid);
    void z(int qid);
    void rx(double theta, int qid);
    void ry(double theta, int qid);
    void rz(double theta, int qid);

    //
    // 2-qubit gates
    // 
    void cx(int ctrl, int targ);
    void cy(int ctrl, int targ);
    void cz(int ctrl, int targ);
    void swap(int qid1, int qid2);

    //
    // Other operations on quantum circuits
    //
    void barrier();
    void setDepths(int numDepths_);
    void print();
    void printInfo();

    void add_level();
};
