#pragma once

#if defined(__cplusplus)
    extern "C" {
#endif

#include "vr_types.h"

#include <stdlib.h>
#include <stddef.h>

// *****************************************************************************
// BASIC ALLOCATION UTILITY
// *****************************************************************************
// Basic debugging utility for tracking raw allocations (inspired by Eskil S.)

// Users can turn on 'debug' logging of raw mallocs for finding memory leaks
#ifdef VR_STD_DEBUG_MEM
    // TODO: finish debug funcs
    extern void*    _vr_debug_malloc(size_t size, char* fname, uint32 line_num);
    extern void*    _vr_debug_realloc(void* ptr, size_t size, char* fname, uint32 line_num);
    extern void     _vr_debug_free(void* buf);
    extern void     _vr_debug_print_memory_log();
    extern void     _vr_debug_reset_memory_log();
    
    #define vr_malloc(n)        _vr_debug_malloc(n, __FILE__, __LINE__)
    #define vr_realloc(n, m)    _vr_debug_realloc(n, m, __FILE__, __LINE__)
    #define vr_free(n)          _vr_debug_free(n)
#else
    #define vr_malloc(n)        malloc(n)
    #define vr_realloc(n, m)    realloc(n, m)
    #define vr_free(n)          free(n)
#endif

// *****************************************************************************
// MEMORY ARENA
// *****************************************************************************
// Simple memory arena implementation (heavily inspired by Casey M.)

#define VR_STD_DEFAULT_MEMORY_ALIGNMENT 4

// TODO: should probably allow users to change this per arena/block
#ifndef VR_STD_DEFAULT_MINIMUM_BLOCK_SIZE
    #define VR_STD_DEFAULT_MINIMUM_BLOCK_SIZE 1024*1024
#endif

typedef struct vrMemoryBlock {
    uint8*  base;
    size_t  size;
    size_t  used;
    struct vrMemoryBlock*  prev;
} vrMemoryBlock;

typedef struct vrMemoryArena {
    vrMemoryBlock*  current_block;
    size_t          minimum_block_size;
    size_t          alignment;
} vrMemoryArena;

inline vrMemoryArena    vr_create_default_arena();
inline void             vr_clear_arena(vrMemoryArena* arena);

inline vrMemoryBlock*   vr_allocate_block(size_t size);
inline void             vr_free_last_block(vrMemoryArena* arena);

inline void*            vr_copy(void* source_ptr, void* dest_ptr, size_t size);

inline size_t           vr_get_alignment_offset(vrMemoryArena* arena);
inline size_t           vr_get_effective_size(vrMemoryArena* arena, size_t init_size);

inline void*            vr_push_size(vrMemoryArena* arena, size_t init_size);
inline void*            vr_push_copy(vrMemoryArena* arena, void* source_ptr, size_t size);

// Quality of life macros for general arena-based allocs
#define vr_push_struct(arena, type) (type*)vr_push_size(arena, sizeof(type))
#define vr_push_array(arena, type, count) (type*)vr_push_size(arena, sizeof(type)*count)

#if defined(__cplusplus)
    }
#endif
