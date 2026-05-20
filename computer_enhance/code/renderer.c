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
RendererRenderString(render_display_buffer* Buffer, s32 XPos, s32 YPos, char* String, u32 FontSize,
                     u32 CutoffWidth)
{
  u32 ExpectedCutoffWidth = XPos + CutoffWidth;
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
                                          stbtt_ScaleForPixelHeight(&Font, FontSize),
                                          *String, &FontWidth, &FontHeight, &FontX, &FontY);

    if(XPos+FontX < 0)
    {
      XPos = 0;
    }
    if(XPos+FontX > ExpectedCutoffWidth)
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

file_scope void
RendererRenderDecoderWindow(render_display_buffer* Buffer, s32 XPos, s32 YPos,
                           s32 TempWidth, s32 TempHeight, decoder_context* DecoderContext)
{
  u32 YIterCount = (ArraySize(DecoderContext->InstructionFields.InstructionBits)) - 3;
  u32 XIterCount = 15;
  s32 Width = TempWidth / XIterCount;
  s32 Height = TempHeight / YIterCount;
  s32 OriginalXPos = XPos;
  s32 OriginalYPos = YPos;

  local_persist u16 TestValue = 0;
  for(u32 YIter = 0; YIter < YIterCount; YIter++)
  {
    u8 DecoderValue = 0;
    for(u32 XIter = XIterCount; XIter > 0; XIter--)
    {
      r32 ColorR = (r32)0.0f;
      r32 ColorG = (r32)0.0f;
      r32 ColorB = (r32)0.0f;
      r32 Value = 0.1f;
      if(YIter == 7)
      {
        DecoderValue = DecoderContext->InstructionFields.Displacement;
        Value = (r32)((DecoderValue & (1 << XIter-1)) >> XIter-1);

        ColorR = Value ? 1.0f : 0.0f;
        ColorG = Value ? 1.0f : 0.0f;
        ColorB = Value ? 1.0f : 0.0f;

      }
      else if(YIter == 8)
      {
        DecoderValue = DecoderContext->InstructionFields.Data;
        Value = (r32)((DecoderValue & (1 << XIter-1)) >> XIter-1);

        ColorR = Value ? 1.0f : 0.0f;
        ColorG = Value ? 1.0f : 0.0f;
        ColorB = Value ? 1.0f : 0.0f;
      }
      else
      {
        if(DecoderContext->InstructionFields.InstructionBits[YIter].IsExists)
        {
          DecoderValue = DecoderContext->InstructionFields.InstructionBits[YIter].Value;
          Value = (r32)((DecoderValue & (1 << XIter-1)) >> XIter-1);

          ColorR = Value ? 1.0f : 0.0f;
          ColorG = Value ? 1.0f : 0.0f;
          ColorB = Value ? 1.0f : 0.0f;
        }
        else
        {
          ColorR = Value;
          ColorG = Value;
          ColorB = Value;
        }
      }

      RendererRenderBox(Buffer, XPos, YPos,
                    Width, Height, ColorR, ColorG, ColorB);

      XPos += Width;
    }
    char ValueString[64];

    if(YIter == 0)
    {
      sprintf(ValueString, "%s 0x%X -> %s\n",
          DecoderIterFieldAsStrings[YIter], DecoderValue,
          DecoderContext->InstructionFields.OpCodeMnemonic);
    }
    else if(YIter == 4)
    {
      sprintf(ValueString, "%s 0x%X -> %s\n",
          DecoderIterFieldAsStrings[YIter], DecoderValue,
          DecoderContext->InstructionFields.RegString);
    }
    else if(YIter == 5)
    {
      sprintf(ValueString, "%s 0x%X -> %s\n",
          DecoderIterFieldAsStrings[YIter], DecoderValue,
          DecoderContext->InstructionFields.RMString);
    }
    else
    {
      sprintf(ValueString, "%s 0x%X\n",
           DecoderIterFieldAsStrings[YIter], DecoderValue);
    }
    RendererRenderString(Buffer, OriginalXPos-30, YPos+25, ValueString, 24, GlobalStringCutoffWidth);

    YPos += Height;
    XPos = OriginalXPos;
  }

  RendererRenderDisplayBox(Buffer, OriginalXPos, OriginalYPos - 30,
      180, 30, 2, 0.17f, 0.17f, 0.17f, 0.1f, 0.1f, 0.1f);
  RendererRenderString(Buffer, OriginalXPos-20, OriginalYPos - 10, "Decoder Window\n", 24, GlobalStringCutoffWidth);
}

