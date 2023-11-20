#include "io.h"
#include "init.h"
#include "mpi.h"

void output_source_serial(params p, double** f){
    std::string file_name = "output/output_source_serial.csv";
    std::ofstream ofs(file_name, std::ofstream::out | std::ofstream::trunc);
    for (int i=0; i<p.nx; i++){
        for (int j=0; j<p.ny; j++){
            ofs << f[i][j];
            if (j != p.ny-1)
                ofs <<',';
        } 
        ofs << '\n';
    }
    ofs.close();
}

void output_serial(params p, int step, double** u_new){
    std::string fnum = std::to_string(step);
    fnum.insert(fnum.begin(), 7 - fnum.length(), '0');
    std::string file_name = "output/output_" + fnum + "_serial.csv";
    std::ofstream ofs(file_name, std::ofstream::out | std::ofstream::trunc);
    for (int i=0; i<p.nx; i++){
        for (int j=0; j<p.ny; j++){
            ofs << u_new[i][j];
            if (j != p.ny-1)
                ofs <<',';
        } 
        ofs << '\n';
    }
    ofs.close();
}

/**
 * @brief      Output the exact solution to the Poisson equation in a csv file
 *
 * @param[in]  p        Parameters
 * @param      f        Matrix containing the source term
 */
void output_source(params p, double** f, int rank){
    std::string srank = std::to_string(rank);
    std::string file_name = "output/output_source_"+ srank +".csv";
    std::ofstream ofs(file_name, std::ofstream::out | std::ofstream::trunc);
    for (int i=p.xmin; i<p.xmax; i++){
        for (int j=p.ymin; j<p.ymax; j++){
            ofs << f[i - p.xmin][j - p.ymin];
            if (j != p.ymax-1)
                ofs <<',';
        } 
        ofs << '\n';
    }
    std::cout<<"I am writing output_source\n";
    ofs.close();
}

/**
 * @brief      Output the approximate solution
 *
 * @param[in]  p      Parameters
 * @param[in]  step   The current number of Jacobi steps performed
 * @param      u_new  Matrix containing the approximate solution
 */
void output(params p, int step, double** u_new, int rank){
    std::string fnum = std::to_string(step);
    fnum.insert(fnum.begin(), 7 - fnum.length(), '0');
    std::string srank = std::to_string(rank);
    // std::string file_name = "output/output_" + fnum + ".csv";
    std::string file_name = "output/output_"+ srank +"_" + fnum + ".csv";
    std::ofstream ofs(file_name, std::ofstream::out | std::ofstream::trunc);
    for (int i=p.xmin; i<p.xmax; i++){
        for (int j=p.ymin; j<p.ymax; j++){
            ofs << u_new[i - p.xmin][j - p.ymin];
            // std::cout<<"i:"<<i<<" j: "<<j<<" unewij: "<<u_new[i][j]<<"\n";
            if (j != p.ymax-1)
                ofs <<',';
        } 
        ofs << '\n';
    }
    ofs.close();
    std::cout<<"printing...\n";
}

/**
 * @brief      Collect the data from all processes and write it to a single file
 *
 * @param[in]  p        Parameters
 * @param      step     The current number of Jacobi steps performed
 * @param      u_new    Matrix containing the approximate solution on the local subgrid of the current process
 * @param[in]  rank     The rank of the current process
 * @param[in]  size     The total number of processes
 */
