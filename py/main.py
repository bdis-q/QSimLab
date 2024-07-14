import numpy as np
from qiskit import QuantumCircuit
from qiskit_aer import AerSimulator

def test():
    numQubits = 4
    qc = QuantumCircuit(numQubits)

    for i in range(numQubits):
        qc.rx(i / numQubits, i)
    
    for i in range(1, numQubits):
        qc.cx(i, i - 1)

    for i in range(numQubits):
        qc.ry(i / numQubits, i)

    for i in range(numQubits):
        qc.rz(i / numQubits, i)

    qc.swap(1, 3)
    return qc

def QiskitSim(qc):
    print("The final state vector: ")
    qc.save_statevector()
    simulator = AerSimulator(method='statevector')
    result = simulator.run(qc).result()
    statevector = result.get_statevector(qc)
    for amp in np.asarray(statevector): 
        print(f"{amp:.2f}")

if __name__ == '__main__':
    qc = test()
    # qc.draw()
    QiskitSim(qc)