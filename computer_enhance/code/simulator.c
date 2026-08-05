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
SimulatorPrintRegisters(char* WritePtr, cpu_program *Program)
{
  u32 Result = 0;

  char PrintBuffer[ECB_TEMP_PRINT_BUFFER_SIZE];
  char* PrintPtr = PrintBuffer;

  PrintPtr += sprintf(PrintPtr, "{\n");
  PrintPtr += sprintf(PrintPtr, "\tAX -> 0x%X [%d] [%d]\n", Program->Registers.A.X, (s16)Program->Registers.A.X, (u16)Program->Registers.A.X);
  PrintPtr += sprintf(PrintPtr, "\tBX -> 0x%X [%d] [%d]\n", Program->Registers.B.X, (s16)Program->Registers.B.X, (u16)Program->Registers.B.X);
  PrintPtr += sprintf(PrintPtr, "\tCX -> 0x%X [%d] [%d]\n", Program->Registers.C.X, (s16)Program->Registers.C.X, (u16)Program->Registers.C.X);
  PrintPtr += sprintf(PrintPtr, "\tDX -> 0x%X [%d] [%d]\n", Program->Registers.D.X, (s16)Program->Registers.D.X, (u16)Program->Registers.D.X);
  PrintPtr += sprintf(PrintPtr, "\n");
  PrintPtr += sprintf(PrintPtr, "\tSP -> 0x%X [%d] [%d]\n", Program->Registers.SP, (s16)Program->Registers.SP, (u16)Program->Registers.SP);
  PrintPtr += sprintf(PrintPtr, "\tBP -> 0x%X [%d] [%d]\n", Program->Registers.BP, (s16)Program->Registers.BP, (u16)Program->Registers.BP);
  PrintPtr += sprintf(PrintPtr, "\tSI -> 0x%X [%d] [%d]\n", Program->Registers.SI, (s16)Program->Registers.SI, (u16)Program->Registers.SI);
  PrintPtr += sprintf(PrintPtr, "\tDI -> 0x%X [%d] [%d]\n", Program->Registers.DI, (s16)Program->Registers.DI, (u16)Program->Registers.DI);
  PrintPtr += sprintf(PrintPtr, "\n");
  PrintPtr += sprintf(PrintPtr, "\tES -> 0x%X [%d] [%d]\n", Program->Registers.ES, (s16)Program->Registers.ES, (u16)Program->Registers.ES);
  PrintPtr += sprintf(PrintPtr, "\tCS -> 0x%X [%d] [%d]\n", Program->Registers.CS, (s16)Program->Registers.CS, (u16)Program->Registers.CS);
  PrintPtr += sprintf(PrintPtr, "\tSS -> 0x%X [%d] [%d]\n", Program->Registers.SS, (s16)Program->Registers.SS, (u16)Program->Registers.SS);
  PrintPtr += sprintf(PrintPtr, "\tDS -> 0x%X [%d] [%d]\n", Program->Registers.DS, (s16)Program->Registers.DS, (u16)Program->Registers.DS);
  PrintPtr += sprintf(PrintPtr, "\n");
  PrintPtr += sprintf(PrintPtr, "\tIP -> 0x%X [%d] [%d]\n", Program->Registers.IP, (s16)Program->Registers.IP, (u16)Program->Registers.IP);
  PrintPtr += sprintf(PrintPtr, "\n");

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
  Program->CurrentOpCodeString  = Fields->OpCodeStats.OpCodeString;
  Program->CurrentOpCode        = Fields->OpCodeStats.OpCode;
  Program->Reg.IsExist          = (Fields->Reg.StateFlags & FIELD_EXISTS) == FIELD_EXISTS || Fields->Reg.FieldValue != 0;
  Program->Reg.Value            = Fields->Reg.FieldValue + (8*Fields->WideFlag.FieldValue);
  Program->Seg.IsExist          = (Fields->SegmentReg.StateFlags & FIELD_EXISTS) == FIELD_EXISTS || Fields->SegmentReg.FieldValue != 0;
  Program->Seg.Value            =  Fields->SegmentReg.FieldValue;
  Program->Destination.IsExist  = (Fields->DestinationFlag.StateFlags & FIELD_EXISTS) == FIELD_EXISTS || Fields->DestinationFlag.FieldValue != 0;
  Program->Destination.Value    = Fields->DestinationFlag.FieldValue;
  Program->Data.IsExist         = (Fields->Data.StateFlags & FIELD_EXISTS) == FIELD_EXISTS || Fields->Data.FieldValue != 0;
  Program->Data.Value           =  Fields->Data.FieldValue;
  Program->Displacement.IsExist  = (Fields->Displacement.StateFlags & FIELD_EXISTS) == FIELD_EXISTS || Fields->Displacement.FieldValue != 0;
  Program->Displacement.Value   =  Fields->Displacement.FieldValue;
  Program->RM.IsExist           = (Fields->RM.StateFlags & FIELD_EXISTS) == FIELD_EXISTS || Fields->RM.FieldValue != 0;
  Program->OpCodeData2          = Fields->Data2.FieldValue;

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
  u32 TestMask = ALU->IsDestLow ? (1 << 8) : (1 << 16);
  b32 IsCarry = ALU->NewValue & (TestMask);
  if(IsCarry)
  {
    *Flags |= CARRY_FLAG;
  }
  else
  {
    *Flags &= ~(CARRY_FLAG);
  }

  u16 TestValue = ALU->NewValue;
  TestValue = TestValue ^ (TestValue>>1);
  TestValue = TestValue ^ (TestValue>>2);
  TestValue = TestValue ^ (TestValue>>4);
  TestValue = (~TestValue) & 0x1;
  if(TestValue)
  {
    *Flags |= PARITY_FLAG;
  }
  else
  {
    *Flags &= ~(PARITY_FLAG);
  }

  if(ALU->IsAFSet)
  {
    *Flags |= AUXILIARY_FLAG;
  }
  else
  {
    *Flags &= ~(AUXILIARY_FLAG);
  }

  if(ALU->NewValue == 0)
  {
    *Flags |= ZERO_FLAG;
  }
  else
  {
    *Flags &= ~(ZERO_FLAG);
  }

  if(ALU->NewValue & 0x8000)
  {
    *Flags |= SIGN_FLAG;
  }
  else
  {
    *Flags &= ~(SIGN_FLAG);
  }


  if(ALU->IsOFSet)
  {
    *Flags |= OVERFLOW_FLAG;
  }
  else
  {
    *Flags &= ~(OVERFLOW_FLAG);
  }

//@NOTE(Emilio): System-Case Driven
//@TODO(Emilio): Interrupt
//@TODO(Emilio): Direction
//@TODO(Emilio): Trap
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
SimulatorSimulateAdd(cpu_program *Program, alu_contents *ALU)
{
  ALU->NewValue = *ALU->Dest;
  reg_mix *NewValueMix = (reg_mix *)&ALU->NewValue;

  if(ALU->IsDestLow)
  {
    NewValueMix->L = NewValueMix->L + ALU->B;
  }
  else if(ALU->IsDestHigh)
  {
    NewValueMix->H = NewValueMix->H + ALU->B;
  }
  else
  {
    ALU->NewValue = ALU->NewValue + ALU->B;
  }

  if(ALU->IsResultTaken)
  {
    *ALU->Dest = ALU->NewValue;
  }

  if(ALU->IsWriteToFlags)
  {
    ALU->IsAFSet = (((ALU->PrevValue & 0xF) + (ALU->B & 0xF)) & 0x10);

    u32 SignBit = ALU->IsDestLow ? (1 << 7) : (1 << 15);
    ALU->IsOFSet = (ALU->NewValue ^ ALU->PrevValue) &
      (~(ALU->PrevValue ^ ALU->B)) & SignBit;

    SimulatorUpdateFlags(&Program->Registers.Flags, ALU);
  }
}

file_scope void
SimulatorSimulateSub(cpu_program *Program, alu_contents *ALU)
{
  ALU->NewValue = *ALU->Dest;
  reg_mix *NewValueMix = (reg_mix *)&ALU->NewValue;

  if(ALU->IsDestLow)
  {
    NewValueMix->L = NewValueMix->L - ALU->B;
  }
  else if(ALU->IsDestHigh)
  {
    NewValueMix->H = NewValueMix->H - ALU->B;
  }
  else
  {
    ALU->NewValue = ALU->NewValue - ALU->B;
  }

  if(ALU->IsResultTaken)
  {
    *ALU->Dest = ALU->NewValue;
  }

  if(ALU->IsWriteToFlags)
  {
    ALU->IsAFSet = (((ALU->PrevValue & 0xF) - (ALU->B & 0xF)) & 0x10);

    u32 SignBit = ALU->IsDestLow ? (1 << 7) : (1 << 15);
    ALU->IsOFSet = (ALU->NewValue ^ ALU->A) &
      (ALU->A ^ ALU->B) & SignBit;

    SimulatorUpdateFlags(&Program->Registers.Flags, ALU);
  }
}

file_scope void
SimulatorSimulateJmp(cpu_program *Program)
{
  switch(Program->CurrentOpCode)
  {
    //@NOTE(Emilio): OP_JE_JZ
    case 0x74:
    {
      if(Program->Registers.Flags & ZERO_FLAG)
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;

    //@NOTE(Emilio): OP_JL_JNGE
    case 0x7C:
    {
      if((Program->Registers.Flags & SIGN_FLAG)     ^
        (Program->Registers.Flags  & OVERFLOW_FLAG))
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;

    //@NOTE(Emilio): OP_JLE_JNG
    case 0x7E:
    {
      if((Program->Registers.Flags & SIGN_FLAG)     ^
        (Program->Registers.Flags  & OVERFLOW_FLAG) |
        (Program->Registers.Flags  & ZERO_FLAG))
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;

    //@NOTE(Emilio): OP_JB_JNAE
    case 0x72:
    {
      if(Program->Registers.Flags & CARRY_FLAG)
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;

    //@NOTE(Emilio): OP_JBE_JNA
    case 0x76:
    {
      if((Program->Registers.Flags & CARRY_FLAG) |
      (Program->Registers.Flags   & ZERO_FLAG))
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;

    //@NOTE(Emilio): OP_JP_JPE
    case 0x7A:
    {
      if((Program->Registers.Flags & PARITY_FLAG) |
      (Program->Registers.Flags   & ZERO_FLAG))
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;

    //@NOTE(Emilio): OP_JO
    case 0x70:
    {
      if(Program->Registers.Flags & OVERFLOW_FLAG)
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;

    //@NOTE(Emilio): OP_JS
    case 0x78:
    {
      if(Program->Registers.Flags & SIGN_FLAG)
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;

    //@NOTE(Emilio): OP_JNE_JNZ
    case 0x75:
    {
      if((Program->Registers.Flags & ZERO_FLAG) == 0)
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;

    //@NOTE(Emilio): OP_JNL_JGE
    case 0x7D:
    {
      if((Program->Registers.Flags & SIGN_FLAG)     ^
        (Program->Registers.Flags  & OVERFLOW_FLAG) == 0)
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;

    //@NOTE(Emilio): OP_JNLE_JG
    case 0x7F:
    {
      if((Program->Registers.Flags & SIGN_FLAG)     ^
        (Program->Registers.Flags  & OVERFLOW_FLAG) |
        (Program->Registers.Flags  & ZERO_FLAG) == 0)
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;

    //@NOTE(Emilio): OP_JNB_JAE
    case 0x73:
    {
      if((Program->Registers.Flags & CARRY_FLAG) == 0)
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;

    //@NOTE(Emilio): OP_JNBE_JA
    case 0x77:
    {
      if((Program->Registers.Flags & CARRY_FLAG) |
        (Program->Registers.Flags  & ZERO_FLAG) == 0)
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;

    //@NOTE(Emilio): OP_JNP_JPO
    case 0x7B:
    {
      if((Program->Registers.Flags & PARITY_FLAG) == 0)
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;

    //@NOTE(Emilio): OP_JNO
    case 0x71:
    {
      if((Program->Registers.Flags & OVERFLOW_FLAG) == 0)
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;

    //@NOTE(Emilio): OP_JNS
    case 0x79:
    {
      if((Program->Registers.Flags & SIGN_FLAG) == 0)
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;

    //@NOTE(Emilio): OP_LOOP
    case 0xE2:
    {
      Program->Registers.C.X = Program->Registers.C.X - 1;
      if(Program->Registers.C.X == 0)
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;

    //@NOTE(Emilio): OP_LOOPZ_LOOPE
    case 0xE1:
    {
      Program->Registers.C.X = Program->Registers.C.X - 1;
      if(Program->Registers.C.X &&
        (Program->Registers.Flags & ZERO_FLAG))
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;

    //@NOTE(Emilio): OP_LOOPNZ_LOOPNE
    case 0xE0:
    {
      Program->Registers.C.X = Program->Registers.C.X - 1;
      if(Program->Registers.C.X &&
        (Program->Registers.Flags & ZERO_FLAG) == 0)
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;

    //@NOTE(Emilio): OP_JXZ
    case 0xE3:
    {
      if(Program->Registers.C.X == 0)
      {
        Program->Registers.IP = Program->Registers.IP + Program->Data.Value;
      }
    } break;
  }


  //@TODO(Emilio): OP_JMP_SEG
  //@TODO(Emilio): OP_JMP_SEG_SHORT
  //@TODO(Emilio): OP_JMP_INDIRECT_SEG
  //@TODO(Emilio): OP_JMP_DIRECT_INTERSEG
  //@TODO(Emilio): OP_JMP_INDIRECT_INTERSEG

}

file_scope void
SimulatorSingleInstruction(cpu_program *Program)
{

  alu_contents ResultContents = {};

  if((*Program->CurrentOpCodeString == 'j') ||
    (ECB_IsStringEqual(Program->CurrentOpCodeString, "loop")))
  {
    SimulatorSimulateJmp(Program);
  }
  else if(ECB_IsStringEqual(Program->CurrentOpCodeString, "mov"))
  {
    alu_contents ALU = SimulatorLoadALU(Program, SIM_OP_MOV);
    SimulatorSimulateMov(Program, &ALU);
  }
  else if(ECB_IsStringEqual(Program->CurrentOpCodeString, "add"))
  {
    alu_contents ALU = SimulatorLoadALU(Program, SIM_OP_ADDITION);
    SimulatorSimulateAdd(Program, &ALU);
  }
  else if(ECB_IsStringEqual(Program->CurrentOpCodeString, "sub"))
  {
    alu_contents ALU = SimulatorLoadALU(Program, SIM_OP_SUBTRACTION);
    SimulatorSimulateSub(Program, &ALU);
  }
  else if(ECB_IsStringEqual(Program->CurrentOpCodeString, "cmp"))
  {
    alu_contents ALU = SimulatorLoadALU(Program, SIM_OP_COMPARISON);
    SimulatorSimulateSub(Program, &ALU);
  }
}
