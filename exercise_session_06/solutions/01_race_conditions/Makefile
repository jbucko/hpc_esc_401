CFLAGS=-O3 -ffast-math -fopenmp -mavx2
CC=cc

all: serial omp_race omp_atomic omp_critical omp_reduction
serial: serial.c
omp_race: omp_race.c
omp_atomic: omp_atomic.c
omp_critical: omp_critical.c
omp_reduction: omp_reduction.c
clean: 
	rm serial omp_race omp_atomic omp_critical omp_reduction
