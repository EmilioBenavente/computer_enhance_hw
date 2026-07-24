#if !defined(_8086_DECODER_H_)
#define _8086_DECODER_H_
//@NOTE(Emilio): Used for sprintf
#include <stdio.h>

#include "8086_CPU_def.h"
#include "ecb_utils.h"

#define TEST_FILE "../../../computer_enhance/perfaware/part1/listing_0042_completionist_decode"
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
  decoder_field RotateCLFlag;
  decoder_field ZeroFlag;
  decoder_field WideFlag;

  decoder_field Mod;
  decoder_field Reg;
  decoder_field SegmentReg;
  decoder_field RM;

  decoder_field Displacement;
  decoder_field Data;
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
  OP_MOV_RM_SEG,
  OP_MOV_SEG_RM,

  //@NOTE(Emilio): PUSH instructions
  OP_PUSH_RM,
  OP_PUSH_REG,
  OP_PUSH_SEG,

  //@NOTE(Emilio): POP instructions
  OP_POP_RM,
  OP_POP_REG,
  OP_POP_SEG,

  //@NOTE(Emilio): XCHNG instrctions
  OP_XCHG_RM_REG,
  OP_XCHG_REG_ACC,

  //@NOTE(Emilio): IN instructions
  OP_IN,
  OP_IN_VAR,

  //@NOTE(Emilio): OUT instructions
  OP_OUT,
  OP_OUT_VAR,

  //@NOTE(Emilio): COMMON instructions
  OP_XLAT,
  OP_LEA,
  OP_LDS,
  OP_LES,
  OP_LAHF,
  OP_SAHF,
  OP_PUSHF,
  OP_POPF,

  //@NOTE(Emilio): ADD instructions
  OP_ADD_RM_RM,
  OP_ADD_IMM_RM,
  OP_ADD_IMM_ACC,

  //@NOTE(Emilio): ADC instructions
  OP_ADC_RM_RM,
  OP_ADC_IMM_RM,
  OP_ADC_IMM_ACC,

  //@NOTE(Emilio): INC instructions
  OP_INC_RM,
  OP_INC_REG,
  OP_INC_AAA,
  OP_INC_DAA,

  //@NOTE(Emilio): SUB instructions
  OP_SUB_RM_RM,
  OP_SUB_IMM_RM,
  OP_SUB_IMM_ACC,

  //@NOTE(Emilio): SBB instructions
  OP_SBB_RM_RM,
  OP_SBB_IMM_RM,
  OP_SBB_IMM_ACC,

  //@NOTE(Emilio): DEC instructions
  OP_DEC_RM,
  OP_DEC_REG,
  OP_DEC_NEG,

  //@NOTE(Emilio): CMP instructions
  OP_CMP_RM_RM,
  OP_CMP_IMM_RM,
  OP_CMP_IMM_ACC,
  OP_CMP_AAS,
  OP_CMP_DAS,
  OP_CMP_MUL,
  OP_CMP_IMUL,
  OP_CMP_AAM,
  OP_CMP_DIV,
  OP_CMP_IDIV,
  OP_CMP_AAD,
  OP_CMP_CBW,
  OP_CMP_CWD,

  //@NOTE(Emilio): LOGIC instructions
  OP_NOT,
  OP_SHL_SAL,
  OP_SHR,
  OP_SAR,
  OP_ROL,
  OP_ROR,
  OP_RCL,
  OP_RCR,

  //@NOTE(Emilio): AND instructions
  OP_AND_RM_RM,
  OP_AND_IMM_RM,
  OP_AND_IMM_ACC,

  //@NOTE(Emilio): TEST instructions
  OP_TEST_RM_RM,
  OP_TEST_IMM_RM,
  OP_TEST_IMM_ACC,

  //@NOTE(Emilio): OR instructions
  OP_OR_RM_RM,
  OP_OR_IMM_RM,
  OP_OR_IMM_ACC,

  //@NOTE(Emilio): XOR instructions
  OP_XOR_RM_RM,
  OP_XOR_IMM_RM,
  OP_XOR_IMM_ACC,

  //@NOTE(Emilio): string instructions
  OP_REP,
  OP_MOVS,
  OP_CMPS,
  OP_SCAS,
  OP_LODS,
  OP_STOS,

  //@NOTE(Emilio): control instructions
  OP_CALL_DIRECT,
  OP_CALL_INDIRECT,
  OP_CALL_DIRECT_INTERSEG,
  OP_CALL_INDIRECT_INTERSEG,

  //@NOTE(Emilio): RET instructions
  OP_RET,
  OP_RET_IMM_SP,
  OP_RET_INTERSEG,
  OP_RET_INTERSEG_SP,

  //@NOTE(Emilio): JMP instructions
  OP_JMP_SEG,
  OP_JMP_SEG_SHORT,
  OP_JMP_INDIRECT_SEG,
  OP_JMP_DIRECT_INTERSEG,
  OP_JMP_INDIRECT_INTERSEG,
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
  OP_LOCK,
  OP_SEGMENT
};

#endif /* _8086_DECODER_H_ */
