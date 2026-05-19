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

file_scope void
RendererRenderString(render_display_buffer* Buffer, s32 XPos, s32 YPos, char* String)
{
  XPos += 30;
  local_persist char FontBuffer[MegaBytes(2)];
  local_persist b32 IsFontInit;

  if(!IsFontInit)
  {
    fread(FontBuffer, 1, MegaBytes(2), fopen("c:/Windows/Fonts/times.ttf", "rb"));
    IsFontInit = 1;
  }

  stbtt_fontinfo Font;
  stbtt_InitFont(&Font, FontBuffer, 0);

  u32 OriginalXPos = XPos;
  while(*String)
  {
    if(*String == ' ')
    {
      XPos += 10;
      String++;
      continue;
    }
    if(*String == '\n')
    {
      YPos += 32;
      XPos = OriginalXPos;
      String++;
      continue;
    }
    s32 FontWidth = 0;
    s32 FontHeight = 0;
    s32 FontX = 0;
    s32 FontY = 0;
    u8* Bitmap = stbtt_GetCodepointBitmap(&Font, 0,
                                          stbtt_ScaleForPixelHeight(&Font, 24),
                                          *String, &FontWidth, &FontHeight, &FontX, &FontY);

    if(XPos+FontX < 0)
    {
      XPos = 0;
    }
    if(XPos+FontX > Buffer->Width)
    {
      break;
    }

    if(YPos+FontY < 0)
    {
      YPos = 0;
    }

    if(YPos+FontY > Buffer->Height)
    {
      break;
    }


    s8* Row = (s8*)Buffer->Memory + (Buffer->Stride * (YPos+FontY)) + (Buffer->BytesPerPixel*(XPos+FontX));
    for(s32 Y = 0; Y < FontHeight; Y++)
    {
      s32* Pixel = (s32*)Row;
      for(s32 X = 0; X < FontWidth; X++)
      {
        u8 Value = Bitmap[Y*FontWidth + X];
        *Pixel++ = (Value << 16) |
                   (Value << 8)  |
                   (Value << 0);
      }
      Row += Buffer->Stride;
    }

    XPos += FontWidth;
    String++;
    stbtt_FreeBitmap(Bitmap, 0);
  }
}

//@INCOMPLETE(Emilio): This is forced to use 8x16 Grid for now.
file_scope void
RendererRenderMemoryWindow(render_display_buffer* Buffer, s32 XPos, s32 YPos,
                           s32 TempWidth, s32 TempHeight, u32 MemoryAddress)
{
  s32 Width = TempWidth / 16;
  s32 Height = TempHeight / 8;
  s32 OriginalXPos = XPos;
  for(u32 YIter = 0; YIter < 8; YIter++)
  {
    for(u32 XIter = 0; XIter < 16; XIter++)
    {
      r32 Color = (r32)SimMemory[YIter*16+XIter + MemoryAddress] / 255.0f;
      RendererRenderBox(Buffer, XPos, YPos,
                    Width, Height, Color, Color, Color);
      char Value[2];
      Value[0] = SimMemory[YIter*16+XIter+MemoryAddress];
      Value[1] = '\0';
      RendererRenderString(Buffer, XPos-15, YPos+50, Value);
      XPos += Width;
    }
    YPos += Height;
    XPos = OriginalXPos;
  }
}


