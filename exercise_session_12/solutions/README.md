## Solutions to Exercise sheet 12


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

Performance-wise, OMP is better compared to GPU, which is understandable as the computation in this particular exercise is simple and bottleneck in GPU case is a data transfer between host and device memory. 

### Exercise 2 [basics/blur]

There is several TODO's altogether in file `blur_openacc.cpp` which need to be addressed. We will comment on them in the following three bullet points

<ol>
    
<li> First TODO is found right above the `blur()` function declaration:

```C++
#ifdef _OPENACC
    // TODO: declare routine accordingly so as to be called from the GPU
#endif
double blur(int pos, const double *u)
{
    return 0.25*(u[pos-1] + 2.0*u[pos] + u[pos+1]);
}
```

    If a function is expected to be executed on device, this function has to be declared so using a routine directive. As we aim for no further parallelization within the `blur()` function, it should be called sequentially. So the line completion should be 

```C++
#ifdef _OPENACC
    #pragma acc routine seq
#endif
double blur(int pos, const double *u)
{
    return 0.25*(u[pos-1] + 2.0*u[pos] + u[pos+1]);
}
```
</li>
    
<li> Two more TODO's are present in  `blur_twice_gpu_naive` below
    
```C++
void blur_twice_gpu_naive(double *in , double *out , int n, int nsteps)
{
    double *buffer = malloc_host<double>(n);

    for (auto istep = 0; istep < nsteps; ++istep) {
        // TODO: offload this loop to the GPU
        for (auto i = 1; i < n-1; ++i) {
            buffer[i] = blur(i, in);
        }

        // TODO: offload this loop to the GPU
        for (auto i = 2; i < n-2; ++i) {
            out[i] = blur(i, buffer);
        }

        in = out;
    }

    free(buffer);
}
```
                                 
where we aim to make device calculations in two separate steps. First, we use values stored in `in` and define `buffer`. Therefore, we need to copy `in` into the device and copy `buffer` out. Second, we want to reuse the values in `buffer` and  UPDATE values in `out`. Therefore, the function should look as follows

```C++
void blur_twice_gpu_naive(double *in , double *out , int n, int nsteps)
{
    double *buffer = malloc_host<double>(n);

    for (auto istep = 0; istep < nsteps; ++istep) {
        #pragma acc parallel loop pcopyin(in[0:n]) pcopyout(buffer[0:n])
        for (auto i = 1; i < n-1; ++i) {
            buffer[i] = blur(i, in);
        }

        #pragma acc parallel loop pcopyin(buffer[0:n]) pcopyout(out[2:n-4])
        for (auto i = 2; i < n-2; ++i) {
            out[i] = blur(i, buffer);
        }

        in = out;
    }

    free(buffer);
}
```              
                                 
</li>
        
<li> There is also `blur_twice_gpu_nocopies` function, in which we should avoid copying data in and out of the device multiple times. 

```C++                          
void blur_twice_gpu_nocopies(double *in , double *out , int n, int nsteps)
{
    double *buffer = malloc_host<double>(n);

    // TODO: move the data needed by the algorithm to the GPU
    {
        for (auto istep = 0; istep < nsteps; ++istep) {
            // TODO: offload this loop to the GPU
            for (auto i = 1; i < n-1; ++i) {
                buffer[i] = blur(i, in);
            }

            // TODO: offload this loop to the GPU
            for (auto i = 2; i < n-2; ++i) {
                out[i] = blur(i, buffer);
            }

            // TODO: offload this loop to the GPU; can you try just the pointer assignment?
            for (auto i = 0; i < n; ++i) {
                in[i] = out[i];
            }
        }
    }

    free(buffer);
}
```
We should complete data clause in front of parallel block. We want to copy in `in`, copy in and out elements of `out` and use temporary array called `buffer`. Then, we just put three clauses to parallel for loops in front of each for loop within the parallel region. The resulting function should look as below
```C++
void blur_twice_gpu_nocopies(double *in , double *out , int n, int nsteps)
{
    double *buffer = malloc_host<double>(n);

    #pragma acc data pcopyin(in[0:n]) pcopy(out[0:n]) pcreate(buffer[0:n])
    {
        for (auto istep = 0; istep < nsteps; ++istep) {
            #pragma acc parallel loop
            for (auto i = 1; i < n-1; ++i) {
                buffer[i] = blur(i, in);
            }

            #pragma acc parallel loop
            for (auto i = 2; i < n-2; ++i) {
                out[i] = blur(i, buffer);
            }

            #pragma acc parallel loop
            for (auto i = 0; i < n; ++i) {
                in[i] = out[i];
            }
        }
    }

    free(buffer);
}
```
</li>
</ol>
    
Regarding performance, OMP again performs best. We also see that `no_copies` version performs approximately two times better than `naive`. This is understandable as there is two times less communication between device and host happening (this indeed indicates that the communication is real bottleneck of computation and not blurring of the array).
   
### Exercise 3 [basics/dot]

There is only one TODO in the last exercise. We need to include data clause to make the data handling efficient and include clause ensuring variable `sum` should keep track a global contributions to the scalar product among all the threads. It is enough to copy in vectors `x,y` and reduce the element-wise products to `sum`. So one should replace the original code

```C++
double dot_gpu(const double *x, const double *y, int n) {
    double sum = 0;
    int i;

    // TODO: Offload this loop to the GPU
    for (i = 0; i < n; ++i) {
        sum += x[i]*y[i];
    }

    return sum;
}
```

with

```C++
double dot_gpu(const double *x, const double *y, int n) {
    double sum = 0;
    int i;

    # pragma acc parallel loop pcopyin(x[:n]) pcopyin(y[:n]) reduction(+:sum)
    for (i = 0; i < n; ++i) {
        sum += x[i]*y[i];
    }

    return sum;
}
```
    
Here again, OMP is faster.    
