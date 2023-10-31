#ifndef __X11__
#define __X11__
#include "../include/gfx.c"
#include "../include/errors.h"

#if !defined(__linux__) || !defined(__UNIX__)
#error "Not an unix system"
#endif

#include <X11/X11lib.h>

// display context -> win
void ol_draw_x11(Display* dis, Window win, OlWindow context)
{
    XImage *ximage;
    ximage = XCreateImage(display, DefaultVisual(display, DefaultScreen(display)), 24,
                                  ZPixmap, 0, (char *)rgba_data, context.w, context.h, 32, 0);
    XPutImage(display, window, DefaultGC(display, DefaultScreen(display)), ximage, 0, 0, 0, 0, context.w, context.h);
    XDestroyImage(ximage);
}

Window ol_newwin_x11(char* fname, int w, int h)
{
    Display* display;
    Window window;
    display = XOpenDisplay(NULL);
    if (!display) {
        ol_error = OL_ENOXDISP;
        return None;
    }

    XCreateSimpleWindow(display, RootWindow(display, DefaultScreen(display)),
                                 0, 0, w, h, 0, BlackPixel(display, DefaultScreen(display)),
                                 WhitePixel(display, DefaultScreen(display)));
    XMapWindow(display, window);
    return window;
}

#endif