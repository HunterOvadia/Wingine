#pragma once

#if defined(__clang__) || defined(__gcc__)
#define STATIC_ASSERT _Static_assert
#else
#define STATIC_ASSERT static_assert
#endif

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;
STATIC_ASSERT(sizeof(uint8) == 1, "Expected uint8 to be 1 byte.");
STATIC_ASSERT(sizeof(uint16) == 2, "Expected uint16 to be 2 bytes.");
STATIC_ASSERT(sizeof(uint32) == 4, "Expected uint32 to be 4 bytes.");
STATIC_ASSERT(sizeof(uint64) == 8, "Expected uint64 to be 8 bytes.");

typedef signed char         int8;
typedef signed short        int16;
typedef signed int          int32;
typedef signed long long    int64;
STATIC_ASSERT(sizeof(int8) == 1, "Expected int8 to be 1 byte.");
STATIC_ASSERT(sizeof(int16) == 2, "Expected int16 to be 2 bytes.");
STATIC_ASSERT(sizeof(int32) == 4, "Expected int32 to be 4 bytes.");
STATIC_ASSERT(sizeof(int64) == 8, "Expected int64 to be 8 bytes.");

typedef float               float32;
typedef double              float64;
STATIC_ASSERT(sizeof(float32) == 4, "Expected f32 to be 4 bytes.");
STATIC_ASSERT(sizeof(float64) == 8, "Expected f64 to be 8 bytes.");

#define STATIC_ARRAY_SIZE(InArray) (sizeof((InArray))/sizeof((InArray)[0]))