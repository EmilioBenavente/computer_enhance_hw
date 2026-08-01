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
  PrintPtr += sprintf(PrintPtr, "[%c] ", (Program->Registers.Flags & 0x0) ? ' ': 'Z');
  PrintPtr += sprintf(PrintPtr, "[%c] ", (Program->Registers.Flags & 0x0) ? ' ': 'Z');
  PrintPtr += sprintf(PrintPtr, "[%c] ", (Program->Registers.Flags & 0x0) ? ' ': 'Z');
  PrintPtr += sprintf(PrintPtr, "[%c] ", (Program->Registers.Flags & 0x0) ? ' ': 'Z');
  PrintPtr += sprintf(PrintPtr, "[%c] ", (Program->Registers.Flags & 0x0) ? ' ': 'Z');
  PrintPtr += sprintf(PrintPtr, "[%c] ", (Program->Registers.Flags & 0x0) ? ' ': 'Z');
  PrintPtr += sprintf(PrintPtr, "[%c] ", (Program->Registers.Flags & 0x0) ? ' ': 'Z');
  PrintPtr += sprintf(PrintPtr, "[%c] ", (Program->Registers.Flags & 0x0) ? ' ': 'Z');
  PrintPtr += sprintf(PrintPtr, "[%c] ", (Program->Registers.Flags & 0x0) ? ' ': 'Z');
  PrintPtr += sprintf(PrintPtr, "[%c] ", (Program->Registers.Flags & 0x0) ? ' ': 'Z');
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

file_scope void
SimulatorFillProgramWithDecoderFields(cpu_program *Program, decoder_opcode *Fields)
{
  //@TODO @HARDCODE(Emilio): Should do something like if(FieldExist) -> Get Value.
  Program->CurrentOpCodeString = Fields->OpCodeStats.OpCodeString;
  Program->Reg.Value    =    Fields->Reg.FieldValue + (8*Fields->WideFlag.FieldValue);
  Program->Reg.IsExist  =   (Fields->Reg.StateFlags & FIELD_EXISTS) == FIELD_EXISTS;
  Program->Seg.Value    =    Fields->SegmentReg.FieldValue;
  Program->Seg.IsExist  =   (Fields->SegmentReg.StateFlags & FIELD_EXISTS) == FIELD_EXISTS;
  Program->Destination.IsExist = (Fields->DestinationFlag.StateFlags & FIELD_EXISTS) == FIELD_EXISTS;
  Program->Destination.Value  = Fields->DestinationFlag.FieldValue;
  Program->Mod.IsExist  = (Fields->Mod.StateFlags & FIELD_EXISTS) == FIELD_EXISTS;
  Program->Mod.Value    = Fields->Mod.FieldValue;
  Program->Data.Value   =    Fields->Data.FieldValue;
  Program->Data.IsExist =   (Fields->Data.StateFlags & FIELD_EXISTS) == FIELD_EXISTS;
  Program->OpCodeData2  = Fields->Data2.FieldValue;
  Program->RM.IsExist   = (Fields->RM.StateFlags & FIELD_EXISTS) == FIELD_EXISTS;

  if(Fields->Mod.FieldValue == 0x3)
  {
    Program->RM.Value = Fields->RM.FieldValue + 24 + (8*Fields->WideFlag.FieldValue);
  }
  else
  {
    //@NOTE(Emilio): The first 24 values have the same 8 values sequentially
    //  with the exception of the first 8 where mod == 0 and rm == 16 bit immediate.
    Program->RM.Value = Fields->RM.FieldValue + 8;
  }
}

file_scope get_register_result
SimulatorGetRegisterPtr(cpu_program *Program, b32 SrcTable)
{
  get_register_result Result = {};
  u16 RegisterOffset = 0;
  if(SrcTable == SIM_REG_TABLE)
  {
    RegisterOffset = Program->Reg.Value % 4;
    if(Program->Reg.Value < 12)
    {
      Result.RegPart1 = (u16 *)(&Program->Registers.A + RegisterOffset);
    }
    else
    {
      Result.RegPart1 = (u16 *)&Program->Registers.SP + RegisterOffset;
    }
  }
  else if(SrcTable == SIM_SEG_TABLE)
  {
    Result.RegPart1 = &(Program->Registers.ES);
    Result.RegPart1 += Program->Seg.Value;
  }
  else if(SrcTable == SIM_RM_TABLE)
  {

    if(Program->RM.Value < 24)
    {
      RegisterOffset = Program->RM.Value % 8;
      switch(RegisterOffset)
      {
        case 0:
        {
          Result.RegPart1 = (u16 *)&Program->Registers.B;
          Result.RegPart2 = &Program->Registers.SI;
        } break;
        case 1:
        {
          Result.RegPart1 = (u16 *)&Program->Registers.B;
          Result.RegPart2 = &Program->Registers.DI;
        } break;
        case 2:
        {
          Result.RegPart1 = &Program->Registers.BP;
          Result.RegPart2 = &Program->Registers.SI;
        } break;
        case 3:
        {
          Result.RegPart1 = &Program->Registers.BP;
          Result.RegPart2 = &Program->Registers.DI;
        } break;
        case 4:
        {
          Result.RegPart1 = &Program->Registers.SI;
        } break;
        case 5:
        {
          Result.RegPart1 = &Program->Registers.DI;
        } break;
        case 6:
        {
          Result.RegPart1 = &Program->Registers.BP;
        } break;
        case 7:
        {
          Result.RegPart1 = (u16 *)&Program->Registers.B;
        } break;
      }
    }
    else if(Program->RM.Value < 36)
    {
      RegisterOffset = Program->RM.Value % 4;

      Result.RegPart1 = (u16*)(&Program->Registers.A + RegisterOffset);
    }
    else
    {
      RegisterOffset = Program->RM.Value % 4;

      Result.RegPart1 = &Program->Registers.SP + RegisterOffset;
    }
  }

  return Result;
}

