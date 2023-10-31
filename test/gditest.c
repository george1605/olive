#include <windows.h>
#include "../include/gdi.c"

int main() {
    // Create a window and get an HDC (for demonstration purposes)
    HWND hwnd = ol_find_hwnd("Firefox");
    HDC hdc = GetDC(hwnd);

    // Define the region you want to capture (coordinates and size)
    int x = 0;
    int y = 0;
    int width = 800;
    int height = 600;

    // Create a compatible memory DC and bitmap
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
    SelectObject(memDC, hBitmap);

    // Use BitBlt to copy the content from the source HDC to the memory DC
    BitBlt(memDC, 0, 0, width, height, hdc, x, y, SRCCOPY);

    // Access the bitmap data
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // Negative height for top-down bitmap
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32; // 32-bit RGBA

    // Allocate memory for the pixel data
    ol_setup(width, height);
    OlWindow win = ol_new_win();

    if (GetDIBits(memDC, hBitmap, 0, height, win.front, &bmi, DIB_RGB_COLORS)) {
        ol_save_ppm("wow.ppm", win);
    }

    // Cleanup
    DeleteObject(hBitmap);
    DeleteDC(memDC);
    ReleaseDC(hwnd, hdc);
    free(win.front);

    return 0;
}
