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

#define plex struct

#define KiloBytes(Value) ((Value) * 1024)
#define MegaBytes(Value) (KiloBytes(Value) * 1024)
#define GigaBytes(Value) (MegaBytes(Value) * 1024)

#define ECB_ASSERT(Expression) if(!(Expression)) {(*(u32*)0) = 0;}

#define ArraySize(Array) (sizeof((Array)) / sizeof((Array)[0]))

file_scope b32
ECB_IsStringEqual(char* A, char* B)
{
  b32 Result = 1;
  while(*A && *B)
  {
    if(*A++ != *B++)
    {
      Result = 0;
      break;
    }
  }

  return Result;
}

#endif //@NOTE(Emilio): _ECB_UTILS_H_
