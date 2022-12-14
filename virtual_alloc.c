#include "virtual_alloc.h"
#include "virtual_sbrk.h"


#include <math.h>
#include <stdio.h>
#include <string.h>

enum STATE {NONE, FREE , ALLOCATED, SPLIT};

//data structure will be in form of binary tree
struct node{
    
    enum STATE state;

    int size;
    int min_size;

    int index;
    struct node *parent;
    struct node *left;
    struct node *right;
    void *mem_block;
};

// took from https://stackoverflow.com/questions/13095488/how-to-implement-overlap-checking-memcpy-in-c
int overlap_p(void *a, void *b, size_t n)
{
    char *x = a, *y =  b;
    return (x<=y && x+n>y) || (y<=x && y+n>x);
}

//return 1 if node is free 2 if split and 3 if allocated else return 0
int state(struct node *nd){
    if(nd->state == FREE){
        return 1;
    }else if(nd->state == ALLOCATED){
        return 2;
    }else if(nd->state == SPLIT){
        return 3;
    }else{
        return 0;
    }
}


//return 0 if not external and 1 if is external note: external if children state is NONE
int isexternal(struct node *nd){
    if(nd->left == NULL && nd->right == NULL)
        return 1;
    return 0;
}

//return 1 if is root else return 0
int isroot(struct node *nd){
    if(nd->parent == NULL)
        return 1;
    return 0;
}

//return right child
struct node *right(struct node *nd){
    if(isexternal(nd))
        return NULL;

    return nd->right;
}

//return left child
struct node *left(struct node *nd){
    if(isexternal(nd))
        return NULL;
    
    return nd->left;
}

//search node for size value k and return the left most if not found return null
struct node * search(void * heapstart, int best_fit_size, enum STATE st) {
    
    if(heapstart == NULL){
        return NULL;
    }
    struct node* root = (struct node*) heapstart;

    //search for left most free node
    for(int i = 0 ; i < pow(2,log2(root->size) - log2(root->min_size) + 1) - 1; i++){
        
        struct node* nd = (struct node*) heapstart + i;
        if(nd->state == st && nd->size == best_fit_size){            
            return nd;
        }
    }

    return NULL;
}

//search a node of mem_block ptr
struct node * search_mem(void * heapstart, void * ptr, enum STATE st){

    if(heapstart == NULL){
        return NULL;
    }

    struct node* root = (struct node*) heapstart;

    //search for node with our memery block ptr
    for(int i = 0 ; i < pow(2,log2(root->size) - log2(root->min_size) + 1) - 1; i++){
        
        struct node* nd = (struct node*) heapstart + i;
        if(nd->state == st && nd->mem_block == ptr){
            return nd;
        }
    }
    return NULL;
}

//split funtion split a node allocate left child and free right child and return the left child
struct node *split(struct node *nd){
    if(nd == NULL ||nd->left == NULL||nd->right == NULL){
        return NULL;
    }
    nd->state = SPLIT;
    nd->left->state = FREE;
    nd->right->state = FREE;
    return nd->left;
}

void init_allocator(void * heapstart, uint8_t initial_size, uint8_t min_size) {
    if(heapstart == NULL){
        return;
    }
    
    int position = virtual_sbrk(0) - heapstart;
    int max_num_of_nodes = ((pow(2,initial_size - min_size + 1))-1);
    // push sbrk with maximum nb of node*size + postion + heap size
    virtual_sbrk(sizeof(struct node)*max_num_of_nodes + pow(2,initial_size) - position);
    
    //initalise root node
    struct node* block = (struct node*) heapstart;
    block->state = FREE;

    block->mem_block = heapstart + sizeof(struct node)*max_num_of_nodes;
    block->size = pow(2,initial_size);
    block->min_size = pow(2,min_size);

    block->index = 0;
    block->parent = NULL;
    block->left = (struct node*) heapstart + 2*block->index + 1;
    block->right = (struct node*) heapstart + 2*block->index + 2;
    
    //create child for every node except leafs
    for(int i = 0; i < (pow(2,initial_size - min_size))-1; i ++){

        struct node* block = (struct node*) heapstart + i;
        
        block->left = (struct node*) heapstart + (2*i + 1);
        
        block->left->parent = block;
        block->left->index = 2*block->index + 1;
        block->left->mem_block = block->mem_block;
        block->left->size = (block->size)/2;

        block->left->state = NONE;
        block->left->left = NULL;
        block->left->right = NULL;
        
        block->right = (struct node*) heapstart + (2*i + 2);
        
        block->right->parent = block;
        block->right->index = 2*block->index + 2;
        block->right->mem_block = block->mem_block + (block->size)/2;;
        block->right->size = (block->size)/2;

        block->right->state = NONE;
        block->right->left = NULL;
        block->right->right = NULL;

    }
}

