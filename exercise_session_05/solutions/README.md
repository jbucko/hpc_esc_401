### Exercise 1 [Vector addition]

We provide both C (using `malloc()` for array initialization) and C++ (using `new[]` for array initialization) codes in `./01_vector_addition` folder. 
The codes should be self-explanatory, here, we stress out few points:
  * one can define `complex_vector` structure to be a type, so one can initialize complex vectors x,y,z as `complex_vector x,y,z;` (similar to initialization of e.g. integers `int i,j,k;`). Then, there is no need to use `struct` identifier anymore when referring to `complex_vector`.
  * one cannot (easily) define a specific lengths of `real` and `img` vectors in the structure/type definition and needs to set a specific length after the structure is initilized.
  * notice the way of passing the more complicated structure into function calls and then using them within the function definitions (`*` vs. ` &` symbols)
  * one cannnot compile `.c` code (including `malloc()` call) using `CC` compiler, but it works the other way around. 

### Exercise 2 [Pointers]

* Indexing of an array `p` starts at 0, so `p[0]` to `p[9]` are elements of this array. Unlike in e.g. Python, `p[-1]` refers to a value in a memory next to `p[0]` (one step "back"), but there is no guerantee what this memory is used for. Such allocations might be dangerous (e.g. overwriting a value which is a part of some other array/variable or some other process).  
* In our case, `p` is a pointer to the first element of array. So value of `p` is an address of the array's first element, something like `0x22ccec`. `*p` is the value that this pointer points to, so the value of the array's first element (equivalent to `p[0]`). `*` in this context is called a dereference operator. `&` is called a reference (or address) operator and in general returns an address of a variable (a place in the memory where a specific variable is stored). So `&p` returns a place in memory (address) where the pointer `p` is stored.
* In C/C++, when an aray with length `n` is initialized, a piece of memory is reserved and the elements are stored **next to each other** - physically in the neighbouring memory slots. So 
`(p+1)` is a pointing to the neighbouring address of `p` and `*(p+1)` is a value stored there, so equivalent to `p[0+1]`. `*p + 1` is adding 1 to the first element, the same as `p[0]+1`.
* `q`is a pointer pointing to the one address "before" `p`. So `q[0]` is some unknown value stored at one memory "slot" before `p[0]`. Then `q[10] = p[9]` and `q[11]` is some unknown value stored right after the last element of array `p`. 

