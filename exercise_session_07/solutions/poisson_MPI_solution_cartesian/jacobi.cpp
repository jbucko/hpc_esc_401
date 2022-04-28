#include "jacobi.h"
#include <math.h>
#include "mpi_module.h"

/**
 * @brief      Computes norm of the difference between two matrices
 *
 * @param[in]  p     Parameters
 * @param      mat1  The matrix 1
 * @param      mat2  The matrix 2
 *
 * @return     Returns \sqrt{\sum(mat1_{ij} - mat2_{ij})^2/(nx*ny)}
 */
double norm_diff(params p, double** mat1, double** mat2){
    double sum = 0.0;
    for (int i=p.xmin; i<p.xmax; i++){
    for (int j=p.ymin; j<p.ymax; j++){
        int idx = i - p.xmin;
        int idy = j- p.ymin;

        sum += (mat1[idx][idy] - mat2[idx][idy])*(mat1[idx][idy] - mat2[idx][idy]);
    }
    }
    // printf("Local sum %g\n",sum);
    double total_sum;
    ALLREDUCE(&sum,&total_sum);
    total_sum /= p.nx*p.ny;
    total_sum = sqrt(total_sum);
    // printf("Function norm_diff (jacobi.cpp l.12): not implemented.\n");
    return total_sum;
}

/**
 * @brief      Compute a Jacobi iteration to solve Poisson equation. This function updates
 *             u_new and u_old in-place.
 *
 * @param[in]  p      Parameters
 * @param      u_new  The new solution
 * @param      u_old  The old solution
 * @param      f      The source term
 */
void jacobi_step(params p, double** u_new, double** u_old, double** f, int my_rank, int size){
    double dx = 1.0/((double)p.nx - 1);
    double dy = 1.0/((double)p.ny - 1);
    // double temp;

    double* fromLeft = new double[p.ymax - p.ymin]; 
    double* fromRight = new double[p.ymax - p.ymin];

    double* fromBottom = new double[p.xmax - p.xmin]; 
    double* fromTop = new double[p.xmax - p.xmin];

    for (int i=p.xmin; i<p.xmax; i++){
        for (int j=p.ymin; j<p.ymax; j++){
            u_old[i - p.xmin][j - p.ymin] = u_new[i - p.xmin][j - p.ymin];
        }
    }

    halo_comm(p, my_rank, size, u_new, fromLeft, fromRight, fromBottom, fromTop); 

    double top, bottom, left, right;
    for (int i=p.xmin; i<p.xmax; i++){
        if (i==0 || i==p.nx-1) continue;
        for (int j=p.ymin; j<p.ymax; j++){
            if (j==0 || j==p.ny-1) continue;
            int idx = i-p.xmin;
            int idy = j-p.ymin;

            if (i==p.xmin) left=fromLeft[idy];
            else left=u_old[idx-1][idy];

            if (i==p.xmax-1) right=fromRight[idy];
            else right=u_old[idx+1][idy];

            if (j==p.ymin) bottom=fromBottom[idx];
            else bottom=u_old[idx][idy-1];        

            if (j==p.ymax-1) top=fromTop[idx];
            else top=u_old[idx][idy+1];     

            u_new[idx][idy] = 0.25*(left + right + bottom + top - dx*dy*f[idx][idy]);
        }
    }
    //if (p.nx!=p.ny) printf("In function jacobi_step (jacobi.cpp l.26): nx != ny, check jacobi updates\n");
}
