#if !defined(_SIMULATOR_H_)
#define _SIMULATOR_H_

#include <stdio.h>
#include "ecb_utils.h"

#include "8086_CPU_def.h"

global_variable u8 SimMemory[MegaBytes(1)];

enum
{
  SIM_OP_MOV,
  SIM_OP_ADDITION,
  SIM_OP_SUBTRACTION,
  SIM_OP_MULTIPLICATION,
  SIM_OP_DIVISION
};

enum
{
  SIM_REG_TABLE,
  SIM_SEG_TABLE,
  SIM_RM_TABLE
};

typedef plex
{
  u32 PrevValue;

  u32 A;
  u32 B;

  b32 IsWriteToFlags;
  b32 IsResultTaken;
} alu_contents;

//@NOTE(Emilio): In indexes such as [bx + di], you need to
//  return bx and di.
typedef plex
{
  u16 *RegPart1;
  u16 *RegPart2;
} get_register_result;


typedef plex
{
  union
  {
    plex
    {
      u8 L;
      u8 H;
    };
    u16 X;
  };
} reg_mix;

typedef plex
{
  reg_mix A;
  reg_mix C;
  reg_mix D;
  reg_mix B;

  u16 SP;
  u16 BP;
  u16 SI;
  u16 DI;

  u16 ES;
  u16 CS;
  u16 SS;
  u16 DS;

  u16 IP;
  u16 Flags;
} cpu_registers;

typedef plex
{
  u32 Value;
  b32 IsExist;
} decoder_value;

typedef plex
{
  char* CurrentOpCodeString;
  decoder_value Reg;
  decoder_value Seg;
  decoder_value RM;
  decoder_value Destination;
  decoder_value Displacement;
  decoder_value Data;
  decoder_value Mod;
  b32 IsRegDestination;
  u16 OpCodeData2;


  cpu_registers Registers;
  u32 MemoryOffset;
  u32 ProgamSize;
  u32 ProcessID;
} cpu_program;

#endif /* _SIMULATOR_H_ */
