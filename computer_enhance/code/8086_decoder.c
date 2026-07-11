#include "8086_decoder.h"

file_scope void
DecoderGetFieldValueAndUpdateBitCount(decoder_field *Field, decoder_stream_pointer *StreamPtr)
{
  u32 ShiftValue = 8 - (StreamPtr->BitCount % 8);
  if(Field->IsFieldExist)
  {
    Field->FieldValue = (StreamPtr->Pointer >> ShiftValue) & Field->FieldMask;
    StreamPtr->BitCount += Field->BitCount;


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
  StreamPtr->BitCount += Fields->BitCount;

  DecoderGetFieldValueAndUpdateBitCount(&Fields->DisplacementFlag, StreamPtr);
  DecoderGetFieldValueAndUpdateBitCount(&Fields->WideFlag, StreamPtr);
  DecoderGetFieldValueAndUpdateBitCount(&Fields->Mod, StreamPtr);
  DecoderGetFieldValueAndUpdateBitCount(&Fields->Reg, StreamPtr);
  DecoderGetFieldValueAndUpdateBitCount(&Fields->RM, StreamPtr);

  if(Fields->DisplacementFlag.IsFieldExist)
  {
    Fields->Displacement = *StreamPtr->Pointer;
    StreamPtr++;

    if(Fields->WideFlag.IsFieldExist || Fields->IsForcedWide)
    {
      Fields->Displacement = *StreamPtr->Pointer;
      StreamPtr++;
    }
  }

  if(Fields->DataFlag.IsFieldExist)
  {
    Fields->Data = *StreamPtr->Pointer;
    StreamPtr++;

    if(Fields->WideFlag.IsFieldExist || Fields->IsForcedWide)
    {
      Fields->Data = *StreamPtr->Pointer;
      StreamPtr++;
    }
  }
}

file_scope decoder_opcode
DecoderGetOpCodeFromStream(char *InputStream)
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
      Result.DisplacementFlag = TempOpCode->DisplacementFlag;
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



file_scope void
DecoderReadByteStream(char **Buffer, char *InputStream)
{
  ECB_ASSERT(InputStream);

  decoder_stream_pointer StreamPtr = {};
  StreamPtr.Pointer  = (u8*)InputStream;
  StreamPtr.BitCount = 0;

  char InstructionByte = 0;
  while(*StreamPtr.Pointer)
  {
    decoder_opcode OpCodeFields = DecoderGetOpCodeFromStream(StreamPtr.Pointer);
    if(OpCodeFields.OpCodeStats.OpCode)
    {
      DecoderExtractValuesFromField(&OpCodeFields, InputStream, 0);
      DecoderPrintInstructionFromFields(&OpCodeFields);
    }
  }
}
