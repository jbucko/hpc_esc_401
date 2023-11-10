### Exercise 1 [Vector addition]

We provide both C (using `malloc()` for array initialization) and C++ (using `new[]` for array initialization) codes in `./01_vector_addition` folder. 
The codes should be self-explanatory, here, we stress out few points:
  * one can define `complex_vector` structure to be a type, so one can initialize complex vectors x,y,z as `complex_vector x,y,z;` (similar to initialization of e.g. integers `int i,j,k;`). Then, there is no need to use `struct` identifier anymore when referring to `complex_vector`.
  * one cannot (easily) define a specific lengths of `real` and `img` vectors in the structure/type definition and needs to set a specific length after the structure is initilized.
  * notice the way of passing the more complicated structure into function calls and then using them within the function definitions (`*` vs. ` &` symbols)
  * one cannnot compile `.c` code (including `malloc()` call) using `CC` compiler by default but there is a way to adjust a code so C++ compiler can handle malloc calls). The compilation works the other way around, so `cc` compiler can handle also C++ syntax. 

### Exercise 2 [Pointers]

* Indexing of an array `p` starts at 0, so `p[0]` to `p[9]` are elements of this array. Unlike in e.g. Python, `p[-1]` refers to a value in a memory next to `p[0]` (one step "back"), but there is no guerantee what this memory is used for. Such allocations might be dangerous (e.g. overwriting a value which is a part of some other array/variable or some other process).  
* In our case, `p` is a pointer to the first element of array. So value of `p` is an address of the array's first element, something like `0x22ccec`. `*p` is the value that this pointer points to, so the value of the array's first element (equivalent to `p[0]`). `*` in this context is called a dereference operator. `&` is called a reference (or address) operator and in general returns an address of a variable (a place in the memory where a specific variable is stored). So `&p` returns a place in memory (address) where the pointer `p` is stored.
* In C/C++, when an aray with length `n` is initialized, a piece of memory is reserved and the elements are stored **next to each other** - physically in the neighbouring memory slots. So 
`(p+1)` is a pointing to the neighbouring address of `p` and `*(p+1)` is a value stored there, so equivalent to `p[0+1]`. `*p + 1` is adding 1 to the first element, the same as `p[0]+1`.
* `q`is a pointer pointing to the one address "before" `p`. So `q[0]` is some unknown value stored at one memory "slot" before `p[0]`. Then `q[10] = p[9]` and `q[11]` is some unknown value stored right after the last element of array `p`. 
* `int **M` is a pointer to pointer. Specifically, it is an array with length of 2 and these two elements are also pointers. First pointer `M[0]` points to yet another array with length 5, similarly `M[1]`. 
* `M` points to `M[0]` and stores an address of `M[0]`, `*M` is a value pointed to by `M`. Equivalent to `M[0]`, but `M[0]` is now a pointer so printing `M[0]` will return another address, but this time it will be an address of the first element of array `M[0]` (with length 5), so equivalent to `&M[0][0]`. `**M` is dereferences the first element of `M[0]`, so it is the actual value of `M[0][0]` (which is 0 in our case).`M[1][3]` is 8 (value at 4-th position of second array),`*(M[0]+1)` is the same as `M[0][1] = 1` and `*(*(M+1)+3)` is the same here as `M[1][3] = 8`. 

* The pointers `M[0],M[1]` are stored next to each other but the arrays {0,1,2,3,4} and {5,6,7,8,9} they are pointing to do not have to be next to each other. These two arrays are just two separate blocks of memory with length of 5*sizeof(int) somewhere in the memory.
* The sample code is to be found in folder ` ./02_pointers/ex05_2.cpp`

### Exercise 3 [Serial 2D Poisson solver]

The final code can be found in `./03_poisson_solver_serial/` folder. Namely, there were three places where you should have added some piece of code:
* `init_f` function in `init.cpp`:

```C++
void init_f(params p, double **f){
    // printf("Function init_f (init.cpp l.97): not implemented.\n");
    double dx=1/((double)p.nx-1), dy=1/((double)p.nx-1);
    for (int i=0;i<p.nx;i++)
        for (int j=0; j<p.ny;j++)
        {
            if (i==0 || j==0 || i==p.nx-1 || j==p.ny - 1)
            {
                f[i][j] = boundary(i*dx,j*dy,p.rhs_function);    
            }
            else
            {
                f[i][j] = source_term(i*dx,j*dy,p.rhs_function);    
            }            
        }
}
```
where you should initialize the source term. You should distinguish the boundary and the area inside the domain. 

* `jacobi_step` function in `jacobi.cpp`:
```C++
    for (int i=1; i<p.nx-1; i++){
        for (int j=1; j<p.ny-1; j++)
            u_new[i][j] = 0.25*(u_old[i-1][j] + u_old[i+1][j] + u_old[i][j-1] + u_old[i][j+1] - dx*dy*f[i][j]);
    }
```
Here, only the double for loop should be added to update `u_new` array using the update formula from the assignment sheet. Beware that the indices should not include first and the last one as here the values from boundary conditions should be preserved.

* `norm_diff` function in `jacobi.cpp`:
```C++
double norm_diff(params p, double** mat1, double** mat2){
    double sum = 0.0;
    for (int i=0; i<p.nx; i++){
    for (int j=0; j<p.ny; j++){
        sum += (mat1[i][j] - mat2[i][j])*(mat1[i][j] - mat2[i][j]);
    }
    }
    sum /= p.nx*p.ny;
    sum = sqrt(sum);
    // printf("Function norm_diff (jacobi.cpp l.12): not implemented.\n");
    return sum;
}
```
The above is just a simple implementation of the square difference formula provided in the assignment sheet.

Once you compile and run the code, you should have recieved solutions as in the figures below, for right hand side 1 and 2, respectively. When done correctly (and kept the default settings in `params.txt`), RHS 1 should exit after 19366 iterations and RHS 2 after 41021 iterations. 

![alt text](https://github.com/jbucko/hpc_esc_401/blob/master/exercise_session_05/solutions/03_poisson_solver_serial/output/solution_rhs1.png)

![alt text](https://github.com/jbucko/hpc_esc_401/blob/master/exercise_session_05/solutions/03_poisson_solver_serial/output/solution_rhs2.png)
