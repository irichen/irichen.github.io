#include "Particles.h"

#define nx 400
#define ny 400
#define nz 400

int smoke_x = 200;
int smoke_y = 200;

Particles::Particles()
{
    for(int y=0; y<ny; y++)
    {
        for(int z=0; z<nz; z++)
        {
          ParticleGridCube pgc;
          pgc.density = 0.0;
          pgc.vel_x = 0.0;
          pgc.vel_y = 2.0;
          particles.push_back(pgc);
        }
    }
}

void Particles::render() const
{
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { -10.0, -10.0, -10.0, 0.0 };
    glShadeModel (GL_SMOOTH);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Generate a cube at (x, y), colored based on the density there.
    for(int y=0; y<ny; y++)
    {
        for(int z=0; z<nz; z++)
        {
            int i = y * nz + z;
            double intensity = fmin(1.0, particles[i].density / 2.0);
            if (intensity > 0.05) {
                glPushMatrix();
                glScalef(0.015, 0.015, 0.015);
                glTranslatef(10, y - 200, z - 200);
                glColor4f(0.0, intensity, 1.0 - intensity, 1.0);
                glutSolidCube(0.99999);
                glPopMatrix();
            }
        }
    }
    glDisable(GL_BLEND);

    glPopAttrib();
}

void Particles::step(int elapsed_time)
{
    // Initialize density updates to zero
    for(int y=0; y<ny; y++)
    {
        for(int z=0; z<nz; z++)
        {
          int i = /**x * ny * nz*/ + y * nz + z;
          particles[i].new_density = 0;
        }
    }

    // Compute diffusion term to immediate neighbors
    for(int y=0; y<ny; y++)
    {
        for(int z=0; z<nz; z++)
        {
          int i = y * nz + z;

          // Compute neighbor indices into particles
          std::vector<int> diffuseTo;

          // Standard diffusion term
          if (y > 0) {
            diffuseTo.push_back(i - ny);
          }
          if (y < ny - 1) {
            diffuseTo.push_back(i + ny);
          }
          if (z > 0) {
            diffuseTo.push_back(i - 1);
          }
          if (z < nz - 1) {
            diffuseTo.push_back(i + 1);
          }
          // Compute amount to diffuse
          double diffuseAmount = particles[i].density * 0.5 / diffuseTo.size();
          for (int j = 0; j < diffuseTo.size(); j++) {
            particles[diffuseTo[j]].new_density += diffuseAmount;
          }
          particles[i].density -= 1.01 * diffuseAmount * diffuseTo.size();
        }
    }

    // Apply diffusion term and reset summation
    for(int y=0; y<ny; y++)
    {
        for(int z=0; z<nz; z++)
        {
          int i = y * nz + z;
          particles[i].density += particles[i].new_density;
          particles[i].new_density = 0;
        }
    }

    // Initialize diffusion lists to zero
    std::vector<std::vector<int>> diffuseFrom((ny * nz));
    for(int i = 0; i < ny * nz; ++i)
      diffuseFrom[i] = *(new std::vector<int>);

    // Compute velocity term, based on backtracing the velocity ray based on (x, y).vel_x and vel_y
    for(int y=0; y<ny; y++)
    {
        for(int z=0; z<nz; z++)
        {
            // Compute j = i - velocity to backtrace, inserting current gridsquare into target list of match
            int i = y * nz + z;
            int j = i - (int)particles[i].vel_x - (int)particles[i].vel_y * nz;
            if (j >= 0 && j < ny * nz) {
              diffuseFrom[j].push_back(i);
            }
        }
    }

    // Complete backtrace by diffusing from gridsquare backtraced to to all backtracing gridsquares
    for (int y=0; y<ny; y++)
    {
        for (int z=0; z<nz; z++)
        {
            int i = y * nz + z;
            double diffuseAmount = particles[i].density / diffuseFrom[i].size();
            for (int j = 0; j < diffuseFrom[i].size(); j++) {
              particles[diffuseFrom[i][j]].new_density += diffuseAmount;
            }
            particles[i].density -= 1.01 * diffuseAmount * diffuseFrom[i].size();
        }
    }

    // Apply velocity term, reset sum
    for(int y=0; y<ny; y++)
    {
        for(int z=0; z<nz; z++)
        {
            int i = y * nz + z;
            particles[i].density += particles[i].new_density;
            particles[i].new_density = 0;
        }
    }

    // Control random movement of smoke source
    // int smoke_move = (int)((double)rand() / RAND_MAX * 4);
    // switch (smoke_move) {
    //   case 0:
    //     if (smoke_x >= 3) {
    //       smoke_x -= 3;
    //     }
    //     break;
    //   case 1:
    //     if (smoke_y < nz - 3) {
    //       smoke_y += 3;
    //     }
    //     break;
    // }

    // Spawn smoke at smoke source
    for(int dy = smoke_x - 1; dy < smoke_x + 2; dy++)
    {
        for(int dz = smoke_y - 1; dz < smoke_y + 2; dz++)
        {
          if (dy > 0 && dy < ny - 1 && dz > 0 && dz < nz - 1) {
            particles[dy * nz + dz].density = 3.0;
          }
        }
    }
}
