#ifndef ARENA_H
#define ARENA_H

// This header file is based on the API provided by Ryan Fleury in his article:
// "Untangling Lifetimes: The Arena Allocator"
// https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator

// --- Includes and Typedefs ---

// Standard library for size_t (or equivalent)
#include <stddef.h>
#include <stdint.h>

// Use a consistent type for 64-bit unsigned integers, as used in the article.
typedef uint64_t U64;

// --- Arena Core Structure ---

// The structure that holds all the metadata for a memory arena.
// Its definition is intentionally visible to users so they can declare a pointer to it.
typedef struct Arena Arena;
struct Arena
{
	// char* used as it guarentees byte-level movement. a void* would forbid pointer arithmetic.
    char *base_pointer;     // Pointer to the start of the entire allocated memory block.
    size_t current_offset;  // The 'bump' pointer offset: where the next allocation will start.
    size_t capacity;        // The total size of the block acquired from the OS.
};

// --- Core Arena Management ---

// Allocates memory for the Arena struct and the large underlying memory block.
// Returns a pointer to the new, initialized Arena, or NULL on failure.
Arena *ArenaAlloc(size_t capacity);

// Returns the large block of memory managed by the arena back to the operating system.
// Should be called when the arena is no longer needed.
void ArenaRelease(Arena *arena);

// --- Memory Allocation (Push) ---

// Allocates 'size' bytes from the arena using bump allocation.
// Returns a pointer to the new memory, or NULL if capacity is exceeded.
void *ArenaPush(Arena *arena, U64 size);

// Allocates 'size' bytes and initializes the memory block to zero.
// Returns a pointer to the new, zeroed memory.
void *ArenaPushZero(Arena *arena, U64 size);

// --- Memory Reclamation (Pop & Reset) ---

// Gets the current offset (or position) of the next allocation.
// This value can be saved to later reset the arena's state.
U64 ArenaGetPos(Arena *arena);

// Moves the 'bump' pointer backward by 'size' bytes.
// This is dangerous and should only be used if 'size' exactly matches the last allocation's size (including padding).
void ArenaPop(Arena *arena, U64 size);

// Resets the 'bump' pointer to a previously saved position ('pos').
// All memory allocated since 'pos' is instantaneously reclaimed for future allocations.
void ArenaSetPosBack(Arena *arena, U64 pos);

// Resets the 'bump' pointer to the very beginning (position 0).
// Effectively frees all allocated memory without returning the block to the OS.
void ArenaClear(Arena *arena);


// --- Convenience Macros ---

// Allocates 'count' elements of 'type'.
#define PushArray(arena, type, count) (type *)ArenaPush((arena), sizeof(type)*(count))

// Allocates 'count' elements of 'type' and zeroes the memory.
#define PushArrayZero(arena, type, count) (type *)ArenaPushZero((arena), sizeof(type)*(count))

// Allocates a single instance of 'type'.
#define PushStruct(arena, type) PushArray((arena), (type), 1)

// Allocates a single instance of 'type' and zeroes the memory.
#define PushStructZero(arena, type) PushArrayZero((arena), (type), 1)

#endif // ARENA_H
