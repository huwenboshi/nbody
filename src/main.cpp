#include <time.h>
#include "utils.h"
#include "sim.h"

int main() {

    // timing variable
    clock_t begin;
    clock_t end;
    float dt_ms;

    // timing information
    float tacc = 0.0;
    float tvel = 0.0;
    float tpos = 0.0;
    float gtacc = 0.0;

    // error meassure between host and device
    float diff_acc = 0.0;

    // simulation parameters
    const size_t npart = 16384;
    const size_t nsteps = 10;
    const float size_x = 1024.0;
    const float size_y = 512.0;
    const float center_x = size_x/2.0;
    const float center_y = size_y/2.0;
    const float scale_x = size_x;
    const float scale_y = size_y;
    const float delta_t = 1.0e2;
    const float scale_mass = 1.0e6;
    const float grav_const = 6.67384e-11;

    // initialize particles on host
    float *part_pos = (float*) safe_calloc(2*npart, sizeof(float));
    float *part_vel = (float*) safe_calloc(2*npart, sizeof(float));
    float *part_acc = (float*) safe_calloc(2*npart, sizeof(float));
    float *part_mass = (float*) safe_calloc(npart, sizeof(float));
    init(npart, part_pos, part_vel, part_acc, part_mass,
            scale_x, scale_y, center_x, center_y, scale_mass);

    // transpose part_pos, part_vel, part_acc
    float *part_pos_t = (float*) safe_calloc(2*npart, sizeof(float));
    float *part_vel_t = (float*) safe_calloc(2*npart, sizeof(float));
    float *part_acc_t = (float*) safe_calloc(2*npart, sizeof(float));
    for(size_t i=0; i<npart; i++) {
        part_pos_t[0*npart+i] = part_pos[2*i+0];
        part_pos_t[1*npart+i] = part_pos[2*i+1];
        part_vel_t[0*npart+i] = part_vel[2*i+0];
        part_vel_t[1*npart+i] = part_vel[2*i+1];
        part_acc_t[0*npart+i] = part_acc[2*i+0];
        part_acc_t[1*npart+i] = part_acc[2*i+1];
    }

    // copy particles to device
    float *gpart_pos;
    float *gpart_vel;
    float *gpart_acc;
    float *gpart_mass;
    cu_safe_falloc(&gpart_pos, 2*npart);
    cu_safe_falloc(&gpart_vel, 2*npart);
    cu_safe_falloc(&gpart_acc, 2*npart);
    cu_safe_falloc(&gpart_mass, npart);
    memcpy_htod(gpart_pos, part_pos_t, 2*npart);
    memcpy_htod(gpart_vel, part_vel_t, 2*npart);
    memcpy_htod(gpart_acc, part_acc_t, 2*npart);
    memcpy_htod(gpart_mass, part_mass, npart);

    // allocate space for host device comparison
    float *cmp_part_acc_t = (float*) safe_calloc(2*npart, sizeof(float));
    float *cmp_part_acc = (float*) safe_calloc(2*npart, sizeof(float));

    // run iterations
    for(size_t i=0; i<nsteps; i++) {

        // step 1 update acceleration on host
        begin = clock();
        update_acc(npart, part_pos, part_vel, part_acc,
                part_mass, grav_const);
        end = clock();
        dt_ms = ((float)(end-begin))/((float)(CLOCKS_PER_SEC)/1.0e9);
        tacc += dt_ms;

        // step 1 update acceleration on device
        begin = clock();
        update_acc_tile_trans_gpu(npart, gpart_pos, gpart_vel, gpart_acc,
                gpart_mass, grav_const);
        end = clock();
        dt_ms = ((float)(end-begin))/((float)(CLOCKS_PER_SEC)/1.0e9);
        gtacc += dt_ms;

        // step 1 verify result
        memcpy_dtoh(cmp_part_acc_t, gpart_acc, 2*npart);
        for(size_t j=0; j<npart; j++) {
            cmp_part_acc[2*j+0] = cmp_part_acc_t[0*npart+j];
            cmp_part_acc[2*j+1] = cmp_part_acc_t[1*npart+j];
        }
        for(size_t j=0; j<npart; j++) {
            float diffx = cmp_part_acc[2*j+0]-part_acc[2*j+0];
            float diffy = cmp_part_acc[2*j+1]-part_acc[2*j+1];
            diffx *= scale_mass;
            diffy *= scale_mass;
            diff_acc += diffx*diffx;
            diff_acc += diffy*diffy;
        }

        // step 2 update velocity
        begin = clock();
        update_vel(npart, part_vel, part_acc, delta_t);
        end = clock();
        dt_ms = ((float)(end-begin))/((float)(CLOCKS_PER_SEC)/1.0e9);
        tvel += dt_ms;

        // step 2 copy velocity from host to device
        for(size_t j=0; j<npart; j++) {
            part_vel_t[0*npart+j] = part_vel[2*j+0];
            part_vel_t[1*npart+j] = part_vel[2*j+1];
        }
        memcpy_htod(gpart_vel, part_vel_t, 2*npart);
   
        // step 3 update position
        begin = clock();
        update_pos(npart, part_pos, part_vel, delta_t);
        end = clock();
        dt_ms = ((float)(end-begin))/((float)(CLOCKS_PER_SEC)/1.0e9);
        tpos += dt_ms;

        // step 3 copy position from host to deice
        for(size_t j=0; j<npart; j++) {
            part_pos_t[0*npart+j] = part_pos[2*j+0];
            part_pos_t[1*npart+j] = part_pos[2*j+1];
        }
        memcpy_htod(gpart_pos, part_pos_t, 2*npart);
    }

    // print out timing information
    printf("update_acc: %fns/particle/step\n", tacc/npart/nsteps);
    printf("update_vel: %fns/particle/step\n", tvel/npart/nsteps);
    printf("update_pos: %fns/particle/step\n", tpos/npart/nsteps);

    // print out timing information for gpu
    printf("update_acc_gpu: %fns/particle/step\n", gtacc/npart/nsteps);

    // print out error information for gpu
    printf("err_acc: %f\n", diff_acc);
}
