/****************************************************************************************/
/*******File   : malloc.c ***************************************************************/
/*******Auther : Mahmoud Abdelmoniem ****************************************************/
/*******Data   : 15/4/2024 **************************************************************/
/*******Version: 0.0*********************************************************************/
/****************************************************************************************/

#include "malloc.h"
#include <unistd.h>
#include <string.h>

free_list *headBlock;
free_list * current ;
char FirsttimeFlag=0;
void * LastLocation ;

void initailiztion()
{
    /*Initialize baseList and headBlock to the current program break*/
    headBlock = (free_list*)sbrk(0);
    
    /*Move LastLocation to a new location in memory by increasing the program break*/
    LastLocation = sbrk(SIZE_OF_SBRK_STEP);
    
    /*Initialize baseList with the following properties*/
    headBlock->next = NULL;                    // Next pointer is set to NULL
    headBlock->address = (void*)headBlock;       // Address is set to the address of baseList
    headBlock->privios = NULL;                  // Previous pointer is set to NULL
    headBlock->free = FREE;                     // Block is marked as free
    headBlock->size = SIZE_OF_SBRK_STEP - SIZE_OF_META_DATA;        // Size is set to SIZE_OF_SBRK_STEP
    
    // Set FirsttimeFlag to 1, indicating that initialization has been done for the first time
    FirsttimeFlag = 1;
}
/**
 * @brief Splits a memory block into two blocks, allocating the first block and leaving the second block free.
 *
 * This function splits the given memory block into two blocks. The first block is allocated
 * to fulfill the requested size, and the second block remains free. It updates the metadata
 * of both blocks and links them appropriately in the free list.
 *
 * @param block Pointer to the memory block to split.
 * @param size Size of the allocated memory block (excluding metadata).
 * @return Pointer to the newly allocated memory block.
 */
free_list* SplitBlock(free_list* block,int size )
{
    
    free_list* templist = (free_list*)(block->address + size + SIZE_OF_META_DATA);
    /*change the the address of the old block*/
    void* tempAddress = block->address;
    templist->address = block->address + size + SIZE_OF_META_DATA;
    templist->next = block->next;
    /*change the size of the old block */
    templist->size = block->size - size - SIZE_OF_META_DATA;

   
    /**make the new block before the old block by changing the privious of the old block and keeping the next the same */
    free_list* tempprivious = block->privios;
    templist->privios = (free_list*)tempAddress;

    /*assign the status of the new block */
    block->free = NOT_FREE;
    /**assign the size of the new block*/
    block->size = size;
    /*linking the new bloke withe the old block by update the next and privious of the new block*/
    block->next = (free_list*)templist->address;
    block->privios = tempprivious;
    /**update the start address of the new block*/
    block->address = tempAddress;

    return (free_list*)tempAddress;
 
}
/**
 * @brief Merges two adjacent free memory blocks into a single block.
 *
 * This function merges two adjacent free memory blocks into a single block. It updates
 * the next pointer of the first block to point to the next block after the second block.
 * If the next block is not NULL, it also updates the previous pointer of the next block
 * to point to the first block. Finally, it updates the size of the first block to be the
 * sum of the sizes of the two blocks plus the size of the metadata of the second block.
 *
 * @param BlockOne Pointer to the first memory block.
 * @param blockTwo Pointer to the second memory block.
 */
void MerageTwoBlk(free_list* BlockOne,free_list* blockTwo)
{
    /**change the next of the first block to the next of the second block */

    BlockOne->next = blockTwo->next;
    if(blockTwo->next != NULL)
         blockTwo->next->privios = (free_list*)BlockOne->address;
    /**change the size of the the block whic is the sum of the sze of the two blocks and 32byte(size of meta data of the second block)*/
    BlockOne->size = BlockOne->size + blockTwo->size + SIZE_OF_META_DATA; 
    

}

/**
 * @brief Allocates a block of memory of the given size.
 *
 * This function allocates a block of memory of the specified size. It first checks
 * the existing free memory blocks to see if there is a block that can accommodate
 * the requested size. If a suitable block is found, it is marked as allocated and
 * returned to the caller. If no suitable block is found, a new block is allocated
 * using the sbrk system call.
 *
 * @param size Size of the memory block to allocate.
 * @return Pointer to the allocated memory block, or NULL if allocation fails.
 */

