#if !defined(_SIMULATOR_H_)
#define _SIMULATOR_H_

#include "ecb_utils.h"

#define HW_FILE "C:/SpiderEnv/computer_enhance_source/computer_enhance/perfaware/part1/listing_0041_add_sub_cmp_jnz"

file_scope u8 SimMemory[MegaBytes(1)];


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
  reg_type B;
  reg_type C;
  reg_type D;

  u16 SP;
  u16 BP;
  u16 SI;
  u16 DI;

  u16 CS;
  u16 DS;
  u16 SS;
  u16 ES;

  u16 PC;
  u16 Flags;
} sim_cpu;

char* CPUFieldsAsStrings[] =
{
  "ax", "ah", "al",
  "bx", "bh", "bl",
  "cx", "ch", "cl",
  "dx", "dh", "dl",
  "sp", "bp", "si",
  "di", "cs", "ds",
  "ss", "es", "pc",
  "F"
};

global_variable u32 GlobalStringCutoffWidth = 450;

#endif //@NOTE(Emilio): _SIMULATOR_H_
