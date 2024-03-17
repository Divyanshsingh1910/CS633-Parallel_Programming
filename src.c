#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[]) 
{
	int myrank,size;
	// initialize MPI
	MPI_Init (&argc, &argv);
	
	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

  	/* Command line arguments */
	int Px = 3, Py = 4, P = 12;
	int N = 512*512;
	int stencil = 5; // or 9
	int num_time_steps = 5; 
	int seed = 42;

	int rows = sqrt(N), cols = rows;
	

	double** data = (double **)malloc(rows*sizeof(double*));
	
	for(int i=0; i<rows; i++){
		data[i] = (double *)malloc(cols*sizeof(double));
	}

	for(int i=0; i<rows; i++){
		for(int j=0; j<cols; j++){
			srand(seed*(myrank + 10));
			data[i][j] = abs(rand() + (i*rand() + j*myrank))/100;
		}
	}

	//printf("ex: %f,%f\n",data[56][89],data[464][46]);
	/* Communicated data */
	double from_left[rows];
	double from_right[rows];

	double from_top[cols];
	double from_bottom[cols];


along_x_communication:
		
	/* every even-process sends to right neighbour && odd-process sends to left*/
	for(int i=0; i<rows; i++){
		switch((myrank%Px) % 2){
		
		case(0):
			if(myrank%Px != Px-1){
				/* Sending to right */
				MPI_Send(&data[i][cols-1], 1 , MPI_DOUBLE, myrank+1 , i,MPI_COMM_WORLD );

				/* Receiving from right*/
				MPI_Recv(&from_right[i], 1 , MPI_DOUBLE , myrank+1 , i , MPI_COMM_WORLD, &status);
			}
			break;

		case(1):
			if(myrank%Px != 0){
				/* Receiving from left */
				MPI_Recv(&from_left[i], 1 , MPI_DOUBLE, myrank-1 , i , MPI_COMM_WORLD, &status);

				/* Sending to left */
				MPI_Send(&data[i][0], 1 , MPI_DOUBLE, myrank-1 , i,MPI_COMM_WORLD );
			}
			break;
		}
	}

	/* every even-process sends to left neighbour && odd-process sends to right */
	for(int i=0; i<rows; i++){
		switch((myrank%Px) % 2){
		
		case(0):
			if(myrank%Px != 0){
				/* Sending to left */
				MPI_Send(&data[i][0], 1 , MPI_DOUBLE, myrank-1 , i,MPI_COMM_WORLD );

				/* Receiving from left*/
				MPI_Recv(&from_left[i], 1 , MPI_DOUBLE , myrank-1 , i , MPI_COMM_WORLD, &status);
			}
			break;

		case(1):
			if(myrank%Px != Px-1){
				/* Receiving from right */
				MPI_Recv(&from_right[i], 1 , MPI_DOUBLE, myrank+1 , i , MPI_COMM_WORLD, &status);

				/* Sending to right */
				MPI_Send(&data[i][cols-1], 1 , MPI_DOUBLE, myrank+1 , i , MPI_COMM_WORLD);
			}
			break;
		}
	}

along_y_communication:
	
	/* Here the parity is checked by
	 * division from Px
	 * So, 			0 , top most
	 *        	   /
	 * myrank/Px =
	 *			   \
	 *				Py-1, bottom most
	 *
	 * every even-process sends to below && odd-process sends to above */
	for(int j=0; j<cols; j++){
		switch( (myrank/Px) % 2 ){
		
		case(0):
			if(myrank/Px != Py-1){
				/* Sending to below */
				MPI_Send(&data[rows-1][j], 1 , MPI_DOUBLE, myrank+Px , j ,MPI_COMM_WORLD );

				/* Receiving from below */
				MPI_Recv(&from_bottom[j], 1 , MPI_DOUBLE , myrank+Px , j , MPI_COMM_WORLD, &status);
			}
			break;

		case(1):
			if(myrank/Px != 0){
				/* Receiving from top */
				MPI_Recv(&from_top[j], 1 , MPI_DOUBLE, myrank-Px , j , MPI_COMM_WORLD, &status);

				/* Sending to top */
				MPI_Send(&data[0][j], 1 , MPI_DOUBLE, myrank-Px , j , MPI_COMM_WORLD);
			}
			break;
		}
	}

	/* every even-process sends to top && odd-process sends to below*/
	for(int j=0; j<cols; j++){
		switch( (myrank/Px) % 2 ){
		
		case(0):
			if(myrank/Px != 0){
				/* Sending to top */
				MPI_Send(&data[0][j], 1 , MPI_DOUBLE, myrank-Px , j ,MPI_COMM_WORLD );

				/* Receiving from top */
				MPI_Recv(&from_bottom[j], 1 , MPI_DOUBLE , myrank-Px , j , MPI_COMM_WORLD, &status);
			}
			break;

		case(1):
			if(myrank/Px != Py-1){
				/* Receiving from below */
				MPI_Recv(&from_bottom[j], 1 , MPI_DOUBLE, myrank+Px , j , MPI_COMM_WORLD, &status);

				/* Sending to below */
				MPI_Send(&data[rows-1][j], 1 , MPI_DOUBLE, myrank+Px , j , MPI_COMM_WORLD);
			}
			break;
		}
	}

	
	/* */

	
	
  	// done with MPI
  	MPI_Finalize();
	return 0;
}

