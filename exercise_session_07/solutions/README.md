# Exercise 1: Ring and deadlocks

## Blocking communication

Using the blocking send and receive, you will observe that your code gets stuck after running. This indicates you encountered a deadlock. The respective implementation can be found in `/01_ring_and_deadlocks/ring_deadlock.c`, pasted below:

```C
#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes and rank of the process
    int size, my_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int my_sum = 0;

    int send_rank = my_rank;  // Send    buffer
    int recv_rank = 0;        // Receive buffer

    // Compute the ranks of left/right neighbours 
    int left_rank  = (my_rank+size-1)%size;
    int right_rank = (my_rank+1)%size;

    for(int i=0; i<size; i++){
        MPI_Ssend(&send_rank, 1, MPI_INT, right_rank, 0, MPI_COMM_WORLD);
        MPI_Recv(&recv_rank, 1, MPI_INT, left_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        send_rank = recv_rank;
        my_sum += recv_rank;       
    }

    printf("I am processor %d out of %d, and the sum is %d\n", my_rank, size, my_sum);

    // Finalize the MPI environment.
    MPI_Finalize();
}
```

The possible way to overcome this and still use blocking communication is to divide the ranks into even and odd and change the order of send and receive instructions as below (full code in `/01_ring_and_deadlocks/ring_split.c`)

```C
for(int i=0; i<size; i++){
    if(my_rank%2==0){
        MPI_Ssend(&send_rank, 1, MPI_INT, right_rank, 0, MPI_COMM_WORLD);
        MPI_Recv(&recv_rank, 1, MPI_INT, left_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    else {
        MPI_Recv(&recv_rank, 1, MPI_INT, left_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);    
        MPI_Ssend(&send_rank, 1, MPI_INT, right_rank, 0, MPI_COMM_WORLD);        
    }
    send_rank = recv_rank;
    my_sum += recv_rank;       
}
```

## Non-blocking communication

The complete implemetation can be found in `/01_ring_and_deadlocks/ring_nonblocking.c`. Here, you do not need to divide the processes into groups, but you need to wait for each round of communication to complete. You need to define arrays of requests and statutes before the for loop:

```C
MPI_Status status[2];
MPI_Request request[2];
```

and then modify the for loop as follows (observe the `Waitall` command:

```C
for(int i=0; i<size; i++){
    MPI_Isend(&send_rank, 1, MPI_INT, right_rank, 0, MPI_COMM_WORLD, &request[0]);       
    MPI_Irecv(&recv_rank, 1, MPI_INT, left_rank, 0, MPI_COMM_WORLD, &request[1]);
    MPI_Waitall(2, request, status); 
    send_rank = recv_rank;
    my_sum += recv_rank;
}
```
The status of the communication is returned (or `MPI_STATUSES_IGNORE` can be used instead of `status` in `Waitall` if one does not care about the status information).

## Cartesian communicator

Again, a complete code is to be found in `/01_ring_and_deadlocks/ring_cartesian.c`. Let us modify the code from above (with non-blokcing communication) in order to use new cartesian communicator. After initializing the MPI environment and infering `size` and `my_rank`, let us define new communicator called `comm`:

```C
// Create the Cartesian communicator
MPI_Comm comm;
int ndims=1;
int reorder=1;
int dim_size[1]; dim_size[0]=size;
int periods[1]; periods[0]=1;
MPI_Cart_create(MPI_COMM_WORLD, ndims, dim_size, periods, reorder, &comm);
```

We defined periodic boundaries so the rightmost rank is a neighbour of leftmost one. The defined topology is 1-dimensional and has `size` ranks. Then we can take advantage of cartesian topology to calculate the left and right neighbours as 

```C
MPI_Cart_shift(comm, 0, 1, &left_rank, &right_rank);
```
Rest of the code remains the same as in `ring_nonblocking.c`, however, the communicator used is not `MPI_COMM_WORLD`, but `comm` instead:

```C
for(int i=0; i<size; i++){
    MPI_Isend(&send_rank, 1, MPI_INT, right_rank, 0, comm, &request[0]);       
    MPI_Irecv(&recv_rank, 1, MPI_INT, left_rank, 0, comm, &request[1]);
    MPI_Waitall(2, request, status); 
    send_rank = recv_rank;
    my_sum += recv_rank;
}
```
