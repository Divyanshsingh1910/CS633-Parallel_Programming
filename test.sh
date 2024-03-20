rm -f output*
mpicc src.c -lm
mpirun -np 12 ./a.out 3 64 1 1 5
gcc tester.c -lm
./a.out 12 3 64 1 1 5
rm a.out
sort -k4n output_actual.txt > output_actual
sort -k4n output_expected.txt > output_expected
diff -s output_actual output_expected
