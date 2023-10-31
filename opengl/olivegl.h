#ifndef OLIVEGL_H_INCLUDED
#define OLIVEGL_H_INCLUDED

#include <GL/gl.h>
#include "../include/gfx.h"

void ol_scale(GLfloat vertices[], float x, int length)
{
    for(int i = 0;i <= length;i++)
        vertices[i] *= x;
}

GLuint ol_texturegl()
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return textureID;
}

void ol_displaygl(OlWindow win)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, win.w, win.h, 0, GL_RGB, GL_UNSIGNED_BYTE, win.front);
}

OlWindow ol_capturegl(int width, int height)
{
    WIDTH = width, HEIGHT = height;
    OlWindow win = ol_new_win();
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_INT, win.front);
    return win;
}

void ol_pushgl(OlWindow ctx)
{
    glDrawPixels(ctx.w, ctx.h, GL_RGB, GL_BITMAP, ctx.front);
}

void ol_loadbmp(char* fname, OlWindow* ctx)
{
    if(ctx == NULL) return;
    FILE* fp = fopen(fname, "r");
    fread(ctx->front, 1, ctx->w * ctx->h, fp);
    fclose(fp);
}

#ifdef __GLUT__
#include "oliveglut.h"
#elif defined(_WIN32)
#include <windows.h>

HWND ol_create_win(char* name, int width, int height)
{
    return CreateWindowExA(0,
                          "GLSample",
                          name,
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          width,
                          height,
                          NULL,
                          NULL,
                          NULL,
                          NULL);
}

int ol_register_win(HINSTANCE hInstance)
{
    WNDCLASSEXA wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = DefWindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassExA(&wcex))
        return 0;
}
#endif

#endif // OLIVEGL_H_INCLUDED