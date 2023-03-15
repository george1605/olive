#ifndef OLIVEGL_H_INCLUDED
#define OLIVEGL_H_INCLUDED

#include <windows.h>
#include <GL/gl.h>
#include "../gfx.c"

typedef struct { float x, y, z; } Ol3fPoint;

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

void ol_run_frame(void(*frame)())
{
    glPushMatrix();
    frame();
    glPopMatrix();
}

void ol_vertices(Ol3fPoint* p, int size)
{
    for(int i = 0;i < size;i++)
        glVertex3d(p[i].x, p[i].y, p[i].z);
}

#ifdef _WIN32
HWND ol_create_win(char* name, int width, int height)
{
    return CreateWindowExA(0,
                          "GLWindow",
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
#endif

#endif // OLIVEGL_H_INCLUDED