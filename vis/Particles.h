/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Particles.h
 * Author: swl
 *
 * Created on April 15, 2016, 12:16 PM
 */

#ifndef PARTICLES_H
#define PARTICLES_H

#include <glm/glm.hpp>
#include <vector>
#if defined(__APPLE_CC__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <math.h>
#endif

class Particles {
public:
    Particles();
    int compute_row_major(int x, int y) const;
    double lerp(double x, double v0, double v1) const;
    void render(int color_opt) const;
    void step();
    void spawn_smoke(double dx, double dy, int size);
    void set_vel_field(int preset);
    void clear();

private:
    struct ParticleGridCube
    {
        double new_density;
        double density;
        double new_vel_x;
        double vel_x;
        double new_vel_y;
        double vel_y;
    };

    std::vector<ParticleGridCube> particles;
};

#endif /* PARTICLES_H */
