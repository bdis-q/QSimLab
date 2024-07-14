# The Programming Assignments of Quantum Computing

# Assignment 1: Operation Matrix Simulations

The assignment description can be found in [240715-a1-OMSim.pdf](https://github.com/bdis-q/QSimLab/blob/a1-omsim/240715-a1-OMSim.pdf). 
To get started, please clone the repository first. 

```shell
git clone https://github.com/bdis-q/QSimLab.git
git checkout a1-omsim
```

Then, you can compile and run this project. Please note that the `Makefile` is designed for Windows. Codes that need to be implemented are marked with `[TODO]` labels. 

```shell
cd QSimLab
make
[path_to_your_QSimLab_dir]/QSimLab/obj/main.exe > output.txt
```

Once you have completed all the missing parts, you can compare the simulation results with the output of the Qiskit simulator provided in the `py/main.py`. We use `qiskit@1.0.2` and `qiskit_aer@0.14.0.1`. 

```shell
python py/main.py > pyoutput.txt
```

After you finish this assignment, you can commit your work locally. The message after `-m` can be determined by yourself. 

```shell
git commit -a -m "My work on a1 is done."
```
