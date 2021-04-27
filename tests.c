#include "virtual_alloc.h"
#include "virtual_sbrk.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <math.h>

void * virtual_heap = NULL;

void * virtual_sbrk(int32_t increment) {
    // Your implementation here (for your testing only)
    return sbrk(increment);
}

int main() {
    // Your own testing code here
    virtual_heap = malloc(1000);
    //redirect our to temp file

    //test init
    freopen("tests_file/test_init.out","w+",stdout);
    init_allocator(virtual_heap,15,12);
    virtual_info(virtual_heap);

    //test malloc
    freopen("tests_file/test_malloc.out","w+",stdout);
    void *ptr0 = virtual_malloc(virtual_heap,1);
    void *ptr1 = virtual_malloc(virtual_heap,1);
    void *ptr2 = virtual_malloc(virtual_heap,1);
    void *ptr3 = virtual_malloc(virtual_heap,1);
    void *ptr4 = virtual_malloc(virtual_heap,1);
    void *ptr5 = virtual_malloc(virtual_heap,8192);
    if(virtual_malloc(virtual_heap,pow(2,16)) != NULL)
        printf("Failed malloc size > inital size\n");
    if(virtual_malloc(NULL,pow(2,13)) != NULL)
        printf("Failed malloc NULL pointer\n");
    if(virtual_malloc(virtual_heap,0) != NULL)
        printf("Failed malloc test size = 0\n");
    virtual_info(virtual_heap);


    //test free
    freopen("tests_file/test_free.out","w+",stdout);
    //check error
    if(virtual_free(NULL, ptr0) != 1 | virtual_free(virtual_heap, NULL) != 1)
        printf("Failed free test heapstart or ptr + NULL\n");

    virtual_free(virtual_heap,ptr0);
    virtual_info(virtual_heap);
    printf("test ptr0 done\n");
    virtual_free(virtual_heap,ptr5);
    virtual_info(virtual_heap);
    printf("test ptr5 done\n");
    virtual_free(virtual_heap,ptr1);
    virtual_info(virtual_heap);
    printf("test ptr1 done\n");
    virtual_free(virtual_heap,ptr3);
    virtual_info(virtual_heap);
    printf("test ptr3 done\n");
    virtual_free(virtual_heap,ptr4);
    virtual_info(virtual_heap);
    printf("test ptr4 done\n");
    virtual_free(virtual_heap,ptr5);
    virtual_info(virtual_heap);
    printf("test ptr5 done\n");
    virtual_free(virtual_heap,ptr2);
    virtual_info(virtual_heap);
    printf("test ptr2 done\n");
    if(virtual_free(virtual_heap,ptr2) != 1)
        printf("Failed free test ptr is already free\n");

    //test realloc
    freopen("tests_file/test_realloc.out","w+",stdout);
    //should act as malloc
    printf("realloc test 1\n");
    ptr0 = virtual_realloc(virtual_heap,NULL,8192);
    virtual_info(virtual_heap);
    virtual_realloc(virtual_heap, virtual_heap + 48*15 + 8192, 1024);
    printf("realloc test 2\n");
    virtual_info(virtual_heap);

    //free
    free(virtual_heap);
    return 0;
}
