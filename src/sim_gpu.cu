#include "sim.h"
#include "sim_gpu.h"

size_t block_size = 128;
size_t max_grid_x = 65535;

/*---------------------------------------------------------------------------*/
// update acceleration
extern "C++" void update_acc_gpu(size_t n, float *part_pos,
    float *part_vel, float *part_acc, float *part_mass,
    float grav_const) {

    // get num of threads and blocks needed
    size_t num_block = (n-1)/block_size+1;

    // compute grid dimension
    size_t num_grid_y = (num_block-1)/max_grid_x+1;
    size_t num_grid_x = num_block < max_grid_x ? num_block : max_grid_x;

    // launch kernel
    dim3 dimBlock(block_size, 1);
    dim3 dimGrid(num_grid_x, num_grid_y);
    update_acc_kernel<<<dimGrid,dimBlock>>>(n, part_pos,
        part_vel, part_acc, part_mass, grav_const);

    // check kernel result
    cudaThreadSynchronize();
    cudaError_t crc = cudaGetLastError();
    if(crc) {
        printf("emptyKernel error=%d:%s\n", crc, cudaGetErrorString(crc));
        exit(1);
    }
}

// kernel function for updating acceleration
__global__ void update_acc_kernel(size_t n, float *part_pos,
    float *part_vel, float *part_acc, float *part_mass,
    float grav_const) {
    
    // get absoluate idx of thread
    size_t i = threadIdx.x+blockDim.x*(blockIdx.x+gridDim.x*blockIdx.y);

    // check if j is within range
    if(i < n) {
        float xi = part_pos[2*i+0];
        float yi = part_pos[2*i+1];
        float mi = part_mass[i];

        part_acc[2*i+0] = 0.0;
        part_acc[2*i+1] = 0.0;

        // aggregate acceleration
        for(size_t j=0; j<n; j++) {
            
            float xj = part_pos[2*j+0];
            float yj = part_pos[2*j+1];
            float mj = part_mass[j];
            
            float dx = xj-xi;
            float dy = yj-yi;
            float r = sqrt(dx*dx+dy*dy)+eps;
                
            // update acceleration
            float f = grav_const*mi*mj/(r*r);
            float fx = f*(dx/r);
            float fy = f*(dy/r);
            part_acc[2*i+0] += fx/mi;
            part_acc[2*i+1] += fy/mi;
        }
    }
}

/*---------------------------------------------------------------------------*/
// update acceleration
extern "C++" void update_acc_tile_gpu(size_t n, float *part_pos,
    float *part_vel, float *part_acc, float *part_mass,
    float grav_const) {

    // define tile size, num of tiles
    size_t tile_size = block_size;
    size_t num_tiles = n/tile_size;
    size_t num_tiles_per_block = 1;
    size_t num_blocks = num_tiles/num_tiles_per_block;

    // compute grid dimension
    size_t num_grid_y = (num_blocks-1)/max_grid_x+1;
    size_t num_grid_x = num_blocks < max_grid_x ? num_blocks : max_grid_x;

    // launch kernel
    // shared mem include: updt_part_pos, updt_part_acc, updt_part_mass
    // int_part_pos, int_part_mass
    dim3 dimBlock(block_size, 1);
    dim3 dimGrid(num_grid_x, num_grid_y);
    size_t shmem_size = 2*((2+1)*tile_size)+2*tile_size;
    shmem_size = shmem_size*sizeof(float);
    update_acc_tile_kernel<<<dimGrid,dimBlock,shmem_size>>>(n, part_pos,
        part_vel, part_acc, part_mass, grav_const, tile_size,
        num_tiles_per_block, num_tiles);

    // check kernel result
    cudaThreadSynchronize();
    cudaError_t crc = cudaGetLastError();
    if(crc) {
        printf("error=%d:%s\n", crc, cudaGetErrorString(crc));
        exit(1);
    }
}

