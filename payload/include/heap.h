#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>

#define panic(x) fprintf(stderr, "PANIC: %s", x);exit(1)

typedef struct chunk_header {
	int prev_size;
	int size;
	struct chunk_header* next;
	struct chunk_header** head;
} chunk_header_t;

typedef struct heap_chunk {
	void* address;
	uint32_t size;
} heap_chunk_t;

extern uint32_t gHeapRemaining;
extern uint32_t gHeapChunkCount;
extern heap_chunk_t gHeapChunkList[3];
extern chunk_header_t* gHeapHeader[32];

void _free(void* ptr);
void* _malloc(uint32_t size);
void _heap_add_chunk(void* address, uint32_t size);

uint32_t _get_zone(uint32_t size);
uint32_t _get_min_alloc(uint32_t size);
chunk_header_t* _get_next_chunk(chunk_header_t* chunk);
chunk_header_t* _get_prev_chunk(chunk_header_t* chunk);
void _link_chunk(chunk_header_t* chunk, unsigned int size);

#endif
