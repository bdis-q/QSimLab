# The Programming Assignments of Quantum Computing

# Assignment 2: State Vector Simulations

The assignment description can be found in [240722-a2-SVSim.pdf](https://github.com/bdis-q/QSimLab/blob/a2-svsim/240722-a2-SVSim.pdf). 
To get started, please make sure that you have already commit the changes made to `a1-omsim`. Then, checkout to the `a2-svsim` branch and merge the work from `a1-omsim`. 

```shell
git checkout a2-svsim
git pull # update this branch
git merge a1-omsim -m "Continue to work on a2."
```

Then, you can compile and run this project. Please note that the `Makefile` is designed for Windows. Codes that need to be implemented are marked with `[TODO]` labels. 

```shell
make clean
make
[path_to_your_QSimLab_dir]/QSimLab/obj/main.exe > output.txt
```

Once you have completed all the missing parts, you can compare the simulation results with the output of the Qiskit simulator provided in the `py/main.py`. We use `qiskit@1.0.2` and `qiskit_aer@0.14.0.1`. 

```shell
python py/main.py > pyoutput.txt
```

After you finish this assignment, you can commit your work locally. The message after `-m` can be determined by yourself. 

```shell
git commit -a -m "My work on a2 is done."
```