void output_rank0(params p, int step, double** u_new, int rank, int size){
    // Define a new 1D array to collect all elements of a local u_new subgrid in a single array of size (p.xmax-p.xmin)*(p.xmax-p.xmin)
    double* u_new_linear = new double[(p.xmax-p.xmin)*(p.ymax-p.ymin)];

    // Collect the elements of a local subgrid u_new into u_new_linear
    int linear_index = 0;
    for (int i=p.xmin; i<p.xmax; i++){
        for (int j=p.ymin; j<p.ymax; j++){
            u_new_linear[linear_index] = u_new[i - p.xmin][j - p.ymin];
            linear_index++;
        }
    }

    // Calculate the total number of elements that will be gathered
    int total_elements = (p.xmax - p.xmin)*(p.ymax - p.ymin);
    
    // create a custom MPI datatype. This will easily allow us to send vectors of different length in a single MPI_Gatherv command
    MPI_Datatype stype;
    MPI_Type_vector(total_elements, 1, 1, MPI_DOUBLE, &stype); 
    MPI_Type_commit( &stype );
    double* u_new_combined_linear; // new combined linear array
    double** u_new_combined; // new combined 2D array/matrix
    int* recvcounts; // array storing the lengths of linearized subgrids used by MPI_Gatherv command
    
    // Collect the information about the number of elements of u_new_linear to be sent to rank 0 later
    recvcounts = new int[size];
    MPI_Gather(&total_elements,1,MPI_INTEGER,recvcounts,1,MPI_INTEGER,0,MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (rank == 0){
        // define the displacements at rank0, at which the incoming lienarized arrays will be stored
        int* displs = new int[size];
        displs[0] = 0;
        for (int i=1;i<size;i++){
            displs[i] = displs[i-1] + recvcounts[i-1];
        }
        // Gather the data from all processes in u_new_combined_linear on the root process
        u_new_combined_linear = new double[p.nx*p.ny];
        MPI_Gatherv(u_new_linear, 1, stype, u_new_combined_linear, recvcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // Free the memory
        delete[] u_new_linear;
        delete[] recvcounts;
        delete[] displs;
    }
    else{
        // Gatherv command for all the ranks except for rank0 - observe the NULL's in the call, this are info specific to and defined only for rank0
        MPI_Gatherv(u_new_linear, 1, stype, NULL, NULL, NULL, NULL, 0, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Collect the linear data in f_combined_linear in a 2D array f_combined
    if (rank == 0){

        u_new_combined = allocateGrid(p.nx, p.ny, u_new_combined);
        for (int i=0; i<p.nx; i++){
            for (int j=0; j<p.ny; j++){
                u_new_combined[i][j] = u_new_combined_linear[i*p.ny + j];
            }
        }

        // Write the data to file on the root process
        output_serial(p, step, u_new_combined);

        // free the memory allocated for f_combined
        for (int i=0; i<p.nx; i++){
            delete[] u_new_combined[i];
        }
        delete[] u_new_combined;
        delete[] u_new_combined_linear;
    }
}

/**
 * @brief      Collect the source data from all processes and write it to a single file
 *
 * @param[in]  p        Parameters
 * @param      f        Matrix containing the source term on the local subgrid of the current process
 * @param[in]  rank     The rank of the current process
 * @param[in]  size     The total number of processes
 */
void output_source_rank0(params p, double** f, int rank, int size){
    // Define a new 1D array to collect all elements of a local f subgrid in a single array of size (p.xmax-p.xmin)*(p.xmax-p.xmin)
    double* f_linear = new double[(p.xmax-p.xmin)*(p.ymax-p.ymin)];

    // Collect the elements of f in f_linear
    int linear_index = 0;
    for (int i=p.xmin; i<p.xmax; i++){
        for (int j=p.ymin; j<p.ymax; j++){
            f_linear[linear_index] = f[i - p.xmin][j - p.ymin];
            linear_index++;
        }
    }
    // Calculate the total number of elements that will be gathered
    int total_elements = (p.xmax - p.xmin)*(p.ymax - p.ymin);
    
    // create a custom MPI datatype. This will easily allow us to send vectors of different length in a single MPI_Gatherv command
    MPI_Datatype stype;
    MPI_Type_vector(total_elements, 1, 1, MPI_DOUBLE, &stype); 
    MPI_Type_commit( &stype );
    double** f_combined; // new combined linear array
    double* f_combined_linear; // new combined 2D array/matrix
    int* recvcounts; // array storing the lengths of linearized subgrids used by MPI_Gatherv command
    
    // Collect the data from all processes in f_combined_linear on the root process
    recvcounts = new int[size];
    MPI_Gather(&total_elements,1,MPI_INTEGER,recvcounts,1,MPI_INTEGER,0,MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0){
        // define the displacements at rank0, at which the incoming lienarized arrays will be stored
        int* displs = new int[size];
        displs[0] = 0;
        for (int i=1;i<size;i++){
            displs[i] = displs[i-1] + recvcounts[i-1];
        }
        // Gather the data from all processes in f_combined_linear on the root process
        f_combined_linear = new double[p.nx*p.ny];
        MPI_Gatherv(f_linear, 1, stype, f_combined_linear, recvcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // Free the memory
        delete[] f_linear;
        delete[] recvcounts;
        delete[] displs;
    }
    else{
        // Gatherv command for all the ranks except for rank0 - observe the NULL's in the call, this are info specific to and defined only for rank0
        MPI_Gatherv(f_linear, 1, stype, NULL, NULL, NULL, NULL, 0, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    // Collect the linear data in f_combined_linear in a 2D array f_combined
    if (rank == 0){
        f_combined = allocateGrid(p.nx, p.ny, f_combined);
        for (int i=0; i<p.nx; i++){
            for (int j=0; j<p.ny; j++){
                f_combined[i][j] = f_combined_linear[i*p.ny + j];
            }
        }

        // Write the data to file on the root process
        output_source_serial(p,f_combined);

        // free the memory allocated for f_combined
        for (int i=0; i<p.nx; i++){
            delete[] f_combined[i];
        }
        delete[] f_combined;
        delete[] f_combined_linear;
    }
}