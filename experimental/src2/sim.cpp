#include "sim.h"
#include "utils.h"
#include <math.h>

// initialize particles, position and velocity
void init(size_t n, float *part_pos, float *part_vel,
        float *part_acc, float *part_mas) {

    // initialize uniform position
    randn_bistd(n/4, part_pos);
    for(size_t i=0; i<n/4; i++) {
        part_pos[2*i+0] /= 8192.0;
        part_pos[2*i+1] /= 8192.0;
        
        part_vel[2*i+0] = part_pos[2*i+0]*65536*2;
        part_vel[2*i+1] = part_pos[2*i+1]*65536*2;

        part_pos[2*i+0] *= screen_scale;
        part_pos[2*i+1] *= screen_scale;
        part_pos[2*i+0] += screen_center-screen_center/2;
        part_pos[2*i+1] += screen_center-screen_center/2;
    }

    randn_bistd(n/4, &part_pos[n/2]);
    for(size_t i=n/4; i<n/2; i++) {
        part_pos[2*i+0] /= 8192.0;
        part_pos[2*i+1] /= 8192.0;
        
        part_vel[2*i+0] = part_pos[2*i+0]*65536*2;
        part_vel[2*i+1] = part_pos[2*i+1]*65536*2;

        part_pos[2*i+0] *= screen_scale;
        part_pos[2*i+1] *= screen_scale;
        part_pos[2*i+0] += screen_center+screen_center/2;
        part_pos[2*i+1] += screen_center+screen_center/2;
    }
    
    randn_bistd(n/4, &part_pos[n]);
    for(size_t i=n/2; i<3*n/4; i++) {
        part_pos[2*i+0] /= 8192.0;
        part_pos[2*i+1] /= 8192.0;
        
        part_vel[2*i+0] = part_pos[2*i+0]*65536*2;
        part_vel[2*i+1] = part_pos[2*i+1]*65536*2;

        part_pos[2*i+0] *= screen_scale;
        part_pos[2*i+1] *= screen_scale;
        part_pos[2*i+0] += screen_center+screen_center/2;
        part_pos[2*i+1] += screen_center-screen_center/2;
    }

    randn_bistd(n/4, &part_pos[3*n/2]);
    for(size_t i=3*n/4; i<n; i++) {
        part_pos[2*i+0] /= 8192.0;
        part_pos[2*i+1] /= 8192.0;
        
        part_vel[2*i+0] = part_pos[2*i+0]*65536*2;
        part_vel[2*i+1] = part_pos[2*i+1]*65536*2;

        part_pos[2*i+0] *= screen_scale;
        part_pos[2*i+1] *= screen_scale;
        part_pos[2*i+0] += screen_center-screen_center/2;
        part_pos[2*i+1] += screen_center+screen_center/2;
    }

    // initialize acceleration to 0
    for(size_t i=0; i<n; i++) {
        part_acc[2*i+0] = 0.0;
        part_acc[2*i+1] = 0.0;
    }

    // initialize mass to uniform
    for(size_t i=0; i<n; i++) {
        part_mas[i] = randu();
    }
}

// update particles
void update(size_t n, float *part_pos, float *part_vel,
        float *part_acc, float *part_mas, float *part_force) {

    // compute gravity
    for(size_t i=0; i<n; i++) {
        
        float xi = part_pos[2*i+0];
        float yi = part_pos[2*i+1];
        float mi = part_mas[i];

        part_force[2*i+0] = 0.0;
        part_force[2*i+1] = 0.0;

        // aggregate gravity
        for(size_t j=0; j<n; j++) {
            
            float xj = part_pos[2*j+0];
            float yj = part_pos[2*j+1];
            float mj = part_mas[j];
            
            if(i != j) {
                float dx = xj-xi;
                float dy = yj-yi;
                float r = sqrt(dx*dx+dy*dy);
                r += 1.5;

                // update force
                float f = mi*mj/(r*r);
                float fx = f*(dx/r);
                float fy = f*(dy/r);
                part_force[2*i+0] += fx;
                part_force[2*i+1] += fy;
            }

        }
    }

    // compute acceleration
    for(size_t i=0; i<n; i++) {
        float mi = part_mas[i];
        part_acc[2*i+0] = part_force[2*i+0]/mi;
        part_acc[2*i+1] = part_force[2*i+1]/mi;
    }

    // update velocity
    for(size_t i=0; i<n; i++) {
        part_vel[2*i+0] += part_acc[2*i+0]*delta_t;
        part_vel[2*i+1] += part_acc[2*i+1]*delta_t;
    }

    // update position
    for(size_t i=0; i<n; i++) {
        part_pos[2*i+0] += part_vel[2*i+0]*delta_t;
        part_pos[2*i+1] += part_vel[2*i+1]*delta_t;
    }
}
