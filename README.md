# The Programming Assignments of Quantum Computing

# Assignment 3: Distributed Quantum Circuit Simulations

The assignment description can be found in [240805-a3-DisQSim.pdf](https://github.com/bdis-q/QSimLab/blob/a3-disqsim/240805-a3-DisQSim.pdf). 
To get started, please make sure that you have already commit the changes made to `a2-svsim`. Then, checkout to the `a3-disqsim` branch and merge the work from `a2-svsim`. 
You may need to fix merge conflicts. 

```shell
git checkout a3-disqsim
git pull # update this branch
git merge a2-svsim -m "Continue to work on a3."
```

Then, you can compile and run this project. Codes that need to be implemented are marked with `[TODO]` labels. 

```shell
make clean
make
mpirun -np 16 obj/main # the number of processes is 16
```

Once you have completed all the missing parts, you can compare the simulation results with the output of the Qiskit simulator provided in the `py/main.py`. We use `qiskit@1.0.2` and `qiskit_aer@0.14.0.1`. 

```shell
python py/main.py > pyoutput.txt
```

After you finish this assignment, you can commit your work locally. The message after `-m` can be determined by yourself. 

```shell
git commit -a -m "My work on a3 is done."
```
