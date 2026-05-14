#include "renderer.h"

file_scope void
RendererRenderTestImage(render_display_buffer* Buffer)
{

  s8* Row = (s8*)Buffer->Memory;
  for(s32 Y = 0; Y < Buffer->Height; Y++)
  {
    s32* Pixel = (s32*)Row;
    for(s32 X = 0; X < Buffer->Width; X++)
    {
      *Pixel++ = (X << 8) | (Y << 16);
    }
    Row += Buffer->Stride;
  }
}

file_scope void
RendererRenderBlankImage(render_display_buffer* Buffer)
{

  s8* Row = (s8*)Buffer->Memory;
  for(s32 Y = 0; Y < Buffer->Height; Y++)
  {
    s32* Pixel = (s32*)Row;
    for(s32 X = 0; X < Buffer->Width; X++)
    {
      *Pixel++ = 0;
    }
    Row += Buffer->Stride;
  }
}

file_scope void
RendererRenderBox(render_display_buffer* Buffer, s32 XPos, s32 YPos, u32 Width, u32 Height,
            r32 R, r32 G, r32 B)
{
  if(XPos < 0)
  {
    Width += XPos;
    XPos = 0;
  }
  if(XPos > Buffer->Width)
  {
    XPos = Buffer->Width;
  }

  if(YPos < 0)
  {
    Height += YPos;
    YPos = 0;
  }

  if(YPos > Buffer->Height)
  {
    YPos = Buffer->Height;
  }

  if((XPos + Width) > Buffer->Width)
  {
    s32 SubValue = (XPos + Width) - Buffer->Width;
    Width -= SubValue;
  }

  if((YPos + Height) > Buffer->Height)
  {
    s32 SubValue = (YPos + Height) - Buffer->Height;
    Height -= SubValue;
  }

  u8 RColor = (u8)(255.0f * R);
  u8 GColor = (u8)(255.0f * G);
  u8 BColor = (u8)(255.0f * B);

  s8* Row = (s8*)Buffer->Memory + (Buffer->Stride * YPos) + (Buffer->BytesPerPixel*XPos);
  for(s32 Y = 0; Y < Height; Y++)
  {
    s32* Pixel = (s32*)Row;
    for(s32 X = 0; X < Width; X++)
    {
      *Pixel++ = (RColor << 16) | (GColor << 8) | BColor;
    }
    Row += Buffer->Stride;
  }
}

file_scope void
RendererRenderDisplayBox(render_display_buffer* Buffer, s32 XPos, s32 YPos,
                         u32 Width, u32 Height, u32 BorderThickness,
                         r32 BorderR, r32 BorderG, r32 BorderB, r32 R, r32 G, r32 B)
{
  RendererRenderBox(Buffer,
                    XPos - BorderThickness, YPos - BorderThickness,
                    Width + (BorderThickness*2), Height + (BorderThickness*2),
                    BorderR, BorderG, BorderB);

  RendererRenderBox(Buffer, XPos, YPos,
                    Width, Height, R, G, B);

}


