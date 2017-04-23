#include "Particles.h"

Particles::Particles()
{
    int nx = 10;
    int ny = 10;
    int nz = 10;
    for(int x=0; x<nx; x++)
    {
        for(int y=0; y<ny; y++)
        {
            for(int z=0; z<nz; z++)
            {
              ParticleGridCube pgc;
              if ((x + y + z) < 5) {
                pgc.density = 5.0;
              } else {
                pgc.density = 0.0;
              }
              particles.push_back(pgc);
            }
        }
    }
}

void Particles::render() const
{
    int nx = 10;
    int ny = 10;
    int nz = 10;
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { 10.0, 10.0, 10.0, 0.0 };
    glShadeModel (GL_SMOOTH);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glColor3f(0.2, 0.5, 0.8);
    glColorMaterial(GL_FRONT, GL_SPECULAR);
    glColor3f(0.9, 0.9, 0.9);
    glColorMaterial(GL_FRONT, GL_AMBIENT);
    glColor3f(0.2, 0.5, 0.8);
    for(int x=0; x<nx; x++)
    {
        for(int y=0; y<ny; y++)
        {
            for(int z=0; z<nz; z++)
            {
              glPushMatrix();
              glTranslatef(x, y, z);
              glutSolidSphere(particles[x * ny * nz + y * nz + z].density / 50, 10, 10);
              glPopMatrix();
            }
        }
    }

    glPopAttrib();
}

void Particles::step(int elapsed_time)
{
  int nx = 10;
  int ny = 10;
  int nz = 10;
  for(int x=0; x<nx; x++)
  {
      for(int y=0; y<ny; y++)
      {
          for(int z=0; z<nz; z++)
          {
            int i = x * ny * nz + y * nz + z;
            particles[i].new_density = 0;
          }
      }
  }
  for(int x=0; x<nx; x++)
  {
      for(int y=0; y<ny; y++)
      {
          for(int z=0; z<nz; z++)
          {
            int i = x * ny * nz + y * nz + z;
            std::vector<int> diffuseTo;
            if (x > 0) {
              diffuseTo.push_back(i + ny * nz);
            }
            if (x < nx - 1) {
              diffuseTo.push_back(i - ny * nz);
            }
            if (y > 0) {
              diffuseTo.push_back(i + ny);
            }
            if (y < ny - 1) {
              diffuseTo.push_back(i - ny);
            }
            if (z > 0) {
              diffuseTo.push_back(i + 1);
            }
            if (z < nz - 1) {
              diffuseTo.push_back(i - 1);
            }
            for (int j = 0; j < diffuseTo.size(); j++) {
              particles[diffuseTo[j]].new_density += particles[i].density / diffuseTo.size();
            }
          }
      }
  }
  for(int x=0; x<nx; x++)
  {
      for(int y=0; y<ny; y++)
      {
          for(int z=0; z<nz; z++)
          {
            int i = x * ny * nz + y * nz + z;
            particles[i].density = particles[i].new_density;
          }
      }
  }
}
