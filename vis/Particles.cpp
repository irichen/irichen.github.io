#include "Particles.h"

#define nx 400
#define ny 400
#define scale 0.0135

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
          pgc.vel_x = 1.5;
          // pgc.vel_x = ((double)rand() / RAND_MAX * 3) - 0.25;
          pgc.vel_y = ((double)rand() / RAND_MAX * 3) - 1.5;
          // int cx = 200;
          // int cy = 200;
          // if (abs(x - cx) + abs(y - cy) < 10) {
          //   int d_cx = x - cx;
          //   int d_cy = y - cy;
          //   pgc.vel_y = d_cx / 10;
          //   pgc.vel_x = -d_cy / 10;
          // }
          particles.push_back(pgc);
        }
    }
}

int Particles::compute_row_major(int x, int y) const
{
  return x + nx * y;
}

double Particles::lerp(double x, double v0, double v1) const
{
  return v0 + x * (v1 - v0);
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
            int i = compute_row_major(x, y);
            double intensity = fmin(0.4, particles[i].density / 2.0);
            if (intensity > 0.01) {
                glPushMatrix();
                glScalef(scale, scale, scale);
                glTranslatef(10, x - (nx / 2), y - (ny / 2));
                glColor4f(intensity, intensity, intensity, 1.0);
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
    for (int k = 0; k < 20; k++)
    {
        for(int y = 1; y < ny - 1; y++)
        {
            for(int x = 1; x < nx - 1; x++)
            {
                int i = compute_row_major(x, y);
                particles[i].new_density = particles[i].density + 0.01 * (particles[i + nx].new_density + particles[i - nx].new_density + particles[i + 1].new_density + particles[i - 1].new_density) / 1.04;
            }
        }
    }

    // Initialize density updates to zero
    for(int y = 0; y < ny; y++)
    {
        for(int x = 0; x < nx; x++)
        {
          int i = compute_row_major(x, y);
          particles[i].density = particles[i].new_density;
          particles[i].new_density = 0;
        }
    }

    // Compute diffusion term to immediate neighbors
    // for(int y = 0; y < ny; y++)
    // {
    //     for(int x = 0; x < nx; x++)
    //     {
    //       int i = compute_row_major(x, y);
    //       if (particles[i].density > 0.01) {
    //         // Compute neighbor indices into particles
    //         std::vector<int> diffuseTo;
    //
    //         // Standard diffusion term
    //         if (y > 0) {
    //           diffuseTo.push_back(i - nx);
    //         }
    //         if (y < ny - 1) {
    //           diffuseTo.push_back(i + nx);
    //         }
    //         if (x > 0) {
    //           diffuseTo.push_back(i - 1);
    //         }
    //         if (x < nx - 1) {
    //           diffuseTo.push_back(i + 1);
    //         }
    //         // Compute amount to diffuse
    //         if (diffuseTo.size()) {
    //           double diffuseAmount = particles[i].density * 0.5 / diffuseTo.size();
    //           for (int j = 0; j < diffuseTo.size(); j++) {
    //             particles[diffuseTo[j]].new_density += diffuseAmount;
    //           }
    //           particles[i].density -= 1.05 * diffuseAmount * diffuseTo.size();
    //         }
    //       } else {
    //         particles[i].density = 0.0;
    //       }
    //     }
    // }
    //
    // // Apply diffusion term and reset summation
    // for(int y = 0; y < ny; y++)
    // {
    //     for(int x = 0; x < nx; x++)
    //     {
    //       int i = compute_row_major(x, y);
    //       particles[i].density += particles[i].new_density;
    //       particles[i].new_density = 0;
    //     }
    // }



    // Compute velocity term, based on backtracing the velocity ray
    // based on (x, y).vel_x and vel_y
    for(int y = 0; y < ny; y++)
    {
        for(int x = 0; x < nx; x++)
        {
            // Compute j = i - velocity to backtrace, inserting current
            // gridsquare into target list of match
            int i = compute_row_major(x, y);
            double backtrace_x = x - particles[i].vel_x;
            backtrace_x = fmax(1, fmin(nx - 2, backtrace_x));
            double backtrace_y = y - particles[i].vel_y;
            backtrace_y = fmax(1, fmin(ny - 2, backtrace_y));
            double s = backtrace_x - (int)backtrace_x;
            double t = backtrace_y - (int)backtrace_y;
            double u00 = particles[compute_row_major((int)backtrace_x, (int)backtrace_y)].density;
            double u01 = particles[compute_row_major((int)backtrace_x, (int)backtrace_y + 1)].density;
            double u10 = particles[compute_row_major((int)backtrace_x + 1, (int)backtrace_y)].density;
            double u11 = particles[compute_row_major((int)backtrace_x + 1, (int)backtrace_y + 1)].density;
            double u0 = lerp(s, u00, u10);
            double u1 = lerp(s, u01, u11);
            particles[i].density = 0.98 * lerp(t, u0, u1);
        }
    }

    // for (int k = 0; k < 20; k++)
    // {
    //     for(int y = 1; y < ny - 1; y++)
    //     {
    //         for(int x = 1; x < nx - 1; x++)
    //         {
    //             int i = compute_row_major(x, y);
    //             particles[i].new_vel_x = particles[i].vel_x + 0.01 * (particles[i + nx].new_vel_x + particles[i - nx].new_vel_x + particles[i + 1].new_vel_x + particles[i - 1].new_vel_x) / 1.04;
    //         }
    //     }
    // }
    //
    // // Initialize density updates to zero
    // for(int y = 0; y < ny; y++)
    // {
    //     for(int x = 0; x < nx; x++)
    //     {
    //       int i = compute_row_major(x, y);
    //       particles[i].density = particles[i].new_density;
    //       particles[i].new_density = 0;
    //     }
    // }

    // Diffuse the velocity field


    // Self-advect the velocity field
    // for(int y = 0; y < ny; y++)
    // {
    //     for(int x = 0; x < nx; x++)
    //     {
    //         // Compute j = i - velocity to backtrace, inserting current
    //         // gridsquare into target list of match
    //         int i = compute_row_major(x, y);
    //         double backtrace_x = x - particles[i].vel_x;
    //         backtrace_x = fmax(1, fmin(nx - 2, backtrace_x));
    //         double backtrace_y = y - particles[i].vel_y;
    //         backtrace_y = fmax(1, fmin(ny - 2, backtrace_y));
    //         double s = backtrace_x - (int)backtrace_x;
    //         double t = backtrace_y - (int)backtrace_y;
    //         double u00_x = particles[compute_row_major((int)backtrace_x, (int)backtrace_y)].vel_x;
    //         double u01_x = particles[compute_row_major((int)backtrace_x, (int)backtrace_y + 1)].vel_x;
    //         double u10_x = particles[compute_row_major((int)backtrace_x + 1, (int)backtrace_y)].vel_x;
    //         double u11_x = particles[compute_row_major((int)backtrace_x + 1, (int)backtrace_y + 1)].vel_x;
    //         double u0_x = lerp(s, u00_x, u10_x);
    //         double u1_x = lerp(s, u01_x, u11_x);
    //         particles[i].vel_x = lerp(t, u0_x, u1_x);
    //         double u00_y = particles[compute_row_major((int)backtrace_x, (int)backtrace_y)].vel_y;
    //         double u01_y = particles[compute_row_major((int)backtrace_x, (int)backtrace_y + 1)].vel_y;
    //         double u10_y = particles[compute_row_major((int)backtrace_x + 1, (int)backtrace_y)].vel_y;
    //         double u11_y = particles[compute_row_major((int)backtrace_x + 1, (int)backtrace_y + 1)].vel_y;
    //         double u0_y = lerp(s, u00_y, u10_y);
    //         double u1_y = lerp(s, u01_y, u11_y);
    //         particles[i].vel_y = lerp(t, u0_y, u1_y);
    //     }
    // }
}

void Particles::spawn_smoke(double dx, double dy)
{
    int dx_nx = (int)(dx * nx);
    int dy_ny = (int)(dy * ny);
    int min_dx_nx = std::max(0, dx_nx - 12);
    int max_dx_nx = std::min(nx - 1, dx_nx + 12);
    int min_dy_ny = std::max(0, dy_ny - 12);
    int max_dy_ny = std::min(ny - 1, dy_ny + 12);
    for(int x = min_dx_nx; x < max_dx_nx; x++)
    {
        for(int y = min_dy_ny; y < max_dy_ny; y++)
        {
          if (abs(x - dx_nx) + abs(y - dy_ny) < 12) {
              particles[compute_row_major(x, y)].density = 5.0;
          }
        }
    }
}
