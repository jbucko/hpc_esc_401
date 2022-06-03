## Solutions to Exercise sheet 11


First of all, in order to compile our codes we need to switch from cray programming environment and go for pgi, in addition, we  need to load necessary modules. That sums up to execute the following:

```
module switch PrgEnv-cray/6.0.10 PrgEnv-pgi/6.0.8
module load daint-gpu cudatoolkit craype-accel-nvidia60
```

To run the code, one can generate a job script or run in an interactive mode. To do the latter, request a node using e.g.

```
salloc -p debug -A uzg2 -t 0:30:00 -C gpu
```
and then run your executable `code.openacc` with parameters `param1, param2,...` as 

```bash
srun code.openacc < param1 > < param2 > ...
```
If you want you can also include other slurm definitions such as `--ntasks 8` in front of your executable.

### Exercise 1 [axpy]

Here, the only task was to offload for loop in `axpy_gpu()` function. 

```C++
void axpy_gpu(int n, double alpha, const double *x, double* y) {

    int i;
    #pragma acc parallel loop pcopyin(x[0:n]) pcopy(y[0:n])
    for(i = 0; i < n; ++i) {
        y[i] += alpha*x[i];
    }
}
```

We use values of both `x,y` and output values of `y`, therefore it is enough to specify `pcopyin(x[0:n])`, but we need `pcopy(y[0:n])`. 

### Exercise 2 [basics/blur]

...to be added...

### Exercise 3 [basics/dot]

...to be added...



