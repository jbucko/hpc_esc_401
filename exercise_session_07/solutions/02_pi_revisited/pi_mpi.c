#include "mpi.h"
#include <stdio.h>
#include <math.h>


int main(int argc, char** argv) {
    int N = 1000000000;
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes and rank of the process
    int size, my_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    double local_sum=0.;
    // double val=1.;
    int pair;
    for(int i=my_rank; i<N; i+=size){
        pair = i%2;
        // val = val*partial_inv_factorial(i, i-size);
        local_sum += pow(-1,pair)/((double) (2*i + 1));
        //pair  = (pair + 1)%2;
    }
    double global_sum=0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        if (my_rank==0){
            printf("calculated pi=%.20g\n", 4*global_sum);
            printf("true pi=%.20g\n", 3.1415926535897932384);
            printf("calculated pi - true pi=%.20g\n", 4*global_sum -3.1415926535897932384);
        }
    // Finalize the MPI environment.
    MPI_Finalize();
}
