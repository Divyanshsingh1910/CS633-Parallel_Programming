base:
	rm -f output*
	mpicc src.c -lm
	mpirun -np 12 ./a.out 3 262144 1 1 9

test:
	rm -f output*
	mpicc src.c -lm
	mpirun -np 3 ./a.out 3 25 1 1 9
	gcc rough.c -lm
	./a.out 3 3 25 1 1 9
	diff output* | wc
	sort -k4n output_actual.txt > output_actual; sort -k4n output_expected.txt > output_expected; vimdiff output_actual output_expected
