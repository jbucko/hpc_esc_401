#!/bin/bash -l

sbatch --ntasks-per-node=$1 --output=poisson_mpi_$1_$2.out run_MPI.job params_${2}.txt 

