#if !defined(_ECB_UTILS_H_)
#define _ECB_UTILS_H_

#include <stdint.h>

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;

typedef int8_t    s8;
typedef int16_t   s16;
typedef int32_t   s32;
typedef int64_t   s64;

typedef float     r32;
typedef double    r64;
typedef uint32_t  b32;


#define global_variable static
#define local_persist static
#define file_scope static


#endif //@NOTE(Emilio): _ECB_UTILS_H_
