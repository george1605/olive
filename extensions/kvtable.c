//
// Created by George on 11/14/2023.
//
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <stdarg.h>
#define TYPE_NUMBER  0x10 // float, double
#define TYPE_VARCHAR 0x20 // char*
#define TYPE_UID     0x30 // unsigned long long usually
#define TYPE_FK      0x40 // pointer
#define CMD_INSERT   0xFF
#define E_NOTEXIST   0x20
#define PRINTF_TYPE(value) _Generic(value, float: "%f", int: "%i", char*: "%s", void*: "%p", default: "%ul")

struct kvvalue {
    void* ptr;
    int type;
};

struct kvpair
{
    void* key, *value;
};

struct kvarray
{
    void* ptr;
    size_t size;
    int type; // like TYPE_NUMBER, TYPE_VARCHAR
};

struct kvtable
{
    struct kvtable* prev, *next;
    struct kvpair pair;
};

struct kvtable* new_kvtable()
{
    struct kvtable *t = malloc(sizeof(struct kvtable));
    t->prev = NULL;
    t->next = NULL;
    t->pair.key = 1, t->pair.value = 1;
    return t;
}

// TO DO expand to other data types
void print_pair(struct kvpair pair)
{
    printf("%i : %i\n", (int)pair.key, (int)pair.value);
}

void print_table(struct kvtable* table)
{
    struct kvtable * p = table;
    while(p != NULL)
    {
        print_pair(p->pair);
        p = p->next;
    }
}

// go to the end
struct kvtable* table_wind(struct kvtable* p)
{
    struct kvtable * q = p;
    while(q->next != NULL)
        q = q->next;
    return q;
}

struct kvpair make_kvpair(void* key, void* value)
{
    struct kvpair kv;
    kv.key = key;
    kv.value = value;
    return kv;
}

struct kvtable* insert_kvpair(void* key, void* value, struct kvtable* table)
{
    struct kvtable* t;
    t = table_wind(table);
    t->next = (struct kvtable*)malloc(sizeof(struct kvtable));
    t->next->pair = make_kvpair(key, value);
    t->next->next = NULL; // why i forgot this null?
    return t;
}

void* search_kv(void* key, struct kvtable* table)
{
    struct kvtable* p = table;
    while(p != NULL)
    {
        if(p->pair.key == key)
            return p->pair.value;
        p = p->next;
    }
    return NULL;
}

void* search_kv_s(char* string, struct kvtable* table)
{
    struct kvtable* p = table;
    while(p != NULL)
    {
        if(strcmp(p->pair.key, string) == 0)
            return p->pair.value;
        p = p->next;
    }
    return NULL;
}

void write_kv(char* fname, struct kvtable* table)
{
    struct kvtable * p = table;
    FILE* fp = fopen(fname, "w");
    while(p != NULL)
    {
        fprintf(fp, "%i : %i\n", (int)p->pair.key, (int)p->pair.value);
        p = p->next;
    }
    fclose(fp);
}

struct kvtable* read_kv(char* fname)
{
    struct kvtable* i = new_kvtable(), *p;
    p = i;
    FILE* fp = fopen(fname, "r");
    while(fscanf(fp, "%i : %i", &(i->pair.key), &(i->pair.value)))
    {
        i->next = malloc(sizeof(struct kvtable));
        i = i->next;
    }
    i->next = NULL;
    fclose(fp);
    return p;
}

/// ////////////////////
///    Type System
/// ///////////////////
#define PRINTARR(value, type)
void printarr_kv(void* x, int type)
{
    if(x == NULL) {
        puts("{}");
        return;
    }

    PRINTARR(x, type);
}

void makearray_kv(int type, ...)
{
    va_list list;
    va_start(list, type);
    switch(type)
    {
        case TYPE_NUMBER:

            break;
    }
}

int main()
{
    printf(PRINTF_TYPE(""), "wow it's good");
    return 0;
}