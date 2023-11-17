#include "gfx.c"
#ifdef _WIN32
#include <windows.h>
#else
#endif

#ifdef _USE_LEGACYGFX_ // only for legacy codebases (USE SDL/SFML/ETC. FOR MODERN APPS)
#include <graphics.h>
#define OL_WAITKEY() while(!kbhit())
/*
 * Returns coordinates of mouse click, or (0, 0) if no such
 * event occured.
 */
OlPoint ol_bgi_getclick()
{
    int x = 0, y = 0;
    if(ismouseclick(WM_LBUTTONDOWN))
    {
            getmouseclick(WM_LBUTTONDOWN, &x, &y);
    }
    OlPoint point;
    point.x = x;
    point.y = y;
    return point;
}

OlPoint ol_bgi_waitclick()
{
    while(!ismouseclick(WM_LBUTTONDOWN))
    {
            delay(500);
    }
    OlPoint point;
    getmouseclick(WM_LBUTTONDOWN, &(point.x), &(point.y));
    return point;
}

void ol_bgi_display(OlWindow win)
{
    putimage(0, 0, win.front, COPY_PUT);
}

OlWindow ol_bgi_capture(int x, int y, int w, int h)
{
    OlWindow win;
    win.front = malloc(32 * w * h);
    win.w = w;
    win.h = h;
    getimage(x, y, x + w, y + h, win.front);
    return win;
}

OlWindow ol_bgi_captureall()
{
    return ol_bgi_capture(0, 0, getmaxx(), getmaxy());
}
#endif

HBITMAP ol_win_flush(OlWindow ol, HDC dc)
{
    if(dc == NULL)
        dc = GetDC(GetForegroundWindow());
    HBITMAP btmp = CreateCompatibleBitmap(dc, ol.w, ol.h);
    SetBitmapBits(btmp, ol.w * ol.h, ol.front);
    SelectObject(dc, btmp);
    return btmp;
}

HWND ol_find_hwnd(const char* name)
{
    return FindWindowA(NULL, name);
}

void ol_capture_dc(HDC dc)
{

}

void ol_set_dc(OlWindow ol, HDC dc)
{
    if (dc == 0)
        dc = GetDC(GetForegroundWindow());
    // free(ol.front);
    ol.front = (u8*)dc;
}

int ol_alloc_cons()
{
    if(!AllocConsole())
        return GetLastError();
    return 0;
}
