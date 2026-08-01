#include "simulator.h"

file_scope void
SimulatorFlashProgram(u8 *ProgramPtr, u32 ProgramSize, u32 OffsetInMemory)
{
  u32 MemoryIndex = (0 + OffsetInMemory) %
    (MegaBytes(1) - 1);
  while(ProgramSize--)
  {
    SimMemory[(MemoryIndex++ % (MegaBytes(1) - 1))] = *ProgramPtr++;
  }
}

file_scope u32
SimulatorPrintRegisters(char* WritePtr, cpu_program *Program, b32 IsHexMode)
{
  u32 Result = 0;

  char PrintBuffer[ECB_TEMP_PRINT_BUFFER_SIZE];
  char* PrintPtr = PrintBuffer;

  PrintPtr += sprintf(PrintPtr, "{\n");
  if(IsHexMode)
  {
    PrintPtr += sprintf(PrintPtr, "\tAX -> 0x%X\n", Program->Registers.A.X);
    PrintPtr += sprintf(PrintPtr, "\tBX -> 0x%X\n", Program->Registers.B.X);
    PrintPtr += sprintf(PrintPtr, "\tCX -> 0x%X\n", Program->Registers.C.X);
    PrintPtr += sprintf(PrintPtr, "\tDX -> 0x%X\n", Program->Registers.D.X);
    PrintPtr += sprintf(PrintPtr, "\n");
    PrintPtr += sprintf(PrintPtr, "\tSP -> 0x%X\n", Program->Registers.SP);
    PrintPtr += sprintf(PrintPtr, "\tBP -> 0x%X\n", Program->Registers.BP);
    PrintPtr += sprintf(PrintPtr, "\tSI -> 0x%X\n", Program->Registers.SI);
    PrintPtr += sprintf(PrintPtr, "\tDI -> 0x%X\n", Program->Registers.DI);
    PrintPtr += sprintf(PrintPtr, "\n");
    PrintPtr += sprintf(PrintPtr, "\tES -> 0x%X\n", Program->Registers.ES);
    PrintPtr += sprintf(PrintPtr, "\tCS -> 0x%X\n", Program->Registers.CS);
    PrintPtr += sprintf(PrintPtr, "\tSS -> 0x%X\n", Program->Registers.SS);
    PrintPtr += sprintf(PrintPtr, "\tDS -> 0x%X\n", Program->Registers.DS);
    PrintPtr += sprintf(PrintPtr, "\n");
    PrintPtr += sprintf(PrintPtr, "\tIP -> 0x%X\n", Program->Registers.IP);
    PrintPtr += sprintf(PrintPtr, "\n");
  }
  else
  {
    PrintPtr += sprintf(PrintPtr, "\tAX -> 0x%d\n", Program->Registers.A.X);
    PrintPtr += sprintf(PrintPtr, "\tBX -> 0x%d\n", Program->Registers.B.X);
    PrintPtr += sprintf(PrintPtr, "\tCX -> 0x%d\n", Program->Registers.C.X);
    PrintPtr += sprintf(PrintPtr, "\tDX -> 0x%d\n", Program->Registers.D.X);
    PrintPtr += sprintf(PrintPtr, "\n");
    PrintPtr += sprintf(PrintPtr, "\tSP -> 0x%d\n", Program->Registers.SP);
    PrintPtr += sprintf(PrintPtr, "\tBP -> 0x%d\n", Program->Registers.BP);
    PrintPtr += sprintf(PrintPtr, "\tSI -> 0x%d\n", Program->Registers.SI);
    PrintPtr += sprintf(PrintPtr, "\tDI -> 0x%d\n", Program->Registers.DI);
    PrintPtr += sprintf(PrintPtr, "\n");
    PrintPtr += sprintf(PrintPtr, "\tES -> 0x%d\n", Program->Registers.ES);
    PrintPtr += sprintf(PrintPtr, "\tCS -> 0x%d\n", Program->Registers.CS);
    PrintPtr += sprintf(PrintPtr, "\tSS -> 0x%d\n", Program->Registers.SS);
    PrintPtr += sprintf(PrintPtr, "\tDS -> 0x%d\n", Program->Registers.DS);
    PrintPtr += sprintf(PrintPtr, "\n");
    PrintPtr += sprintf(PrintPtr, "\tIP -> 0x%d\n", Program->Registers.IP);
    PrintPtr += sprintf(PrintPtr, "\n");
  }

  PrintPtr += sprintf(PrintPtr, "\tFLAGS -> ");
  PrintPtr += sprintf(PrintPtr, "[%c] ", (Program->Registers.Flags & CARRY_FLAG)      ? 'C' : ' ');
  PrintPtr += sprintf(PrintPtr, "[%c] ", (Program->Registers.Flags & PARITY_FLAG)     ? 'P' : ' ');
  PrintPtr += sprintf(PrintPtr, "[%c] ", (Program->Registers.Flags & AUXILIARY_FLAG)  ? 'A' : ' ');
  PrintPtr += sprintf(PrintPtr, "[%c] ", (Program->Registers.Flags & ZERO_FLAG)       ? 'Z' : ' ');
  PrintPtr += sprintf(PrintPtr, "[%c] ", (Program->Registers.Flags & SIGN_FLAG)       ? 'S' : ' ');
  PrintPtr += sprintf(PrintPtr, "[%c] ", (Program->Registers.Flags & TRAP_FLAG)       ? 'T' : ' ');
  PrintPtr += sprintf(PrintPtr, "[%c] ", (Program->Registers.Flags & INTERRUPT_FLAG)  ? 'I' : ' ');
  PrintPtr += sprintf(PrintPtr, "[%c] ", (Program->Registers.Flags & DIRECTION_FLAG)  ? 'D' : ' ');
  PrintPtr += sprintf(PrintPtr, "[%c] ", (Program->Registers.Flags & OVERFLOW_FLAG)   ? 'O' : ' ');
  PrintPtr += sprintf(PrintPtr, "\n}\n");
 
  if(WritePtr)
  {
    //@HARDCODE(Emilio): We should make functions for new line and comments
    Result = sprintf(WritePtr, "%s", PrintBuffer);
  }
  else
  {
    //@INCOMPLETE @TODO(Emilio): printf is not a cross compatible function, thank you windows..
  }

  return Result;
}

