base: halo

clean:
	rm -f a.out halo

halo: src.c
	rm -f a.out halo
	mpicc src.c -lm -o halo