void* My_malloc(int size)
{
    void* BloackAddress; /* Address of the allocated block */
    size = ((size+7)/8)*8;
    /* If this is the first allocation, initialize the memory management system */
    if (FirsttimeFlag == 0)
    {
        initailiztion();
    }

    /* Start from the head of the block list */
    current = headBlock;
                
    /* Iterate through the block list */
    while (current)
    {
        /* If the block is free */
        if (current->free == FREE)
        {
            /* If the block size is exactly equal to the requested size */
            if (current->size == size + SIZE_OF_META_DATA)
            {

                BloackAddress = current->address + SIZE_OF_META_DATA; /* Adjust for metadata */
                /*Change the status of the block to NOT_FREE*/
                current->free = NOT_FREE;
                /*updatre the size of the new block*/
                current->size = size;
                if(current->next == NULL)
                {
                    /*increasing the program breack since the previous block is the last free block*/
                    current->next = (free_list*)sbrk(SIZE_OF_SBRK_STEP);
                    /*update the size of the added block by th SIZE_OF_SBRK_STEP*/
                    current->next->size = SIZE_OF_SBRK_STEP - SIZE_OF_META_DATA;
                    /*linking the added block with the list and set its meata data */
                    current->next->address = current->next;
                    current->next->privios = current;
                    current->next->free = FREE;
                    current->next->next = NULL;
                }
                return BloackAddress;
            }
            /* If the block size is greater than the requested size plus metadata */
            else if (current->size > (size + SIZE_OF_META_DATA*2))
            {
                /* Split the block */
                free_list* tempBlock = SplitBlock(current, size);
                BloackAddress = tempBlock->address + SIZE_OF_META_DATA; /* Adjust for metadata */
                return BloackAddress;
            }
            else
            {
                current = current->next;
            }
        }
        else
        {
            current = current->next;
        }
    }

    /* If no suitable block was found, allocate a new block */
     /* Increase the program break */
    current = headBlock; /* Reset the current block to the head */
    LastLocation = sbrk(SIZE_OF_SBRK_STEP);
    /* Iterate through the block list again */
    while (current)
    {
        if (current->next == NULL)
        {
            /* Increase the size of the last block */
            if(current->next == FREE)
            {               
                current->size += SIZE_OF_SBRK_STEP -SIZE_OF_META_DATA;
                /* Split the block */
                free_list* tempBlock = SplitBlock(current, size);
                BloackAddress = tempBlock->address + SIZE_OF_META_DATA; /* Adjust for metadata */
            }
            else
            {
                
                current->next = (free_list*)(LastLocation);
                current->next->next=NULL;
                current->next->privios = (free_list*)current->address;
                current->next->free=FREE; 
                current->next->size = SIZE_OF_SBRK_STEP - SIZE_OF_META_DATA;
                free_list* tempBlock = SplitBlock(current->next, size);
                BloackAddress = tempBlock->address + SIZE_OF_META_DATA; /* Adjust for metadata */
            }

            return BloackAddress;
        }

        current = current->next;
    }
    return NULL;
}

/**
 * @brief Frees the memory block pointed to by ptr and merges adjacent free blocks if possible.
 *
 * This function marks the memory block pointed to by ptr as free and then checks
 * if the adjacent blocks are also free. If the next or previous block is free,
 * it merges the blocks into a single larger block to avoid fragmentation.
 *
 * @param ptr Pointer to the memory block to free.
 */
void my_free(void* ptr)
{
    /* Mark the block as free */
    free_list* Templist = (free_list*)(ptr-32);
    if(ptr!=0)
    {
        if(Templist->free != FREE)
            Templist->free = FREE;
        else{
            perror("rrrrrrrrr");
        } 

    /* Start from the head of the block list */
    current = headBlock;
    while (current)
    {
        if(current->next != NULL)
        {
            if (current->free==FREE && current->next->free==FREE)
            {
                MerageTwoBlk(current,current->next);
            }
        }
        /* Check if the current block is the last block */
        if (current->next == NULL)
        {
            /* If the last block is larger than the size of a single sbrk step, reduce the program break */
            if (current->size > SIZE_OF_SBRK_STEP)
            {
                
                
                LastLocation = sbrk(SIZE_OF_SBRK_STEP* -1);
                current->size -=  SIZE_OF_SBRK_STEP;
                break;
            }
            //Templist = current->next;
        }

        current = current->next;
    }

    }
    
}
/**
 * @brief Allocates memory for an array of num elements of size bytes each and initializes the memory to zero.
 *
 * This function allocates memory for an array of num elements, each of which is size bytes long.
 * It then initializes all the bytes in the allocated memory to zero. The total amount of memory
 * allocated is num * size bytes.
 *
 * @param num Number of elements to allocate memory for.
 * @param size Size of each element, in bytes.
 * @return Pointer to the allocated memory block, or NULL if allocation fails.
 */
void* my_calloc(size_t num,size_t size)
{
    void* ptr;
    long int totalSize = num * size;
    if(num == 0 )
    {
        return NULL;
    }
    else if(totalSize < 0){
        return NULL;
    }
    else{
        ptr = malloc(totalSize);
        
        memset(ptr,0,totalSize);
        
    }
        
    return ptr;

}
/**
 * @brief Changes the size of the memory block pointed to by ptr to the given newSize.
 *
 * This function reallocates the memory block pointed to by ptr to the size specified
 * by newSize. It allocates a new memory block of the given size, copies the contents
 * of the old block to the new block, and then frees the old block. If the reallocation
 * fails, NULL is returned and the original block is left unchanged.
 *
 * @param ptr Pointer to the memory block to reallocate.
 * @param newSize New size for the memory block.
 * @return Pointer to the reallocated memory block, or NULL if reallocation fails.
 */
