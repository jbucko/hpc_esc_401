#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Arrays of status/request for non-blocking communication
    MPI_Status status[2];
    MPI_Request request[2];

    // Get the number of processes and rank of the process
    int size, my_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Create the Cartesian communicator
    MPI_Comm comm;
    int ndims=1;
    int reorder=1;
    int dim_size[1]; dim_size[0]=size;
    int periods[1]; periods[0]=1;
    MPI_Cart_create(MPI_COMM_WORLD, ndims, dim_size, periods, reorder, &comm);

    int my_sum = 0;

    int send_rank = my_rank;  // Send    buffer
    int recv_rank = 0;        // Receive buffer

    // Compute the ranks of left/right neighbours 
    int left_rank, right_rank;
    MPI_Cart_shift(comm, 0, 1, &left_rank, &right_rank);
    printf("I am rank %d and left neighbour is %d, right is %d\n", my_rank, left_rank, right_rank);

    for(int i=0; i<size; i++){
        MPI_Isend(&send_rank, 1, MPI_INT, right_rank, 0, MPI_COMM_WORLD, &request[0]);       
        MPI_Irecv(&recv_rank, 1, MPI_INT, left_rank, 0, MPI_COMM_WORLD, &request[1]);
        MPI_Waitall(2, request, status); 
        send_rank = recv_rank;
        my_sum += recv_rank;       
    }

    printf("I am processor %d out of %d, and the sum is %d\n", my_rank, size, my_sum);

    // Finalize the MPI environment.
    MPI_Finalize();
}
