#include "gfx.c"

int main()
{
    ol_setup(350, 350);
    OlWindow win = ol_new_win();
    OlColor r[4] = { color(255, 0, 0), color(0, 0, 255), color(0, 255, 0), color(128, 128, 128) };
    ol_bgradient(win, r, 200, 100);
    OlPoint t[3] = {{200, 100}, {250, 250}, {300, 300}};
    ol_quad_bezier(win, t);
    ol_save_ppm("main.ppm", win);
    return 0;
}