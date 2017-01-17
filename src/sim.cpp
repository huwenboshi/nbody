#include <math.h>
#include "sim.h"
#include "utils.h"

// initialize particles, position and velocity
void init(size_t n, float *part_pos, float *part_vel,
        float *part_acc, float *part_mass,
        float scale_x, float scale_y,
        float center_x, float center_y, float scale_mass) {

    // initialize uniform position 
    for(size_t i=0; i<n; i++) {
        part_pos[2*i+0] = randu()-0.5;
        part_pos[2*i+1] = randu()-0.5;
        part_pos[2*i+0] *= scale_x;
        part_pos[2*i+1] *= scale_y;
        part_pos[2*i+0] += center_x;
        part_pos[2*i+1] += center_y;
    }

    // initialize velocity to 0
    for(size_t i=0; i<n; i++) {
        part_vel[2*i+0] = 0.0;
        part_vel[2*i+1] = 0.0;
    }

    // initialize acceleration to 0
    for(size_t i=0; i<n; i++) {
        part_acc[2*i+0] = 0.0;
        part_acc[2*i+1] = 0.0;
    }

    // initialize mass to uniform
    for(size_t i=0; i<n; i++) {
        part_mass[i] = randu();
        part_mass[i] *= scale_mass;
    }
}

// update particle acceleration
void update_acc(size_t n, float *part_pos, float *part_vel,
        float *part_acc, float *part_mass, float grav_const) {
    
    for(size_t i=0; i<n; i++) {
        
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

// update particle velocity
void update_vel(size_t n, float *part_vel, float *part_acc,
        float delta_t) {
    for(size_t i=0; i<n; i++) {
        part_vel[2*i+0] += part_acc[2*i+0]*delta_t;
        part_vel[2*i+1] += part_acc[2*i+1]*delta_t;
    }
}

// update particle position
void update_pos(size_t n, float *part_pos, float *part_vel,
        float delta_t) {
    for(size_t i=0; i<n; i++) {
        part_pos[2*i+0] += part_vel[2*i+0]*delta_t;
        part_pos[2*i+1] += part_vel[2*i+1]*delta_t;
    }
}
