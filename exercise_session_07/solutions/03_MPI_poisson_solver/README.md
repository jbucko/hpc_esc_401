We provide different versions of the MPI solution:
* `poisson_MPI_solution_minimal` folder contains minimal code for MPI parallelization of Poisson solver. This means, we adopt only the simplest method for domain decomposition and halo communication.
* `poisson_MPI_solution` folder contains full (MPI) solution, one can find three domain decomposition strategies as well as three halo communication approaches (see the exercise sheet).
* `poisson_HYBRID_solution_minimal` folder contains hybrid (MPI+OMP compatible) code. However, this is based only on the simplest (minimal) version of the solution.  
