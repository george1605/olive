#ifndef __OLIVE_GLUT__
#define __OLIVE_GLUT__
#include <gl/glut.h>
#include "../include/gfx.c"

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

void ol_save_screen(char* filename)
{
    ol_setup(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    OlWindow win = ol_new_win();
    glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, win.front);
    ol_save_ppm(filename, win);
    free(win.front);
}

int ol_contextgl(int win, int x, int y, int w, int h, void(*draw)())
{
    if(!win)
        return;
    int _x = glutCreateSubWindow(win, x, y, w, h);
    glutDisplayFunc(draw);
    return _x;
}

int ol_glut_menu(char** opt, void(*f)(int))
{
    int i = glutCreateMenu(f), j = 0;
    for(;opt[j] != NULL;j++)
        glutAddMenuEntry(opt[j], j);
    return i;
}

int ol_glut_cmenu(char** opt, void(*f)(int))
{
    int k = ol_glut_menu(opt, f);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    return k;
}

#endif 