# Exercise session 02

## Exercise 01
The simplest way to setup your git repository is to first create it on bitbucket/github/gitlab. You can then clone this new empty repository on your machine with:
 `git clone https://github.com/<username>/<new_repo>.git`
 You can then add some files/directories locally, which will not modify the remote version of the repository. Some modifications of the repository (for example a new folder named `solution_exercise_02`) can be pushed to the remote version with the following commands: 
 
 `mkdir solution_exercise_02`
 
 `git add solution_exercise_02`
 
 `git commit -m "added solution directory for exercise 02"`
 
 `git push`
 
## Exercise 02

**What is cc?**

`cc` refers to the C compiler wrapper command on Cray systems (use `CC` for compiling C++ code, `ftn` for Fortran). You can find more information about compiling on Eiger here: https://user.cscs.ch/computing/compilation/

**What does the flag -O3 do? What happens if we change this to -O0?**

This flag enables compiler optimizations, which will attempt to improve the performance and/or code size at the expense of compilation time. There are several levels of optimization possible (from 0 to 3). Most optimizations are completely disabled with the level 0 flag `-O0`.

**Which programming environment are you currently using?**

 The default environment on Eiger is the Cray environment. To change from Cray to Gnu environment, use the command `module switch PrgEnv-cray PrgEnv-gnu `.
 
 **What are the results of commands `module list`, `module avail` and `module spider`?**
 
 `module list` displays the list of all modules that are currently loaded, `module avail` all the modules that are available for loading (directly) and `module spider` all the modules on a system (see https://lmod.readthedocs.io/en/latest/135_module_spider.html for more details). 
 
## Exercise 03

**-   Run sinfo -p debug. What can you see? How can you print only your jobs? Or any other user?**

The command `sinfo` is used to view partition and node information for a system running Slurm. Here, we look at the "debug" partition with the `-p debug` flag. A list of submitted jobs can be viewed with `squeue`, to only print one single user's jobs, use `squeue -u <username>`

**Logging errors to separate file**
To separate the standard output and errors from the code execution, use different SLURM options: `--output` for output file and `--error` for file catching all the errors. So you should have these two lines in your job script:

```console
#SBATCH --output="output.log"
#SBATCH --error="error.log"
```
For more options and details, see https://slurm.schedmd.com/sbatch.html. 

**First jobscript running hostname**

See `hostname/hostname.job`, and `hostname/hostname.log`

**Running cpi_mpi and cpi_omp**

Jobscripts and outputs can be found in the `mpi` and `omp` folders

**BONUS: customizing squeue**

Use the friendly command `squeue --Format=JOBID,USERNAME,PRIORITY,ACCOUNT,NAME,NUMNODES,STATECOMPACT,REASON,STARTTIME,TIMELEFT,PRIORITY -u <username>`

## Exercise 04

Environment variable `OMP_NUM_THREADS` is crucial in letting OMP know how many threads to launch. So even with `--cpus-per-task=10` but setting `OMP_NUM_THREADS=1`, your code will run on a single thread (thus being serial).

Following table shows the execution times of OMP code with `nSteps = 3'000'000'000`.

|Number of threads | 1 | 2 | 5 | 10 | 20 | 50 | 100 | 128 |
|-------|---|---|---|---|---|---|---|---|
|Running time (s) |  4.502 | 2.291 | 0.899 | 0.458 | 0.228 | 0.098 | 0.104 | 0.116 |

BONUS: From the attached figure one can see that the OMP speedup for the code is close to ideal for number of threads up to ~50, we do not gain much by further increasing the thread number. The possible explanation is that (given the simplicity of the code), the runtime starts to be dominated by updating of `sum` variable, which is locked while being modified by a specific thread. More threads are capable of faster calculation, but the reduction cannot be made faster anymore.

![alt text](https://github.com/jbucko/hpc_esc_401/blob/master/exercise_session_02/solutions/speedup/OMP_speedup.jpeg)


