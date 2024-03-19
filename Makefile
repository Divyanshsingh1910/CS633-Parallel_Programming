
base:
	mpicc src.c -lm
	mpirun -np 12 ./a.out 3 25 1 1 5