file_scope lea_result
SimulatorLoadRegisterValue(cpu_program *Program, u16 RegisterIndex)
{
  lea_result Result = {};

  if(RegisterIndex >= 8)
  {
    RegisterIndex = (RegisterIndex - 8);
    Result.Address = (u16 *)&Program->Registers.A + RegisterIndex;
    Result.Value = *Result.Address;
  }
  else
  {
    u16 RegOffset = RegisterIndex % 4;
    reg_mix *RegPtr = &Program->Registers.A + RegOffset;

    Result.Address = (u16 *)RegPtr;
    if(RegisterIndex < 4)
    {
      Result.Value = RegPtr->L;
      Result.IsLow = 1;
    }
    else
    {
      Result.Value = RegPtr->H;
      Result.IsHigh = 1;
    }
  }

  return Result;
}


file_scope lea_result
SimulatorLoadEffectiveAddress(cpu_program *Program, u16 RMIndex)
{
  lea_result Result = {};

  if(RMIndex == RM_16BIT_IMM_CASE)
  {
    Result.Value = Program->Data.Value;
  }
  else
  {
    lea_contents Contents = {};

    if(Program->Displacement.IsExist)
    {
      Contents.Displacement = Program->Displacement.Value;
    }

    RMIndex = RMIndex % 8;

    switch(RMIndex)
    {
      case 0:
      {
        Contents.Left   = (u16 *)&Program->Registers.B;
        Contents.Right  = &Program->Registers.SI;
      } break;
      case 1:
      {
        Contents.Left   = (u16 *)&Program->Registers.B;
        Contents.Right  = &Program->Registers.DI;
      } break;
      case 2:
      {
        Contents.Left   = &Program->Registers.BP;
        Contents.Right  = &Program->Registers.SI;
      } break;
      case 3:
      {
        Contents.Left   = &Program->Registers.BP;
        Contents.Right  = &Program->Registers.DI;
      } break;
      case 4:
      {
        Contents.Left   = &Program->Registers.SI;
      } break;
      case 5:
      {
        Contents.Left   = &Program->Registers.DI;
      } break;
      case 6:
      {
        Contents.Left   = &Program->Registers.BP;
      } break;
      case 7:
      {
        Contents.Left   = (u16 *)&Program->Registers.B;
      } break;
    }

    u16 MemAddress = *(Contents.Left) + 
      ((Contents.Right) ? *(Contents.Right) : 0) +
      + Contents.Displacement;
    Result.Address = (u16 *)&SimMemory[MemAddress];
    Result.Value   = SimMemory[MemAddress];
  }

  return Result;
}


