Task description
In this assignment you will be implementing a simple dynamic memory allocator with a similar interface to the standard libary functions (such as malloc) that you are familiar with.
You will be implementing your allocator as a library of functions that can be called by other programs, rather than as a standalone executable.
Introduction
The standard library and other real world allocator implementations obtain raw areas of memory from the kernel using the mmap and brk syscalls. Under a simplified memory model, these raw areas of memory are described as “the heap”. The allocator implements internal data structures that keep track of which parts of these raw blocks of memory are allocated or not. As you will be familiar, programs use library functions such as malloc and free to request dynamic memory from and return dynamic memory to the allocator. It is the allocator’s responsibility to use its internal data structures to mark memory that is allocated so that it does not overlap with any other allocations made, and to allow memory that is freed to be allocated to future dynamic memory requests. Furthermore, it is the allocator’s responsibility to appropriately call syscalls to obtain further memory from the operating system if required for an allocation, or to return memory to the operating system if it is no longer required.
In this assignment, your allocator will manage a virtualised heap using a function we provide that simulates using brk to manipulate a real process heap. When dynamic memory is requested from your allocator, it will allocate it from this virtual
“heap”.
Memory allocation process
Your virtual heap is simply a contiguous region of memory that you have read and write access to. Details on how you manage it are below in “Managing your Virtual Heap Memory”.
You will implement a simple buddy allocation algorithm to manage your virtual heap. Buddy allocation fulfils all allocations from a starting block of memory that is 2n bytes in size, where n is a non-negative integer. During the allocation process, the starting memory is repeatedly halved, creating blocks of size 2i bytes where i is a non-negative integer and i < n. i also has a minimum value that we will refer to as MIN.
Allocation Algorithm
When a request for an allocation of k bytes of memory is made, follow this algorithm:

1. If there exists an unallocated block of size 2j bytes such that 2j-1 < k ≤ 2j, allocate and return the leftmost such unallocated block. Exception: if there exists an unallocated block of size 2MIN and k ≤ 2MIN, allocate and return the leftmost such block (because there are no smaller blocks to allocate)
2. If there exist no such unallocated blocks, create an unallocated block of size 2j bytes (if k ≤ 2MIN, let j = MIN) by the below steps
   1
3. Split the leftmost unallocated block of size 2j+1 bytes in half. Allocate and return the left half for this request. The right half becomes a free unallocated block of size 2j. Blocks which are split are not allocatable. The 2 child blocks which result from a split are called buddies of each other.
4. Ifnounallocatedblockofsize2j+1exists,repeatthelaststepwiththeleftmostunallocatedblockofsize2j+2,andso on as required, up to splitting the original block of 2n bytes.
5. Ifnoblockofsuitablesizecanbefound,returnanappropriateerrorasdescribedbelowinthefunctionsyouhaveto implement
   Note the following:
   • The entire unallocated block is allocated for the request. In buddy allocation, there is often some wasted space because requested memory is less than the appropriate power-of-2 block size.
   • “Left”referstosmallermemoryaddresses
   • Anyareaofmemorycanonlybeallocatedonce.Ifallocationsucceeds,callerswillexpecttohaveexclusiveaccess,
   at the pointer you return, to the number of bytes of memory they requested.
   • Sizelimitsaredefinedbythetypeswehavespecifiedforthefunctionsyouimplement.
   Deallocation algorithm
   When a previously allocated block of memory of size 2j bytes is requested to be freed, follow this algorithm:
6. Ifthebuddyblockofthefreedblockisalsounallocated,mergethetwobuddiestoformanunallocatedblockofsize 2j+1 .
7. Repeat the previous step if the buddy block of this new 2j+1 block is also unallocated. Continue until no more unallo- cated buddy blocks can be merged.
   Note the following:
   • Unallocatedbuddyblockswhichhavebeenmergedarenolongereligibleforallocation,onlythenewparentunallo- cated block can be allocated (unless it is split up again)
   Reallocation algorithm
   When an allocated piece of memory is requested to be reallocated, follow this algorithm:
8. Makeanewrequestforallocationatthenewrequestedsize,computingasifthepreviouspieceofmemoryisfreed
9. If the new allocation succeeds, the original data in the previous piece of memory should be made available at the
   new allocated block according to the details for the virtual_realloc function below
10. Ifthenewallocationfails,theoriginalallocationmustbeunchanged
    Managing your Virtual Heap Memory
    All functions that you have to implement accept a heapstart parameter, which is a pointer to the start of the contiguous region of memory that represents your virtual heap.
    Your code can call a virtual_sbrk function that you can use to determine and change the size of your virtual heap space, analogously to the real-world sbrk and brk syscalls; its prototype is below. You do not need to use the real sbrk or brk for this assignment.
    void \* virtual_sbrk(int32_t increment);
    The “program break” of your virtual heap refers to the address of the first byte after the end of your heap. (For the avoidance of doubt, “program break” in this document always refers to your virtual heap, and not any real program break of your process memory layout).
    2

