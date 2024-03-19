rm -f output*
mpicc src.c -lm
mpirun -np 12 ./a.out 3 262144 1 1 9
