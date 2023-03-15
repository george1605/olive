#include "gfx.c"

int main()
{
    ol_setup(350, 400);
    OlWindow win = ol_new_win();
    COLOR = RED;
    ol_line_bres(win, 150, 150, 200, 300);
    OlColor r = color(255, 0, 0), x = color(0, 0, 255);
    ol_gradient(win, r, x, 200, 100);
    ol_zoom(win, 199, 99, 10);
    ol_save_ppm("main.ppm", win);
    return 0;
}