// kernel function for updating acceleration
__global__ void update_acc_tile_kernel(size_t n, float *part_pos,
    float *part_vel, float *part_acc, float *part_mass, float grav_const,
    size_t tile_size, size_t num_tiles_per_block, size_t num_tiles) {
  
    // get pointer to shared memory
    extern __shared__ float shmem[];
    float *updt_part_pos = &shmem[0];
    float *updt_part_acc = &shmem[tile_size*2];
    float *updt_part_mass = &shmem[tile_size*2*2];
    float *int_part_pos = &shmem[tile_size*2*2+tile_size];
    float *int_part_mass = &shmem[tile_size*2*2+tile_size+tile_size*2];

    // get indices
    size_t tidx = threadIdx.x;
    size_t bdim = blockDim.x;
    size_t bidx = blockIdx.x+gridDim.x*blockIdx.y;
    size_t poff = tile_size*num_tiles_per_block*bidx;

    // iterate through tiles responsible by this block
    for(size_t i=0; i<num_tiles_per_block; i++) {

        // load particle position, mass, init acc
        #pragma unroll
        for(size_t j=0; j*bdim+tidx<tile_size; j+=bdim) {
            size_t pidx = poff+i*tile_size+j*bdim+tidx;
            if(pidx < n) {
                updt_part_pos[2*(j*bdim+tidx)+0] = part_pos[2*pidx+0];
                updt_part_pos[2*(j*bdim+tidx)+1] = part_pos[2*pidx+1];
                updt_part_mass[j*bdim+tidx] = part_mass[pidx];
                updt_part_acc[2*(j*bdim+tidx)+0] = 0.0;
                updt_part_acc[2*(j*bdim+tidx)+1] = 0.0;
            }
        }

        // iterate through other tiles
        for(size_t k=0; k<num_tiles; k++) {

            // load particle position, mass from other tiles
            #pragma unroll
            for(size_t j=0; j*bdim+tidx<tile_size; j+=bdim) {
                size_t pidx = k*tile_size+j*bdim+tidx;
                if(pidx < n) {
                    int_part_pos[2*(j*bdim+tidx)+0] = part_pos[2*pidx+0];
                    int_part_pos[2*(j*bdim+tidx)+1] = part_pos[2*pidx+1];
                    int_part_mass[j*bdim+tidx] = part_mass[pidx];
                }
            }
            __syncthreads();

            // compute acceleration
            for(size_t ii=0; ii*bdim+tidx<tile_size; ii+=bdim) {
                
                float xi = updt_part_pos[2*(ii*bdim+tidx)+0];
                float yi = updt_part_pos[2*(ii*bdim+tidx)+1];
                float mi = updt_part_mass[ii*bdim+tidx];
        
                #pragma unroll
                for(size_t jj=0; jj<tile_size; jj++) {
                    float xj = int_part_pos[2*jj+0];
                    float yj = int_part_pos[2*jj+1];
                    float mj = int_part_mass[jj];

                    float dx = xj-xi;
                    float dy = yj-yi;
                    float r = sqrt(dx*dx+dy*dy)+eps;
                
                    float f = grav_const*mi*mj/(r*r);
                    float fx = f*(dx/r);
                    float fy = f*(dy/r);
                    
                    updt_part_acc[2*(ii*bdim+tidx)+0] += fx/mi;
                    updt_part_acc[2*(ii*bdim+tidx)+1] += fy/mi;
                }
            }
            __syncthreads();
        }
        
        // write to global memory
        for(size_t j=0; j*bdim+tidx<tile_size; j+=bdim) {
            size_t pidx = poff+i*tile_size+j*bdim+tidx;
            if(pidx < n) {
                part_acc[2*pidx+0] = updt_part_acc[2*(j*bdim+tidx)+0];
                part_acc[2*pidx+1] = updt_part_acc[2*(j*bdim+tidx)+1];
            }
        }
        __syncthreads();
    }
}

/*---------------------------------------------------------------------------*/
// update acceleration
extern "C++" void update_acc_tile_trans_gpu(size_t n, float *part_pos,
    float *part_vel, float *part_acc, float *part_mass,
    float grav_const) {

    // define tile size, num of tiles
    size_t tile_size = block_size;
    size_t num_tiles = n/tile_size;
    size_t num_tiles_per_block = 1;
    size_t num_blocks = num_tiles/num_tiles_per_block;

    // compute grid dimension
    size_t num_grid_y = (num_blocks-1)/max_grid_x+1;
    size_t num_grid_x = num_blocks < max_grid_x ? num_blocks : max_grid_x;

    // launch kernel
    // shared mem include: updt_part_pos, updt_part_acc, updt_part_mass
    // int_part_pos, int_part_mass
    dim3 dimBlock(block_size, 1);
    dim3 dimGrid(num_grid_x, num_grid_y);
    size_t shmem_size = 2*((2+1)*tile_size)+2*tile_size;
    shmem_size = shmem_size*sizeof(float);
    update_acc_tile_trans_kernel<<<dimGrid,dimBlock,shmem_size>>>(n, part_pos,
        part_vel, part_acc, part_mass, grav_const, tile_size,
        num_tiles_per_block, num_tiles);

    // check kernel result
    cudaThreadSynchronize();
    cudaError_t crc = cudaGetLastError();
    if(crc) {
        printf("error=%d:%s\n", crc, cudaGetErrorString(crc));
        exit(1);
    }
}

