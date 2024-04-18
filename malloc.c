/****************************************************************************************/
/*******File   : malloc.c ***************************************************************/
/*******Auther : Mahmoud Abdelmoniem ****************************************************/
/*******Data   : 15/4/2024 **************************************************************/
/*******Version: 0.0*********************************************************************/
/****************************************************************************************/

#include "malloc.h"
#include <unistd.h>
#include <string.h>
free_list *baseList;
free_list *headBlock;
free_list * current ;
char FirsttimeFlag=0;
void * LastLocation ;

void initailiztion()
{
    /*Initialize baseList and headBlock to the current program break*/
    baseList = (free_list*)sbrk(0);
    headBlock = (free_list*)sbrk(0);
    
    /*Move LastLocation to a new location in memory by increasing the program break*/
    LastLocation = sbrk(SIZE_OF_SBRK_STEP);
    
    /*Initialize baseList with the following properties*/
    baseList->next = NULL;                    // Next pointer is set to NULL
    baseList->address = (void*)baseList;       // Address is set to the address of baseList
    baseList->privios = NULL;                  // Previous pointer is set to NULL
    baseList->free = FREE;                     // Block is marked as free
    baseList->size = SIZE_OF_SBRK_STEP;        // Size is set to SIZE_OF_SBRK_STEP
    
    // Set FirsttimeFlag to 1, indicating that initialization has been done for the first time
    FirsttimeFlag = 1;
}
/**
 * 
 * 
 * 
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

void MerageTwoBlk(free_list* BlockOne,free_list* blockTwo)
{
    /**change the next of the first block to the next of the second block */
    BlockOne->next = blockTwo->next;
    if(blockTwo->next != NULL)
         blockTwo->next->privios = BlockOne->address;
    /**change the size of the the block whic is the sum of the sze of the two blocks and 32byte(size of meta data of the second block)*/
    BlockOne->size = BlockOne->size + blockTwo->size + SIZE_OF_META_DATA; 

}

/** Function to allocate memory */
void* My_malloc(int size)
{
    void* BloackAddress; /* Address of the allocated block */

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
               // free_list* lastfree = 
                current->free = NOT_FREE;
                current->size = size;
                current->next = (free_list*)sbrk(SIZE_OF_SBRK_STEP);
                current->next->size = SIZE_OF_SBRK_STEP;
                current->next->address = current->next;
                current->next->privios = current;
                current->next->free = FREE;
                current->next->next = NULL;
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
    LastLocation = sbrk(SIZE_OF_SBRK_STEP); /* Increase the program break */
    current = headBlock; /* Reset the current block to the head */

    /* Iterate through the block list again */
    while (current)
    {
        if (current->next == NULL)
        {
            /* Increase the size of the last block */
            current->size += SIZE_OF_SBRK_STEP;
            /* Split the block */
            free_list* tempBlock = SplitBlock(current, size);
            BloackAddress = tempBlock->address + SIZE_OF_META_DATA; /* Adjust for metadata */
            return BloackAddress;
        }

        current = current->next;
    }
    return NULL;
}

/** Function to free memory */
void my_free(void* ptr)
{
    /* Mark the block as free */
    free_list* Templist = (free_list*)(ptr-32);
    Templist->free = FREE;

    if((void*)(Templist->next) != NULL)
    {
        if((Templist->next)->free == FREE)
        {
            MerageTwoBlk(Templist,Templist->next);
        }
    }
    if(Templist->privios != NULL)
    {
        if((Templist->privios)->free == FREE )
        {
            MerageTwoBlk(Templist->privios ,Templist);
        }   
    }


    /* Start from the head of the block list */
    current = headBlock;
    while (current)
    {
        /* Check if the current block is the last block */
        if (current->next == NULL)
        {
            /* If the last block is larger than the size of a single sbrk step, reduce the program break */
            if (current->size > SIZE_OF_SBRK_STEP)
            {
                LastLocation = sbrk(SIZE_OF_SBRK_STEP * -1);
            }
            //Templist = current->next;
        }

        current = current->next;
    }
}

void* my_calloc(int num,int size)
{
    long int totalSize = num * size;
    void *ptr = My_malloc(totalSize);
    if(ptr != NULL)
    {
        memset(ptr,0,totalSize);
    }
    return ptr;

}
void* my_realloc(void* ptr, size_t newSize)
{
    void* newptr = My_malloc(newSize);
    if(newptr != NULL)
    {
        memcpy(newptr,ptr,((free_list*)(ptr-SIZE_OF_META_DATA))->size);
        my_free(ptr);
    }
    return newptr;

}
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
    printf("\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
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
    my_free(ptr4);
    DisplayBlocks();
    ptr4 = (int*)My_malloc(160);
    DisplayBlocks();
    my_free(ptr2);
    my_free(ptr3);
    ptr3 = (int*)My_malloc(7000);
    my_free(ptr3);
    my_free(ptr4);
    my_free(ptr1);
    my_free(ptr2);    
    int*ptr5= (int*)My_malloc(3000);
    ptr1=(int*)My_malloc(7000);
    int*ptr6 = (int*)My_malloc(130000);
    DisplayBlocks();
    return 0;
}


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