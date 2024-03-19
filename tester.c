#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

int 	cols,	/* number of columns in the matrix */
		rows,	/* number of rows in the matrix */
		Px,		/* number of processes involved in decomposition along x-axis */
		Py,		/* number of processes involved in decomposition along y-axis */
		width;	/* halo region width = stencil/4 */

double	**data = NULL,	/* the matrix containing data */
		**temp = NULL; 	/* to temporarily store new values after computation */

void swap(double ***a, double ***b)
{
	double **c = *a;
	*a = *b;
	*b = c;
}

double get_val(int i, int j, int *nneighbours)
{
	if(i >= 0 && i < rows && j >= 0 && j < cols){
		(*nneighbours) = *nneighbours + 1;
		return data[i][j];
	}
	return 0;
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
	num_time_steps = 10,	/* number of steps */
	seed = 42,
	stencil = 5;
	
	Px = 3;	/* default value */	

	/* all command line arguments provided */
	if(argc == 7){
		P = atoi(argv[1]);
		Px = atoi(argv[2]),
		N = atoi(argv[3]),
		num_time_steps = atoi(argv[4]),
		seed = atoi(argv[5]),
		stencil = atoi(argv[6]);
	}

	Py = P/Px;
	rows = Py*sqrt(N),
	cols = Px*sqrt(N);
	width = stencil/4;
	
	/* allocating memory for the matrices */
	data = (double **)malloc(rows*sizeof(double*));
	temp = (double **)malloc(rows*sizeof(double*));
	for(int i=0; i<rows; i++){
		data[i] = (double *)malloc(cols*sizeof(double));
		temp[i] = (double *)malloc(cols*sizeof(double));
	}

	int side = (int) sqrt(N);

	/* initializing the matrix with random values */
	for(int i=0; i<rows; i++){
		for(int j=0; j<cols; j++){
			/*srand(seed*(myrank + 10));
			data[i][j] = abs(rand() + (i*rand() + j*myrank))/100;*/
			data[i][j] = j%side;
		}
	}

	for(int steps = 0; steps < num_time_steps; steps++){
		for(int i = 0; i < rows; i++)
			for(int j = 0; j < cols; j++)
				compute(i, j);
		swap(&data, &temp);
	}

	FILE *file = fopen("output_expected.txt", "w");
	for(int i = 0; i < rows; i++)
		for(int j = 0; j < cols; j++){
			int rank = (j/side) + (i/side)*Px;
			fprintf(file, "%lf %d %d %d\n", data[i][j], i%(int)side, j%(int)side, rank);
	}
	return 0;
}
