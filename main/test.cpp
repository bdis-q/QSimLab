#include "omsim.h"

QCircuit Deutsch_Jozsa() {
    // test circuit
    int numQubits = 4;
    QCircuit qc(numQubits, "Deutsch-Jozsa");
    qc.x(0);
    qc.barrier();
    for (int i = 0; i < numQubits; ++ i) {
        qc.h(i);
    }
    qc.cx(3, 0);
    qc.barrier();
    for (int i = 1; i < numQubits; ++ i) {
        qc.h(i);
    }
    qc.print();
    return qc;
}

int main() {
    Matrix<DTYPE> sv(1 << 4, 1);
    sv.data[0][0] = 1;

    QCircuit qc = Deutsch_Jozsa();
    Matrix<DTYPE> opMat = OMSim(sv, qc);

    cout << "Operation matrix:" << endl;
    opMat.print();

    cout << "State vector after operation:" << endl;
    sv.print();

    return 0;
}