file_scope void
SimulatorFillProgramWithDecoderFields(cpu_program *Program, decoder_opcode *Fields)
{
  //@TODO @HARDCODE(Emilio): Should do something like if(FieldExist) -> Get Value.
  Program->CurrentOpCodeString = Fields->OpCodeStats.OpCodeString;
  Program->Reg.IsExist  =   (Fields->Reg.StateFlags & FIELD_EXISTS) == FIELD_EXISTS || Fields->Reg.FieldValue != 0;
  Program->Reg.Value    =    Fields->Reg.FieldValue + (8*Fields->WideFlag.FieldValue);
  Program->Seg.IsExist  =   (Fields->SegmentReg.StateFlags & FIELD_EXISTS) == FIELD_EXISTS || Fields->SegmentReg.FieldValue != 0;
  Program->Seg.Value    =    Fields->SegmentReg.FieldValue;
  Program->Destination.IsExist = (Fields->DestinationFlag.StateFlags & FIELD_EXISTS) == FIELD_EXISTS || Fields->DestinationFlag.FieldValue != 0;
  Program->Destination.Value  = Fields->DestinationFlag.FieldValue;
  Program->Data.IsExist =   (Fields->Data.StateFlags & FIELD_EXISTS) == FIELD_EXISTS || Fields->Data.FieldValue != 0;
  Program->Data.Value   =    Fields->Data.FieldValue;
  Program->RM.IsExist   = (Fields->RM.StateFlags & FIELD_EXISTS) == FIELD_EXISTS || Fields->RM.FieldValue != 0;
  Program->OpCodeData2  = Fields->Data2.FieldValue;

  if(Program->Seg.IsExist)
  {
    Program->Destination.IsExist  = 1;
    Program->Destination.Value    = Fields->OpCodeStats.OpCode == 0x8E ? 1 : 0;
  }

  if(Fields->Mod.FieldValue == 0x3)
  {
    Program->RM.Value = Fields->RM.FieldValue + 24 + (8*Fields->WideFlag.FieldValue);
    if(Program->Seg.IsExist)
    {
      //@NOTE(Emilio): Segment values only operate on 16 bit values.
      Program->RM.Value += 8;
    }
  }
  else
  {
    //@NOTE(Emilio): The first 24 values have the same 8 values sequentially
    //  with the exception of the first 8 where mod == 0 and rm == 16 bit immediate.
    Program->RM.Value = Fields->RM.FieldValue + 8;
  }
}

