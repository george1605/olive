#include <stdint.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>

// Define the struct for relative pointer
typedef struct {
    uintptr_t offset;
} OlRelPtr;

// Function to initialize the relative pointer with an offset
void ol_init_relptr(OlRelPtr* relptr, uintptr_t targetAddress) {
    relptr->offset = targetAddress - (uintptr_t)&relptr->offset;
}

// Function to calculate the actual pointer address
uintptr_t ol_get_relptr(const OlRelPtr* relptr) {
    return (uintptr_t)relptr + relptr->offset;
}

uint8_t ol_byteat_relptr(const OlRelPtr* relptr, size_t pos) {
    uint8_t* p = (uint8_t*) ol_get_relptr(relptr);
    return p[pos];
}

void ol_free_relptr(const OlRelPtr* relptr) {
    void* p = (void*)ol_get_relptr(relptr);
    free(p);
}

/***
================= TEST ====================

void useRelativePointer(OlRelPtr* relptr) {
    uintptr_t absoluteAddress = (uintptr_t)malloc(5);
    memset((void*)absoluteAddress, 0xCD, 5);

    ol_init_relptr(relptr, absoluteAddress);
    uintptr_t pointerAddress = ol_get_relptr(relptr);

    // Print the results
    printf("Absolute Address: %lu\n", absoluteAddress);
    printf("Offset: %lu\n", relptr->offset);
    printf("Byte at 0: %i\n", ol_byteat_relptr(relptr, 0));
    printf("Calculated Pointer Address: %lu\n", pointerAddress);
    ol_free_relptr(relptr);
}

int main() {
    // Create an instance of OlRelPtr
    OlRelPtr relPtr;

    // Use the relative pointer
    useRelativePointer(&relPtr);

    return 0;
}
*/