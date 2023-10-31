// OBJ File Loader
#ifndef __OBJ__
#define __OBJ__

#include "olivegl.h"
#include <stdio.h>

typedef struct {
    char* name;
} OlModel;

void ol_skip_till(FILE* fp, char c)
{
    char t;
    while(t != c)
        t = fgetc(fp);
}

void ol_obj_line(FILE* filename, char* buffer, int len)
{
    if(fgetc(filename) == '#')
        ol_skip_till(filename, '\n');
    fgets(buffer, len, filename); 
}

void ol_obj_read(FILE* fp)
{
    char c;
    while(c = fgetc(fp)) {
        switch(c)
        {
            case 'v':
                double v1, v2, v3;
                fscanf(fp, "%lf %lf %lf", &v1, &v2, &v3);
                glVertex3d(v1, v2, v3);
            break;
        }
    }
}

#endif