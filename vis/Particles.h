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
    void render() const;
    void step(int elapsed_time);
private:
    struct ParticleGridCube
    {
        double new_density;
        double density;
    };

    std::vector<ParticleGridCube> particles;
};

#endif /* PARTICLES_H */