void* my_realloc(void* ptr, size_t newSize)
{
    void* newptr;
    if(ptr == NULL)
    {
        newptr = malloc(newSize);
    }
    else if(0 == newSize && NULL!=ptr)
    {
        free(ptr);
    }
    else
    {
        
        newptr = malloc(newSize);
        if(newSize > ((free_list*)(ptr-32))->size)
        {
            memcpy(newptr,ptr,((free_list*)(ptr-32))->size);
        }
        else{
            memcpy(newptr,ptr,newSize);
        }
        
        free(ptr);
    }
    return newptr;

}
/**
 * @brief Displays information about each memory block in the free list.
 *
 * This function iterates through the free list and prints information about each memory block,
 * including its address, next and previous block addresses, size, and state (free or allocated).
 */
void DisplayBlocks()
{
    free_list* ptlist = headBlock;
    while(ptlist)
    {
        printf("address\t\t\t\tnext\t\t\t\tprev\t\t\t\tsize\t\t\t\tstate\n%p\t\t\t%p\t\t\t%p\t\t\t%d\t\t\t\t%d\n-------------------------------------------------------------------------------\n"
        ,(ptlist->address),ptlist->next,
        ptlist->privios,ptlist->size,ptlist->free);
        ptlist = ptlist->next;
    }
    printf("\n##################################################################################################################################################################\n");
}

void* malloc(size_t size)
{
    return My_malloc(size);
}
void free(void*ptr)
{
    my_free(ptr);
}

void* realloc(void* ptr,size_t newsize)
{
    
    return my_realloc(ptr,newsize);
}


void* calloc(size_t num ,size_t size)
{
    
    return my_calloc(num,size);
}

/*
#define NUM_ALLOCS 1000000
#define MAX_SIZE 102400
#define MAX_ITERATIONS 1000000

void random_alloc_free_test() {
    srand((unsigned int)time(NULL));
    
    void* pointers[NUM_ALLOCS] = {NULL};
    
    for (int i = 0; i < MAX_ITERATIONS; ++i) {
        int index = rand() % NUM_ALLOCS;
        if (pointers[index] == NULL) {
            // Allocate memory
            size_t size = (size_t)(rand() % MAX_SIZE) + 1;
            pointers[index] = malloc(size);
            if (pointers[index] != NULL) {
                printf("Allocated memory of size %zu at address %p\n", size, pointers[index]);
            } else {
                fprintf(stderr, "Allocation failed for size %zu\n", size);
            }
        } else {
            // Free memory
            printf("Freeing memory at address %p\n", pointers[index]);
            free(pointers[index]);
            pointers[index] = NULL;
        }
    }
    
    // Free remaining allocated memory
    for (int i = 0; i < NUM_ALLOCS; ++i) {
        if (pointers[i] != NULL) {
            printf("Freeing remaining memory at address %p\n", pointers[i]);
            free(pointers[i]);
            pointers[i] = NULL;
        }
    }
}

int main() {
    printf("Starting random allocation and deallocation test...\n");
    random_alloc_free_test();
    printf("Test complete.\n");
    return 0;
}*/
/*
int main()
{

    int *ptr1 = (int*)malloc(100);
    int *ptr2 = (int*)malloc(27);
    int *ptr3 = (int*)malloc(600);



int main(void)
{
    
    printf("%p\n",sbrk(0));
    int *ptr1 = (int*)My_malloc(50000);
    int *ptr2 = (int*)My_malloc(50000);
    
    int *ptr3 = (int*)My_malloc(50000);
    int *ptr4 = (int*)My_malloc(50000);
    printf("%p\n",sbrk(0));
    my_free(ptr1);
    my_free(ptr2);
    my_free(ptr3);
    my_free(ptr4);

    printf("%p\n",sbrk(0));

    DisplayBlocks();

    ptr4 = (int*)My_malloc(160);
    ptr3 = (int*)My_malloc(7000);
    my_free(ptr3);
    my_free(ptr4);   
    int*ptr5= (int*)My_malloc(3000);
    ptr1=(int*)My_malloc(7000);
    int*ptr6 = (int*)My_malloc(130000);
    DisplayBlocks();
    return 0;
}
*/
/*
int main()
{

    int *ptr[10000];
    for (int  i = 0; i < 10000; i++)
    {
        
    //if(i == 505)
    //sleep(10);
    //getchar();
        ptr[i] = (int*)malloc(10240);
        //printf("address[%d] = %p\n",i,ptr[i]);
        
        //if(ptr[i] == ptr[i-1] )
        //getchar();

    }
    for (int j = 5000 - 1; j < 10000; j += 1)
    {
        if(j == 9998)
        sleep(10);
        free(ptr[j]);
        //printf("address[%d] = %p\n",j,ptr[j]);
    }
    



} */
//gcc -o main.exe malloc.c && ./main.exe