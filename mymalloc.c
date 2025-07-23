#include <stddef.h>
#include <string.h>
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
		if (newBlock == (void*)-1) {
			return NULL;
		}
		newBlock->size = bytes;
		newBlock->next = NULL;
		newBlock->status = 1;

		freeBlockPtr = newBlock;
		return (void*)(freeBlockPtr+1);
	}

	// iterate over the free blocks linkedlist to find a big enough free block for the user
	Block* list = freeBlockPtr;
	Block* last = NULL;
	while (list != NULL) {
		if (list->size >= bytes && list->status == 0) {
			list->status = 1;
			// returns the memory region after header |Block + sizeof(Block)| usable_ptr|
			return (void*)(list+1); 
		}
		last = list;
		list = list->next;
	}

	// No big enough block is found, allocate 
	Block* newBlock = sbrk(sizeof(Block) + bytes);
	if (newBlock == (void*)-1) {
		return NULL;
	}

	newBlock->status = 1;
	newBlock->size = bytes;
	newBlock->next = NULL;
	
	last->next = newBlock;
	return (void*) (newBlock+1);
}

void myfree(void* ptr) {
}

int main() {
	char* a = (char*)mymalloc(10);
	char* b = (char*)mymalloc(20);
	char* c = (char*)mymalloc(30);

	// Write to allocated memory
	strcpy(a, "hello");
	strcpy(b, "world");
	strcpy(c, "!");

	printf("%s %s%s\n", a, b, c);  // Should print: hello world!

	// Print memory addresses 
	printf("a: %p\n", a);
	printf("b: %p\n", b);
	printf("c: %p\n", c);

	// Check distance between blocks (rough fragmentation check)
	printf("Distance a -> b: %ld bytes\n", (char*)b - (char*)a);
	printf("Distance b -> c: %ld bytes\n", (char*)c - (char*)b);

	return 0;
}

