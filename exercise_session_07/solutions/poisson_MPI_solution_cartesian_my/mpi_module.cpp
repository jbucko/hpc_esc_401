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

int create_cartesian(params *p,int my_rank, int* indices, int* left_rank, int* right_rank, int* top_rank, int* bottom_rank){
	printf("Setting cartesian topology\n");
	
	MPI_Comm comm_cartesian;
	// int indices1[2];
    int ndims=2;
    int reorder=1;
    int dim_size[2]; dim_size[0]=p->nproc_x; dim_size[1]=p->nproc_y;
    int periods[2]; periods[0]=0; periods[1]=0;
    MPI_Cart_create(MPI_COMM_WORLD, ndims, dim_size, periods, reorder, &comm_cartesian);
    int my_rank_cart;
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank_cart);
    MPI_Cart_coords(comm_cartesian,my_rank_cart,2,indices);
    printf("My old rank: %d, new rank in cart: %d and indices: [%d, %d]\n",my_rank,my_rank_cart,indices[0],indices[1]);

 //    MPI_Cart_get(comm_cartesian, 2, dim_size, periods, indices1);
	// printf("My old rank: %d, new rank in cart: %d and indices: [%d, %d]\n",my_rank,my_rank_cart,indices1[0],indices1[1]);

	// MPI_Cart_shift(comm_cartesian, 1, 1, left_rank, right_rank);
	// MPI_Cart_shift(comm_cartesian, 0, 1, bottom_rank, top_rank);
	MPI_Cart_shift(comm_cartesian, 0, 1, left_rank, right_rank);
	MPI_Cart_shift(comm_cartesian, 1, 1, bottom_rank, top_rank);

	printf("I am rank %d, my neighbours are: left: %d, right: %d, bottom: %d, top: %d\n",my_rank_cart,*left_rank, *right_rank, *bottom_rank, *top_rank);
	p->comm = comm_cartesian;
	return 0;
}

int close_MPI(){
	MPI_Finalize();
	return 0;
}

int mpi_get_domain(params p, int my_rank, int size, int* indices, int* min_x, int* max_x, int* min_y, int* max_y){
	/*
	define indexing here
	*/

	//**********************************
	//approach 1

	// *min_y = p.ny/p.nproc_y*indices[1];
	// if (indices[1] == p.nproc_y -1) *max_y = p.ny;
	// else *max_y = p.ny/p.nproc_y*(indices[1]+1);

	// *min_x = p.nx/p.nproc_x*indices[0];
	// if (indices[0] == p.nproc_x -1) *max_x = p.nx;
	// else *max_x = p.nx/p.nproc_x*(indices[0]+1);

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

    printf("New rank in cart: %d and indices: [%d, %d]\n",my_rank,indices[0],indices[1]);

	if (indices[0] < p.nx%p.nproc_x){
		*min_x = (p.nx/p.nproc_x+1)*indices[0];
		*max_x = (p.nx/p.nproc_x+1)*(indices[0]+1);	
	}
	else{
		*min_x = p.nx/p.nproc_x*indices[0] + p.nx%p.nproc_x;
		*max_x = p.nx/p.nproc_x*(indices[0]+1) + p.nx%p.nproc_x;		
	}

	if (indices[1] < p.ny%p.nproc_y){
		*min_y = (p.ny/p.nproc_y+1)*indices[1];
		*max_y = (p.ny/p.nproc_y+1)*(indices[1]+1);	
	}
	else{
		*min_y = p.ny/p.nproc_y*indices[1] + p.ny%p.nproc_y;
		*max_y = p.ny/p.nproc_y*(indices[1]+1) + p.ny%p.nproc_y;		
	}
	

	printf("I am rank %d and my domain is: xmin, xmax, ymin, ymax: %d %d %d %d\n\n",my_rank,*min_x,*max_x,*min_y,*max_y);
	return 0;
}

