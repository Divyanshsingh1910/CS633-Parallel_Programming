#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <math.h>

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


