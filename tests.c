#include "virtual_alloc.h"
#include "virtual_sbrk.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void * virtual_heap = NULL;

void * virtual_sbrk(int32_t increment) {
    // Your implementation here (for your testing only)
    return sbrk(increment);
}

int main() {
    // Your own testing code here
    virtual_heap = malloc(1000);
    //redirect our to temp file
    freopen("test.out","w",stdout);
    init_allocator(virtual_heap,15,12);
    virtual_info(virtual_heap);
    free(virtual_heap);
}