file_scope alu_contents
SimulatorLoadALU(cpu_program *Program, sim_op_type OpType)
{
  alu_contents Result = {};

  b32 IsResultTaken  = (OpType != SIM_OP_COMPARISON); 
  b32 IsWriteToFlags = (OpType != SIM_OP_MOV);

  Result.IsResultTaken = IsResultTaken;
  Result.IsWriteToFlags = IsWriteToFlags;

  //@NOTE(Emilio): SEG and RM
  if(Program->Seg.IsExist)
  {
    u16 SegmentValue = 0;
    u16* SegPtr = &Program->Registers.ES;
    SegPtr += Program->Seg.Value;
    SegmentValue = *SegPtr;

    lea_result RMValue = {};
    u16 RMIndex = Program->RM.Value;
    if(RMIndex < 24)
    {
      RMValue = SimulatorLoadEffectiveAddress(Program, RMIndex);
    }
    else
    {
      RMValue = SimulatorLoadRegisterValue(Program, (RMIndex - 24));
    }

    if(Program->Destination.Value)
    {
      Result.Dest = SegPtr;
      Result.A = *SegPtr;

      Result.B = RMValue.Value;
    }
    else
    {
      Result.Dest       = RMValue.Address;
      Result.A          = RMValue.Value;
      Result.IsDestLow  = RMValue.IsLow;
      Result.IsDestHigh = RMValue.IsHigh;

      Result.B = *SegPtr;
    }
  }
  //@NOTE(Emilio): DATA and [REG/RM]
  else if(Program->Data.IsExist)
  {
    Result.B = Program->Data.Value;

    if(Program->RM.IsExist)
    {
      lea_result RMValue = {};
      u16 RMIndex = Program->RM.Value;
      if(RMIndex < 24)
      {
        RMValue = SimulatorLoadEffectiveAddress(Program, RMIndex);
      }
      else
      {
        RMValue = SimulatorLoadRegisterValue(Program, (RMIndex - 24));
      }

      Result.Dest       = RMValue.Address;
      Result.A          = RMValue.Value;
      Result.IsDestLow  = RMValue.IsLow;
      Result.IsDestHigh = RMValue.IsHigh;
    }
    else
    {
      lea_result RegValue = {};
      RegValue = SimulatorLoadRegisterValue(Program, Program->Reg.Value);

      Result.Dest       = RegValue.Address;
      Result.A          = RegValue.Value;
      Result.IsDestLow  = RegValue.IsLow;
      Result.IsDestHigh = RegValue.IsHigh;
    }
  }
  else
  {
    //@NOTE(Emilio): REG and RM
    lea_result RegValue = SimulatorLoadRegisterValue(Program, Program->Reg.Value);

    lea_result RMValue = {};
    u16 RMIndex = Program->RM.Value;
    if(RMIndex < 24)
    {
      RMValue = SimulatorLoadEffectiveAddress(Program, RMIndex);
    }
    else
    {
      RMValue = SimulatorLoadRegisterValue(Program, (RMIndex - 24));
    }

    if(Program->Destination.Value)
    {
      Result.Dest       = RegValue.Address;
      Result.IsDestLow  = RegValue.IsLow;
      Result.IsDestHigh = RegValue.IsHigh;

      Result.A = RegValue.Value;
      Result.B = RMValue.Value;
    }
    else
    {
      Result.Dest       = RMValue.Address;
      Result.IsDestLow  = RMValue.IsLow;
      Result.IsDestHigh = RMValue.IsHigh;

      Result.A = RMValue.Value;
      Result.B = RegValue.Value;
    }
  }
  Result.PrevValue = *Result.Dest;

  return Result;
}


file_scope void
SimulatorUpdateFlags(u16 *Flags, alu_contents* ALU)
{
//@TODO(Emilio): Carry
//@TODO(Emilio): Parity
//@TODO(Emilio): Aux
  if(ALU->NewValue == 0)
  {
    *Flags ^= ZERO_FLAG;
  }

  if(ALU->NewValue & 0x8000)
  {
    *Flags ^= SIGN_FLAG;
  }
//@TODO(Emilio): Trap
//@TODO(Emilio): Interrupt
//@TODO(Emilio): Direction
//@TODO(Emilio): Overflow
}

file_scope void
SimulatorSimulateMov(cpu_program *Program, alu_contents *ALU)
{
  ALU->NewValue = *ALU->Dest;
  reg_mix *NewValueMix = (reg_mix *)&ALU->NewValue;

  if(ALU->IsDestLow)
  {
    NewValueMix->L = ALU->B;
  }
  else if(ALU->IsDestHigh)
  {
    NewValueMix->H = ALU->B;
  }
  else
  {
    ALU->NewValue = ALU->B;
  }

  if(ALU->IsResultTaken)
  {
    *ALU->Dest = ALU->NewValue;
  }

  if(ALU->IsWriteToFlags)
  {
    SimulatorUpdateFlags(&Program->Registers.Flags, ALU);
  }
}

file_scope void
SimulatorSingleInstruction(cpu_program *Program)
{

  alu_contents ResultContents = {};

  if(*Program->CurrentOpCodeString == 'j')
  {
    printf("jmp\n");
  }
  else if(ECB_IsStringEqual(Program->CurrentOpCodeString, "mov"))
  {
    alu_contents ALU = SimulatorLoadALU(Program, SIM_OP_MOV);
    SimulatorSimulateMov(Program, &ALU);
  }
  else if(ECB_IsStringEqual(Program->CurrentOpCodeString, "add"))
  {
    SimulatorLoadALU(Program, SIM_OP_ADDITION);
  }
  else if(ECB_IsStringEqual(Program->CurrentOpCodeString, "sub"))
  {
    SimulatorLoadALU(Program, SIM_OP_SUBTRACTION);
    printf("sub\n");
  }
  else if(ECB_IsStringEqual(Program->CurrentOpCodeString, "cmp"))
  {
    SimulatorLoadALU(Program, SIM_OP_SUBTRACTION);
    printf("cmp\n");
  }
}
