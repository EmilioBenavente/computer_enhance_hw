#if !defined(_DECODER_H_)
#define _DECODER_H_

#include <stdio.h>
#include "ecb_utils.h"
#include "sim8086.inl"

typedef plex
{
  b32 IsExists;
  u8 Value;
  u8 BitCount;
  u8 ValueMask;
} instruction_bits;

enum DecoderIterFields
{
  F_OpCode = 0,
  F_DisplacementBit,
  F_WideBit,
  F_Mod,
  F_Reg,
  F_OctalCode,
  F_RM,
  F_DispLow,
  F_DispHigh,
  F_DataLow,
  F_DataHigh
};

file_scope char* DecoderIterFieldAsStrings[] =
{
  "Operation Code:",
  "Destination Bit:",
  "Wide Bit:",
  "Mod Field:",
  "Reg Field:",
  "RM Field:",
  "Displacement Field:",
  "Data Field:"
};

typedef plex
{
  char* OpCodeMnemonic;
  union
  {
    plex
    {
      instruction_bits OpCode;
      instruction_bits IsDestination;
      instruction_bits IsWide;
      instruction_bits Mod;
      instruction_bits Reg;
      instruction_bits OctalCode;
      instruction_bits RM;
      instruction_bits DispLow;
      instruction_bits DispHigh;
      instruction_bits DataLow;
      instruction_bits DataHigh;
    };
    instruction_bits InstructionBits[11];
  };

  char RegString[24];
  char RMString[24];
  s16 Displacement;
  s16 Data;

} instruction_fields;

typedef plex
{
  s32 BytesRead;
  s32 BitsReadInByte;
  s32 BitPosition;

  char  CurrentInstruction[32];
  char* InstructionWritePtr;
  char  Comments[256];
  char* CommentPtr;
  instruction_fields InstructionFields;
} decoder_context;


file_scope instruction_fields SimISA[] =
{
  MOV_RM_TO_FROM_REG,
  MOV_IMM_TO_RM,
  MOV_IMM_TO_REG,
  MOV_MEM_TO_ACC,
  MOV_ACC_TO_MEM,
  MOV_RM_TO_SEG,
  MOV_SEG_TO_RM,

  ADD_RM_WITH_REG,
  ADD_IMM_TO_RM,
  ADD_IMM_TO_ACC,

  SUB_RM_WITH_REG,
  SUB_IMM_FROM_RM,
  SUB_IMM_FROM_ACC,

  CMP_RM_AND_REG,
  CMP_IMM_WITH_RM,
  CMP_IMM_WITH_ACC
};

file_scope char* RegTable[] =
{
  "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh",
  "ax", "cx", "dx", "bx", "sp", "bp", "si", "di"
};

file_scope char* RMTable[] =
{
  "[bx + si", "[bx + di", "[bp + si", "[bp + di", "[si", "[di", "NO!", "[bx",
  "[bx + si", "[bx + di", "[bp + si", "[bp + di", "[si", "[di", "[bp", "[bx",
  "[bx + si", "[bx + di", "[bp + si", "[bp + di", "[si", "[di", "[bp", "[bx",
  "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh",
  "ax", "cx", "dx", "bx", "sp", "bp", "si", "di"
};


#endif //@NOTE(Emilio): _DECODER_H_
