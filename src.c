#include <stdio.h>
#include "mpi.h"
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

int 	cols,	/* number of columns in the matrix */
		rows,	/* number of rows in the matrix */
		Px,		/* number of processes involved in decomposition along x-axis */
		Py,		/* number of processes involved in decomposition along y-axis */
		width,	/* halo region width = stencil/4 */
		myrank;	/* rank of the current process */

double	**data = NULL,	/* the matrix containing data */
		**temp = NULL; 	/* to temporarily store new values after computation */

bool    has_left_neighbour = true,		/* whether there exists left neighbour */
		has_right_neighbour = true,		/* whether there exists right neighbour */
		has_top_neighbour = true,		/* whether there exists top neighbour */
		has_bottom_neighbour = true;	/* whether there exists bottom neighbour */

double	*from_left = NULL,		/* data received from left neighbour */
		*from_right = NULL,		/* data received from right neighbour */
		*from_top = NULL,		/* data received from top neighbour */
		*from_bottom = NULL;	/* data received from bottom neighbour */

double	*to_left = NULL,	/* data sent to left neighbour*/
		*to_right = NULL,	/* data sent to right neighbour*/
		*to_top = NULL,		/* data sent to top neighbour*/
		*to_bottom = NULL;	/* data sent to bottom neighbour*/

double get_val(int i, int j, int *nneighbours)
{
	if (i < 0) {
		if (has_top_neighbour){
			(*nneighbours) = *nneighbours + 1;
			return from_top [cols*(i+width) + j];
		}
		else
			return 0;
	} else if (i >= rows) {
		if (has_bottom_neighbour){
			(*nneighbours) = *nneighbours + 1;
			return from_bottom[cols*(i - rows) + j];
		}
		else
			return 0;
	} else if (j < 0) {
        if (has_left_neighbour){
			(*nneighbours) = *nneighbours + 1;
			return
				from_left[(j+width)*rows + i];
		}
		else return 0;
	} else if (j >= cols) {
        if (has_right_neighbour){
			(*nneighbours) = *nneighbours + 1; return from_right[rows*(j-cols) + i];
		}
		else
			return 0;
	}

	(*nneighbours) = *nneighbours + 1;
	return data[i][j];
}

void print_array(double *array, int size)
{
	return ;
	if(!array)
		return;

	for(int i = 0; i < size; i++)
		printf("%d rank %lf\n", myrank, array[i]);
}

