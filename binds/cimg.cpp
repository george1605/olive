#ifndef __CIMG__
#define __CIMG__

#include "CImg.h"
using namespace cimg_library;

extern "C" {
    #include "../include/gfx.c"
}

namespace ol
{
    class Window
    {
    private:
        OlWindow win;
    public:
        Window(int x, int y)
        {
            ol_setup(x, y);
            this->win = ol_new_win(); 
        }
        void load(char* ppmFile)
        {
            ol_load_ppm(ppmFile, &win);
        }
        OlWindow base()
        {
            return this->win;
        }
        void fill(size_t color)
        {
            ol_fill(win, color);
        }
        void save(const char* fname)
        {
            ol_save_ppm((char*)fname, win);
        }
        ~Window()
        {
            free(this->win.front);
        }
    };

    class Viewer
    {
    private:
        CImg<unsigned char> data;
        CImgDisplay disp;
    public:
        Viewer(const char* name,Window win)
        {
            OlWindow base = win.base();
            CImg<unsigned char> data(base.front, base.w, base.h, 1, 1, false);
            this->disp = CImgDisplay(data, name);
        }
    };
}

#endif

int main()
{
    ol::Window win = ol::Window(300, 300);
    win.fill(0xff00ff);
    win.save("wow.ppm");
    while(1);
    return 0;
}