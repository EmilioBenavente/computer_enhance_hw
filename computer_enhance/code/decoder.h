#if !defined(_DECODER_H_)
#define _DECODER_H_

#include "ecb_utils.h"
#include "sim8086.inl"

typedef plex
{
  s32 BytesRead;
  s32 BitsReadInByte;
  s32 BitPosition;

} decoder_context;

typedef plex
{
  b32 IsExists;
  u8 Bits;
  u8 BitCount;
} instruction_bits;

typedef plex
{
  char* OpCodeMnemonic;
  union
  {
    plex
    {
      instruction_bits OpCode;
      instruction_bits IsDisplacement;
      instruction_bits IsWide;
      instruction_bits Mod;
      instruction_bits Reg;
      instruction_bits RM;
      instruction_bits OctalCode;
      instruction_bits DispLow;
      instruction_bits DispHigh;
      instruction_bits DataLow;
      instruction_bits DataHigh;
    };
    instruction_bits InstructionBits[10];
  };
} instruction_type;

file_scope instruction_type SimISA[] =
{
  MOV_RM_TO_FROM_REG,
  MOV_IMM_TO_RM,
  MOV_IMM_TO_REG,
  MOV_MEM_TO_ACC,
  MOV_ACC_TO_MEM,
  MOV_RM_TO_SEG,
  MOV_SEG_TO_RM
};


#endif //@NOTE(Emilio): _DECODER_H_
