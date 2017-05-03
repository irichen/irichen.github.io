#include "Particles.h"

#define nx 400
#define ny 400

int smoke_x = 200;
int smoke_y = 200;

Particles::Particles()
{
    for(int x = 0; x < nx; x++)
    {
        for(int y = 0; y < ny; y++)
        {
          ParticleGridCube pgc;
          pgc.density = 0.0;
          pgc.vel_x = 2.0;
          pgc.vel_y = 0.0;
          particles.push_back(pgc);
        }
    }
}

int Particles::compute_row_major(int x, int y) const
{
  return x + nx * y;
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
    for(int y = 0; y < ny; y++)
    {
        for(int x = 0; x < nx; x++)
        {
            double intensity = fmin(1.0, particles[compute_row_major(x, y)].density / 2.0);
            if (intensity > 0.05) {
                glPushMatrix();
                glScalef(0.015, 0.015, 0.015);
                glTranslatef(-300, x - 200, y - 200);
                glColor4f(fabs(intensity - 0.5), intensity, 1.0 - intensity, 1.0);
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
    for(int y = 0; y < ny; y++)
    {
        for(int x = 0; x < nx; x++)
        {
          particles[compute_row_major(x, y)].new_density = 0;
        }
    }

    // Compute diffusion term to immediate neighbors
    for(int y = 0; y < ny; y++)
    {
        for(int x = 0; x < nx; x++)
        {
          int i = compute_row_major(x, y);

          // Compute neighbor indices into particles
          std::vector<int> diffuseTo;

          // Standard diffusion term
          if (y > 0) {
            diffuseTo.push_back(i - nx);
          }
          if (y < ny - 1) {
            diffuseTo.push_back(i + nx);
          }
          if (x > 0) {
            diffuseTo.push_back(i - 1);
          }
          if (x < nx - 1) {
            diffuseTo.push_back(i + 1);
          }
          // Compute amount to diffuse
          if (diffuseTo.size()) {
            double diffuseAmount = particles[i].density * 0.5 / diffuseTo.size();
            for (int j = 0; j < diffuseTo.size(); j++) {
              particles[diffuseTo[j]].new_density += diffuseAmount;
            }
            particles[i].density -= 1.01 * diffuseAmount * diffuseTo.size();
          }
        }
    }

    // Apply diffusion term and reset summation
    for(int y = 0; y < ny; y++)
    {
        for(int x = 0; x < nx; x++)
        {
          int i = compute_row_major(x, y);
          particles[i].density += particles[i].new_density;
          particles[i].new_density = 0;
        }
    }

    // Initialize diffusion lists to zero
    std::vector<std::vector<int>> diffuseFrom((nx * ny));
    for(int i = 0; i < nx * ny; ++i)
      diffuseFrom[i] = *(new std::vector<int>);

    // Compute velocity term, based on backtracing the velocity ray
    // based on (x, y).vel_x and vel_y
    for(int y = 0; y < ny; y++)
    {
        for(int x = 0; x < nx; x++)
        {

            // Compute j = i - velocity to backtrace, inserting current
            // gridsquare into target list of match
            int i = compute_row_major(x, y);
            int backtrace_x = x - (int)particles[i].vel_x;
            backtrace_x = std::max(0, std::min(nx - 1, backtrace_x));
            int backtrace_y = y - (int)particles[i].vel_y;
            backtrace_y = std::max(0, std::min(ny - 1, backtrace_y));
            diffuseFrom[compute_row_major(backtrace_x, backtrace_y)].push_back(i);
        }
    }

    // Complete backtrace by diffusing from gridsquare backtraced to
    // all backtracing gridsquares
    for(int y = 0; y < ny; y++)
    {
        for(int x = 0; x < nx; x++)
        {
            int i = compute_row_major(x, y);
            if (diffuseFrom[i].size()) {
                double diffuseAmount = particles[i].density / diffuseFrom[i].size();
                for (int j = 0; j < diffuseFrom[i].size(); j++) {
                  particles[diffuseFrom[i][j]].new_density += diffuseAmount;
                }
                particles[i].density -= 1.01 * diffuseAmount * diffuseFrom[i].size();
            }
        }
    }

    // Apply velocity term, reset sum
    for(int y = 0; y < ny; y++)
    {
        for(int x = 0; x < nx; x++)
        {
            int i = compute_row_major(x, y);
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
    //     if (smoke_y < ny - 3) {
    //       smoke_y += 3;
    //     }
    //     break;
    // }

    // Spawn smoke at smoke source
    for(int dx = smoke_x - 1; dx < smoke_x + 2; dx++)
    {
        for(int dy = smoke_y - 1; dy < smoke_y + 2; dy++)
        {
          if (dx > 0 && dx < nx - 1 && dy > 0 && dy < ny - 1) {
            particles[dx + nx * dy].density = 3.0;
          }
        }
    }
}
