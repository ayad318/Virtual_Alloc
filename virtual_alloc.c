#include "virtual_alloc.h"
#include "virtual_sbrk.h"


#include <math.h>

enum STATE {FREE , SPLIT, ALLOCATED};

//data structure will be in form of binary tree
struct node{
    
    enum STATE state;

    void *mem_block;
    int size;
    struct node *parent;
    struct node *left;
    struct node *right;
};

void init_allocator(void * heapstart, uint8_t initial_size, uint8_t min_size) {
    
    int position = virtual_sbrk(0) - heapstart;
    int max_num_of_nodes = 2^(initial_size - min_size + 1);
    virtual_sbrk(sizeof(struct node)*max_num_of_nodes + pow(2,initial_size) - position);

    struct node* block = (struct node*) heapstart;
    block->state = FREE;
    block->size = pow(2,initial_size);
}

void * virtual_malloc(void * heapstart, uint32_t size) {
    // Your code here
    return NULL;
}

int virtual_free(void * heapstart, void * ptr) {
    // Your code here
    return 1;
}

void * virtual_realloc(void * heapstart, void * ptr, uint32_t size) {
    // Your code here
    return NULL;
}

void virtual_info(void * heapstart) {
    struct node* block = (struct node*) heapstart;
    if(block->state == FREE){
        printf("free %d\n",block->size);
    }
}
