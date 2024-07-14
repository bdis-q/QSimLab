import numpy as np
from qiskit import QuantumCircuit
from qiskit_aer import AerSimulator
from qiskit.quantum_info import Operator

def test():
    numQubits = 4
    qc = QuantumCircuit(numQubits)
    qc.x(0)
    qc.barrier()
    for i in range(numQubits):
        qc.h(i)
    qc.cx(2, 0)
    qc.barrier()
    for i in range(1, numQubits):
        qc.h(i)
    for i in range(numQubits//2):
        qc.swap(i, numQubits-i-1)
    return qc

def QiskitSim(qc):
    print("The operation matrix: ")
    for row in np.asarray(Operator(qc)):
        for e in row:
            print(f"{e:.2f}", end=" ")
        print()

    print("The final state vector: ")
    qc.save_statevector()
    simulator = AerSimulator(method='statevector')
    result = simulator.run(qc).result()
    statevector = result.get_statevector(qc)
    for amp in np.asarray(statevector): 
        print(f"{amp:.6f}")

if __name__ == '__main__':
    qc = test()
    # qc.draw()
    QiskitSim(qc)