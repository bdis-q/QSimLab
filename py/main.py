import numpy as np
from math import sqrt
from qiskit import QuantumCircuit
from qiskit_aer import AerSimulator

def separableqc():
    numQubits = 11
    numWorkers = 16
    qc = QuantumCircuit(numQubits)

    # initialize the state vector
    initsv = np.zeros(1 << numQubits)
    for i in range(numWorkers):
        initsv[int(i * ((1 << numQubits) / numWorkers) + i)] = 1 / sqrt(numWorkers)
    qc.initialize(initsv, [i for i in range(numQubits)])

    # add gates
    qc.cx(numQubits-1, 0)
    for i in range(1, numQubits, 2):
        qc.cy(i, i - 1)
    for i in range(1, numQubits, 2):
        qc.cz(i, i - 1)
    for i in range(numQubits):
        qc.rx(i / numQubits, i)
    for i in range(numQubits):
        qc.ry(i / numQubits, i)
    for i in range(numQubits):
        qc.rz(i / numQubits, i)
    qc.swap(0, 2)
    qc.swap(1, 3)

    return qc

def QiskitSim(qc):
    print("The final state vector: ")
    qc.save_statevector()
    simulator = AerSimulator(method='statevector')
    result = simulator.run(qc).result()
    statevector = result.get_statevector(qc)
    for amp in np.asarray(statevector): 
        print(f"{amp:.4f}")

if __name__ == '__main__':
    qc = separableqc()
    # qc.draw()
    QiskitSim(qc)