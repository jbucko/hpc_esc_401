# Exercise sheet 11 - solutions

## Exercise 1

In order to run the code in parallel on a GPU, we need to add respective `pragma` directives in front of parallel region:

```C++
#include <stdio.h> 
#include "gettime.h"
#include "openacc.h" //not necessary

static long steps = 1000000000;

int main (int argc, const char *argv[]) {
    int i; double x;
    double pi;
    char *p;
    double step = 1.0/(double) steps;
    for (int j=0;j<5;j++){
        double sum = 0.0;
        double start = getTime();
        #pragma acc parallel 
        #pragma acc loop reduction(+:sum) private(x) // private not necessary
        for (i=0; i < steps; i++) {
            x = (i+0.5)*step;
            sum += 4.0 / (1.0+x*x);
        }
        pi = step * sum;
        double delta = getTime() - start;
        printf("PI = %.16g computed in %.4g seconds\n", pi, delta);
    }
}
```

Notice a necessary reduction regarding `sum` variable. On the other hand, `private(x)` is not required here as scalars (and loop variables) are by default private.

Find job script and output file in folder `openacc`. Note that it is enough to run on a single CPU per task, as parallelization happens on the GPU. First iteration is significantly longer that the remaining ones (see below). The reason is GPU needs to be set up and data transfer handled prior the computation.

```
PI = 3.141592653589793 computed in 1.429 seconds
PI = 3.141592653589793 computed in 0.01288 seconds
PI = 3.141592653589793 computed in 0.02531 seconds
PI = 3.141592653589793 computed in 0.05022 seconds
PI = 3.141592653589793 computed in 0.101 seconds
```

## Exercise 2

Instead of five, we obtain only a single print statement (time). For larger number of blocks and threads per block, we get times ~0.01s similar to those obtained in openacc (not initial time). With CUDA the situation is similar and one should also get first slow iteration followed by much faster ones. However, CUDA code timing is now different - it measures only the time of parallel computation and not of the whole code. If we did that and included a for loop, we would get results similar to openacc.

## Exercise 3

The results of parametric study is shown in figure below. We can see that for small total number of threads (Number of Threads x Number of Blocks), the performance is low. When enough number of threads are used (order of 10000 here), the improvement is not anymore that rapid. Note that the maximum number of threads used in our test is 600x160=96000 and maximum number of threads per GPU is 56x2048=114688 thus more that the maximum number we required (56 SM's per GPU, each up to 2048 threads). Let us assume the most extreme case - 600x160, thus 600 blocks each requesting 160 threads. As threads are scheduled in warps (32 threads running on 32 CPUs), 160 threads will in reality mean 160/32=5 warps per block. An SM can run floor(2048/160)=12 blocks at once and thus in order to execute 600 blocks, we need 600/12=50 of SMs. As 50<56, full computation can fit on a P100 GPU at once. 

![alt text](https://github.com/jbucko/hpc_esc_401/blob/master/exercise_session_11/solutions/cuda/cuda_parallelization_study.jpg)
