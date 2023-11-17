#include "io.h"
#include "init.h"
#include "jacobi.h"
#include "mpi_module.h"

int main(int argc, char *argv[]){


    int size, my_rank;
    // int min_x,max_x,min_y,max_y;

    start_MPI(&my_rank,&size);


    const char* file_name="params.txt";

    // Read the parameter file and store information in a params structure (defined in init.h)
    params p;
    readParameters(file_name, &p);

    // get domain boundaries
    mpi_get_domain(p.nx, p.ny, my_rank, size, &p);

    // // Initialize the matrices used in the Jacobi iteration
    double **f, **u_old, **u_new;

    // // First allocate memory for each matrix
    f = allocateGrid(p.xmax - p.xmin, p.ymax - p.ymin, f);
    u_old = allocateGrid(p.xmax - p.xmin, p.ymax - p.ymin, u_old);
    u_new = allocateGrid(p.xmax - p.xmin, p.ymax - p.ymin, u_new);
    // Initialize the value of matrices
    init_variables(p, f, u_old, u_new);

    // // Output the source term of the Poisson equation in a csv file
    if (p.store_rank0==0) output_source(p, f, my_rank);
    if (p.store_rank0==1) output_source_rank0(p, f, my_rank, size);

    // // Do a first jacobi step
    if (p.communication==3)
        jacobi_step_optimized(p, u_new, u_old, f, my_rank, size);
    else
        jacobi_step(p, u_new, u_old, f, my_rank, size);

    // // Compute differences and norm
    double diff = norm_diff(p, u_new, u_old);

    // // Initialize the Jacobi step conter
    int nstep=1;

    // // Main loop for the Jacobi iterations
    while (diff>p.tol && nstep<p.nstep_max){
        if (my_rank==0) printf("Step %d, Diff=%g\n", nstep, diff);
        if (p.communication==3)
            jacobi_step_optimized(p, u_new, u_old, f, my_rank, size);
        else
            jacobi_step(p, u_new, u_old, f, my_rank, size);
        diff = norm_diff(p, u_new, u_old);
        nstep++;
        
        if (nstep%p.foutput==0 && p.store_rank0 == 0) output(p, nstep, u_new, my_rank);
        if (nstep%p.foutput==0 && p.store_rank0 == 1) output_rank0(p, nstep, u_new, my_rank, size);
    }
    if (my_rank==0) printf("Step %d, Diff=%g\n", nstep, diff);
    // //final output
    if (p.store_rank0==0) output(p, nstep, u_new, my_rank);
    if (p.store_rank0==1) output_rank0(p, nstep, u_new, my_rank, size);

    close_MPI();
    return 0;
}
