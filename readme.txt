2d newtonian collission-less n-body simulation in c and cuda

as of dec 24, 2014
added new parallel function for updating acceleration
- using tiling and shared memory to hide global memory latency
- 68x speed up on t10 on 2048 particles
- 254x speed up on t10 on 4096 particles
- 168x speed up on t10 on 8192 particles
- 195x speed up on t10 on 16384 particles
- 222x speed up on t10 on 32768 particles
- 314x speed up on m2070 on 16384 particles
added new parallel function that enhances data coalescence
- 350x speed up on m2070 on 16384 particles
added -use_fast_math flag in make file
- 400x speed up on m2070 on 16384 particles
- 292x speed up on t10 on 16384 particles

as of dec 23, 2014
parallelized function for updating acceleration
- 1 thread per particle, simple update
- 31x speed up on t10 on 2048 particles
- 52x speed up on t10 on 4096 particles
- 75x speed up on t10 on 8192 particles
- 62x speed up on t10 on 16384 particles

as of dec 19, 2014
simple 2d sequantial collission less newtonian n-body simulation code
- uniform spacial initialization with 0 initial velocity
- particle-particle method (n^2), leapfrog update
