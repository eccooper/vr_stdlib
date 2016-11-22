#pragma once

#ifndef VR_STD_NO_TYPES // if user defines these themselves

// Standard types

#if defined(__cplusplus)
    extern "C" {
#endif

#ifdef VR_STD_NO_STDINT // TODO: should work on any platform we actually use?
    typedef signed char     int8;
    typedef signed short    int16;
    typedef signed int      int32;
    typedef signed long     int64;

    typedef unsigned char   uint8;
    typedef unsigned short  uint16;
    typedef unsigned int    uint32;
    typedef unsigned long   uint64;
     
    typedef float           real32;
    typedef double          real64;

    typedef unsigned char   boolean;
 #else
    #include <stdint.h>

    typedef int8_t          int8;
    typedef int16_t         int16;
    typedef int32_t         int32;
    typedef int64_t         int64;

    typedef uint8_t         uint8;
    typedef uint16_t        uint16;
    typedef uint32_t        uint32;
    typedef uint64_t        uint64;

    typedef float           real32;
    typedef double          real64;

    typedef uint8_t         boolean;
#endif // VR_STD_NO_STDINT

#if defined(__cplusplus)
    }
#endif

#endif // VR_STD_NO_TYPES
