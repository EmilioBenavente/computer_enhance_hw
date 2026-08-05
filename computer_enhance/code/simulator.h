#if !defined(_SIMULATOR_H_)
#define _SIMULATOR_H_

#include <stdio.h>
#include "ecb_utils.h"

#include "8086_CPU_def.h"

global_variable u8 SimMemory[MegaBytes(1)];

#define CARRY_FLAG      (1 << 0)
#define PARITY_FLAG     (1 << 2)
#define AUXILIARY_FLAG  (1 << 4)
#define ZERO_FLAG       (1 << 6)
#define SIGN_FLAG       (1 << 7)
#define TRAP_FLAG       (1 << 8)
#define INTERRUPT_FLAG  (1 << 9)
#define DIRECTION_FLAG  (1 << 10)
#define OVERFLOW_FLAG   (1 << 11)

typedef enum
{
  SIM_OP_MOV,
  SIM_OP_ADDITION,
  SIM_OP_SUBTRACTION,
  SIM_OP_MULTIPLICATION,
  SIM_OP_DIVISION,
  SIM_OP_COMPARISON
} sim_op_type;

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
  u32 NewValue;
  u16* Dest;

  b32 IsDestLow;
  b32 IsDestHigh;
  b32 IsWriteToFlags;
  b32 IsResultTaken;
  b32 IsAFSet;
  b32 IsOFSet;
} alu_contents;

//@NOTE(Emilio): In indexes such as [bx + di], you need to
//  return bx and di.
typedef plex
{
  u16 *Left;
  u16 *Right;

  u16 Displacement;
} lea_contents;

typedef plex
{
  u16 *Address;
  u16 Value;

  b32 IsLow;
  b32 IsHigh;
} lea_result;

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


//@NOTE(Emilio): decoder_value gets filled in by the decoder_field passed by the user.
typedef plex
{
  u32 Value;
  b32 IsExist;
} decoder_value;

typedef plex
{
  //@NOTE(Emilio): Values for arithmetic
  char* CurrentOpCodeString;
  u16   CurrentOpCode;
  decoder_value Reg;
  decoder_value Seg;
  decoder_value RM;
  decoder_value Destination;
  decoder_value Displacement;
  decoder_value Data;
  u16 OpCodeData2;

  //@NOTE(Emilio): Global to the process.
  cpu_registers Registers;
  u32 MemoryOffset;
  u32 ProgamSize;
  u32 ProcessID;
} cpu_program;

#endif /* _SIMULATOR_H_ */
