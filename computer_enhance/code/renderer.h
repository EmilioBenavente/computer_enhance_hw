#if !defined(_RENDERER_H_)
#define _RENDERER_H_

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "ecb_utils.h"

typedef plex
{
  void* Memory;
  s32 Width;
  s32 Height;
  s32 BytesPerPixel;
  s32 Stride;
} render_display_buffer;



#endif //@NOTE(Emilio): _RENDERER_H_
