#include <stdlib.h>
#include <stdio.h>

#ifndef UTILS
#define UTILS

// safely open a file
FILE* safe_fopen(const char* filename, const char *operation);

// safe allocate memory
void* safe_calloc(size_t nelements, size_t sizeof_element);

// generate random uniform floating point
float randu();

// generate standard bivariate normal points
void randn_bistd(size_t n, float *points);

#endif
