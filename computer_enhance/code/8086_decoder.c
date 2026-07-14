#include "8086_decoder.h"

file_scope void
DecoderGetFieldValueAndUpdateBitCount(decoder_field *Field, decoder_stream_pointer *StreamPtr)
{
  u32 ShiftValue = 8 - (StreamPtr->BitCount + Field->FieldBitCount);
  if(Field->IsFieldExist)
  {
    Field->FieldValue = (*StreamPtr->Pointer >> ShiftValue) & Field->FieldMask;
    StreamPtr->BitCount += Field->FieldBitCount;


    if(StreamPtr->BitCount > 7)
    {
      StreamPtr->BitCount -= 8;
      StreamPtr->Pointer++;
    }
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
    Fields->Displacement.FieldValue = *StreamPtr->Pointer;
    StreamPtr++;

    if(Fields->WideFlag.IsFieldExist || Fields->IsForcedWide)
    {
      Fields->Displacement.FieldValue |= (*StreamPtr->Pointer < 8);
      StreamPtr++;
    }
  }

  if(Fields->Data.IsFieldExist)
  {
    Fields->Data.IsFieldExist = 1;
    Fields->Data.IsFieldExist = *StreamPtr->Pointer;
    StreamPtr++;

    if(Fields->WideFlag.IsFieldExist || Fields->IsForcedWide)
    {
      Fields->Data.IsFieldExist |= (*StreamPtr->Pointer << 8);
      StreamPtr++;
    }
  }
}

file_scope decoder_opcode
DecoderGetOpCodeFromStream(u8 *InputStream)
{
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

  b32 IsWide = Fields->WideFlag.IsFieldExist || Fields->IsForcedWide;
  
  char* RegString = 0;
  if(Fields->Reg.IsFieldExist)
  {
    u32 RegIndex = Fields->Reg.FieldValue + (8*IsWide);
    RegString = RegTable[RegIndex];
  }

  char* RMString = 0;
  if(Fields->RM.IsFieldExist)
  {
    u32 RMIndex = Fields->RM.FieldValue + (8*Fields->Mod.FieldValue);
    if(Fields->Mod.FieldValue == 0x3)
    {
      RMIndex += (8*IsWide);
    }
    RMString = RMTable[RMIndex];
  }

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


  if(WritePtr)
  {
    Result = sprintf(WritePtr, "%s", PrintBuffer);
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
  WriteBuffer->ContentSize +=
    DecoderPrintInstructionFromFields(WriteBuffer->Content, Fields);
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
  }
}

