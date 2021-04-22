#include "virtual_alloc.h"
#include "virtual_sbrk.h"

#include <stdio.h>
#include <stdlib.h>

void * virtual_heap = NULL;

void * virtual_sbrk(int32_t increment) {
    // Your implementation here (for your testing only)
    void * sbrk = virtual_heap;
    void *sbrk = sbrk + increment;
    return (void *)(-1);
}

int main() {
    // Your own testing code here
    virtual_heap = malloc(1000);
    init_allocator(virtual_heap, 15, 12);
    free(virtual_heap);

}
