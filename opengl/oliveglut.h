#ifndef __OLIVE_GLUT__
#define __OLIVE_GLUT__
#include <gl/glut.h>

int ol_create_win(char* name, int width, int height)
{
    glutInitWindowSize(width, height);
    glutInitWindowPosition(0, 0);
    return glutCreateWindow(name);
}

void ol_glut_init(int* argc, char** argv)
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
}

int ol_contextgl(int win, int x, int y, int w, int h, void(*draw)())
{
    if(!win)
        return;
    int x = glutCreateSubWindow(win, x, y, w, h);
    glutDisplayFunc(draw);
    return x;
}

#endif 