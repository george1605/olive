#ifndef __DSA__
#define __DSA__
#include <malloc.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct _OlList {
    int prop;
    struct _OlList* next, *prev;
} OlLinkedList;

typedef struct _OlStack {
    void* ptr;
    size_t size, top;
} OlStack;

OlStack ol_new_stack(size_t size)
{
    OlStack stack;
    stack.ptr = malloc(size);
    stack.size = size;
    stack.top = 0;
    return stack;
}

void ol_free_stack(OlStack stack)
{
    free(stack.ptr);
}

static OlLinkedList* ol_new_list_size(int size)
{
    OlLinkedList *head = (OlLinkedList*)malloc(sizeof(OlLinkedList)), *p = head;

    while(size-- > 0)
    {
        p->next = (OlLinkedList*)malloc(sizeof(OlLinkedList));
        p->next->prev = p;
        p = p->next;
    }
    p->next = NULL;
    return head;
}

void ol_list_insert(OlLinkedList* node1, OlLinkedList* node2)
{
    node1->next = (OlLinkedList*)malloc(sizeof node1);
    node2->prev = node1->next;
}

int ol_list_size(OlLinkedList* head)
{
#ifdef _USE_RECCURSIVE_
    if(head == NULL)
        return 1;
    else
        return 1 + ol_list_size(head->next);

#else
    int p = 0;
    while(head != NULL)
    {
        p++;
        head = head->next;
    }
    return p;
#endif
}

void ol_list_fill(OlLinkedList* head, int value)
{
    while(head->next != NULL)
    {
        head->prop = value;
        head = head->next;
    }
}

void ol_list_free(OlLinkedList* p)
{
    OlLinkedList* t;
    while(p->next != NULL)
    {
        t = p;
        p = p->next;
        free(t);
    }
}

void ol_delete_node(OlLinkedList* list)
{
    if(list->next != NULL)
        list->next->prev = list->prev;
    if(list->prev != NULL)
        list->prev->next = list->next;
}

OlLinkedList* ol_find_cycle(OlLinkedList* head)
{
    OlLinkedList* turtle = head, *hare = head;
    while(turtle != NULL && hare != NULL)
    {
        hare = hare->next->next;
        turtle = turtle->next;
        if(turtle == hare)
            return turtle;
    }
    return NULL;
}
#endif