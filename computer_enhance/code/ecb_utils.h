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
#define ECB_TEMP_PRINT_BUFFER_SIZE 1024

#define ArraySize(Array) (sizeof((Array)) / sizeof((Array)[0]))

//@NOTE(Emilio): This initial implementation forces all file reads to be under 32 bits
typedef plex
{
  void* Content;
  u32 ContentSize;
} ecb_file_result;

#define ECB_READ_ENTIRE_FILE(name) ecb_file_result name(char* Filename)
ECB_READ_ENTIRE_FILE(ECBReadEntireFileStub)
{
  ecb_file_result Result = {};

  return Result;
}
#define ECB_WRITE_ENTIRE_FILE(name) u32 name(ecb_file_result* FileResult, char* Filename)
ECB_WRITE_ENTIRE_FILE(ECBWriteEntireFileStub)
{
  return 0;
}
#define ECB_FREE_FILE(name) void name(ecb_file_result* FileResult)
ECB_FREE_FILE(ECBFreeFileStub)
{
  return;
}

typedef plex
{
  void* Content;
  u32 ContentSize;
  u32 ContentCapacity;
} ecb_string;
#define ECB_CREATE_STRING(name) ecb_string name(char *InitialContent, u32 InitialContentSize, u32 Capacity)
ECB_CREATE_STRING(ECBCreateStringStub)
{
  ecb_string Result = {};

  return Result;
}

#define ECB_FREE_STRING(name) void name(ecb_string *String)
ECB_FREE_STRING(ECBFreeStringStub)
{
  return;
}


file_scope u32
ECB_WriteToString(ecb_string *StringBuffer, char *String, u32 StringCount)
{
  u32 Result = 0;

  char *ContentPtr = (char*)StringBuffer->Content;
  while(*String)
  {
    *ContentPtr++ = *String++;

    if(++Result > StringCount ||
      (Result + StringBuffer->ContentSize > StringBuffer->ContentCapacity))
    {
      Result = 0;
      break;
    }
  }

  StringBuffer->Content = (char *)StringBuffer->Content + Result;
  StringBuffer->ContentSize += Result;
  return Result;
}

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

file_scope u32
ECB_GetStringLength(char* String)
{
  u32 Result = 0;
  while(*String++)
  {
    Result++;
  }

  return Result;
}
#endif //@NOTE(Emilio): _ECB_UTILS_H_
