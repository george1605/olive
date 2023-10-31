#include "gfx.c"
#ifdef _WIN32
#include <windows.h>
#else
#error "Not an Win32 Platform!"
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
