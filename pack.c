#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <stdbool.h>
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

void pack_column (double **base, int nrows, int ncols, int myrank, int Px,
	       	int Py, double *outbuf, int offset_inside_row, int width) {
	// matrix is base[nrows][ncols];
	// offset_inside_row is 0 for left column, (ncols/Px) for right column NOT (ncols/Px -1)
	// width = 1 for stencil = 5, 2 for stencil = 9


	int iter;
       	int packcount = 0;
	int iterstart = (nrows/Py) * (myrank/Px);

	int iterend = (has_bottom_neighbour ? (iterstart + nrows/Py) : ncols);

	int col_num = ( has_right_neighbour? (myrank%Py)*(ncols/Py) + offset_inside_row : ncols);
	col_num -= width ;

	for (iter = iterstart; iter < iterend; iter ++) {
		MPI_Pack (base[iter] + col_num, width, MPI_DOUBLE, outbuf,
				width, &packcount, MPI_COMM_WORLD);
	}
	printf ("rank %d packed %d elements into outbuf\nwidth=%d\n", myrank, packcount, width);
	return ;
}


