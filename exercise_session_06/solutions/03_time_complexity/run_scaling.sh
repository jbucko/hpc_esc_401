#!/bin/bash -l
sbatch --output=scaling100.out run_nbody_for1_args.job 100
sleep 10
sbatch --output=scaling500.out run_nbody_for1_args.job 500
sleep 10
sbatch --output=scaling1000.out run_nbody_for1_args.job 1000
sleep 10
sbatch --output=scaling5000.out run_nbody_for1_args.job 5000
sleep 10
sbatch --output=scaling10000.out run_nbody_for1_args.job 10000
sleep 10
sbatch --output=scaling20000.out run_nbody_for1_args.job 20000
sleep 10
sbatch --output=scaling50000.out run_nbody_for1_args.job 50000
