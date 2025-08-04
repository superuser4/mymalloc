# My malloc
This my mini malloc implementation, this project mainly exists for educational purposes, to teach me how malloc and free work under the hood. This is not meant to be a production grade allocator or meant to have advanced features many allocators support.

## Features
- Single linked list containing memory blocks
- Usage of sbrk to ask the kernel for memory
- Appropiate block of memory search strategy is first-fit
- myfree() supports (adjacent) next block coalescing to reduce fragmentation

## What's missing
- No seperated lists for different sizes and/or free/allocated blocks or bins like glibc malloc
- no usage of mmap for larger chunks (eg. bigger than 128kb like glibc malloc does)
- No sophisticated search strategies (basic linear first-fit)
- Fixed Block header structure
- No thread safety
- No protections against exploitation (no stack canaries, no guard pages)


