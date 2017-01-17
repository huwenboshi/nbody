#include <stdlib.h>

#ifndef SIM
#define SIM

// simulation parameters
const float npart = 8192*2;
const float screen_size = 800.0;
const float screen_center = 400.0;
const float screen_scale = 800.0;
const float mass_scale = 1.0;
const float delta_t = 0.01;
const float grav_const = 6.67384e-11;

// initialize particles, position and velocity
void init(size_t n, float *part_pos, float *part_vel,
        float *part_acc, float *part_mas);

// update particles
void update(size_t n, float *part_pos, float *part_vel,
        float *part_acc, float *part_mas, float *part_force);

#endif
