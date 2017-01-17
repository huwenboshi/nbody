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

// allocate float memory on gpu
void cu_safe_falloc(float **g_f, size_t n_elem);

// free memory on gpu
void cu_free(void *g_d);

// copy from cpu space f to gpu space g_f
void memcpy_htod(float *g_f, float *f, size_t n_elem);

// copy from gpu space g_f to cpu space f
void memcpy_dtoh(float *f, float *g_f, size_t n_elem);

#endif