void communicate(){
	int position = 0;
	MPI_Status status;

	/* packing data to send to neighbours */
	if(has_left_neighbour){
		position = 0;
		for(int j = 0; j < width; j++)
			for(int i = 0; i < rows; i++)
				MPI_Pack(&data[i][j], 1, MPI_DOUBLE, to_left, rows*width * sizeof(double), &position, MPI_COMM_WORLD);
	}
	
	if(has_right_neighbour){
		position = 0;
		for(int j = cols - width; j < cols; j++)
			for(int i = 0; i < rows; i++)
				MPI_Pack(&data[i][j], 1, MPI_DOUBLE, to_right, rows*width * sizeof(double), &position, MPI_COMM_WORLD);
	}
	
	if(has_top_neighbour){
		position = 0;
		for(int i = 0; i < width; i++)
			for(int j = 0; j < cols; j++)
				MPI_Pack(&data[i][j], 1, MPI_DOUBLE, to_top, cols*width * sizeof(double), &position, MPI_COMM_WORLD);
	}

	if(has_bottom_neighbour){
		position = 0;
		for(int i = rows - width; i < rows; i++)
			for(int j = 0; j < cols; j++)
				MPI_Pack(&data[i][j], 1, MPI_DOUBLE, to_bottom, cols*width * sizeof(double), &position, MPI_COMM_WORLD);
	}

/* note: as each process has distinct neighbours, while sending and receiving messages, tag isn't necessary, that's why tag = 0 everywhere */

/* along_x_communication */
	/* every even-process sends to right neighbour && odd-process sends to left*/

	switch((myrank%Px) % 2){
		case(0):
			if(has_right_neighbour){
				/* Sending to right */
				MPI_Send(to_right, rows*width*sizeof(double), MPI_PACKED, myrank + 1, 0, MPI_COMM_WORLD);

				/* Receiving from right*/
				MPI_Recv(from_right, rows*width, MPI_DOUBLE, myrank + 1, 0, MPI_COMM_WORLD, &status);
			}
			break;

		case(1):
			if(has_left_neighbour){
				/* Receiving from left */
				MPI_Recv(from_left, rows*width, MPI_DOUBLE, myrank - 1, 0, MPI_COMM_WORLD, &status);

				/* Sending to left */
				MPI_Send(to_left, rows*width*sizeof(double), MPI_PACKED, myrank - 1, 0, MPI_COMM_WORLD);
			}
			break;
	}

	/* every even-process sends to left neighbour && odd-process sends to right */
	switch((myrank%Px) % 2){	
		case(0):
			if(has_left_neighbour){
				/* Sending to left */
				MPI_Send(to_left, rows*width*sizeof(double), MPI_PACKED, myrank - 1, 0, MPI_COMM_WORLD);

				/* Receiving from left*/
				MPI_Recv(from_left, rows*width, MPI_DOUBLE, myrank - 1, 0, MPI_COMM_WORLD, &status);
			}
			break;

		case(1):
			if(has_right_neighbour){
				/* Receiving from right */
				MPI_Recv(from_right, rows*width, MPI_DOUBLE, myrank + 1, 0, MPI_COMM_WORLD, &status);

				/* Sending to right */
				MPI_Send(to_right, rows*width*sizeof(double), MPI_PACKED, myrank + 1, 0, MPI_COMM_WORLD);
			}
			break;
	}

/* along_y_communication */
	
	/* Here the parity is checked by
	 * division from Px
	 * So, 			0 , top most
	 *        	   /
	 * myrank/Px =
	 *			   \
	 *				Py-1, bottom most
	 */
	 
	/* every even-process sends to bottom && odd-process sends to top */
	switch((myrank/Px) % 2){	
		case(0):
			if(has_bottom_neighbour){
				/* Sending to bottom */
				MPI_Send(to_bottom, cols*width*sizeof(double), MPI_PACKED, myrank + Px, 0, MPI_COMM_WORLD);

				/* Receiving from bottom */
				MPI_Recv(from_bottom, cols*width * sizeof(double), MPI_PACKED, myrank + Px, 0, MPI_COMM_WORLD, &status);
			}
			break;

		case(1):
			if(has_top_neighbour){
				/* Receiving from top */
				MPI_Recv(from_top, cols*width * sizeof(double), MPI_PACKED, myrank - Px, 0, MPI_COMM_WORLD, &status);

				/* Sending to top */
				MPI_Send(to_top, cols*width*sizeof(double), MPI_PACKED, myrank - Px, 0, MPI_COMM_WORLD);
			}
			break;
	}

	/* every even-process sends to top && odd-process sends to bottom*/
	switch((myrank/Px) % 2){
		case(0):
			if(has_top_neighbour){
				/* Sending to top */
				MPI_Send(to_top, cols*width*sizeof(double), MPI_PACKED, myrank - Px, 0, MPI_COMM_WORLD);

				/* Receiving from top */
				MPI_Recv(from_top, cols*width * sizeof(double), MPI_PACKED, myrank - Px, 0, MPI_COMM_WORLD, &status);
			}
			break;

		case(1):
			if(has_bottom_neighbour){
				/* Receiving from bottom */
				MPI_Recv(from_bottom, cols*width * sizeof(double), MPI_PACKED, myrank + Px, 0, MPI_COMM_WORLD, &status);

				/* Sending to bottom */
				MPI_Send(to_bottom, cols*width*sizeof(double), MPI_PACKED, myrank + Px, 0, MPI_COMM_WORLD);
			}
			break;
	}
	print_array (from_bottom, rows*width);
	print_array (from_top, rows*width);
	print_array (from_left, rows*width);
	print_array (from_right, rows*width);
}

void swap(double ***a, double ***b)
{
	double **c = *a;
	*a = *b;
	*b = c;
}

