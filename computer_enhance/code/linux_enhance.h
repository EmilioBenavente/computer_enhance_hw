#if !defined(_LINUX_HANDMADE_H_)
#define _LINUX_HANDMADE_H_

#include <stdio.h>
#include <stdlib.h>

#include "ecb_utils.h"

ECB_FREE_STRING(LinuxFreeString)
{
  if(String->Content)
  {
    free(String->Content);
    String->Content = 0;
  }

  String->ContentSize = 0;
  String->ContentCapacity = 0;
}

ECB_CREATE_STRING(LinuxCreateString)
{
  ecb_string Result = {};

  if(Capacity)
  {
    Result.Content = malloc(Capacity);
    if(Result.Content)
    {
      Result.ContentCapacity = Capacity;
    }
  }

  if(InitialContent)
  {
    ECB_WriteToString(&Result, InitialContent, InitialContentSize);
  }

  return Result;
}


ECB_FREE_FILE(LinuxFreeFile)
{
  if(FileResult->Content)
  {
    free(FileResult->Content);
    FileResult->Content = 0;
  }
  FileResult->ContentSize = 0;
}

ECB_READ_ENTIRE_FILE(LinuxReadEntireFile)
{
  ecb_file_result Result = {};
  FILE *FilePtr = fopen(Filename, "r");
  if(FilePtr)
  {
    fseek(FilePtr, 0, SEEK_END);
    s32 BytesToRead = ftell(FilePtr);
    fseek(FilePtr, 0, 0);

    Result.ContentSize = BytesToRead;
    Result.Content = malloc(Result.ContentSize);
    if(Result.Content)
    {
      s32 BytesRead = fread(Result.Content, sizeof(char), BytesToRead, FilePtr);
      if(BytesToRead == BytesRead)
      {
        //@NOTE(Emilio): Intentionally left empty, end of function.
      }
      else
      {
        //@TODO(Emilio): Logging -> Bad Read from file.
        LinuxFreeFile(&Result);
      }
    }
    else
    {
      //@TODO(Emilio): Logging -> Could not allocate memory for buffer holding file contents
    }
  }
  else
  {
    //@TODO(Emilio): Logging -> Could not open file **filename** */
  }

  return Result;
}

ECB_WRITE_ENTIRE_FILE(LinuxWriteEntireFile)
{
  u32 Result = 0;
  FILE *FilePtr = fopen(Filename, "w");
  if(FilePtr)
  {
    Result = fwrite(FileResult->Content, sizeof(char), FileResult->ContentSize, FilePtr);
  }
  else
  {
    //@TODO(Emilio): Logging -> Could not create writable file handle for file **filename** */
  }

  return Result;
}

#define WND_WIDTH  1080
#define WND_HEIGHT 720

#endif /* _LINUX_HANDMADE_H_ */
