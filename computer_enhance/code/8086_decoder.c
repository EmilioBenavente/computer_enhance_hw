#include "8086_decoder.h"

file_scope void
DecoderGetFieldValueAndUpdateBitCount(decoder_field *Field, decoder_stream_pointer *StreamPtr)
{
  if(Field->IsFieldPadding)
  {
    StreamPtr->BitCount += Field->FieldBitCount;
  }
  else
  {
    u32 ShiftValue = 8 - (StreamPtr->BitCount + Field->FieldBitCount);
    if(Field->IsFieldExist)
    {
      Field->FieldValue = (*StreamPtr->Pointer >> ShiftValue) & Field->FieldMask;
      StreamPtr->BitCount += Field->FieldBitCount;
    }
  }

  if(StreamPtr->BitCount > 7)
  {
    StreamPtr->BitCount -= 8;
    StreamPtr->Pointer++;
  }
}

file_scope void
DecoderExtractValuesFromField(decoder_opcode *Fields, decoder_stream_pointer *StreamPtr)
{
  StreamPtr->BitCount += Fields->OpCodeStats.BitCount;

  DecoderGetFieldValueAndUpdateBitCount(&Fields->DestinationFlag, StreamPtr);
  DecoderGetFieldValueAndUpdateBitCount(&Fields->WideFlag, StreamPtr);
  DecoderGetFieldValueAndUpdateBitCount(&Fields->Mod, StreamPtr);
  DecoderGetFieldValueAndUpdateBitCount(&Fields->Reg, StreamPtr);
  DecoderGetFieldValueAndUpdateBitCount(&Fields->RM, StreamPtr);

  b32 IsDisplacementExist = (Fields->Mod.FieldValue == 0x1) ||
    (Fields->Mod.FieldValue == 0x2)                       ||
    (Fields->Mod.FieldValue == 0x0 && Fields->RM.FieldValue == 7);
  if(IsDisplacementExist)
  {
    Fields->Displacement.IsFieldExist = 1;
    Fields->Displacement.FieldValue = (s32)((s8)*StreamPtr->Pointer);
    StreamPtr->Pointer++;

    if((Fields->Mod.FieldValue == 0x2) ||
      (Fields->Mod.FieldValue == 0x0 && Fields->RM.FieldValue == 7))
    {
      s32 OriginalValue = (Fields->Displacement.FieldValue & 0xFF);
      Fields->Displacement.FieldValue &= 0xFF;
      Fields->Displacement.FieldValue |= (*StreamPtr->Pointer << 8);
      StreamPtr->Pointer++;
    }
  }

  if(Fields->Data.IsFieldExist)
  {
    Fields->Data.FieldValue = (s32)((s8)*StreamPtr->Pointer);
    StreamPtr->Pointer++;

    if(Fields->WideFlag.FieldValue || Fields->IsForcedWide)
    {
      s32 OriginalValue = (Fields->Data.FieldValue & 0xFF);
      Fields->Data.FieldValue &= 0xFF;
      Fields->Data.FieldValue |= (*StreamPtr->Pointer << 8);
      StreamPtr->Pointer++;
    }
  }
}

file_scope decoder_opcode
DecoderGetOpCodeFromStream(u8 *InputStream)
{
  ECB_ASSERT(OpCodes8086Table[OP_CODE_TABLE_8086_SIZE - 1].OpCodeStats.OpCode);

  decoder_opcode Result = {};

  s32 OpCodeIndex = 0;
  b32 IsValidOpCode = 0;
  while(!IsValidOpCode)
  {
    decoder_opcode *TempOpCode = &OpCodes8086Table[OpCodeIndex];
    char OpCode = *InputStream;

    decoder_opcode_stats OpCodeTest = TempOpCode->OpCodeStats;
    b32 OpCodeTestResult = (OpCode >> OpCodeTest.Shift) & OpCodeTest.Mask;
    if(OpCodeTestResult == OpCodeTest.OpCode)
    {
      Result.OpCodeStats      = TempOpCode->OpCodeStats;
      Result.DestinationFlag  = TempOpCode->DestinationFlag;
      Result.WideFlag         = TempOpCode->WideFlag;
      Result.Mod              = TempOpCode->Mod;
      Result.Reg              = TempOpCode->Reg;
      Result.RM               = TempOpCode->RM;
      Result.Displacement     = TempOpCode->Displacement;
      Result.Data             = TempOpCode->Data;
      break;
    }

    OpCodeIndex++;
  }

  return Result;
}

