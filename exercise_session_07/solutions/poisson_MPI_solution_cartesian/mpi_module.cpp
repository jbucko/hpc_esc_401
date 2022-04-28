#include <mpi.h>
#include <stdio.h>
#include "init.h"

int start_MPI(int* my_rank, int* size){
	printf("Setting MPI environment\n");
	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD,size);
	MPI_Comm_rank(MPI_COMM_WORLD,my_rank);
	return 0;
}

int close_MPI(){
	MPI_Finalize();
	return 0;
}

int mpi_get_domain(params *p, int my_rank, int size){
	/*
	define indexing here
	*/

	//**********************************
	//approach 1
	// *min_y = 0;
	// *max_y = p.ny; //will go up to max_y-1

	// *min_x = p.nx/size*my_rank;
	// if (my_rank == size -1) *max_x = p.nx;
	// else *max_x = nx/size*(my_rank+1);

	//**********************************
	//approach 2
	// *min_y = 0;
	// *max_y = p.ny; //will go up to max_y-1

	// if (my_rank < p.nx%size){
	// 	*min_x = (p.nx/size+1)*my_rank;
	// 	*max_x = (p.nx/size+1)*(my_rank+1);	
	// }
	// else{
	// 	*min_x = p.nx/size*my_rank + p.nx%size;
	// 	*max_x = p.nx/size*(my_rank+1) + p.nx%size;		
	// }


	//**********************************
	//approach 3 - cartesian topology

    int indices[2];
    int dims[2] = {0, 0};

    MPI_Dims_create(size, 2, dims);

    p->nproc_x = dims[0];
    p->nproc_y = dims[1];

    printf("Setting cartesian topology\n");
    MPI_Comm comm_cartesian;
    
    int periods[2]={0,0};
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &comm_cartesian);
    p->comm = comm_cartesian;

    MPI_Comm_rank(p->comm,&my_rank);
    MPI_Cart_coords(p->comm,my_rank,2,indices);

    MPI_Cart_shift(p->comm, 0, 1, &p->left_rank, &p->right_rank);
    MPI_Cart_shift(p->comm, 1, 1, &p->bottom_rank, &p->top_rank);

    printf("I am rank %d, my neighbours are: left: %d, right: %d, bottom: %d, top: %d\n",my_rank,p->left_rank, p->right_rank, p->bottom_rank, p->top_rank);


	if (indices[0] < p->nx%p->nproc_x){
		p->xmin = (p->nx/p->nproc_x+1)*indices[0];
		p->xmax = (p->nx/p->nproc_x+1)*(indices[0]+1);	
	}
	else{
		p->xmin = p->nx/p->nproc_x*indices[0] + p->nx%p->nproc_x;
		p->xmax = p->nx/p->nproc_x*(indices[0]+1) + p->nx%p->nproc_x;		
	}

	if (indices[1] < p->ny%p->nproc_y){
		p->ymin = (p->ny/p->nproc_y+1)*indices[1];
		p->ymax = (p->ny/p->nproc_y+1)*(indices[1]+1);	
	}
	else{
		p->ymin = p->ny/p->nproc_y*indices[1] + p->ny%p->nproc_y;
		p->ymax = p->ny/p->nproc_y*(indices[1]+1) + p->ny%p->nproc_y;		
	}

	printf("I am rank %d and my domain is: xmin, xmax, ymin, ymax: %d %d %d %d\n",my_rank,p->xmin,p->xmax,p->ymin,p->ymax);
	return 0;
}

