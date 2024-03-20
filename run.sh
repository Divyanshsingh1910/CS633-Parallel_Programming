#!/bin/bash

echo 262144 5
for i in {1..5}; do
    rm -f output*
    mpicc src.c -lm
	mpirun -np 12 ./a.out 4 262144 10 40 5
    rm a.out
done

echo 262144 9
for i in {1..5}; do
    rm -f output*
    mpicc src.c -lm
	mpirun -np 12 ./a.out 4 262144 10 40 9
    rm a.out
done

echo 4194304 5
for i in {1..5}; do
    rm -f output*
    mpicc src.c -lm
	mpirun -np 12 ./a.out 4 4194304 10 40 5
    rm a.out
done

echo 4194304 9
for i in {1..5}; do
    rm -f output*
    mpicc src.c -lm
	mpirun -np 12 ./a.out 4 4194304 10 40 9
    rm a.out
done
