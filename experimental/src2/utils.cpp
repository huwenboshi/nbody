#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define PI 3.14159265358979323846

// safely open a file
FILE* safe_fopen(const char* filename, const char *operation) {
    FILE* file_ptr = fopen(filename, operation);
    if(!file_ptr) {
        fprintf(stderr, "Error: Could not open file %s!\n", filename);
        exit(1);
    }
    return file_ptr;
}

// safely allocate n_elem of elements, each with size sizeof_elem
void* safe_calloc(size_t n_elem, size_t sizeof_elem) {
    void* ptr = calloc(n_elem, sizeof_elem);
    if(!ptr) {
        size_t nbytes = n_elem*sizeof_elem;
        fprintf(stderr, "Error: Could not allocate ");
        fprintf(stderr, "%u bytes of memory!\n", (unsigned int)nbytes);
        exit(1);
    }
    return ptr;
}

// randu generate uniform random variable
float randu() {
    return (float)rand() / (float)RAND_MAX;
}

// generate standard bivariate normal points
void randn_bistd(size_t n, float *points) {
    
    // iterate through the number of points
    for(size_t i=0; i<n; i++) {
        float u = randu();
        float v = randu();
        float x = sqrt(-2.0*log(u))*cos(2.0*PI*v);
        float y = sqrt(-2.0*log(u))*sin(2.0*PI*v);
        
        points[2*i+0] = x;
        points[2*i+1] = y;
    }
}

