#include <stdlib.h>
#include <stdio.h>
#include <cuda.h>

// allocate memory on gpu
extern "C++" void cu_safe_falloc(float **g_f, size_t n_elem) {
    void *gptr;
    cudaError_t crc = cudaMalloc(&gptr, n_elem*sizeof(float));
    if(crc) {
        printf("cudaMalloc Error=%d:%s\n", crc, cudaGetErrorString(crc));
        exit(1);
    }
    *g_f = (float*) gptr;
}

// free memory on gpu
extern "C++" void cu_free(void *g_d) {
   cudaError_t crc = cudaFree(g_d);
   if (crc) {
      printf("cudaFree Error=%d:%s\n", crc, cudaGetErrorString(crc));
      exit(1);
   }
}

// copy from cpu space f to gpu space g_f
extern "C++" void memcpy_htod(float *g_f, float *f, size_t n_elem) {
   cudaError_t crc = cudaMemcpy((void*)g_f, f, sizeof(float)*n_elem,
                    cudaMemcpyHostToDevice);
   if (crc) {
      printf("cudaMemcpyHostToDevice float Error=%d:%s\n",crc,
              cudaGetErrorString(crc));
      exit(1);
   }
}

// copy from gpu space g_f to cpu space f
extern "C++" void memcpy_dtoh(float *f, float *g_f, size_t n_elem) {
   cudaError_t crc = cudaMemcpy(f, (void*)g_f, sizeof(float)*n_elem,
                    cudaMemcpyDeviceToHost);
   if (crc) {
      printf("cudaMemcpyDeviceToHost float Error=%d:%s\n",crc,
              cudaGetErrorString(crc));
      exit(1);
   }
   return;
}
