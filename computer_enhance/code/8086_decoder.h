#if !defined(_8086_DECODER_H_)
#define _8086_DECODER_H_

#include "8086_CPU_def.h"
#include "ecb_utils.h"

#define TEST_FILE "../../../computer_enhance/perfaware/part1/listing_0037_single_register_mov"

//@NOTE(Emilio): OpCode SHOULD be extracted vis OpCode = (OpCode >> Shift) & Mask;
typedef plex
{
  u32 OpCode;
  u32 Mask;
  u32 Shift;
} decoder_opcode_stats;

typedef plex
{
  b32 IsFieldExist;
  s32 FieldValue;
} decoder_field;

//@INCOMPLETE(Emilio): Will add more missing fields
typedef plex
{
  decoder_opcode_stats OpCodeStats;

  decoder_field DisplacementFlag;
  decoder_field WideFlag;

  decoder_field Mod;
  decoder_field Reg;
  decoder_field RM;

  s32 Displacement;
  s32 Data;
} decoder_opcode;

decoder_opcode OpCodes8086Table[OP_CODE_TABLE_8086_SIZE] =
{
  OP_MOV_RM_RM,
  OP_MOV_IMM_RM
};

#endif /* _8086_DECODER_H_ */
