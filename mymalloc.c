#include <stddef.h>
#include <unistd.h>
#include <stdio.h>

typedef struct Block {
	size_t size;
	int status; // 1 for used 0 for free
	struct Block* next;
} Block;

Block* freeBlockPtr = NULL;

void* mymalloc(size_t bytes) {
	// malloc ran for first time, initalize freeBlockPtr linked-list
	if (freeBlockPtr == NULL) {
		Block* newBlock = sbrk(sizeof(Block) + bytes);
		newBlock->size = bytes;
		newBlock->next = NULL;

		freeBlockPtr = newBlock;
		freeBlockPtr->status = 1;
		return (void*)(freeBlockPtr+1);
	}

	// iterate over the free blocks linkedlist to find a big enough free block for the user
	Block* list = freeBlockPtr;
	while (list != NULL) {
		if (list->size >= bytes && list->status == 0) {
			list->status = 1;
			// returns the memory region after header |Block + sizeof(Block)| usable_ptr|
			return (void*)(list+1); 
		}
		list = list->next;
	}

	// No big enough block is found, allocate 
	Block* newBlock = sbrk(sizeof(Block) + bytes);
	newBlock->status = 1;
	newBlock->size = bytes;
	newBlock->next = NULL;

	list->next = newBlock;
	return list->next;
}

void free(void* ptr) {
}

int main() {
	mymalloc(64);
}