void fill_has_neighbours()
{
    if(myrank%Px == 0) /* 0, 3, 6, 9 */
        has_left_neighbour = false;
    
	if(myrank%Px == Px - 1) /* 2, 5, 8, 11 */
        has_right_neighbour = false;
    
	if(myrank/Px == 0) /* 0, 1, 2 */
        has_top_neighbour = false;
    
	if(myrank/Px == Py - 1) /* 9, 10, 11 */
        has_bottom_neighbour = false;
}
void compute(int i, int j)
{
    double sum_neighbours = 0;
	int nneighbours = 0;

   	for(int _ = 1; _ <= width; _++)
		sum_neighbours += get_val(i, j - _, &nneighbours);

   	for(int _ = 1; _ <= width; _++)
		sum_neighbours += get_val(i, j + _, &nneighbours);
   	
	for(int _ = 1; _ <= width; _++)
		sum_neighbours += get_val(i - _, j, &nneighbours);
   	
	for(int _ = 1; _ <= width; _++)
		sum_neighbours += get_val(i + _, j, &nneighbours);
	
	temp[i][j] = (data[i][j] + sum_neighbours)/(nneighbours + 1);
}

int main(int argc, char *argv[]) 
{
	int N = 512*512,		/* number of data points per process */
	P = 12,					/* total number of processes */
	num_time_steps = 1,	/* number of steps */
	seed = 42,
	stencil = 5;
	
	Px = 3;	/* default value */	

	/* all command line arguments provided */
	if(argc == 6){
		Px = atoi(argv[1]),
		N = atoi(argv[2]),
		num_time_steps = atoi(argv[3]),
		seed = atoi(argv[4]),
		stencil = atoi(argv[5]);
	}
	
	/* initialize MPI */
	MPI_Init (&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &P);

	Py = P/Px;
	rows = cols = sqrt(N);
	width = stencil/4;

	MPI_Status status;
	
	/* filling neighbours */
	fill_has_neighbours();
	
	/* allocating memory for the matrices */
	data = (double **)malloc(rows*sizeof(double*));
	temp = (double **)malloc(rows*sizeof(double*));
	for(int i=0; i<rows; i++){
		data[i] = (double *)malloc(cols*sizeof(double));
		temp[i] = (double *)malloc(cols*sizeof(double));
	}

	/* initializing the matrix with random values */
	for(int i=0; i<rows; i++){
		for(int j=0; j<cols; j++){
			/*srand(seed*(myrank + 10));
			data[i][j] = abs(rand() + (i*rand() + j*myrank))/100;*/
			data[i][j] = 1;
		}
	}

	int position = 0;
	/* allocating memory for data going to send to/receive from neighbours */
	if(has_left_neighbour){
		from_left	= (double *)malloc(rows*width * sizeof(double));
		to_left		= (double *)malloc(rows*width * sizeof(double));
	}
	
	if(has_right_neighbour){
		from_right	= (double *)malloc(rows*width * sizeof(double));
		to_right	= (double *)malloc(rows*width * sizeof(double));
	}
	
	if(has_top_neighbour){
		from_top	= (double *)malloc(cols*width * sizeof(double));
		to_top		= (double *)malloc(cols*width * sizeof(double));
	}

	if(has_bottom_neighbour){
		from_bottom	= (double *)malloc(cols*width * sizeof(double));
		to_bottom	= (double *)malloc(cols*width * sizeof(double));
	}

	double start_time, end_time;
	start_time = MPI_Wtime();
/* stencil communication + computation*/
	for(int steps = 0; steps < num_time_steps; steps++){
		communicate();
		for(int i=0; i<rows; i++){
			for(int j=0; j<cols; j++){
				compute(i, j);
			}
		}
		/* as temp contains new values, we need to swap data and temp */
		swap(&data, &temp); 
	}
	end_time = MPI_Wtime();
	end_time -= start_time;
	double max_time;
	MPI_Reduce (&end_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	if (myrank == 0)
		printf ("%lf\n", max_time);

	/* debugging */
	FILE *file = fopen("output_actual.txt", "a");

	if(myrank)
		MPI_Recv(NULL, 0, MPI_INT, myrank - 1, 0, MPI_COMM_WORLD, &status);
	
	for(int i = 0; i < rows; i++)
		for(int j = 0; j < cols; j++){
			// if(data[i][j] != 1)
				// printf("myrank: %d, data[%d][%d] = %lf\n", myrank, i, j, data[i][j]);
			fprintf(file, "%lf\n", data[i][j], i, j, myrank);
			// fprintf(file, "%lf %d %d %d\n", data[i][j], i, j, myrank);
		}

  	if(myrank < P - 1)
		MPI_Send(NULL, 0, MPI_INT, myrank + 1, 0, MPI_COMM_WORLD);

	/* done with MPI */
  	MPI_Finalize();
	return 0;
}
