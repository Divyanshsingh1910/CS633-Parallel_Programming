#include <stdio.h>
#include "mpi.h"
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

int 	cols, rows, P, Px, Py, myrank;
double	**data;
double	*from_left, *from_right, *from_top, *from_bottom;
bool    has_left_neighbour = true, has_right_neighbour = true, has_top_neighbour = true, has_bottom_neighbour = true;

void fill_has_neighbours()
{
    if(myrank%Px == 0) // 0, 3, 6, 9
        has_left_neighbour = false;
    if(myrank%Px == Px - 1) // 2, 5, 8, 11
        has_right_neighbour = false;
    if(myrank/Px == 0) // 0, 1, 2
        has_top_neighbour = false;
    if(myrank/Px == Py - 1) // 9, 10, 11
        has_bottom_neighbour = false;
}

double get_val(int i, int j)
{
	return (i >= 0 && i < rows && j >= 0 && j < cols) ? data[i][j] : 0;
}

void compute(int i, int j)
{
    double sum = 0;
	int nneighbours = 4;
    if(i == 0){
		if(has_top_neighbour)
			sum += from_top[j];
		else
			nneighbours--;
    }
    if(i == rows - 1){
		if(has_bottom_neighbour)
			sum += from_bottom[j];
        else
			nneighbours--;
	}
    if(j == 0){
		if(has_left_neighbour)
			sum += from_left[i];
		else
        	nneighbours--;
	}
    if(j == cols - 1){
		if(has_right_neighbour)
			sum += from_right[i];
		else
        	nneighbours--;
	}
	data[i][j] = (data[i][j] + sum + get_val(i, j - 1) + get_val(i, j + 1) + get_val(i - 1, j) + get_val(i + 1, j))/(nneighbours + 1);
}

int main(int argc, char *argv[]) 
{
	// initialize MPI
	MPI_Init (&argc, &argv);
	
	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);


  	/* Command line arguments */
	Px = 3, Py = 4;
	P = 12;
	int N = 512*512;
	int stencil = 5; // or 9
	int num_time_steps = 5; 
	int seed = 42;

	rows = sqrt(N), cols = rows;
	
	fill_has_neighbours();

	data = (double **)malloc(rows*sizeof(double*));
	
	for(int i=0; i<rows; i++){
		data[i] = (double *)malloc(cols*sizeof(double));
	}

	for(int i=0; i<rows; i++){
		for(int j=0; j<cols; j++){
			srand(seed*(myrank + 10));
			data[i][j] = abs(rand() + (i*rand() + j*myrank))/100;
		}
	}

	/* Communicated data */
	from_left = has_left_neighbour? (double*)malloc(rows*sizeof(double)):NULL;
	from_right = has_right_neighbour? (double*)malloc(rows*sizeof(double)):NULL;

	from_top = has_top_neighbour? (double*)malloc(cols*sizeof(double)):NULL;
	from_bottom = has_bottom_neighbour? (double*)malloc(cols*sizeof(double)):NULL;

along_x_communication:
		
	/* every even-process sends to right neighbour && odd-process sends to left*/
	for(int i=0; i<rows; i++){
		switch((myrank%Px) % 2){
		
		case(0):
			if(has_right_neighbour){
				/* Sending to right */
				MPI_Send(&data[i][cols-1], 1 , MPI_DOUBLE, myrank+1 , i,MPI_COMM_WORLD );

				/* Receiving from right*/
				MPI_Recv(&from_right[i], 1 , MPI_DOUBLE , myrank+1 , i , MPI_COMM_WORLD, &status);
			}
			break;

		case(1):
			if(has_left_neighbour){
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
			if(has_left_neighbour){
				/* Sending to left */
				MPI_Send(&data[i][0], 1 , MPI_DOUBLE, myrank-1 , i,MPI_COMM_WORLD );

				/* Receiving from left*/
				MPI_Recv(&from_left[i], 1 , MPI_DOUBLE , myrank-1 , i , MPI_COMM_WORLD, &status);
			}
			break;

		case(1):
			if(has_right_neighbour){
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
			if(has_bottom_neighbour){
				/* Sending to below */
				MPI_Send(&data[rows-1][j], 1 , MPI_DOUBLE, myrank+Px , j ,MPI_COMM_WORLD );

				/* Receiving from below */
				MPI_Recv(&from_bottom[j], 1 , MPI_DOUBLE , myrank+Px , j , MPI_COMM_WORLD, &status);
			}
			break;

		case(1):
			if(has_top_neighbour){
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
			if(has_top_neighbour){
				/* Sending to top */
				MPI_Send(&data[0][j], 1 , MPI_DOUBLE, myrank-Px , j ,MPI_COMM_WORLD );

				/* Receiving from top */
				MPI_Recv(&from_bottom[j], 1 , MPI_DOUBLE , myrank-Px , j , MPI_COMM_WORLD, &status);
			}
			break;

		case(1):
			if(has_bottom_neighbour){
				/* Receiving from below */
				MPI_Recv(&from_bottom[j], 1 , MPI_DOUBLE, myrank+Px , j , MPI_COMM_WORLD, &status);

				/* Sending to below */
				MPI_Send(&data[rows-1][j], 1 , MPI_DOUBLE, myrank+Px , j , MPI_COMM_WORLD);
			}
			break;
		}
	}

	
stencil_computation:
	for(int i=0; i<rows; i++){
		for(int j=0; j<cols; j++){
			compute(i, j);
		}
	}

  	// done with MPI
  	MPI_Finalize();
	return 0;
}
