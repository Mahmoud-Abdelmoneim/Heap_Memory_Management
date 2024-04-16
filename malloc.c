#include "malloc.h"
#include <unistd.h>

free_list *baseList;
free_list *headBlock;
free_list * current ;
char FirsttimeFlag=0;
void * LastLocation ;
void initailiztion()
{
    baseList = (free_list*)sbrk(0);
    headBlock = (free_list*)sbrk(0);
    LastLocation = sbrk(SIZE_OF_SBRK_STEP);
    baseList->next = NULL;
    baseList->address = (void*)baseList;
    baseList->privios = NULL;
    baseList->free = FREE;
    baseList->size = SIZE_OF_SBRK_STEP;
    FirsttimeFlag = 1;
    
}

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
    /**change the size of the the block whic is the sum of the sze of the two blocks and 32byte(size of meta data of the second block)*/
    BlockOne->size = BlockOne->size + blockTwo->size +32; 

}




void* My_malloc(int size)
{

    void * BloackAddress;
    if(FirsttimeFlag == 0)
    {
        initailiztion();
    }
    current = headBlock;

    while (current)
    {
        if(current->next!=NULL)
        {
            if(current->free == FREE && current->next->free == FREE)
            {
                /**Mearge the two block*/
                MerageTwoBlk(current,current->next);
            }

        }

        current = current->next;
    }
    current = headBlock;
    while (current)
    {
        
        if(current->free == FREE)
        {
            if(current->size == size)
            {
                BloackAddress = current->address + 32;
                return BloackAddress;
            }
            else if(current->size > size+32)
            {
                /*split the block*/
                free_list * tempBlock = SplitBlock(current,size);
                BloackAddress = tempBlock->address +32;
                return BloackAddress;
            }
            else{
                current= current->next;
            }

        }
        else{
            current = current->next;
        }
        //test = current->size; 
        //current = current->next;
    }

    LastLocation = sbrk(SIZE_OF_SBRK_STEP);
    current = headBlock;
    while (current)
    {
        /* code */
        if(current->next == NULL)
        {
            current->size += SIZE_OF_SBRK_STEP;
            free_list* tempBlock = SplitBlock(current,size);
            BloackAddress = tempBlock->address +32;
            return BloackAddress;  
            //current->next->next;
        }
      
        current = current->next;  
        
    }
    
     


      

}
void my_free(void* ptr)
{

    ((free_list*)(ptr-32))->free = FREE;

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

int main(void)
{
    free_list x;
    
    int *ptr1 = (int*)My_malloc(2000);
    int *ptr2 = (int*)My_malloc(6000);
    
    int *ptr3 = (int*)My_malloc(2000);
    int *ptr4 = (int*)My_malloc(150);
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


//gcc -o main.exe malloc.c && ./main.exe