file_scope void
RendererRenderCPUWindow(render_display_buffer* Buffer, s32 XPos, s32 YPos,
                           s32 TempWidth, s32 TempHeight, sim_cpu* CPUContext)
{
  u32 YIterCount = 14;
  u32 XIterCount = 15;
  s32 Width = TempWidth / (XIterCount+2);
  s32 Height = TempHeight / YIterCount;
  s32 OriginalXPos = XPos;
  s32 OriginalYPos = YPos;

  s32 FieldStringIter = 0;
  u16* CPUContextIter = (u16*)&CPUContext->A;
  local_persist u16 TestValue = 0;
  for(u32 YIter = 0; YIter < YIterCount; YIter++)
  {
    u16 RegisterValue = *CPUContextIter;

    for(u32 XIter = XIterCount+1; XIter > 0; XIter--)
    {
      u32 Value = (r32)((RegisterValue & (1 << XIter-1)) >> XIter-1);

      r32 ColorR = Value ? 1.0f : 0.0f;
      r32 ColorG = Value ? 1.0f : 0.0f;
      r32 ColorB = Value ? 1.0f : 0.0f;

      if(YIter < 4 && XIter == 8)
      {
        XPos += (Width);
      }

      RendererRenderBox(Buffer, XPos, YPos,
           Width, Height, ColorR, ColorG, ColorB);

      XPos += Width;
    }

    char ValueString[64];
    if(YIter < 4)
    {
      sprintf(ValueString, "%s: %d\n",
          CPUFieldsAsStrings[FieldStringIter++], RegisterValue);
      RendererRenderString(Buffer, OriginalXPos-30, YPos+25, ValueString, 24, GlobalStringCutoffWidth);

      reg_type RegisterValueAsRegType = *((reg_type*)CPUContextIter);
      sprintf(ValueString, "%s: %d\n",
          CPUFieldsAsStrings[FieldStringIter++], RegisterValueAsRegType.High);
      RendererRenderString(Buffer, OriginalXPos + 100, YPos+25, ValueString, 24, GlobalStringCutoffWidth);

      sprintf(ValueString, "%s: %d\n",
          CPUFieldsAsStrings[FieldStringIter++], RegisterValueAsRegType.Low);
      RendererRenderString(Buffer, OriginalXPos + (TempWidth/2) - 80, YPos+25, ValueString, 24, GlobalStringCutoffWidth);

    }
    else
    {
      sprintf(ValueString, "%s: %d\n",
          CPUFieldsAsStrings[FieldStringIter++], RegisterValue);
      RendererRenderString(Buffer, OriginalXPos-30, YPos+25, ValueString, 24,GlobalStringCutoffWidth);
    }

    *CPUContextIter++;
    YPos += Height;
    XPos = OriginalXPos;
  }

  RendererRenderDisplayBox(Buffer, OriginalXPos, OriginalYPos - 20,
      180, 30, 2, 0.17f, 0.17f, 0.17f, 0.1f, 0.1f, 0.1f);
  RendererRenderString(Buffer, OriginalXPos-20, OriginalYPos, "CPU Window\n", 24, GlobalStringCutoffWidth);

}



//@INCOMPLETE(Emilio): This is forced to use 8x16 Grid for now.
file_scope void
RendererRenderMemoryWindow(render_display_buffer* Buffer, s32 XPos, s32 YPos,
                           s32 TempWidth, s32 TempHeight, u32 MemoryAddress,
                           u32 CurrentInstruction, u32 CurrentInstructionRange)
{
  s32 Width = TempWidth / 16;
  s32 Height = TempHeight / 8;
  s32 OriginalXPos = XPos;
  s32 OriginalYPos = YPos;
  for(u32 YIter = 0; YIter < 8; YIter++)
  {
    for(u32 XIter = 0; XIter < 16; XIter++)
    {
      u32 MemoryIndex = YIter*16+XIter + MemoryAddress;
      r32 ColorR = (r32)0.0f;
      r32 ColorG = (r32)0.0f;
      r32 ColorB = (r32)0.0f;
      if(MemoryIndex == CurrentInstruction)
      {
        ColorR = 0.3f;
        ColorG = 0.61f;
        ColorB = 0.3f;
      }
      else if((MemoryIndex <= CurrentInstruction + CurrentInstructionRange-1) &&
        (MemoryIndex > CurrentInstruction))
      {
        ColorR = 0.1f;
        ColorG = 0.41f;
        ColorB = 0.3f;
      }
      else
      {
        ColorR = (r32)SimMemory[MemoryIndex] / 255.0f;
        ColorG = (r32)SimMemory[MemoryIndex] / 255.0f;
        ColorB = (r32)SimMemory[MemoryIndex] / 255.0f;
      }

      RendererRenderBox(Buffer, XPos, YPos,
                    Width, Height, ColorR, ColorG, ColorB);
      char Value[2];
      Value[0] = SimMemory[YIter*16+XIter+MemoryAddress];
      Value[1] = '\0';

      char ValueAddress[24];
      sprintf(ValueAddress, "0x%X", YIter*16+XIter+MemoryAddress);
      RendererRenderString(Buffer, XPos-30, YPos+15, ValueAddress, 18, GlobalStringCutoffWidth);
      RendererRenderString(Buffer, XPos-15, YPos+50, Value, 24, GlobalStringCutoffWidth);
      XPos += Width;
    }
    YPos += Height;
    XPos = OriginalXPos;
  }

  RendererRenderDisplayBox(Buffer, OriginalXPos, OriginalYPos - 30,
      180, 30, 2, 0.17f, 0.17f, 0.17f, 0.1f, 0.1f, 0.1f);
  RendererRenderString(Buffer, OriginalXPos-20, OriginalYPos - 10, "Memory Window\n", 24, GlobalStringCutoffWidth);
}


