#include "include/visualise.h"

int main()
{
    OlWindow win = ol_visual_file("olive.exe");
    ol_save_ppm("main.ppm", win);
}