// kernel function for updating acceleration
__global__ void update_acc_tile_trans_kernel(size_t n, float *part_pos,
    float *part_vel, float *part_acc, float *part_mass, float grav_const,
    size_t tile_size, size_t num_tiles_per_block, size_t num_tiles) {
  
    // get pointer to shared memory
    extern __shared__ float shmem[];
    float *updt_part_pos = &shmem[0];
    float *updt_part_acc = &shmem[tile_size*2];
    float *updt_part_mass = &shmem[tile_size*2*2];
    float *int_part_pos = &shmem[tile_size*2*2+tile_size];
    float *int_part_mass = &shmem[tile_size*2*2+tile_size+tile_size*2];

    // get indices
    size_t tidx = threadIdx.x;
    size_t bdim = blockDim.x;
    size_t bidx = blockIdx.x+gridDim.x*blockIdx.y;
    size_t poff = tile_size*num_tiles_per_block*bidx;

    // iterate through tiles responsible by this block
    for(size_t i=0; i<num_tiles_per_block; i++) {

        // load particle position, mass, init acc
        #pragma unroll
        for(size_t j=0; j*bdim+tidx<tile_size; j+=bdim) {
            size_t pidx = poff+i*tile_size+j*bdim+tidx;
            if(pidx < n) {
                updt_part_pos[0*tile_size+(j*bdim+tidx)] = part_pos[0*n+pidx];
                updt_part_pos[1*tile_size+(j*bdim+tidx)] = part_pos[1*n+pidx];
                updt_part_mass[j*bdim+tidx] = part_mass[pidx];
                updt_part_acc[0*tile_size+(j*bdim+tidx)] = 0.0;
                updt_part_acc[1*tile_size+(j*bdim+tidx)] = 0.0;
            }
        }

        // iterate through other tiles
        for(size_t k=0; k<num_tiles; k++) {

            // load particle position, mass from other tiles
            #pragma unroll
            for(size_t j=0; j*bdim+tidx<tile_size; j+=bdim) {
                size_t pidx = k*tile_size+j*bdim+tidx;
                if(pidx < n) {
                    int_part_pos[0*tile_size+(j*bdim+tidx)] = part_pos[0*n+pidx];
                    int_part_pos[1*tile_size+(j*bdim+tidx)] = part_pos[1*n+pidx];
                    int_part_mass[j*bdim+tidx] = part_mass[pidx];
                }
            }
            __syncthreads();

            // compute acceleration
            for(size_t ii=0; ii*bdim+tidx<tile_size; ii+=bdim) {
                
                float xi = updt_part_pos[0*tile_size+(ii*bdim+tidx)];
                float yi = updt_part_pos[1*tile_size+(ii*bdim+tidx)];
                float mi = updt_part_mass[ii*bdim+tidx];
        
                #pragma unroll
                for(size_t jj=0; jj<tile_size; jj++) {
                    float xj = int_part_pos[0*tile_size+jj];
                    float yj = int_part_pos[1*tile_size+jj];
                    float mj = int_part_mass[jj];

                    float dx = xj-xi;
                    float dy = yj-yi;
                    float r = sqrt(dx*dx+dy*dy)+eps;
                
                    float f = grav_const*mi*mj/(r*r);
                    float fx = f*(dx/r);
                    float fy = f*(dy/r);
                    
                    updt_part_acc[0*tile_size+(ii*bdim+tidx)] += fx/mi;
                    updt_part_acc[1*tile_size+(ii*bdim+tidx)] += fy/mi;
                }
            }
            __syncthreads();
        }
        
        // write to global memory
        for(size_t j=0; j*bdim+tidx<tile_size; j+=bdim) {
            size_t pidx = poff+i*tile_size+j*bdim+tidx;
            if(pidx < n) {
                part_acc[0*n+pidx] = updt_part_acc[0*tile_size+(j*bdim+tidx)];
                part_acc[1*n+pidx] = updt_part_acc[1*tile_size+(j*bdim+tidx)];
            }
        }
        __syncthreads();
    }
}

