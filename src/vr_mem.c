#include "vr_mem.h"

// *****************************************************************************
// BASIC ALLOCATION UTILITY
// *****************************************************************************

#ifdef VR_STD_DEBUG_MEM
void* _vr_debug_malloc(size_t size, char* fname, uint32 line_num) {
    void* result = NULL;

    return result;
}

void* _vr_debug_realloc(void* ptr, size_t size, char* fname, uint32 line_num) {
    void* result = NULL;

    return result;
}

void _vr_debug_free(void* buf) {

}

void _vr_debug_print_memory_log() {

}

void _vr_debug_reset_memory_log() {

}
#endif

// *****************************************************************************
// MEMORY ARENA
// *****************************************************************************

inline vrMemoryBlock* vr_allocate_block(size_t size) {
    // a block isn't just an array of bytes, account for additional members
    vrMemoryBlock* block = vr_malloc(sizeof(vrMemoryBlock) + size);

    // our byte array starts at the beginning of the struct, otherwise an offset is needed
    block->base = (uint8*)block;
    block->size = size;
    block->used = 0;
    block->prev = NULL;
    return block;
}

inline vrMemoryArena vr_create_default_arena() {
    // initialize an arena with default parameters
    vrMemoryArena arena;
    arena.current_block = NULL;
    arena.minimum_block_size = VR_STD_DEFAULT_MINIMUM_BLOCK_SIZE;
    arena.alignment = VR_STD_DEFAULT_MEMORY_ALIGNMENT;
    return arena;
}

inline void* vr_copy(void* source_ptr, void* dest_ptr, size_t size) {
    // copy over as if simple byte arrays
    uint8* src_target = (uint8*)source_ptr;
    uint8* dst_target = (uint8*)dest_ptr;
    while(size--) {
        *dst_target = *src_target++;
    }

    return dest_ptr;
}

inline size_t vr_get_alignment_offset(vrMemoryArena* arena) {
    size_t offset = 0;

    // where are we at in the current block
    size_t result_ptr = (size_t)arena->current_block->base + arena->current_block->used;
    size_t mask = arena->alignment - 1;
    if(result_ptr & mask) {
        // if we are offset then add the remainder to correct
        offset = arena->alignment - (result_ptr & mask);
    }

    return offset;
}

inline size_t vr_get_effective_size(vrMemoryArena* arena, size_t init_size) {
    size_t size = init_size;

    // calculate new size with respect to alignment
    size_t alignment_offset = vr_get_alignment_offset(arena);

    size += alignment_offset;

    return size;
}

inline void* vr_push_size(vrMemoryArena* arena, size_t init_size) {
    void* result = 0;

    size_t size;

    // calculate actual size after alignment
    if(arena->current_block) {
        size = vr_get_effective_size(arena, init_size);
    }

    // will this allocation fit in the current block?
    if((!arena->current_block) || ((arena->current_block->used + size) > arena->current_block->size)) {
        // don't need to worry about alignment on a brand new block
        size = init_size;

        // TODO: make this changeable if needed
        arena->minimum_block_size = VR_STD_DEFAULT_MINIMUM_BLOCK_SIZE;

        // make the block at least this size unless the allocation is larger
        size_t block_size;
        if(size >= arena->minimum_block_size) {
            block_size = size;
        } else {
            block_size = arena->minimum_block_size;
        }

        // insert new block into the arena
        vrMemoryBlock* block = vr_allocate_block(block_size);
        block->prev = arena->current_block;
        arena->current_block = block;
    }

    // now we can fit this allocation into the current block
    size_t offset = vr_get_alignment_offset(arena);

    // get pointer to our region
    result = arena->current_block->base + arena->current_block->used + offset;

    // update used size
    arena->current_block->used += size;

    return result;
}

inline void* vr_push_copy(vrMemoryArena* arena, void* source_ptr, size_t size) {
    void* result = vr_push_size(arena, size);

    vr_copy(source_ptr, result, size);

    return result;
}

inline void vr_free_last_block(vrMemoryArena* arena) {
    vrMemoryBlock* free_block = arena->current_block;
    arena->current_block = free_block->prev;
    vr_free(free_block);
}

inline void vr_clear_arena(vrMemoryArena* arena) {
    while(arena->current_block) {
        vr_free_last_block(arena);
    }
}
