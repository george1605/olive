#include <windows.h>
#include "oliveglut.h"

void display() 
{
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_POLYGON);
    glVertex2f(-0.5, -0.5);
    glVertex2f(-0.5,  0.5);
    glVertex2f(0.5,  0.5);
    glVertex2f(0.5, -0.5);
    glEnd();

    glFlush();
}

void init() 
{
    glClearColor(0.000, 0.110, 0.392, 0.0); // JMU Gold

    glColor3f(0.314, 0.314, 0.000); // JMU Purple

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

void set(int p)
{
    if(p == 0)
        glutHideWindow();
}

int main(int argc, char** argv) 
{
    ol_glut_init(&argc, argv);
    ol_create_win("wow", 500, 500);
    glutDisplayFunc(display);
    char* x[] = {"Test", NULL};
    ol_glut_cmenu(x, set);
    init();
    glutMainLoop();
}  