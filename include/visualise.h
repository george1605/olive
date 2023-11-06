#include "gfx.c"
#include <stdio.h>
#include <stdint.h>

// visualise files
// bytes 0xa0 0xbc -> coordinates of point (0xa0, 0xbc)
OlWindow ol_visual(uint8_t* dataset, int size)
{
    uint8_t b1, b2;
    int k = 0;
    ol_setup(256, 256);
    OlWindow win = ol_new_win();
    b1 = dataset[0];
    while(k < size)
    {
        b2 = dataset[k++];
        win.front[b2 * win.w + b1] = WHITE;
        b1 = b2;
    }
    return win;
}

OlWindow ol_visual_file(char* fname)
{
    FILE* fp = fopen(fname, "rb");
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    rewind(fp);
    uint8_t* data = (uint8_t*)malloc(size);
    fread(data, 1, size, fp);
    OlWindow win = ol_visual(data, size);
    free(data);
    fclose(fp);
    return win;
}