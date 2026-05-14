#if !defined(_WIN32_APP_H_)
#define _WIN32_APP_H_
#include <windows.h>
#include "ecb_utils.h"

//@NOTE(Emilio): For now we will include this here
//  but when we add support for hot-reloading
//  we will need to load this manually
#include "simulator.c"
#include "decoder.c"
#include "renderer.c"



#define WND_WIDTH  1920
#define WND_HEIGHT 1080

typedef plex
{
  BITMAPINFO Info;
  void* Memory;
  s32 Width;
  s32 Height;
  s32 BytesPerPixel;
  s32 Stride;
} win32_display_buffer;

typedef plex
{
  LPVOID Contents;
  DWORD ContentSize;
} file_result;



//@NOTE(Emilio): Globals
global_variable b32 GlobalIsGameRunning;
global_variable win32_display_buffer GlobalDisplayBuffer;

#endif //@NOTE(Emilio): _WIN32_APP_H_
