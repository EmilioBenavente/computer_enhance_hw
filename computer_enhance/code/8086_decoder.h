#if !defined(_8086_DECODER_H_)
#define _8086_DECODER_H_
//@NOTE(Emilio): Used for sprintf
#include <stdio.h>

#include "8086_CPU_def.h"
#include "ecb_utils.h"

#define TEST_FILE "../../../computer_enhance/perfaware/part1/listing_0040_challenge_movs"
#define TEMP_PRINT_BUFFER_SIZE 1024

//@NOTE(Emilio): OpCode SHOULD be extracted vis OpCode = (OpCode >> Shift) & Mask;
typedef plex
{
  char* OpCodeString;
  u32 OpCode;
  u32 Mask;
  u32 Shift;
  u32 BitCount;
} decoder_opcode_stats;

typedef plex
{
  b32 IsFieldExist;
  b32 IsFieldPadding;

  s32 FieldMask;
  s32 FieldBitCount;
  s32 FieldValue;
} decoder_field;

//@INCOMPLETE(Emilio): Will add more missing fields
typedef plex
{
  decoder_opcode_stats OpCodeStats;

  decoder_field DestinationFlag;
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
  OP_MOV_RM_RM,
  OP_MOV_IMM_RM,
  OP_MOV_IMM_REG


};

#endif /* _8086_DECODER_H_ */
