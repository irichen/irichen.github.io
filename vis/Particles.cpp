#include "Particles.h"

#define nx 100
#define ny 100
#define nz 100

Particles::Particles()
{
    // for(int x=0; x<nx; x++)
    // {
        for(int y=0; y<ny; y++)
        {
            for(int z=0; z<nz; z++)
            {
              ParticleGridCube pgc;
              // if ((/**abs(x - 3) +*/ abs(y - 3) + abs(z - 3)) < 5) {
              //   pgc.density = 5.0;
              // } else {
                pgc.density = 0.0;
              // }
              particles.push_back(pgc);
            }
        }
    // }
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
    // for(int x=0; x<nx; x++)
    // {
        for(int y=0; y<ny; y++)
        {
            for(int z=0; z<nz; z++)
            {
              glPushMatrix();
              glScalef(0.05, 0.05, 0.05);
              glTranslatef(10, y - 50, z - 50);
              int i = /**x * ny * nz*/ + y * nz + z;
              double intensity = fmin(0.5, particles[i].density / 5.0);
              glColor4f(intensity, intensity, intensity, 1.0);
              glutSolidCube(0.99999);
              glPopMatrix();
            }
        }
    // }
    glDisable(GL_BLEND);

    glPopAttrib();
}

void Particles::step(int elapsed_time)
{
  // for(int x=0; x<nx; x++)
  // {
      for(int y=0; y<ny; y++)
      {
          for(int z=0; z<nz; z++)
          {
            int i = /**x * ny * nz*/ + y * nz + z;
            particles[i].new_density = 0;
          }
      }
  // }
  // for(int x=0; x<nx; x++)
  // {
      for(int y=0; y<ny; y++)
      {
          for(int z=0; z<nz; z++)
          {
            int i = /**x * ny * nz*/ + y * nz + z;

            // Compute neighbor indices into particles
            std::vector<int> diffuseTo;
            // if (x > 0) {
            //   diffuseTo.push_back(i - ny * nz);
            // }
            // if (x < nx - 1) {
            //   diffuseTo.push_back(i + ny * nz);
            // }
            if (y > 0) {
              diffuseTo.push_back(i - ny);
            }
            if (y < ny - 1) {
              diffuseTo.push_back(i + ny);
              diffuseTo.push_back(i + ny);
              diffuseTo.push_back(i + ny);
              diffuseTo.push_back(i + ny);
            }
            if (z > 0) {
              diffuseTo.push_back(i - 1);
            }
            if (z < nz - 1) {
              diffuseTo.push_back(i + 1);
            }
            // Compute amount to diffuse
            double diffuseAmount = particles[i].density * 0.3 / diffuseTo.size();
            for (int j = 0; j < diffuseTo.size(); j++) {
              particles[diffuseTo[j]].new_density += diffuseAmount;
            }
            particles[i].density -= 1.03 * diffuseAmount * diffuseTo.size();
          }
      }
  // }
  double t_density = 0.0;
  // for(int x=0; x<nx; x++)
  // {
      for(int y=0; y<ny; y++)
      {
          for(int z=0; z<nz; z++)
          {
            int i = /**x * ny * nz +*/ y * nz + z;
            particles[i].density += particles[i].new_density;
            t_density += particles[i].density;
          }
      }
      int smoke_center_y = (int)((double)rand() / RAND_MAX * ny);
      int smoke_center_z = (int)((double)rand() / RAND_MAX * nz);
      for(int dy = smoke_center_y - 3; dy < smoke_center_y + 4; dy++)
      {
          for(int dz = smoke_center_z - 3; dz < smoke_center_z + 4; dz++)
          {
            if (dy > 0 && dy < ny - 1 && dz > 0 && dz < nz - 1) {
              particles[dy * nz + dz].density = 3.0;
            }
          }
      }
  // }
  // printf("%f\n", (float)t_density);
}
