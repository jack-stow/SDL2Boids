#include "arena.h"
#include<stdio.h>
#include<stdlib.h>
#include <assert.h>
#include <string.h>

// Allocates memory or the Arena struct and the large underlying memory block.
// Returns a pointer to the new, initialized Arena, or NULL on failure
Arena *ArenaAlloc(size_t capacity){
	// TODO: consider doing malloc(sizeof(Arena)+capacity) and setting the struct to the first few bytes of the block
	// This would keep the entire structure in contiguous memory. better cache+less heap fragmentation.
	// however, it would make block resizing more difficult + potential other issues.
	Arena *arena = malloc(sizeof(Arena));
	if (!arena) return NULL;

	arena->base_pointer = malloc(capacity);
	if (arena->base_pointer == NULL){
		// handle partial allocation failure
		free(arena);
		return NULL;
	}
	arena->current_offset = 0;
	arena->capacity = capacity;
	return arena;
}

// Returns the large block of memory managed by the arena back to the operating system.
// Should be called when the arena is no longer needed.
void ArenaRelease(Arena *arena){
	free(arena->base_pointer);
	free(arena);
}

// Allocates 'size' bytes from the arena using bump allocation
// Returns a pointer to the new memory, or NULL if capacity is exceeded.
void *ArenaPush(Arena *arena, U64 size){
	// confirm that there's enough room left in the arena.
	if (arena->current_offset + size > arena->capacity) return NULL;
	void *ptr = arena->base_pointer + arena->current_offset;
	arena->current_offset += size;
	return ptr;
}

// Allocates 'size' bytes and initializes the memory block to zero.
// Returns a pointer to the new, zeroed memory.
void *ArenaPushZero(Arena *arena, U64 size){
	void *ptr = ArenaPush(arena, size);
	if (!ptr) return NULL;
	memset(ptr, 0, size);
	return ptr;
}

// Gets the current offset (or position) of the next allocation.
// This value can be saved to later reset the arena's state
U64 ArenaGetPos(Arena *arena){
	return arena->current_offset;
}

// Moves the 'bump' pointer backward by 'size' bytes.
// This is dangerous and should only be used if 'size' exactly matches the last allocation's size (including padding).
void ArenaPop(Arena *arena, U64 size){
	assert(size <= arena->current_offset); // sanity check
	arena->current_offset -= size;
}

// Resets the 'bump' pointer to a previously saved position ('pos').
// All memory allocated since 'pos' is instantaneously reclaimed for future allocations.
void ArenaSetPosBack(Arena *arena, U64 pos){
	arena->current_offset = pos;
}

// Resets the 'bump' pointer to the very beginning (position 0).
// Effectively frees all allocated memory without returning the block to the OS.
void ArenaClear(Arena *arena){
	arena->current_offset = 0;
}

