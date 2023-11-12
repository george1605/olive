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
        void fill_grad(OlColor col1, OlColor col2)
        {
            ol_gradient(win, col1, col2, win.w, win.h);
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
            CImg<uint32_t> data(base.front, base.w, base.h, 1, 1, false);
            this->disp = CImgDisplay(data, name);
        }
        bool closed()
        {
            return disp.is_closed();
        }
        void wait()
        {
            disp.wait();
        }
    };
}

#endif

int main()
{
    ol::Window win = ol::Window(300, 300);
    OlColor col1 = {255, 0, 255};
    OlColor col2 = {175, 100, 200};
    win.fill_grad(col1, col2);
    ol::Viewer display("Ma man!", win);
    while(!display.closed())
    {
        display.wait();
    }
    return 0;
}