#define OUTPUT_ANIMATION 0

#include <stdlib.h>
#include <stdio.h>

#include "Particles.h"

#if OUTPUT_ANIMATION
#include <opencv2/opencv.hpp>
#endif


inline float clip(const float& n, const float& lower, const float& upper)
{
    return glm::max(lower, glm::min(n, upper));
}

float theta = M_PI;
float phi = M_PI_2;
float dist = 2.5;
int width = 800;
int height = 800;
int frame = 0;
const int render_step = 3;
bool mouse_down = false;
int color_opt = 1;
int size = 10;
bool pause = false;

Particles particles;

void display(void);

void reshape(int width, int height);

void idle(void)
{
    if (!pause) {
      particles.step();
    }
    glutPostRedisplay();
    if(frame/render_step >= 300)
        return;
    if(frame%render_step == 0)
    {
        #if OUTPUT_ANIMATION
        cv::Mat3b image(height, width);
        glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, image.data);
        cv::flip(image, image, 0);
        char fn[512];
        sprintf(fn, "result/%04d.png", frame/render_step);
        cv::imwrite(fn, image);
        #endif
    }
    frame++;
}

void mouse(int button, int state, int x, int y);

void mouse_motion(int x, int y);

void motion(int x, int y);

void keyboard(unsigned char c, int x, int y)
{
    switch(c)
    {
      case 'q':
          color_opt = 1;
          break;
      case 'w':
          color_opt = 2;
          break;
      case 'e' :
          color_opt = 3;
          break;
      case 'r' :
          color_opt = 4;
          break;
      case 'u' :
          particles.set_vel_field(1);
          break;
      case 'i' :
          particles.set_vel_field(2);
          break;
      case 'o' :
          particles.set_vel_field(3);
          break;
      case 'p' :
          particles.set_vel_field(4);
          break;
      case 't' :
          size++;
          break;
      case 'y' :
          size--;
          break;
      case 'a' :
          pause = !pause;
          break;
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(width, height);

    (void)glutCreateWindow("GLUT Program");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

    return EXIT_SUCCESS;
}

void reshape(int w, int h)
{
    width = w;
    height = h;
    glViewport(0, 0, w, h);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // your drawing code goes here
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, 1, 0.01, 100);
    gluLookAt(dist*sin(phi)*cos(theta), dist*cos(phi), dist*sin(phi)*sin(theta),
            0, 0, 0,
            0, 1, 0);
    particles.render(color_opt);
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        particles.spawn_smoke(1.0 - (double)y / glutGet(GLUT_WINDOW_HEIGHT), (double)x / glutGet(GLUT_WINDOW_WIDTH), size);
    }
}

void motion(int x, int y)
{
    particles.spawn_smoke(1.0 - (double)y / glutGet(GLUT_WINDOW_HEIGHT), (double)x / glutGet(GLUT_WINDOW_WIDTH), size);
}