void * virtual_malloc(void * heapstart, uint32_t size) {
    
    if(heapstart == NULL){
        return NULL;
    }
    struct node* root = (struct node*) heapstart;
    if((size > root->size) ||(size == 0)){
        return NULL;
    }
    //get best fit size in form of 2^k where k is log2 initial size + 1
    int best_fit_size = pow(2,ceil(log2(size)));
    
    //best_fit_size has a minimum value for alocation
    if(best_fit_size < root->min_size)
        best_fit_size = root->min_size;
    
    //search for left most free node
    struct node *srch_res = search(heapstart,best_fit_size,FREE);
    if(srch_res != NULL){
        srch_res->state = ALLOCATED;
        return srch_res->mem_block;
    }

    //search for the smallest free block
    int bigger_size = best_fit_size*2;
    int found = 0;
    while(found == 0){
        srch_res = search(heapstart,bigger_size,FREE);
        if(srch_res != NULL){
            found = 1;
        }

        //cannot find free memry blcok
        if(bigger_size > root->size){
            return NULL;
        }
        bigger_size *= 2;
    }

    struct node* left_child = srch_res;

    while(left_child!=NULL){
        left_child = split(left_child);

        if(left_child->size == best_fit_size){
            left_child->state = ALLOCATED;

            return left_child->mem_block;
        }
    }


    return NULL;
}

int virtual_free(void * heapstart, void * ptr) {
    if(heapstart == NULL || ptr == NULL){

        return 1;
    }
    struct node *nd = search_mem(heapstart,ptr,ALLOCATED);
    if(nd == NULL){

        return 1;
    }
    nd->state = FREE;
    while(nd->parent != NULL ){
        nd = nd->parent;
        if(nd->left->state == FREE &&nd->right->state == FREE ){
            nd->left->state = NONE;
            nd->right->state = NONE;
            nd->state=FREE;
        }else{
            return 0;
        }
    }
    return 0;
}

void * virtual_realloc(void * heapstart, void * ptr, uint32_t size) {
    
    if(ptr == NULL){
        return virtual_malloc(heapstart,size);
    }else if(size == 0){
        int res = virtual_free(heapstart,ptr);
        if(res == 1){
            return NULL;
        }else{
            return ptr;
        }
    }

    struct node* root = (struct node*) heapstart;
    if(size > root->size){
        return NULL;
    }
    int best_fit_size = pow(2,ceil(log2(size)));
    
    //best_fit_size has a minimum value for alocation
    if(best_fit_size < root->min_size)
        best_fit_size = root->min_size;

    void *newptr = NULL;
    
    int nb = virtual_free(heapstart,ptr);
    if( nb == 0){
        newptr = virtual_malloc(heapstart,size);
    }else{
        return NULL;
    }
    if(newptr == NULL){
        //undo free
        struct node *freed_node = search_mem(heapstart,ptr,FREE);
        if(freed_node == NULL){
            freed_node = search_mem(heapstart,ptr,NONE);
        }
        if(freed_node->state == FREE){
            freed_node->state = ALLOCATED;
        }else if(freed_node->state == NONE){
            struct node *nd = freed_node->parent;
            while(nd->state == FREE || nd->state == NONE){
                split(nd);
                nd->left->state = SPLIT;
                nd = nd->parent;
            }
            freed_node->state = ALLOCATED;
        }
        return NULL;
    }

    if(overlap_p(newptr,ptr,size)){
        memmove(newptr,ptr,size);
        return newptr;
    }
    return memcpy(newptr,ptr,size);
}

void virtual_info(void * heapstart) {
    struct node* block = (struct node*) heapstart;
    
    if(block->state == SPLIT){
        //left node
        void* heap1 = (void*) block->left;
        virtual_info(heap1);
        //right node
        void* heap2 = (void*) block->right;
        virtual_info(heap2);
    }
    if(block->state == FREE){
        printf("free %d\n",block->size);
    }

    if(block->state == ALLOCATED){
        printf("allocated %d\n",block->size);
    }
}
