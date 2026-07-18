#if !defined(_8086_DECODER_H_)
#define _8086_DECODER_H_
//@NOTE(Emilio): Used for sprintf
#include <stdio.h>

#include "8086_CPU_def.h"
#include "ecb_utils.h"

#define TEST_FILE "../../../computer_enhance/perfaware/part1/listing_0041_add_sub_cmp_jnz"
#define TEMP_PRINT_BUFFER_SIZE 1024

//@NOTE(Emilio): OpCode SHOULD be extracted vis OpCode = (OpCode >> Shift) & Mask;
typedef plex
{
  char* OpCodeString;
  u32 StateFlags;

  u32 OpCode;
  u32 Mask;
  u32 Shift;
  u32 BitCount;
} decoder_opcode_stats;

typedef plex
{
  u32 StateFlags;

  s32 FieldMask;
  s32 FieldBitCount;
  s32 FieldValue;
} decoder_field;

//@INCOMPLETE(Emilio): Will add more missing fields
typedef plex
{
  decoder_opcode_stats OpCodeStats;

  decoder_field DestinationFlag;
  decoder_field SignExtendFlag;
  decoder_field WideFlag;

  decoder_field Mod;
  decoder_field Reg;
  decoder_field RM;

  decoder_field Displacement;
  decoder_field Data;

  b32 IsForcedWide;
} decoder_opcode;

typedef plex
{
  u8* Pointer;
  u32 BitCount;
} decoder_stream_pointer;

char *RegTable[REG_TABLE_8086_SIZE] =
{
  "al", "cl", "dl", "bl",
  "ah", "ch", "dh", "bh",
  "ax", "cx", "dx", "bx",
  "sp", "bp", "si", "di"
};


#define RM_16BIT_IMM_CASE 0x6
char *RMTable[RM_TABLE_8086_SIZE] =
{
  "bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "NOOO!", "bx",
  "bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "bp", "bx",
  "bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "bp", "bx",
  "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh",
  "ax", "cx", "dx", "bx", "sp", "bp", "si", "di"
};

decoder_opcode OpCodes8086Table[OP_CODE_TABLE_8086_SIZE] =
{
  //@NOTE(Emilio): MOV instructions
  OP_MOV_RM_RM,
  OP_MOV_IMM_RM,
  OP_MOV_IMM_REG,
  OP_MOV_MEM_ACC,
  OP_MOV_ACC_MEM,

  //@NOTE(Emilio): ADD instructions
  OP_ADD_RM_RM,
  OP_ADD_IMM_RM,
  OP_ADD_IMM_ACC,

  //@NOTE(Emilio): SUB instructions
  OP_SUB_RM_RM,
  OP_SUB_IMM_RM,
  OP_SUB_IMM_ACC,

  //@NOTE(Emilio): CMP instructions
  OP_CMP_RM_RM,
  OP_CMP_IMM_RM,
  OP_CMP_IMM_ACC,

  //@NOTE(Emilio): RET instructions
  OP_RET,
  OP_RET_IMM_SP,
  OP_RET_INTERSEG,
  OP_RET_INTERSEG_SP,

  //@NOTE(Emilio): JMP instructions
  OP_JE_JZ,
  OP_JL_JNGE,
  OP_JLE_JNG,
  OP_JB_JNAE,
  OP_JBE_JNA,
  OP_JP_JPE,
  OP_JO,
  OP_JS,
  OP_JNE_JNZ,
  OP_JNL_JGE,
  OP_JNLE_JG,
  OP_JNB_JAE,
  OP_JNBE_JA,
  OP_JNP_JPO,
  OP_JNO,
  OP_JNS,
  OP_LOOP,
  OP_LOOPZ_LOOPE,
  OP_LOOPNZ_LOOPNE,
  OP_JCXZ,

  //@NOTE(Emilio): INT Instructions
  OP_INT,
  OP_INT_3,
  OP_INTO,
  OP_IRET,

  //@NOTE(Emilio): Processor Control Instructions
  OP_CLC,
  OP_CMC,
  OP_STC,
  OP_CLD,
  OP_STD,
  OP_CLI,
  OP_STI,
  OP_HLT,
  OP_WAIT,
  OP_LOCK

};

#endif /* _8086_DECODER_H_ */
