#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "include/dsa.h"

int main()
{
    OlLinkedList* p = ol_new_list_size(10);
    printf("Good her!");
    ol_list_insert(p, p->next);
    printf("Size: %i\n", ol_list_size(p));
    printf("Look: %i\n", ol_find_cycle(p) != NULL);
    ol_list_free(p);
    return 0;
}