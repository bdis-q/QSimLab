#include "qgate.h"

QGate::QGate() {
    gname = "NULL";
    controlQubits = {};
    targetQubits = {};
    gmat = nullptr;
}

/**
 * @brief Construct a new QGate::QGate object, initialize the gate matrix with the given name
 * 
 * @param gname_ the gate name
 * @param controls_ control qubits
 * @param targets_ target qubits
 */
QGate::QGate(string gname_, vector<int> controls_, vector<int> targets_) {
    gname = gname_;
    controlQubits = controls_;
    targetQubits = targets_;
    gmat = Matrix<DTYPE>::MatrixDict[gname];
    if (gmat == nullptr) {
        cout << "[ERROR] Gate " << gname << " not found in MatrixDict" << endl;
        exit(1);
    }
}

/**
 * @brief Construct a new QGate::QGate object with a parameter
 * 
 * @param gname_ the gate name
 * @param controls_ control qubits
 * @param targets_ target qubits
 * @param theta a parameter
 */
QGate::QGate(string gname_, vector<int> controls_, vector<int> targets_, double theta) {
    gname = gname_;
    controlQubits = controls_;
    targetQubits = targets_;
    
    string matkey = gname + to_string(theta);
    gmat = Matrix<DTYPE>::MatrixDict[matkey];
    if (gmat != nullptr) { // the gate matrix already exists
        cout << "[DEBUG] Matrix already exists: " << matkey << ", " << gmat << endl;
        return;
    }

    Matrix<DTYPE> mat;
    if (gname == "RX") {
        mat.rotationX(theta);
    } else if (gname == "RY") {
        mat.rotationY(theta);
    } else if (gname == "RZ") {
        mat.rotationZ(theta);
    } else {
        cout << "[ERROR] Gate " << gname << " not implemented" << endl;
        exit(1);
    }
    Matrix<DTYPE>::MatrixDict[matkey] = make_shared<Matrix<DTYPE>>(move(mat));
    gmat = Matrix<DTYPE>::MatrixDict[matkey];
}

/**
 * @brief Copy construct a new QGate::QGate object
 * 
 * @param other 
 */
QGate::QGate(const QGate& other) {
    gname = other.gname;
    controlQubits = other.controlQubits;
    targetQubits = other.targetQubits;
    gmat = other.gmat;
}

/**
 * @brief Copy assignment
 * 
 * @param other 
 * @return QGate& 
 */
QGate& QGate::operator=(const QGate& other) {
    gname = other.gname;
    controlQubits = other.controlQubits;
    targetQubits = other.targetQubits;
    gmat = other.gmat;
    return *this;
}

// Return the number of input/output qubits of the gate
int QGate::numQubits() {
    return controlQubits.size() + targetQubits.size();
}

// Return the number of control qubits of the gate
int QGate::numControls() {
    return controlQubits.size();
}

// Return the number of target qubits of the gate
int QGate::numTargets() {
    return targetQubits.size();
}

// Check if the gate is an identity gate
bool QGate::isIDE() {
    return gname == "IDE";
}

// Check if the gate is a placeholder gate
bool QGate::isMARK() {
    return gname == "MARK";
}

// Check if the gate is a single-qubit gate
bool QGate::isSingle() {
    return gname != "IDE" && gname != "MARK" && controlQubits.size() == 0 && targetQubits.size() == 1;
}

// Check if the gate is a 2-qubit controlled gate
bool QGate::is2QubitControlled() {
    return gname != "MARK" && controlQubits.size() == 1 && targetQubits.size() == 1;
}

// Check if qubit[qid] is a control qubit of the gate
bool QGate::isControlQubit(int qid) {
    return find(controlQubits.begin(), controlQubits.end(), qid) != controlQubits.end();
}

// Check if qubit[qid] is a target qubit of the gate
bool QGate::isTargetQubit(int qid) {
    return gname != "IDE" && gname != "MARK" && find(targetQubits.begin(), targetQubits.end(), qid) != targetQubits.end();
}

// Print the gate information
void QGate::print() {
    cout << "===== Gate: " << gname << " =====" << endl;
    cout << "Control qubits: ";
    for (vector<int>::size_type i = 0; i < controlQubits.size(); i++) {
        cout << controlQubits[i] << " ";
    }
    cout << endl;
    cout << "Target qubits: ";
    for (vector<int>::size_type i = 0; i < targetQubits.size(); i++) {
        cout << targetQubits[i] << " ";
    }
    cout << endl;
    gmat->print();
}

// Destructor
QGate::~QGate() {
    return;
}

// Compare two integers by their absolute values
// Control qubits can be negative to denote 0-controlled
bool compareByAbsoluteValue(int a, int b) {
    return std::abs(a) < std::abs(b);
}