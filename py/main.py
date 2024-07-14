from qiskit import QuantumCircuit
from mqt import ddsim
from mqt.ddsim import CircuitSimulator
import time
from math import pi
from qiskit_aer import AerSimulator
import qiskit.quantum_info as qi

from qcircuit import *

def qcircuit():
    numQubits = 4
    qc = QuantumCircuit(numQubits)
    qc.x(0)
    qc.barrier()
    for i in range(numQubits):
        qc.h(i)
    qc.cx(1, 0)
    for i in range(1, numQubits):
        qc.h(i)
    # measure

def qiskitSim(circ) :
    simulator = AerSimulator(method='statevector')
#     circ = transpile(circ, simulator)
    result = simulator.run(circ).result()
#     statevector = result.get_statevector(circ)
#     print(statevector)
    # print(result.data(0))

if __name__ == '__main__':
    circ = qcircuit()
    qiskitSim(circ)
