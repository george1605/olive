#include <stdint.h>
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
OlInput ol_input_layer(uint32_t* ptr, int size, int type)
{
    OlInput i;
    i.data = ptr, i.size = size;
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