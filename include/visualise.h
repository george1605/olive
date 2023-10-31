#include "gfx.c"
#include <stdio.h>
#include <stdint.h>

// visualise files
OlWindow ol_visual(char* fname)
{
    uint8_t b1, b2;
    FILE* fp = fopen(fname, "rb");
    ol_setup(256, 256);
    OlWindow win = ol_new_win();
    b1 = fgetc(fp);
    while((b2 = fgetc(fp)) != EOF)
    {
        win.front[b2 * win.w + b1] = RED;
        b1 = b2;
    }
    fclose(fp); // don't forget that
    return win;
}