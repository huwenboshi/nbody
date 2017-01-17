#include <stdlib.h>

#ifndef SIM
#define SIM

const float eps = 1.0;

// initialize particles, position and velocity
void init(size_t n, float *part_pos, float *part_vel,
        float *part_acc, float *part_mass,
        float scale_x, float scale_y,
        float center_x, float center_y,
        float scale_mass);

// update acceleration
void update_acc(size_t n, float *part_pos, float *part_vel,
        float *part_acc, float *part_mass, float grav_const);

// update velocity
void update_vel(size_t n, float *part_vel, float *part_acc, float delta_t);

// update particles
void update_pos(size_t n, float *part_pos, float *part_vel, float delta_t);

// update acceleration using gpu
void update_acc_gpu(size_t n, float *part_pos, float *part_vel,
        float *part_acc, float *part_mass, float grav_const);

// update acceleration using gpu
void update_acc_tile_gpu(size_t n, float *part_pos, float *part_vel,
        float *part_acc, float *part_mass, float grav_const);

// update acceleration using gpu
void update_acc_tile_trans_gpu(size_t n, float *part_pos, float *part_vel,
        float *part_acc, float *part_mass, float grav_const);

#endif
