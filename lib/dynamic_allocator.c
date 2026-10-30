/*
 * dynamic_allocator.c
 *
 *  Created on: Sep 21, 2023
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"

/************************************************************************************/
//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//
//TODO: [PROJECT'26.GM#1] DYNAMIC ALLOCATOR - [0] GIVEN FUNCTIONS
//==================================
// [1] INITIALIZE BUDDY FREE LISTS:
//==================================
void buddy_init()
{
	//panic("buddy_init is not implemented yet");

	for (int l = BUDDY_LOWER_LEVEL; l <= BUDDY_UPPER_LEVEL; ++l)
	{
		LIST_INIT(&(freeBlockLists[l]));
	}
}
//====================================
// [2] PRINT SIZE OF FREE LISTS:
//====================================
void PrintFreeBlockLists()
{
	for (int i = BUDDY_LOWER_LEVEL; i <= BUDDY_UPPER_LEVEL; ++i)
	{
		cprintf("Level#%d: %d\n", i, LIST_SIZE(&freeBlockLists[i]));
	}
}
/************************************************************************************/

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
//===========================
// [1] GET BLOCK SIZE:
//===========================
__inline__ uint32 get_block_size(void* va)
{
	struct BlockMetaData *curBlkMetaData = ((struct BlockMetaData *)va - 1) ;
//		return (pow(2,curBlkMetaData->level));
	return (1U << curBlkMetaData->level);
//	uint32 *curBlkMetaData = ((uint32 *)va - 1) ;
//	return (*curBlkMetaData) & ~(0x1);
}
//===========================
// [2] ALLOCATE BLOCK:
//===========================
void *alloc_block(uint32 requiredSize)
{
    if (requiredSize == 0)
        return NULL;

    uint32 totalSize = requiredSize + META_DATA_SIZE;
    int target_level = BUDDY_LOWER_LEVEL;

    while (target_level <= BUDDY_UPPER_LEVEL && BLOCK_SIZE(target_level) < totalSize)
        target_level++;

    if (target_level > BUDDY_UPPER_LEVEL)
        return NULL;

    int level = target_level;
    while (level <= BUDDY_UPPER_LEVEL && LIST_EMPTY(&freeBlockLists[level]))
        level++;

    struct BlockMetaData *block = NULL;

    if (level > BUDDY_UPPER_LEVEL)
    {
        block = (struct BlockMetaData *)create_buddy_space();
        if (!block)
            return NULL;

        level = BUDDY_UPPER_LEVEL;

        struct BlockMetaData *buddy = (struct BlockMetaData *)((char *)block + BLOCK_SIZE(level));
        block->level = level;
        buddy->level = level;
        buddy->is_free = 1;

        LIST_INSERT_HEAD(&freeBlockLists[level], buddy);
    }
    else
    {
        block = LIST_FIRST(&freeBlockLists[level]);
        LIST_REMOVE(&freeBlockLists[level], block);
    }

    while (level > target_level)
    {
        level--;
        struct BlockMetaData *buddy = (struct BlockMetaData *)((char *)block + BLOCK_SIZE(level));
        block->level = level;
        buddy->level = level;
        buddy->is_free = 1;
        LIST_INSERT_HEAD(&freeBlockLists[level], buddy);
    }

    block->is_free = 0;
    return (void *)((char *)block + META_DATA_SIZE);
}

//===================================================
// [3] FREE BLOCK WITH COALESCING:
//===================================================
void free_block(void *va)
{
	struct BlockMetaData *freeBlock = ((struct BlockMetaData *)va - 1) ;
	freeBlock->is_free = 1;

	while (freeBlock-> level < BUDDY_UPPER_LEVEL){
			//find buddy
			uint32 size = BLOCK_SIZE(freeBlock->level);
			uint32 buddyAddress = (uint32)freeBlock ^ size;
			struct BlockMetaData *buddyBlock = (struct BlockMetaData *)buddyAddress ;

			if(!buddyBlock->is_free || buddyBlock->level != freeBlock->level){
				break;
			}else{
				LIST_REMOVE(&freeBlockLists[buddyBlock->level], buddyBlock);
				if(buddyBlock < freeBlock)
					freeBlock = buddyBlock;

				freeBlock->level++;
			}

	}

	if (freeBlock->level == BUDDY_UPPER_LEVEL)
		delete_buddy_space(freeBlock);

	LIST_INSERT_TAIL(&freeBlockLists[freeBlock->level], freeBlock);


}

//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//

//===========================
// [1] REALLOCATE BLOCK:
//===========================
void *realloc_block(void* va, uint32 newSize)
{
	//TODO: [PROJECT'26.BONUS#1] KERNEL REALLOC - realloc_block
	//Your code is here
	//Comment the following line
	panic("realloc_block() Not implemented yet");
}