int halo_comm(params p, int my_rank, int size, double** u, double* fromLeft, double* fromRight, double* fromBottom, double* fromTop){
	for (int j=0;j<(p.ymax - p.ymin);j++) {fromLeft[j] = 0; fromRight[j] = 0;}
		for (int j=0;j<(p.xmax - p.xmin);j++) {fromBottom[j] = 0; fromTop[j] = 0;}


    /*-----------------------------------------------------*/
    /*---------------compose columns manually--------------*/
    /*-----------------------------------------------------*/

	// int len_col=p.ymax - p.ymin, stride_col=1;
	// int len_row=p.xmax - p.xmin, stride_row=p.ymax - p.ymin;

	double* column_to_right = new double [p.ymax - p.ymin];
	for (int j=0;j<(p.ymax - p.ymin);j++) column_to_right[j] = u[p.xmax - p.xmin - 1][j]; 
	double* column_to_left = new double [p.ymax - p.ymin];
	for (int j=0;j<(p.ymax - p.ymin);j++) column_to_left[j] = u[0][j]; 

	double* row_to_top = new double [p.xmax - p.xmin];
	for (int j=0;j<(p.xmax - p.xmin);j++) row_to_top[j] = u[j][p.ymax - p.ymin - 1]; 
	double* row_to_bottom = new double [p.xmax - p.xmin];
	for (int j=0;j<(p.xmax - p.xmin);j++) row_to_bottom[j] = u[j][0]; 

	// MPI_Request request[8];
	// for (int i=0;i<8;i++) {request[i] = MPI_REQUEST_NULL;}

	// // send to right, receive from right 
	// if (p.right_rank>=0){
	// MPI_Isend(column_to_right,len_col,MPI_DOUBLE,p.right_rank,0,p.comm,&request[0]); //send to right
	// MPI_Irecv(fromRight,len_col,MPI_DOUBLE,p.right_rank,0,p.comm,&request[1]); //receive from right
	// }
	

	// // //send to left, receive from left
	// if (p.left_rank>=0) {
	// MPI_Isend(column_to_left,len_col,MPI_DOUBLE,p.left_rank,0,p.comm,&request[2]); //send to left
 //    MPI_Irecv(fromLeft,len_col,MPI_DOUBLE,p.left_rank,0,p.comm,&request[3]); //receive from left
	// }
   
 //    // send to top, receive from top
	// if (p.top_rank>=0) {
	// MPI_Isend(row_to_top,len_row,MPI_DOUBLE,p.top_rank,0,p.comm,&request[4]); //send to top
	// MPI_Irecv(fromTop,len_row,MPI_DOUBLE,p.top_rank,0,p.comm,&request[5]); //receive from top
 //    }

 // //    //send to bottom, receive from bottom
 //    if (p.bottom_rank>=0) {
 //    MPI_Isend(row_to_bottom,len_row,MPI_DOUBLE,p.bottom_rank,0,p.comm,&request[6]); //send to bottom
 //    MPI_Irecv(fromBottom,len_row,MPI_DOUBLE,p.bottom_rank,0,p.comm,&request[7]); //receive from bottom
	// }


	// MPI_Waitall(8,request,MPI_STATUS_IGNORE);

    /*-----------------------------------------------------*/
    /*---------------------MPI_Datatype--------------------*/
    /*-----------------------------------------------------*/


	MPI_Datatype column_type;
	int len_col=p.ymax - p.ymin, stride_col=1;
    MPI_Type_vector(len_col, 1, stride_col, MPI_DOUBLE, &column_type);
    MPI_Type_commit(&column_type);

	MPI_Datatype row_type;
	int len_row=p.xmax - p.xmin, stride_row=p.ymax - p.ymin;
	// if (my_rank==0 || my_rank ==0) stride_row = 42;
	// if (my_rank==3 || my_rank ==3) stride_row = 42;

	// int len_row=p.xmax - p.xmin, stride_row=40;
    MPI_Type_vector(len_row, 1, stride_row, MPI_DOUBLE, &row_type);
    MPI_Type_commit(&row_type);



	MPI_Request request[8];
	for (int i=0;i<8;i++) {request[i] = MPI_REQUEST_NULL;}
	MPI_Request request1[2],request2[2],request3[2],request4[2];

	// send to right, receive from right 
	if (p.right_rank>=0){
	MPI_Isend(u[p.xmax - p.xmin - 1],1,column_type,p.right_rank,0,p.comm,&request[0]); //send to right
	MPI_Irecv(fromRight,len_col,MPI_DOUBLE,p.right_rank,0,p.comm,&request[1]); //receive from right
	}
	

	// //send to left, receive from left
	if (p.left_rank>=0) {
	MPI_Isend(u[0],1,column_type,p.left_rank,0,p.comm,&request[2]); //send to left
    MPI_Irecv(fromLeft,len_col,MPI_DOUBLE,p.left_rank,0,p.comm,&request[3]); //receive from left
	}
   
    // send to top, receive from top
	if (p.top_rank>=0) {
	// MPI_Isend(row_to_top,len_row,MPI_DOUBLE,p.top_rank,0,p.comm,&request[4]); //send to top
	MPI_Isend(&u[0][p.ymax - p.ymin - 1],1,row_type,p.top_rank,0,p.comm,&request[4]); //send to top
	MPI_Irecv(fromTop,len_row,MPI_DOUBLE,p.top_rank,0,p.comm,&request[5]); //receive from top
    }

 //    //send to bottom, receive from bottom
    if (p.bottom_rank>=0) {
	// MPI_Isend(row_to_bottom,len_row,MPI_DOUBLE,p.bottom_rank,0,p.comm,&request[6]);
    MPI_Isend(&u[0][0],1,row_type,p.bottom_rank,0,p.comm,&request[6]); //send to bottom
    MPI_Irecv(fromBottom,len_row,MPI_DOUBLE,p.bottom_rank,0,p.comm,&request[7]); //receive from bottom
	}

    // printf("I am rank %d, before Waitall\n",my_rank);

	MPI_Waitall(8,request,MPI_STATUS_IGNORE);

	// if (my_rank == 1) for (int k=0;k<161;k++){
	// 	printf("I am rank %d, receiveing from bottom:k: %d, val:  %g\n",my_rank,k,fromBottom[k]);
	// }
	// if (my_rank == 0) for (int k=0;k<161;k++){
	// 	printf("I am rank %d, should send to top:k: %d, val: %g\n",my_rank,k,row_to_top[k]);
	// }
	// if (my_rank==0) for (int k=0;k<161*41;k++) {
	// 	printf("col41: %g\n",u[0][40]);
	// 	printf("col42: %g\n",u[0][41]);
	// 	printf("col43: %g\n",u[0][421]);
	// 	printf("%g\n",*(*u + k));
	// 	if (k%41==0) printf("\nrow %d:\n",k/41);
	// }
	// printf("My rank %d, size of u: %lu\n",my_rank, sizeof(u)/sizeof(double));
	printf("I am rank %d, col_length: %d, row_length: %d, column stride: %d, row stride: %d, sending to top from: u[%d,%d] \n",my_rank,len_col,len_row,stride_col, stride_row ,0,p.ymax - p.ymin - 1);
	// if (my_rank == 0) for (int k=0;k<161;k++){
	// 	printf("I am rank %d, sending to top: %g, I should send %g\n",my_rank,*(u[k]+p.ymax - p.ymin - 1),row_to_top[k]);
	// }
	// if (my_rank == 0) printf("I am rank %d, my neighbours: left: %d, right: %d,bottom: %d,top: %d\n",my_rank,p.left_rank,p.right_rank,p.bottom_rank,p.top_rank);

    // if (my_rank==0) {for (int k=0;k<161;k++) printf("%d: sent to top: u[k][p.ymax - p.ymin - 1]: %g\n",k,u[k][p.ymax - p.ymin - 1]);}
    // if (my_rank==1) {for (int k=0;k<161;k++) printf("%d: u[k][0]: %g\n",k,u[k][0]);}
    // if (my_rank==1) {for (int k=0;k<161;k++) printf("%d: fromTop[k]: %g\n",k,fromTop[k]);}
	// MPI_Waitall(2,request1,MPI_STATUS_IGNORE);
	// printf("I am rank %d, after Waitall1\n",my_rank);
	// MPI_Waitall(2,request2,MPI_STATUS_IGNORE);
	// printf("I am rank %d, after Waitall2\n",my_rank);
	// MPI_Waitall(2,request3,MPI_STATUS_IGNORE);
	// printf("I am rank %d, after Waitall3\n",my_rank);
	// MPI_Waitall(2,request4,MPI_STATUS_IGNORE);
	// printf("I am rank %d, after Waitall4\n",my_rank);

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