//@NOTE(Emilio): Returns the number bytes printed.
file_scope u32
DecoderPrintInstructionFromFields(char *WritePtr, decoder_opcode *Fields)
{
  u32 Result = 0;

  char PrintBuffer[TEMP_PRINT_BUFFER_SIZE];
  char* PrintPtr = PrintBuffer;
  PrintPtr += sprintf(PrintPtr, "%s ", Fields->OpCodeStats.OpCodeString);

  b32 IsWide = Fields->WideFlag.FieldValue || Fields->IsForcedWide;
 
  char* RegString = 0;
  if(Fields->Reg.IsFieldExist)
  {
    u32 RegIndex = Fields->Reg.FieldValue + (8*IsWide);
    RegString = RegTable[RegIndex];
  }

  char RMString[64] = {0};
  if(Fields->RM.IsFieldExist)
  {
    u32 RMIndex = Fields->RM.FieldValue + (8*Fields->Mod.FieldValue);
    if(Fields->Mod.FieldValue == 0x3)
    {
      RMIndex += (8*IsWide);
    }

    if(Fields->Mod.FieldValue == 3)
    {
      sprintf(RMString, "%s", RMTable[RMIndex]);
    }
    else
    {
      sprintf(RMString, "[%s]", RMTable[RMIndex]);

      if(Fields->Displacement.IsFieldExist)
      {
        s16 DisplacementValue = (s16)Fields->Displacement.FieldValue;
        if(DisplacementValue == 0)
        {
          sprintf(RMString, "[%s]", RMTable[RMIndex]);
        }
        else if(DisplacementValue > 0)
        {
          sprintf(RMString, "[%s + %d]", RMTable[RMIndex], DisplacementValue);
        }
        else
        {
          sprintf(RMString, "[%s - %d]", RMTable[RMIndex], (DisplacementValue * -1));
        }
      }
    }
  }


  if(Fields->Data.IsFieldExist)
  {
    s16 DataValue = (s16)Fields->Data.FieldValue;
    if(Fields->Reg.IsFieldExist)
    {
      if(DataValue > 0)
      {
        PrintPtr += sprintf(PrintPtr, "%s, %d", RegString, DataValue);
      }
      else
      {
        PrintPtr += sprintf(PrintPtr, "%s, -%d", RegString, (DataValue * -1));
      }
    }
    else
    {
      char* ImplicitSize = IsWide ? "word" :"byte";
      if(DataValue > 0)
      {
        PrintPtr += sprintf(PrintPtr, "%s, %s %d", RMString, ImplicitSize, DataValue);
      }
      else
      {
        PrintPtr += sprintf(PrintPtr, "%s, %s -%d", RMString, ImplicitSize, (DataValue * -1));
      }
    }
  }
  else
  {
  //@NOTE(Emilio): The basic case allows us to switch REG and RM field
  //  based on DestinationFlag, but when both registers aren't present
  //  we need to force one of the registers to act like the other
  //  making this scheme useless.
#if 1
    //@INCOMPLETE(Emilio): Lots of things not considered yet,
    //  for further instructions, such as if the Reg Field
    //  does not exist. Plus Displacement and Data Fields.
    if(Fields->DestinationFlag.FieldValue)
    {
      PrintPtr += sprintf(PrintPtr, "%s, %s", RegString, RMString);
    }
    else
    {
      PrintPtr += sprintf(PrintPtr, "%s, %s", RMString, RegString);
    }
#endif
  }



  if(WritePtr)
  {
    //@HARDCODE(Emilio): We should make functions for new line and comments
    Result = sprintf(WritePtr, "%s\n", PrintBuffer);
  }
  else
  {
    //@INCOMPLETE @TODO(Emilio): printf is not a cross compatible function, thank you windows..
  }

  return Result;
}

file_scope void
DecoderPrintSingleInstruction(ecb_string *WriteBuffer, decoder_opcode *Fields)
{
  u32 BytesRead =
    DecoderPrintInstructionFromFields(WriteBuffer->Content, Fields);

  WriteBuffer->ContentSize +=
    BytesRead;
  (u8 *)(WriteBuffer->Content) +=
    BytesRead;

}

file_scope decoder_opcode
DecoderReadSingleInstruction(u8 *InputStream, u32 *ReturnNewInstructionOffset)
{
  ECB_ASSERT(InputStream);

  decoder_opcode Result = {};

  Result = DecoderGetOpCodeFromStream(InputStream);

  if(Result.OpCodeStats.OpCode)
  {
    decoder_stream_pointer StreamPtr = {};
    StreamPtr.Pointer  = (u8*)InputStream;
    StreamPtr.BitCount = 0;

    DecoderExtractValuesFromField(&Result, &StreamPtr);

    if(ReturnNewInstructionOffset)
    {
      *ReturnNewInstructionOffset = StreamPtr.Pointer - InputStream;
    }
  }

  return Result;
}

file_scope void
DecoderReadByteStream(ecb_string *WriteBuffer, u8 *InputStream)
{
  ECB_ASSERT(InputStream);

  char InstructionByte = 0;
  u32 InstructionCount = 0;
  while(*InputStream)
  {
    decoder_opcode OpCodeFields = DecoderGetOpCodeFromStream(InputStream);
    if(OpCodeFields.OpCodeStats.OpCode)
    {
      OpCodeFields = DecoderReadSingleInstruction(InputStream, &InstructionCount);
      DecoderPrintSingleInstruction(WriteBuffer, &OpCodeFields);

      InputStream += InstructionCount;
    }
    else
    {
      return;
    }
  }
}

