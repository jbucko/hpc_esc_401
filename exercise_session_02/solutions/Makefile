cpi_omp: cpi_omp.c
	cc -O3 -o cpi_omp -fopenmp cpi_omp.c 

cpi_mpi: cpi_mpi.c
	cc -O3 -o cpi_mpi cpi_mpi.c 

cpi: cpi.c
	cc -O3 -o cpi cpi.c

all: cpi_omp cpi_mpi cpi 

clean:
	rm -f cpi_omp cpi_mpi cpi
