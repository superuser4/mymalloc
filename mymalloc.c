#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

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
	Block* header = (Block*)ptr - 1;
	
	if (ptr == NULL || header->status == 0) {
		return;
	}
	header->status = 0;

	if (header->next != NULL) {
		if (header->next->status == 0) {
			// check adjacency
			char* end_of_block = (char*)header + sizeof(Block) + header->size;  
			if ((void*)end_of_block == (void*)header->next) {
				// mark as header->next memory region as this block's
				header->size += header->next->size;
				
				// remove from linked list
				header->next = header->next->next;
			}
		}
	}
}

int main() {
    printf("========== Basic Allocation ==========\n");
    char* a = (char*)mymalloc(10);
    char* b = (char*)mymalloc(20);
    char* c = (char*)mymalloc(30);
    strcpy(a, "Hello");
    strcpy(b, "World");
    strcpy(c, "!");
    printf("a: %s\nb: %s\nc: %s\n", a, b, c);

    printf("\n========== Address & Distance Check ==========\n");
    printf("a: %p\n", (void*)a);
    printf("b: %p\n", (void*)b);
    printf("c: %p\n", (void*)c);
    printf("Distance a -> b: %ld bytes\n", (char*)b - (char*)a);
    printf("Distance b -> c: %ld bytes\n", (char*)c - (char*)b);

    printf("\n========== Free and Reuse ==========\n");
    myfree(b);
    char* b2 = (char*)mymalloc(20);
    printf("Reallocated b2 (should reuse b): %p (was %p)\n", (void*)b2, (void*)b);
    strcpy(b2, "Reuse");
    printf("b2: %s\n", b2);

    printf("\n========== Coalescing Check ==========\n");
    myfree(a);
    myfree(c);
    // Now, a and c are free and (probably) adjacent; test a bigger alloc:
    char* ac = (char*)mymalloc(45);
    printf("Allocated ac (should fit in coalesced a+c): %p\n", (void*)ac);
    strcpy(ac, "Coalesced");
    printf("ac: %s\n", ac);

    printf("\n========== Double Free and NULL Free Safety ==========\n");
    myfree(ac);    // Normal free
    myfree(ac);    // Double free (should be safe)
    myfree(NULL);  // NULL free (should be safe)

    printf("\n========== Stress Test (Allocate + Free Loop) ==========\n");
    for (int i = 0; i < 1000; i++) {
        char* temp = (char*)mymalloc(16);
        assert(temp != NULL);  // Make sure allocation succeeds
        temp[0] = 'A';
        myfree(temp);
    }
    printf("Stress test passed.\n");

    printf("\n========== All tests completed successfully ==========\n");
    return 0;
}

