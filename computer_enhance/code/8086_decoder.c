#include "8086_decoder.h"


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
DecoderReadByteStream(char *InputStream)
{
  ECB_ASSERT(InputStream);

  char InstructionByte = 0;
  while(*InputStream)
  {
    decoder_opcode OpCodeFields = DecoderGetOpCodeFromStream(InputStream);
    if(OpCodeFields.OpCodeStats.OpCode)
    {
      

    }




  }
}
