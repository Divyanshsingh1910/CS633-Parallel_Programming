#!/bin/bash

echo N = 512*512 stencil = 5
for i in {1..5}; do
    rm -f output*
    mpicc src.c -lm
	mpirun -np 12 ./a.out 4 262144 10 40 5
    rm a.out
done

echo N = 512*512 stencil = 9
for i in {1..5}; do
    rm -f output*
    mpicc src.c -lm
	mpirun -np 12 ./a.out 4 262144 10 40 9
    rm a.out
done

echo N = 2048*2048 stencil = 5
for i in {1..5}; do
    rm -f output*
    mpicc src.c -lm
	mpirun -np 12 ./a.out 4 4194304 10 40 5
    rm a.out
done

echo N = 2048*2048 stencil = 9
for i in {1..5}; do
    rm -f output*
    mpicc src.c -lm
	mpirun -np 12 ./a.out 4 4194304 10 40 9
    rm a.out
done
