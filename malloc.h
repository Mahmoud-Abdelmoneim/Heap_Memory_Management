#ifndef MALOC_H
#define MALLOC_H
#include <stdio.h>
#include <unistd.h>


#define SIZE_OF_META_DATA    32
#define SIZE_OF_PAGE         4096
#define SIZE_OF_SBRK_STEP    SIZE_OF_PAGE * 32

typedef enum {
    FREE,
    NOT_FREE
}block_state;



typedef enum{
    ALLOCATED,
    NOT_ALLOCATED
}NewBlock_state;
typedef struct free{
    
    void* address;
    struct free *next;
    struct free *privios;
    int size;
    block_state free;
}free_list;



void* My_malloc(int size);
void my_free(void* ptr);
#endif





