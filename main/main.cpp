#include "svsim.h"

QCircuit test() {
    // test circuit
    int numQubits = 4;
    QCircuit qc(numQubits, "test");
    for (int i = 0; i < numQubits; ++ i) {
        qc.rx((double)i / numQubits, i);
    }
    for (int i = 1; i < numQubits; ++ i) {
        qc.cx(i, i - 1);
    }
    for (int i = 0; i < numQubits; ++ i) {
        qc.ry((double)i / numQubits, i);
    }
    for (int i = 0; i < numQubits; ++ i) {
        qc.rz((double)i / numQubits, i);
    }
    qc.swap(1, 3);
    qc.print();
    return qc;
}

int main() {
    Matrix<DTYPE> sv(1 << 4, 1);
    sv.data[0][0] = 1;

    QCircuit qc = test();
    SVSim(sv, qc);

    cout << "The final state vector: " << endl;
    sv.print();

    return 0;
}