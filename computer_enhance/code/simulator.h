#if !defined(_SIMULATOR_H_)
#define _SIMULATOR_H_

#include "ecb_utils.h"

#define HW_FILE "C:/SpiderEnv/computer_enhance_source/computer_enhance/perfaware/part1/listing_0046_add_sub_cmp"



typedef plex
{
  union
  {
    plex
    {
      u8 Low;
      u8 High;
    };
    u16 Value;
  };
} reg_type;

typedef plex
{
  reg_type A;
  reg_type C;
  reg_type D;
  reg_type B;

  u16 SP;
  u16 BP;
  u16 SI;
  u16 DI;

  u16 ES;
  u16 CS;
  u16 SS;
  u16 DS;

  u16 PC;
  u16 Flags;

  u16 RegIndex;
  u16 RMIndex;
  b32 IsFlagsUpdated;

} sim_cpu;

char* CPUFieldsAsStrings[] =
{
  "ax", "ah", "al",
  "cx", "ch", "cl",
  "dx", "dh", "dl",
  "bx", "bh", "bl",
  "sp", "bp", "si",
  "di", "es", "cs",
  "ss", "ds", "pc",
  "F"
};

enum alu_code
{
  ALU_MOV = 0,
  ALU_ADD,
  ALU_SUB,
  ALU_CMP
};

enum reg_op_state
{
  RM_TO_REG = 0,
  REG_TO_RM,
  IMM_TO_REG,
  IMM_TO_RM,
  REG_TO_IMM,
  RM_TO_IMM
};


global_variable u8 SimMemory[MegaBytes(1)];
global_variable u32 GlobalStringCutoffWidth = 450;
global_variable u8 PCUndos[64] = {};
global_variable s32 PCUndoIter;


#endif //@NOTE(Emilio): _SIMULATOR_H_
