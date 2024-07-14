#include "omsim.h"

QCircuit test() {
    // test circuit
    int numQubits = 4;
    QCircuit qc(numQubits, "test");
    qc.x(0);
    qc.barrier();
    for (int i = 0; i < numQubits; ++ i) {
        qc.h(i);
    }
    qc.cx(2, 0);
    qc.barrier();
    for (int i = 1; i < numQubits; ++ i) {
        qc.h(i);
    }
    for (int i = 0; i < numQubits/2; ++ i) {
        qc.swap(i, numQubits - i - 1);
    }
    qc.print();
    return qc;
}

int main() {
    Matrix<DTYPE> sv(1 << 4, 1);
    sv.data[0][0] = 1;

    QCircuit qc = test();
    Matrix<DTYPE> opMat = OMSim(sv, qc);

    cout << "The operation matrix: " << endl;
    opMat.print();

    cout << "The final state vector: " << endl;
    sv.print();

    return 0;
}