#if !defined(_WIN32_HANDMADE_H_)
#define _WIN32_HANDMADE_H_

#include <stdio.h>
#include <windows.h>

#include "ecb_utils.h"

ECB_FREE_STRING(Win32FreeString)
{
  if(String->Content)
  {
    VirtualFree(String->Content, 0, MEM_RELEASE);
    String->Content = 0;
  }

  String->ContentSize = 0;
  String->ContentCapacity = 0;
}

ECB_CREATE_STRING(Win32CreateString)
{
  ecb_string Result = {};

  if(Capacity)
  {
    Result.Content = VirtualAlloc(0, Capacity, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
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


ECB_FREE_FILE(Win32FreeFile)
{
  if(FileResult->Content)
  {
    VirtualFree(FileResult->Content, 0, MEM_RELEASE);
    FileResult->Content = 0;
  }
  FileResult->ContentSize = 0;
}

ECB_READ_ENTIRE_FILE(Win32ReadEntireFile)
{
  ecb_file_result Result = {};
  HANDLE FileHandle = CreateFileA(Filename, GENERIC_READ, FILE_SHARE_READ,
    0, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, 0);
  if(FileHandle != INVALID_HANDLE_VALUE)
  {
    DWORD BytesToRead = GetFileSize(FileHandle, 0);

    Result.ContentSize = BytesToRead;
    Result.Content = VirtualAlloc(0, Result.ContentSize, MEM_COMMIT, PAGE_READWRITE);
    if(Result.Content)
    {
      DWORD BytesRead = 0;
      if(ReadFile(FileHandle, Result.Content, BytesToRead, &BytesRead, 0))
      {
        if(BytesToRead == BytesRead)
        {
          //@NOTE(Emilio): Intentionally left empty, end of function.
        }
        else
        {
          //@TODO(Emilio): Logging -> Bad Read from file.
          Win32FreeFile(&Result);
        }
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

ECB_WRITE_ENTIRE_FILE(Win32WriteEntireFile)
{
  u32 Result = 0;
  HANDLE FileHandle = CreateFileA(Filename, GENERIC_WRITE, FILE_SHARE_WRITE,
    0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

  if(FileHandle != INVALID_HANDLE_VALUE)
  {
    WriteFile(FileHandle, FileResult->Content, FileResult->ContentSize, &Result, 0);
  }
  else
  {
    //@TODO(Emilio): Logging -> Could not create writable file handle for file **filename** */
  }

  return Result;
}

#define WND_WIDTH  1080
#define WND_HEIGHT 720

#endif /* _WIN32_HANDMADE_H_ */