The increment parameter indicates the number of bytes to increase (positive increment) or decrease the virtual heap size, by changing the program break by the same amount. If the call is successful, virtual_sbrk returns the previous program break of your virtual heap. If the call is unsuccessful (for example because the virtual heap cannot increase further in size), virtual_sbrk returns (void _)(-1) (errno is not set).
If virtual_sbrk indicates it cannot increase your virtual heap space and this would cause your allocation to fail, then you should return the appropriate error for your function.
Functions to implement
Implement the following functions for your allocator. Do not write any main() function. Other programs will directly call your functions.
void init_allocator(void _ heapstart, uint8_t initial_size, uint8_t min_size);
This function will be called exactly once before any other functions in your allocator are called.
In this function, initialise your buddy allocation data structures and complete any preparation in the virtual heap memory you have been provided. This will be passed to each of your allocator functions, using the heapstart pointer only. You cannot pass any other state between your functions other than what is in your virtual heap. You may not use the standard library’s dynamic memory (such as malloc), or global variables, or files (even if you store pointers to external memory within your virtual heap).
Your buddy allocator starts with an initial unallocated block of memory of 2initial_size bytes. The minimum size of allocatable blocks will be 2min_size.
It is up to you how you lay out your data structures in your virtual heap, but it must semantically behave as the buddy allocator described above. Use virtual_sbrk to set your virtual heap size as you require.
void _ virtual_malloc(void _ heapstart, uint32_t size);
Request an allocation from your allocator of size bytes. Follow the buddy allocation algorithm outlined to return a pointer to the block of memory that you have allocated for the caller. Return NULL if you cannot fulfil the allocation or if size is 0. Newly allocated memory does not need to be initialised.
int virtual_free(void _ heapstart, void _ ptr);
ptr is a pointer to a previously allocated block of memory. Your allocator should free the allocation according to the buddy allocation algorithm above. If successful, return 0. If ptr is not a pointer to a block of memory that was previously allocated, return non-zero.
void _ virtual_realloc(void _ heapstart, void _ ptr, uint32_t size);
Resize a previously allocated block of memory pointed to by ptr to a new size of size bytes, according to the buddy allocation algorithm above. The contents of the new block of memory should be identical to the old. If the size is smaller, the contents should be truncated. If the size is larger, the newly added memory region does not need to be initialised. Return the pointer to the new allocation (which may be identical to ptr). Return NULL if you cannot fulfil the reallocation. In this case, the previously allocated block of memory should not be freed and should be unchanged. If ptr is NULL, you should behave as if virtual_malloc(size) was called. If size is 0 (including if ptr is NULL in this case), you should behave as if virtual_free(ptr) was called (always return NULL, even if the free would fail).
void virtual_info(void _ heapstart);
Print out information about the current state of your buddy allocator to standard output. Output one line per allocatable block, allocated or unallocated, from “left” to “right”, in the following format: <allocation status> <size>. Allocation status is either allocated or free and size is in bytes (of the entire block).
3

Example
Suppose that heapstart = 0x1000 and init_allocator is called with initial_size = 15 and min_size = 12. The diagram below shows the initial state of the virtual heap. Note that the space and location of your data structures is up to you and depends on your implementation of the buddy allocator. Note that virtual_sbrk has been used to set the virtual program break appropriately to fit what is being placed on the virtual heap.
Suppose that virtual_malloc(heapstart, 8000) is called. 212 < 8000 ≤ 213 , but we only have an unallocated block of size 215. Therefore, we split the starting block in half twice, then allocate the leftmost block of size 213 for this request. The address of this block is returned to the caller.
Suppose that virtual_malloc(heapstart, 10000) is called. 213 < 10000 ≤ 214 , so we allocate our only unallocated 214 block and return its address to the caller.
4

At this stage, if virtual_info(heapstart) was called, we expect the output:
allocated 8192
free 8192
allocated 16384
Suppose the allocated 213 block were freed. It will merge with its unallocated buddy to the right, forming an unallocated 214 size block. However, the buddy of this is not free, so there is no more merging that occurs. If the allocated 214 size block were also freed, it would merge with its buddy to reform the original 215 size block.
Restrictions
In your allocator library code:
• Youmaynotaccessoutsidetheboundsofyourvirtualheapwithoutusingvirtual_sbrktoresizeitproperly
• Youmaynotusedynamicmemoryofanykind,includinganyfromthestandardlibraryallocator,brk,sbrk,mmap,
alloca or variable-length arrays, except that which you obtain from virtual_sbrk.
• Youmaynotaccessanyfiles,oruseanyglobalorstaticvariables.
In your testing code (code that sets up and runs test cases which call your allocator library code): • Theaboverestrictionsdonotapply.
• Youmayusedynamicmemory.Youmaynotuseallocaorvariable-lengtharrays. If your submission violates any of these restrictions, it may receive 0.
5

Test Cases
You must write test cases for this assignment. Details on execution and marking of your test cases is included below.
You will need to create your own virtual heap memory area and write your own virtual_sbrk function in your testing code for your library to access.
