#include "virtual_alloc.h"
#include "virtual_sbrk.h"


#include <math.h>
#include <stdio.h>

enum STATE {NONE, FREE , ALLOCATED, SPLIT};

//data structure will be in form of binary tree
struct node{
    
    enum STATE state;

    void *mem_block;
    int size;
    int min_size;

    int index;
    struct node *parent;
    struct node *left;
    struct node *right;
};

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

//node size function
int size(struct node *nd){
    return nd->size;
}

//node index function
/*int index(struct node *nd){
    return nd->index;
}*/

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

//return parent
struct node *parent(struct node *nd){
    if(isroot(nd))
        return NULL;
    return nd->parent;
}

//return buddy
struct node *buddy(struct node *nd){

    if(right(parent(nd))== nd)
        return left(parent(nd));
    
    if(left(parent(nd))== nd)
        return right(parent(nd));
    
    return NULL;
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
        //struct node nd = nds[i];
        //printf("%p\n",nd);
        //printf("index=%d, size=%d, best_fit_size=%d, state=%d\n",nd->index,nd->size, best_fit_size, nd->state);
        if(nd->state == st){
            if(nd->size == best_fit_size){
                //printf("hello\n");
                //nd->state = ALLOCATED;
                //printf("index=%d, size=%d, best_fit_size=%d, state=%d\n",nd->index,nd->size, best_fit_size, nd->state);
                return nd;
            }
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
    //printf("node size: %d\nleft size: %d\n right size: %d\n",nd->size,nd->left->size,nd->right->size);
    return nd->left;
}

void init_allocator(void * heapstart, uint8_t initial_size, uint8_t min_size) {
    //printf("size of struct: %lu\n",sizeof(struct node));
    //printf("hello\n");
    int position = virtual_sbrk(0) - heapstart;
    int max_num_of_nodes = ((pow(2,initial_size - min_size + 1))-1);
    //printf("position:%d\n max num of node: %d\n intial size:%d\n min size: %d\n",position,max_num_of_nodes,initial_size, min_size);
    virtual_sbrk(sizeof(struct node)*max_num_of_nodes + pow(2,initial_size) - position);
    int new_position = virtual_sbrk(0) - heapstart;
    //printf("new position %d",new_position);
    struct node* block = (struct node*) heapstart;
    block->state = FREE;

    block->mem_block = heapstart + sizeof(struct node)*max_num_of_nodes;
    block->size = pow(2,initial_size);
    block->min_size = pow(2,min_size);

    block->index = 0;
    block->parent = NULL;
    block->left = (struct node*) heapstart + 2*block->index + 1;
    block->right = (struct node*) heapstart + 2*block->index + 2;
    //printf("%p\n",block->left);
    //printf("%p\n",block->right);
    //printf("hello\n");
    //create child for every node except leafs
    for(int i = 0; i < (pow(2,initial_size - min_size))-1; i ++){
        //printf("hello\n");
        struct node* block = (struct node*) heapstart + i;
        //printf("%p\n",block);
        block->left = (struct node*) heapstart + (2*i + 1);
        //printf("%p\n",block->left);
        block->left->parent = block;
        block->left->index = 2*block->index + 1;
        block->left->mem_block = block->mem_block;
        //block->left->min_size = block->min_size;
        block->left->size = (block->size)/2;
        block->left->state = NONE;
        block->left->left = NULL;
        block->left->right = NULL;
        
        block->right = (struct node*) heapstart + (2*i + 2);
        //printf("%p\n",block->right);
        block->right->parent = block;
        block->right->index = 2*block->index + 2;
        block->right->mem_block = block->mem_block + (block->size)/2;;
        //block->right->min_size = block->min_size;
        block->right->size = (block->size)/2;
        block->right->state = NONE;
        block->right->left = NULL;
        block->right->right = NULL;

        //printf("index=%d, size=%d, state=%d\n left=%p, right=%p\n mem_block=%p\n",block->index,block->size, block->state,block->left,block->right,block->mem_block);

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
    //printf("index=%d, size=%d, state=%d\n left=%p, right=%p\n mem_block=%p\n",left_child->index,left_child->size, left_child->state,left_child->left,left_child->right,left_child->mem_block);
    while(left_child!=NULL){
        left_child = split(left_child);
        //printf("best fit size = %d, child size %d\n",best_fit_size,left_child->size);
        //printf("index=%d, size=%d, state=%d\n left=%p, right=%p\n mem_block=%p\n",left_child->index,left_child->size, left_child->state,left_child->left,left_child->right,left_child->mem_block);
        if(left_child->size == best_fit_size){
            left_child->state = ALLOCATED;
            //printf("hello\n");
            //printf("index=%d, size=%d, state=%d\n left=%p, right=%p\n mem_block=%p\n",left_child->index,left_child->size, left_child->state,left_child->left,left_child->right,left_child->mem_block);
            return left_child->mem_block;
        }
    }


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
    //int index = block->index;
    printf("index=%d, size=%d, state=%d\n left=%p, right=%p\n mem_block=%p\n",block->index,block->size, block->state,block->left,block->right,block->mem_block);
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
    /*if(block->state == NONE){
        printf("none %d\n",block->size);
        //left node
        //virtual_info(heapstart + (2*index + 1));
        //right node
        //virtual_info(heapstart + (2*index + 2));
    }*/
}
