base:
	rm output_actual.txt
	mpicc src.c -lm
	mpirun -np 3 ./a.out 3 25 1 1 5
