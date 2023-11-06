#ifndef __STR_H__
#define __STR_H__
#include <malloc.h>
#include <stdlib.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#define ELF_MAGIC 0
#define EXE_MAGIC 1
#define JPG_MAGIC 2
#define STR(s) (string){ .ptr = s, .size = strlen(s) }

/**
 * A list of magic headers of ELF, EXE and JPG
*/
char* magic_list[] = {
    "\x7f\x45\x4c\x46", "MZ", "\xFF\xD8\xFF\xE0"
};

typedef struct _string {
    char* ptr;
    size_t size;
} string;

typedef struct _binstring {
    uint8_t* ptr;
    size_t size;
} binstring;

char ol_check_magic(binstring str, int magic)
{
    return memcmp(str.ptr, magic_list[magic], str.size);
}

string ol_str_alloc(size_t s)
{
    string str;
    str.ptr = (char*)malloc(s);
    str.size = s;
    str.ptr[0] = '\0';
    return str;
}

string ol_str_from(FILE* file, size_t s)
{
    string str = ol_str_alloc(s);
    fread(str.ptr, 1, s, file);
    return str;
}

uint64_t ol_simple_hash(string str) {
    uint64_t hash = 5381;
    int c;
    char* s = str.ptr;
    while ((c = *s++) != 0) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

void ol_whitespaces(string arg, int* pos)
{
    int i, k = 0;
    for(i = 0;i < arg.size;i++)
        if(arg.ptr[i] == ' ')
            pos[k++] = i;
}

int ol_str_includes(string str, string substr)
{
    return (strstr(str.ptr, substr.ptr) != NULL);
}

char ol_str_valid(string input)
{
    if(input.ptr == NULL)
        return 0;
    return (strlen(input.ptr) == input.size);
}

size_t ol_str_first(string arg, char c)
{
    size_t pos;
    for(pos = 0;pos < arg.size;pos++)
        if(arg.ptr[pos] == c)
            return pos;
    return arg.size;
}

size_t ol_multip_hash(string arg, size_t table_size) {
    const float A = 0.6180339887;  // the golden ratio
    size_t hash = 0;
    char *p, *str = arg.ptr; 

    for (p = str; *p != '\0'; p++) {
        hash = hash * A + (*p);
    }

    float fractional_part = hash * A - floor(hash * A);
    return (size_t)(table_size * fractional_part);
}

size_t ol_csv_tokens(string arg)
{
    int i, tk = 0;
    for(int i = 0;i < arg.size;i++)
        if(arg.ptr[i] == ',')
            tk++;
    return (tk + 1);
}

char ol_html_check(string arg)
{
    return (arg.ptr[0] == '<' && arg.ptr[arg.size - 1] == '>');
}
#endif