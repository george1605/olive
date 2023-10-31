#include "include/visualise.h"

int main()
{
    OlWindow win = ol_visual("flower.jpg");
    ol_brescircle(win, 30, 40, 60);
    ol_save_ppm("main.ppm", win);
}