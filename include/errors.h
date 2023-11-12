#include <stdio.h>
#include <stdlib.h>
#include "gfx.h"
#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

#define OL_ENOENT     0x1 // no entity, similar to E_NOENT
#define OL_EBADFD     0x2 // bad file descriptor
#define OL_ENULLP     0x4 // null pointer, if win.front == NULL
#define OL_EWIDTH     0x8 // width > uint16_t max or width == 0
#define OL_EHEIGHT    0x10 // same as OL_EWIDTH
#define OL_EBUFOVER   0x20 // buffer overflow
#define OL_ENOXDISP   0x40 // no X11 display, it is null
#define OL_ENETWSA    0x80 // winsock WSA error at init

#define OL_SCRNDIM   0x10 // checks if dimensions > screen w or h
#define OL_UINTMAX   0x20 // checks if is bigger than 65000
int ol_error = 0;

int ol_check_win(OlWindow win)
{
    if(win.front == NULL)
        return OL_ENULLP;
    int x = ((win.w == 0) ? OL_EWIDTH : 0);
    x |=  ((win.h == 0) ? OL_EHEIGHT : 0);
    return x;
}

int ol_check_dim(OlWindow win)
{
#ifdef _WIN32
#include <windows.h>
    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);
    return (win.w < w || win.h < h);
#endif
}

void* ol_check_file(FILE* fp)
{
    int f = ferror(fp);
    return strerror(f);
}