file_scope u32
SimulatorGetValueFromRegister(cpu_program *Program, get_register_result *RegisterResult, b32 SrcTable)
{
  u32 Result = 0;
  if((SrcTable == SIM_REG_TABLE) ||
    ((SrcTable == SIM_RM_TABLE) && (Program->RM.Value >= 24)))
  {
    if(Program->Reg.Value < 4)
    {
      Result = ((reg_mix *)RegisterResult->RegPart1)->L;
    }
    else if(Program->Reg.Value < 8)
    {
      Result = ((reg_mix *)RegisterResult->RegPart1)->H;
    }
    else
    {
      Result = *RegisterResult->RegPart1;
    }
  }
  else if(SrcTable == SIM_SEG_TABLE)
  {
    Result = *RegisterResult->RegPart1;
  }
  else if(SrcTable == SIM_RM_TABLE)
  {
    if(Program->RM.Value == RM_16BIT_IMM_CASE)
    {
      Result = Program->Displacement.Value;
    }
    else
    {

      u32 RegisterOffset = Program->Reg.Value % 8;
      if(RegisterOffset > 3)
      {
        Result = *RegisterResult->RegPart1;
      }
      else
      {
        Result = *RegisterResult->RegPart1 + *RegisterResult->RegPart2;
      }
    }
  }
  return Result;
}


//@NOTE(Emilio): START HERE!!!
//@TODO(Emilio): We need to consider memory ops with next time.
file_scope void
SimulatorSimMov(cpu_program *Program, alu_contents *ResultContents)
{
  get_register_result FirstReg = {};
  get_register_result SecondReg = {};

  if(Program->Seg.IsExist)
  {
    FirstReg = SimulatorGetRegisterPtr(Program, SIM_SEG_TABLE);
    if(Program->RM.IsExist)
    {
      SecondReg = SimulatorGetRegisterPtr(Program, SIM_RM_TABLE);
    }
  }
  else if(Program->Reg.IsExist)
  {
    FirstReg = SimulatorGetRegisterPtr(Program, SIM_REG_TABLE);
    if(Program->RM.IsExist)
    {
      SecondReg = SimulatorGetRegisterPtr(Program, SIM_RM_TABLE);

      if(Program->Mod.Value == 0x3)
      {
        u32 RegisterValue = 0;
        if(Program->Destination.Value)
        {
           RegisterValue = SimulatorGetValueFromRegister(Program, &SecondReg, SIM_RM_TABLE);
          if(FirstReg.RegPart2)
          {
            u32 MemoryIndex = SimulatorGetValueFromRegister(Program, &FirstReg, SIM_RM_TABLE);
            SimMemory[MemoryIndex] = RegisterValue;
          }
          else
          {
            *FirstReg.RegPart1 = RegisterValue;
          }
        }
        else
        {
           RegisterValue = SimulatorGetValueFromRegister(Program, &FirstReg, SIM_REG_TABLE);

          if(SecondReg.RegPart2)
          {
            u32 MemoryIndex = SimulatorGetValueFromRegister(Program, &SecondReg, SIM_RM_TABLE);
            SimMemory[MemoryIndex] = RegisterValue;
          }
          else
          {
            *SecondReg.RegPart1 = RegisterValue;
          }
        }
      }
      else
      {

      }
    }
    else
    {
      *FirstReg.RegPart1 = Program->Data.Value;
    }

  }
  else if(Program->RM.IsExist)
  {
    FirstReg = SimulatorGetRegisterPtr(Program, SIM_RM_TABLE);
    u32 MemoryIndex = SimulatorGetValueFromRegister(Program, &FirstReg, SIM_RM_TABLE);
    SimMemory[MemoryIndex] = Program->Data.Value;
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
    SimulatorSimMov(Program, &ResultContents);
  }
  else if(ECB_IsStringEqual(Program->CurrentOpCodeString, "add"))
  {
    printf("add\n");
  }
  else if(ECB_IsStringEqual(Program->CurrentOpCodeString, "sub"))
  {
    printf("sub\n");
  }
  else if(ECB_IsStringEqual(Program->CurrentOpCodeString, "cmp"))
  {
    printf("cmp\n");
  }
}
