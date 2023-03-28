#include "gfx.c"
#define INPUT_GRAY 0
#define INPUT_RGB 1
#define CELL_SIZE(x) typedef uint ## x ## _t OlCell;
CELL_SIZE(32);

typedef struct {
    OlCell* data;
    int size;
} OlInput;

typedef OlInput OlLayer;
/* 
    * Use input from OlWindow 
*/

void ol_to_gray(uint32_t* ptr, float* t, int size)
{
    for(int i = 0;i < size;i++)
        t[i] = (float)(GETBYTE(ptr[i], 0) + GETBYTE(ptr[i], 1) + GETBYTE(ptr[i], 2))/3;
}

OlInput ol_input_layer(uint32_t* ptr, int size, int type)
{
    OlInput i;
    if(type == INPUT_GRAY)
    {
        i.data = malloc(size * sizeof(float));
        ol_to_gray(ptr, i.data, size);
    } else {
        i.data = ptr, i.size = size;
    }
    return i;
}

void ol_multiply(OlLayer l1, OlLayer l2)
{
    for(int i = 0;i <= l1.size;i++)
        l1.data[i] *= l2.data[i];
}

float ol_result(OlLayer l1, float bias)
{
    float s1 = bias;
    for(int i = 0;i <= l1.size;i++)
        s1 += l1.data[i];
    return s1;
}

float ol_max_activ(OlLayer l2)
{
    float x = 0;
    for(int i = 0;i < l2.size;i++)
        if(x < l2.data[i])
            x = l2.data[i];
    return x;
}