int halo_comm(params p, int my_rank, int size, double** u, double* fromLeft, double* fromRight, double* fromBottom, double* fromTop){
	for (int j=0;j<(p.ymax - p.ymin);j++) {fromLeft[j] = 0; fromRight[j] = 0;}
    for (int i=0;i<(p.xmax - p.xmin);i++) {fromBottom[i] = 0; fromTop[i] = 0;}


    /*-----------------------------------------------------*/
    /*---------------compose columns manually--------------*/
    /*-----------------------------------------------------*/




	// double* column_to_right = new double [p.ymax - p.ymin];
	// for (int j=0;j<(p.ymax - p.ymin);j++) column_to_right[j] = u[p.xmax - p.xmin - 1][j]; 
	// double* column_to_left = new double [p.ymax - p.ymin];
	// for (int j=0;j<(p.ymax - p.ymin);j++) column_to_left[j] = u[0][j]; 


	// int right_rank = (my_rank + 1)%size;
	// int left_rank = (my_rank +size - 1)%size;

	// MPI_Request request1[2],request2[2];
	
	// MPI_Isend(column_to_right,p.ymax - p.ymin,MPI_DOUBLE,right_rank,0,MPI_COMM_WORLD,&request1[0]); //send to right
	// MPI_Irecv(fromLeft,p.ymax - p.ymin,MPI_DOUBLE,left_rank,0,MPI_COMM_WORLD,&request1[1]); //receive from left

	// MPI_Isend(column_to_left,p.ymax - p.ymin,MPI_DOUBLE,left_rank,0,MPI_COMM_WORLD,&request2[0]); //send to left
	// MPI_Irecv(fromRight,p.ymax - p.ymin,MPI_DOUBLE,right_rank,0,MPI_COMM_WORLD,&request2[1]); //receive from right

	// MPI_Waitall(2,request1,MPI_STATUS_IGNORE);
	// MPI_Waitall(2,request2,MPI_STATUS_IGNORE);

    /*-----------------------------------------------------*/
    /*---------------------MPI_Datatype--------------------*/
    /*-----------------------------------------------------*/


	MPI_Datatype column_type;
    int len_col=p.ymax - p.ymin, stride_col=p.xmax - p.xmin;
    MPI_Type_vector(len_col, 1, stride_col, MPI_DOUBLE, &column_type);
    MPI_Type_commit(&column_type);

	MPI_Datatype row_type;
	int len_row=p.xmax - p.xmin, stride_row=1;
    MPI_Type_vector(len_row, 1, stride_row, MPI_DOUBLE, &row_type);
    MPI_Type_commit(&row_type);

	MPI_Request request[8];
    for (int i=0;i<8;i++) {request[i] = MPI_REQUEST_NULL;}

	// send to right, receive from left 
    if (p.right_rank>=0){
	    MPI_Isend(&u[p.xmax - p.xmin - 1][0],1,column_type,p.right_rank,0,p.comm,&request[0]);
        MPI_Irecv(fromRight,len_col,MPI_DOUBLE,p.right_rank,0,p.comm,&request[1]);
    }

    if (p.left_rank>=0){
        MPI_Isend(&u[0][0],1,column_type,p.left_rank,0,p.comm,&request[2]);
        MPI_Irecv(fromLeft,len_col,MPI_DOUBLE,p.left_rank,0,p.comm,&request[3]);
    }

    if (p.top_rank>=0){
        MPI_Isend(&u[0][0],1,row_type,p.top_rank,0,p.comm,&request[4]);
        MPI_Irecv(fromTop,len_row,MPI_DOUBLE,p.top_rank,0,p.comm,&request[5]);
    }

    if (p.bottom_rank>=0){
        MPI_Isend(&u[0][p.ymax - p.ymin - 1],1,row_type,p.bottom_rank,0,p.comm,&request[6]);
        MPI_Irecv(fromBottom,len_row,MPI_DOUBLE,p.bottom_rank,0,p.comm,&request[7]); 
    }

	MPI_Waitall(8,request,MPI_STATUS_IGNORE);

	MPI_Type_free(&column_type);
	MPI_Type_free(&row_type);


	return 0;
}


int ALLREDUCE(double* loc_diff, double* loc_sumdiff){

	MPI_Allreduce(loc_diff, loc_sumdiff, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	return 0;

	}
int BARRIER(){
	MPI_Barrier(MPI_COMM_WORLD);
	return 0;
}
