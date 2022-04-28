CFLAGS=-O3 -ffast-math -mavx2
# CC=/usr/local/Cellar/open-mpi/4.1.0/bin/mpicxx
# CC=mpicxx
# CC=/usr/local/Cellar/gcc/10.2.0_4/bin/g++-10
CC=CC
all: mpi_module.o init.o io.o jacobi.o main.o main main_argv.o main_argv
init.o: init.cpp init.h
	${CC} ${CFLGAS} -c -o init.o init.cpp 
io.o: io.cpp io.h
	${CC} ${CFLGAS} -c -o io.o io.cpp
jacobi.o: jacobi.cpp jacobi.h
	${CC} ${CFLGAS} -c -o jacobi.o jacobi.cpp
mpi_module.o: mpi_module.cpp mpi_module.h
	${CC} ${CFLGAS} -c -o mpi_module.o mpi_module.cpp
main.o: main.cpp mpi_module.o jacobi.o io.o init.o
	${CC} ${CFLAGS} -c -o main.o main.cpp 
main:
	${CC} -o main main.o init.o io.o jacobi.o mpi_module.o

main_argv.o: main_argv.cpp mpi_module.o jacobi.o io.o init.o
	${CC} ${CFLAGS} -c -o main_argv.o main_argv.cpp 
main_argv:
	${CC} -o main_argv main_argv.o init.o io.o jacobi.o mpi_module.o
clean: 
	rm -f main_argv main_argv.o main main.o init.o io.o jacobi.o mpi_module.o
