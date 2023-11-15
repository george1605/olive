#ifndef __RELPTR__
#define __RELPTR__
#include <malloc.h>
#include <memory.h>
#define OL_OFFSET(obj, ptr) (size_t)(&obj - ptr)
#define OL_DEREFREL(type, ptr) *(type*)(&ptr + ptr.offset)

typedef struct 
{
  size_t offset; // offset relative to this object
  size_t size;
} OlRelPtr;

void ol_setup_relptr(OlRelPtr* relptr, void* ptr)
{
  ptr->offset = (relptr - ptr);
}

// get the base pointer
void* ol_deref_relptr(OlRelPtr* ptr)
{
  if(ptr->offset == 0) return NULL;
  return (void*)(ptr + ptr->offset);
}

void ol_free_relptr(OlRelPtr* ptr)
{
  free((void*)(ptr + ptr->offset));
  ptr->offset = 0; // marks that points to no data - safety reasons
}

#endif
