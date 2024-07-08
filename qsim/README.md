# QSim

This repo provides some basic data structures and functions necessary for quantum circuit simulation. 

## 1. Basic Data Structures

### 1.1. The Matrix Structure

> matrix.[h/cpp]

In `matrix.[h/cpp]`, we implement the matrix structure. The gate matrices of common quantum gates are recorded in `static map<string, shared_ptr<Matrix>> MatrixDict`. Related operations, including matrix initialization, tensor product, addition and multiplication are implemented. 

The data type of the matrix elements can be set in `matrix.h` as follows. 

```cpp
#define DTYPE complex<double>
```

### 1.2. Quantum Gates

> qgate.[h/cpp]

In `qgate.[h/cpp]`, we implement the structure of quantum gates `QGate`, which has four data members, i.e., `gname`, `controlQubits`, `targetQubits`, `gmat`. To save memory footprints, `gmat` is a shared pointer to a global gate matrix map `MatrixDict` defined in `matrix.[h/cpp]`. 
When creating a new gate, we can first add a new gate matrix entry to `MatrixDict` by modifying function `void Matrix<T>::initMatrixDict()`. Then, make the `gmat` member of this new gate point to this entry. 

### 1.3. Quantum Circuits

> qcircuit.[h/cpp]

In `qcircuit.[h/cpp]`, we implement the structure of quantum circuits and provide an interface for creating a quantum circuit and adding gates. Please note that $q_0$ represents the low-order (least significant) qubit. 

## 2. Quantum Circuit Simulations

### 2.1. Operation Matrix Simulation (OMSim)

> omsim.[h/cpp]

In a quantum circuit, the operations on qubits are described using quantum gates. Mathematically, each quantum gate can be represented by a complex unitary matrix. Given a quantum gate $G_X$, if it has $i$ input qubits, mathematically speaking, $G_X$ is a $2^i \times 2^i$ matrix. However, when $G_X$ is applied to a quantum circuit, its input qubits may not be adjacent, thus covering more qubits. We use $M_X$ to denote the complete gate matrix of $G_X$ in the circuit. If $G_X$ covers $j$ qubits, $M_X$ has a size of $2^j \times 2^j$. 
How to construct the complete gate matrix of a multi-qubit gate can be found [here](https://zhuanlan.zhihu.com/p/105251205). 

The state vector of an $n$-qubit circuit has a size of $2^n$. To change its state, the operation matrix simulation constructs an operation matrix of $2^n \times 2^n$ at each level of the circuit. 
Let $M_i^j$ be the $i$-th quantum gate matrix from high-order to low-order at the $j$-th level. 
The operation matrix $O_j$ of level $j$ can be constructed by

$$
    O_j=M_0^j \otimes M_1^j \otimes M_2^j \otimes \ldots, 
$$

where $\otimes$ refers to the tensor product of matrices. If no gate is applied to a qubit, it can be supplemented with a $2\times2$ identity gate $I$. 
The operation matrix of a $T$-level circuit, denoted as $O$, can be constructed by

$$
    O = O_{T-1} \cdot O_{T-2} \cdot \ldots \cdot O_0. 
$$

Then, the state vector after $T$ levels can be updated as $\ket{\phi_T} = O \ket{\phi_0}$, where $\ket{\phi_0}$ is the initial state vector. 
The time complexity of the multiplication of the $2^n \times 2^n$ operation matrix and $2^n$ state vector at every level is $O(2^{2n})$. 

### 2.2. State Vector Simulation (SVSim)

> svsim.[h/cpp]

State vector simulation is a useful algorithm for the simulation of quantum circuits, especially in distributed situations. 
Instead of constructing the entire operation matrix, the state vector simulation directly applies the transformations specified by quantum gates to the state vector to reduce the overhead. 

For a single-qubit gate $G_A$, applying it to the $k$-th qubit is equivalent to applying its gate matrix to every pair of amplitudes whose binary indices differ only in the $k$-th bits $(k=0,\ldots,n-1)$:

$$
\begin{bmatrix}
    \alpha_{*...*0_k*...*}^\prime \\
    \alpha_{*...*1_k*...*}^\prime
\end{bmatrix} =G_A \cdot
\begin{bmatrix}
    \alpha_{*...*0_k*...*} \\
    \alpha_{*...*1_k*...*}
\end{bmatrix}.
$$

The stride between $\alpha_{*...*0_k*...*}$ and $\alpha_{*...*1_k*...*}$ is $2^k$, i.e., $(*...*1_k*...*)_2-(*...*0_k*...*)_2=2^k$. 

For a 2-qubit gate controlled-$G_B$ on a control qubit $q_c$ and a target qubit $q_t$, assuming $c>t$, $G_B$ of size $2\times2$ is applied to $q_t$ if $q_c=\ket{1}$:

$$
\begin{bmatrix}
\alpha_{*...*1_c*...*0_t*...*}^\prime\\
\alpha_{*...*1_c*...*1_t*...*}^\prime
\end{bmatrix}=G_{B}\cdot\begin{bmatrix}
\alpha_{*...*1_c*...*0_t*...*}\\
\alpha_{*...*1_c*...*1_t*...*}
\end{bmatrix}.
$$

For a non-controlled 2-qubit gate $G_C$ of size $4\times4$ on $q_c$ and $q_t$, SVSim can be described as: 

$$
\begin{bmatrix}
\alpha_{*...*0_c*...*0_t*...*}^\prime\\
\alpha_{*...*0_c*...*1_t*...*}^\prime\\
\alpha_{*...*1_c*...*0_t*...*}^\prime\\
\alpha_{*...*1_c*...*1_t*...*}^\prime
\end{bmatrix}=
G_{C} \cdot
\begin{bmatrix}
\alpha_{*...*0_c*...*0_t*...*}\\
\alpha_{*...*0_c*...*1_t*...*}\\
\alpha_{*...*1_c*...*0_t*...*}\\
\alpha_{*...*1_c*...*1_t*...*}
\end{bmatrix},
$$

where the strides between these four amplitudes are $2^t$, $2^c-2^t$, and $2^t$. 


Since each gate on the circuit requires performing SVSim on the entire state vector, for an $n$-qubit quantum circuit with $T$ levels, the time complexity of SVSim is $O(n \cdot 2^n \cdot T)$. 
