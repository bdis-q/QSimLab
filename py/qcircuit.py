from qiskit import *
import random
from math import pi
from qiskit.circuit.library.standard_gates import TGate
import qiskit.quantum_info as qi

def RandomRegular(numQubits, numDepths):
    qc = QuantumCircuit(numQubits)

    while True:
        if qc.depth() % 10 == 2:
            for j in range(numQubits-1, 0, -2):
                qc.cx(j, j-1)
            for j in range(numQubits-2, 0, -2):
                qc.cx(j, j-1)
        else:
            for j in range(numQubits):
                gTyp = random.randint(0, 2)
                if gTyp == 0:
                    qc.h(j)
                elif gTyp == 1:
                    qc.z(j)
                else:
                    qc.x(j)
        if qc.depth() == numDepths:
            break
    qc.save_statevector()
    print(f'[RandomRegular] #Qubits: {numQubits} #Depths: {qc.depth()}')
    return qc


def RandomRandom(numQubits, numDepths):
    qc = QuantumCircuit(numQubits)

    # set a random initial state
    # psi = qi.random_statevector(2 ** numQubits, seed=100)
    # qc.set_statevector(psi)

    while True:
        if qc.depth() % 10 == 2:
            for j in range(numQubits-1, 0, -2):
                qc.cx(j, j-1)
            for j in range(numQubits-2, 0, -2):
                qc.cx(j, j-1)
        else:
            for j in range(numQubits):
                gTyp = random.randint(0, 3)
                if gTyp == 0:
                    qc.h(j)
                elif gTyp == 1:
                    qc.z(j)
                elif gTyp == 2:
                    qc.x(j)
                else:
                    qc.ry(random.uniform(0, 2 * pi), j)
        if qc.depth() == numDepths:
            break
#     circ.measure_all()
    qc.save_statevector()
    print(f'[RandomRandom] #Qubits: {numQubits} #Depths: {qc.depth()}')
    return qc

def VQC(numQubits=15):
    ''' Circuit 2 '''
    qc = QuantumCircuit(numQubits)

    for _ in range(20):
        for _ in range(2):
            for i in range(numQubits):
                qc.ry(random.uniform(0, 2 * pi),i)
        for i in range(numQubits-1):
            qc.cx(i, i+1)
            qc.barrier()
        for _ in range(2):
            for i in range(numQubits):
                qc.ry(random.uniform(0, 2 * pi),i)
    qc.save_statevector()
    print(f'[VQC] #Qubits: {numQubits} #Depths: {qc.depth()}')
    return qc

def QFT(nqubit):
    circ = QuantumCircuit(nqubit)
    ct = TGate().control(1)
    
    circ.h(nqubit-1)

    #nqubit为偶数
    ii = 0
    jj = nqubit-1
    while(ii<jj) :
        circ.swap(ii,jj)
        ii+=1
        jj-=1

    circ.h(0)
    circ.cs(1,0)
    circ.barrier()

    circ.h(1)
    circ.append(ct, [2, 0])
    circ.cs(2,1)
    circ.barrier()

    n = nqubit-1
    i = 2
    while(i<n) :
        circ.h(i)

        t = i
        tt = 0
        while(t>=2) :
            circ.crz(1/(2**(t+1)), i+1, tt)
            tt+=1
            t-=1
        circ.append(ct, [i+1, i-1])
        circ.cs(i+1,i)
        i+=1
        circ.barrier()
    circ.h(n)
    return circ