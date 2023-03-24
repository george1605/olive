#include "ol.c"

int main(int argc, char** argv)
{
    ol_sdl_init();
    OlPanel p = ol_sdl_new("Window", 1000, 1000, 0);
    while(1);
    return 0;
}