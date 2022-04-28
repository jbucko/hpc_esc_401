#ifndef MPI_MODULE
#define MPI_MODULE


// #include "init.h"

int start_MPI(int* my_rank, int* size);
int close_MPI();
int mpi_get_domain(params *p, int my_rank, int size);
int halo_comm(params p, int my_rank, int size, double** u, double* fromLeft, double* fromRight, double* fromBottom, double* fromTop);
int ALLREDUCE(double* loc_diff, double* loc_sumdiff);
int BARRIER();
int create_cartesian(params p,int my_rank,int* indices, int* left_rank, int* right_rank, int* top_rank, int* bottom_rank);

#endif