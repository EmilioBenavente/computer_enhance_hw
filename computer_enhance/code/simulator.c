#include "simulator.h"

file_scope void
SimulatorFlashProgram(void* MemoryChipPtr, void* Program,
                      u32 MemoryAddress, u32 SizeOfProgram)
{
  ECB_ASSERT(MemoryAddress < MegaBytes(1));
  ECB_ASSERT(SizeOfProgram < MegaBytes(1));
  ECB_ASSERT(MemoryAddress + SizeOfProgram < MegaBytes(1));

  u8* MemoryPtr  = ((u8*)MemoryChipPtr) + MemoryAddress;
  u8* ProgramPtr = (u8*)Program;

  for(u32 Iter = 0; Iter < SizeOfProgram; Iter++)
  {
    *MemoryPtr++ = *ProgramPtr++;
  }
}

file_scope void
SimulatorUpdateFlagResisters(sim_cpu* SimulatorContext,
                             u16 ALUCode, u16 NewValue, u16 A, u16 B)
{
  SimulatorContext->IsFlagsUpdated = 1;
  b32 TestCarry = 0;
  b32 TestParity = 0;
  b32 TestAux = 0;
  b32 TestZero = 0;
  b32 TestSign = 0;
  b32 TestOverflow = 0;
  b32 TestIntEn = 0;
  b32 TestDirection = 0;
  b32 TestTrap = 0;

  switch(ALUCode)
  {
    case ALU_ADD:
    case ALU_SUB:
    case ALU_CMP:
    {
      TestOverflow = 1;
      TestSign = 1;
      TestZero = 1;
      TestAux = 1;
      TestParity = 1;
      TestCarry = 1;
    } break;
  }

  if(TestCarry)
  {
    s32 TestValue = 0;
    if(ALUCode == ALU_ADD)
    {
      TestValue = A + B;
    }
    else
    {
      TestValue = A - B;
    }
    if(TestValue < 0)
    {
      SimulatorContext->Flags ^= (1 << 0);
    }
  }

  if(TestParity)
  {
    b32 TestPass = 0;
    u16 TestValue = NewValue;
    while(TestValue)
    {
      TestPass ^= (TestValue & 1);
      TestValue >>= 1;
    }

    if(TestPass)
    {
      SimulatorContext->Flags ^= (1 << 1);
    }
  }

  if(TestAux)
  {
    b32 TestPass = (NewValue << 5) != (A << 5);
    if(TestPass)
    {
      SimulatorContext->Flags ^= (1 << 2);
    }
  }

  if(TestZero)
  {
    if(NewValue == 0)
    {
      SimulatorContext->Flags ^= (1 << 3);
    }
  }

  if(TestSign)
  {
    if(NewValue & (1 << 15))
    {
      SimulatorContext->Flags ^= (1 << 4);
    }
  }

  if(TestOverflow)
  {
    u16 TestValue = (1<<15);
    if(((A ^ TestValue) & (B ^ TestValue) & 0x80) != 0)
    {
      SimulatorContext->Flags ^= (1 << 5);
    }
  }

  if(TestIntEn)
  {

  }

  if(TestDirection)
  {

  }

  if(TestTrap)
  {

  }


}


file_scope void
SimulatorUpdatePC(decoder_context* DecoderContext, sim_cpu* RegisterPtr)
{
  if(GlobalIsGamePaused == 0)
  {
    PCUndos[PCUndoIter++] = DecoderContext->BytesRead;
    RegisterPtr->PC += DecoderContext->BytesRead;
  }
  else
  {
    if(GlobalStepOffset > 0)
    {
      PCUndos[PCUndoIter++] = DecoderContext->BytesRead;
      RegisterPtr->PC += DecoderContext->BytesRead;

      GlobalStepOffset = 0;
    }
    else if(GlobalStepOffset < 0)
    {
      PCUndoIter--;
      if(PCUndoIter < 0)
      {
        PCUndoIter = 0;
      }
      RegisterPtr->PC -= PCUndos[PCUndoIter];
      GlobalStepOffset = 0;
    }
  }
}

file_scope void
SimulatorSimulateBranchInstruction(decoder_context* DecoderContext, sim_cpu* SimulatorContext)
{

}

file_scope void
SimulatorSimulateALUInstruction(decoder_context* DecoderContext, sim_cpu* SimulatorContext, u16 ALUCode)
{
  u16* RegisterPtr = (u16*)&SimulatorContext->A;
  u16* RMPtr = (u16*)&SimulatorContext->A;
  u16* RMSecondPtr = (u16*)&SimulatorContext->A;

  b32 IsRegLow = 0;
  b32 IsRegHigh = 0;
  b32 IsRMLow = 0;
  b32 IsRMHigh = 0;
  b32 IsRMSpecialCase = 0;

  u16 RegOpState = 0;
  u16 RegIndex = 0;
  u16 RMIndex = 0;

  SimulatorContext->RMIndex = -1;
  SimulatorContext->RegIndex = -1;

  //@NOTE(Emilio): Get offsets from the register table into the sim_cpu plex
  if(DecoderContext->InstructionFields.Reg.IsExists)
  {
    RegOpState = IMM_TO_REG;
    RegIndex = DecoderContext->InstructionFields.Reg.Value;
    if(DecoderContext->InstructionFields.IsWide.Value == 0)
    {
      IsRegLow = RegIndex < 4;
      IsRegHigh = RegIndex >= 4;
      RegIndex %= 4;
    }
    RegisterPtr += RegIndex;
    SimulatorContext->RegIndex = RegIndex;
  }

  if(DecoderContext->InstructionFields.Seg.IsExists)
  {
    RegOpState = IMM_TO_REG;
    RegIndex = DecoderContext->InstructionFields.Seg.Value + 8;
    RegisterPtr += RegIndex;
    SimulatorContext->RegIndex = RegIndex;
  }

  if(DecoderContext->InstructionFields.RM.IsExists)
  {
    RegOpState =
      DecoderContext->InstructionFields.DataLow.IsExists ?
      IMM_TO_RM : RM_TO_REG;
    RegOpState =
      DecoderContext->InstructionFields.IsDestination.Value ?
      RegOpState : REG_TO_RM;
    RMIndex = DecoderContext->InstructionFields.RM.Value;
    if(DecoderContext->InstructionFields.Mod.Value == 3)
    {
      if((DecoderContext->InstructionFields.IsWide.Value == 0) &&
          (DecoderContext->InstructionFields.OpCode.Value != SimISA[5].OpCode.Value))
      {
        IsRMLow = RMIndex < 4;
        IsRMHigh = RMIndex >= 4;
        RMIndex %= 4;
      }
      RMPtr += RMIndex;
    }
    else
    {
      switch(RMIndex)
      {
        case 0:
        {
          RMPtr = (u16*)&SimulatorContext->B;
          RMSecondPtr = &SimulatorContext->SI;
        } break;

        case 1:
        {
          RMPtr = (u16*)&SimulatorContext->B;
          RMSecondPtr = &SimulatorContext->DI;
        } break;

        case 2:
        {
          RMPtr = &SimulatorContext->BP;
          RMSecondPtr = &SimulatorContext->SI;
        } break;

        case 3:
        {
          RMPtr = &SimulatorContext->BP;
          RMSecondPtr = &SimulatorContext->DI;
        } break;

        case 4:
        {
          RMPtr = &SimulatorContext->SI;
        } break;

        case 5:
        {
          RMPtr = &SimulatorContext->DI;
        } break;

        case 6:
        {
          IsRMSpecialCase = DecoderContext->InstructionFields.Mod.Value == 6 ? 1 : 0;
          RMPtr = &SimulatorContext->BP;
        } break;

        case 7:
        {
          RMPtr = (u16*)&SimulatorContext->B;
        } break;
      }
    }

    SimulatorContext->RMIndex = RMIndex;

  }


  //@NOTE(Emilio): Do the ALU instruction
  switch(ALUCode)
  {
    case ALU_MOV:
    {
      switch(RegOpState)
      {
        case REG_TO_RM:
        case RM_TO_REG:
        {
          reg_type* RegOne = 0;
          reg_type* RegTwo = 0;
          b32 IsRegOneLow  = 0;
          b32 IsRegOneHigh = 0;
          b32 IsRegTwoLow  = 0;
          b32 IsRegTwoHigh = 0;

          if(RegOpState == REG_TO_RM)
          {
            SimulatorContext->RMIndex = RegIndex;
            SimulatorContext->RegIndex = RMIndex;
            RegOne = (reg_type*)RMPtr;
            RegTwo = (reg_type*)RegisterPtr;
            IsRegOneLow  = IsRMLow;
            IsRegOneHigh = IsRMHigh;
            IsRegTwoLow  = IsRegLow;
            IsRegTwoHigh = IsRegHigh;
          }
          else
          {
            RegOne = (reg_type*)RegisterPtr;
            RegTwo = (reg_type*)RMPtr;
            IsRegOneLow  = IsRegLow;
            IsRegOneHigh = IsRegHigh;
            IsRegTwoLow  = IsRMLow;
            IsRegTwoHigh = IsRMHigh;
          }

          if(IsRegOneLow)
          {
            if(IsRegTwoLow)
            {
              RegOne->Low = RegTwo->Low;
            }
            else if(IsRegTwoHigh)
            {
              RegOne->Low = RegTwo->High;
            }
            else
            {
              RegOne->Low = RegTwo->Value & 0xFF;
            }
          }
          else if(IsRegOneHigh)
          {
            if(IsRegTwoLow)
            {
              RegOne->High = RegTwo->Low;
            }
            else if(IsRegTwoHigh)
            {
              RegOne->High = RegTwo->High;
            }
            else
            {
              RegOne->High = (RegTwo->Value >> 4) & 0xFF;
            }
          }
          else
          {
            if(IsRegTwoLow)
            {
              RegOne->Value = RegTwo->Low;
            }
            else if(IsRegTwoHigh)
            {
              RegOne->Value = RegTwo->High;
            }
            else
            {
              RegOne->Value = RegTwo->Value;
            }
          }
        } break;

        case IMM_TO_RM:
        {
          *RMPtr = DecoderContext->InstructionFields.Data;
        } break;

        case IMM_TO_REG:
        {
          reg_type* RegisterValueAsRegType = (reg_type*)RegisterPtr;
          if(IsRegLow)
          {
            RegisterValueAsRegType->Low = DecoderContext->InstructionFields.Data;
          }
          else if(IsRegHigh)
          {
            RegisterValueAsRegType->High = DecoderContext->InstructionFields.Data;
          }
          else
          {
            RegisterValueAsRegType->Value = DecoderContext->InstructionFields.Data;
          }
        } break;
      }

    } break;

    case ALU_ADD:
    {
      switch(RegOpState)
      {
        case REG_TO_RM:
        case RM_TO_REG:
        {
          reg_type* RegOne = 0;
          reg_type* RegTwo = 0;
          b32 IsRegOneLow  = 0;
          b32 IsRegOneHigh = 0;
          b32 IsRegTwoLow  = 0;
          b32 IsRegTwoHigh = 0;

          if(RegOpState == REG_TO_RM)
          {
            SimulatorContext->RMIndex = RegIndex;
            SimulatorContext->RegIndex = RMIndex;
            RegOne = (reg_type*)RMPtr;
            RegTwo = (reg_type*)RegisterPtr;
            IsRegOneLow  = IsRMLow;
            IsRegOneHigh = IsRMHigh;
            IsRegTwoLow  = IsRegLow;
            IsRegTwoHigh = IsRegHigh;
          }
          else
          {
            RegOne = (reg_type*)RegisterPtr;
            RegTwo = (reg_type*)RMPtr;
            IsRegOneLow  = IsRegLow;
            IsRegOneHigh = IsRegHigh;
            IsRegTwoLow  = IsRMLow;
            IsRegTwoHigh = IsRMHigh;
          }

          u16 PrevValue = RegOne->Value;
          if(IsRegOneLow)
          {
            if(IsRegTwoLow)
            {
              RegOne->Low += RegTwo->Low;
            }
            else if(IsRegTwoHigh)
            {
              RegOne->Low += RegTwo->High;
            }
            else
            {
              RegOne->Low += RegTwo->Value & 0xFF;
            }
          }
          else if(IsRegOneHigh)
          {
            if(IsRegTwoLow)
            {
              RegOne->High += RegTwo->Low;
            }
            else if(IsRegTwoHigh)
            {
              RegOne->High += RegTwo->High;
            }
            else
            {
              RegOne->High += (RegTwo->Value >> 4) & 0xFF;
            }
          }
          else
          {
            if(IsRegTwoLow)
            {
              RegOne->Value += RegTwo->Low;
            }
            else if(IsRegTwoHigh)
            {
              RegOne->Value += RegTwo->High;
            }
            else
            {
              RegOne->Value += RegTwo->Value;
            }
          }

          SimulatorUpdateFlagResisters(SimulatorContext, ALU_ADD, RegOne->Value,
              PrevValue, RegTwo->Value);
        } break;

        case IMM_TO_RM:
        {
          u16 PrevValue = *RMPtr;
          *RMPtr += DecoderContext->InstructionFields.Data;

          SimulatorUpdateFlagResisters(SimulatorContext, ALU_ADD, *RMPtr,
              PrevValue, DecoderContext->InstructionFields.Data);
        } break;

        case IMM_TO_REG:
        {
          reg_type* RegisterValueAsRegType = (reg_type*)RegisterPtr;
          u16 PrevValue = RegisterValueAsRegType->Value;
          if(IsRegLow)
          {
            RegisterValueAsRegType->Low += DecoderContext->InstructionFields.Data;
          }
          else if(IsRegHigh)
          {
            RegisterValueAsRegType->High += DecoderContext->InstructionFields.Data;
          }
          else
          {
            RegisterValueAsRegType->Value += DecoderContext->InstructionFields.Data;
          }

          SimulatorUpdateFlagResisters(SimulatorContext, ALU_ADD,
              RegisterValueAsRegType->Value,
              PrevValue, DecoderContext->InstructionFields.Data);
        } break;
      }

    } break;

    case ALU_SUB:
    {
      switch(RegOpState)
      {
        case REG_TO_RM:
        case RM_TO_REG:
        {
          reg_type* RegOne = 0;
          reg_type* RegTwo = 0;
          b32 IsRegOneLow  = 0;
          b32 IsRegOneHigh = 0;
          b32 IsRegTwoLow  = 0;
          b32 IsRegTwoHigh = 0;

          if(RegOpState == REG_TO_RM)
          {
            SimulatorContext->RMIndex = RegIndex;
            SimulatorContext->RegIndex = RMIndex;
            RegOne = (reg_type*)RMPtr;
            RegTwo = (reg_type*)RegisterPtr;
            IsRegOneLow  = IsRMLow;
            IsRegOneHigh = IsRMHigh;
            IsRegTwoLow  = IsRegLow;
            IsRegTwoHigh = IsRegHigh;
          }
          else
          {
            RegOne = (reg_type*)RegisterPtr;
            RegTwo = (reg_type*)RMPtr;
            IsRegOneLow  = IsRegLow;
            IsRegOneHigh = IsRegHigh;
            IsRegTwoLow  = IsRMLow;
            IsRegTwoHigh = IsRMHigh;
          }

          u16 PrevValue = RegOne->Value;
          if(IsRegOneLow)
          {
            if(IsRegTwoLow)
            {
              RegOne->Low -= RegTwo->Low;
            }
            else if(IsRegTwoHigh)
            {
              RegOne->Low -= RegTwo->High;
            }
            else
            {
              RegOne->Low -= RegTwo->Value & 0xFF;
            }
          }
          else if(IsRegOneHigh)
          {
            if(IsRegTwoLow)
            {
              RegOne->High -= RegTwo->Low;
            }
            else if(IsRegTwoHigh)
            {
              RegOne->High -= RegTwo->High;
            }
            else
            {
              RegOne->High -= (RegTwo->Value >> 4) & 0xFF;
            }
          }
          else
          {
            if(IsRegTwoLow)
            {
              RegOne->Value -= RegTwo->Low;
            }
            else if(IsRegTwoHigh)
            {
              RegOne->Value -= RegTwo->High;
            }
            else
            {
              RegOne->Value -= RegTwo->Value;
            }
          }

          SimulatorUpdateFlagResisters(SimulatorContext, ALU_SUB, RegOne->Value,
              PrevValue, RegTwo->Value);
        } break;

        case IMM_TO_RM:
        {
          u16 PrevValue = *RMPtr;
          *RMPtr -= DecoderContext->InstructionFields.Data;

          SimulatorUpdateFlagResisters(SimulatorContext, ALU_SUB, *RMPtr,
              PrevValue, DecoderContext->InstructionFields.Data);
        } break;

        case IMM_TO_REG:
        {
          reg_type* RegisterValueAsRegType = (reg_type*)RegisterPtr;
          u16 PrevValue = RegisterValueAsRegType->Value;
          if(IsRegLow)
          {
            RegisterValueAsRegType->Low -= DecoderContext->InstructionFields.Data;
          }
          else if(IsRegHigh)
          {
            RegisterValueAsRegType->High -= DecoderContext->InstructionFields.Data;
          }
          else
          {
            RegisterValueAsRegType->Value -= DecoderContext->InstructionFields.Data;
          }

          SimulatorUpdateFlagResisters(SimulatorContext, ALU_SUB,
              RegisterValueAsRegType->Value,
              PrevValue, DecoderContext->InstructionFields.Data);
        } break;
      }

    } break;

    case ALU_CMP:
    {

      u16 TempValue = 0;

      switch(RegOpState)
      {
        case REG_TO_RM:
        case RM_TO_REG:
        {
          reg_type* RegOne = 0;
          reg_type* RegTwo = 0;
          b32 IsRegOneLow  = 0;
          b32 IsRegOneHigh = 0;
          b32 IsRegTwoLow  = 0;
          b32 IsRegTwoHigh = 0;

          if(RegOpState == REG_TO_RM)
          {
            SimulatorContext->RMIndex = RegIndex;
            SimulatorContext->RegIndex = RMIndex;
            RegOne = (reg_type*)RMPtr;
            RegTwo = (reg_type*)RegisterPtr;
            IsRegOneLow  = IsRMLow;
            IsRegOneHigh = IsRMHigh;
            IsRegTwoLow  = IsRegLow;
            IsRegTwoHigh = IsRegHigh;
          }
          else
          {
            RegOne = (reg_type*)RegisterPtr;
            RegTwo = (reg_type*)RMPtr;
            IsRegOneLow  = IsRegLow;
            IsRegOneHigh = IsRegHigh;
            IsRegTwoLow  = IsRMLow;
            IsRegTwoHigh = IsRMHigh;
          }

          u16 PrevValue = RegOne->Value;
          if(IsRegOneLow)
          {
            if(IsRegTwoLow)
            {
              TempValue = RegOne->Low - RegTwo->Low;
            }
            else if(IsRegTwoHigh)
            {
              TempValue = RegOne->Low - RegTwo->High;
            }
            else
            {
              TempValue = RegOne->Low - RegTwo->Value & 0xFF;
            }
          }
          else if(IsRegOneHigh)
          {
            if(IsRegTwoLow)
            {
              TempValue = RegOne->High - RegTwo->Low;
            }
            else if(IsRegTwoHigh)
            {
              TempValue = RegOne->High - RegTwo->High;
            }
            else
            {
              TempValue = RegOne->High - (RegTwo->Value >> 4) & 0xFF;
            }
          }
          else
          {
            if(IsRegTwoLow)
            {
              TempValue = RegOne->Value - RegTwo->Low;
            }
            else if(IsRegTwoHigh)
            {
              TempValue = RegOne->Value - RegTwo->High;
            }
            else
            {
              TempValue = RegOne->Value - RegTwo->Value;
            }
          }

          SimulatorUpdateFlagResisters(SimulatorContext, ALU_CMP, TempValue,
              PrevValue, RegTwo->Value);
        } break;

        case IMM_TO_RM:
        {
          u16 PrevValue = *RMPtr;
          TempValue = *RMPtr - DecoderContext->InstructionFields.Data;

          SimulatorUpdateFlagResisters(SimulatorContext, ALU_SUB, TempValue,
              PrevValue, DecoderContext->InstructionFields.Data);
        } break;

        case IMM_TO_REG:
        {
          reg_type* RegisterValueAsRegType = (reg_type*)RegisterPtr;
          u16 PrevValue = RegisterValueAsRegType->Value;
          if(IsRegLow)
          {
            TempValue = RegisterValueAsRegType->Low - DecoderContext->InstructionFields.Data;
          }
          else if(IsRegHigh)
          {
            TempValue = RegisterValueAsRegType->High - DecoderContext->InstructionFields.Data;
          }
          else
          {
            TempValue = RegisterValueAsRegType->Value - DecoderContext->InstructionFields.Data;
          }
          SimulatorUpdateFlagResisters(SimulatorContext, ALU_SUB, TempValue,
             PrevValue, DecoderContext->InstructionFields.Data);

        } break;
      }

    } break;
  }


}

file_scope void
SimulatorSimulateSingleInstruction(decoder_context* DecoderContext, sim_cpu* SimulatorContext)
{
  SimulatorUpdatePC(DecoderContext, SimulatorContext);
  if(ECB_IsStringEqual(DecoderContext->InstructionFields.OpCodeMnemonic, "mov"))
  {
    SimulatorSimulateALUInstruction(DecoderContext, SimulatorContext, ALU_MOV);
  }
  else if(ECB_IsStringEqual(DecoderContext->InstructionFields.OpCodeMnemonic, "add"))
  {
    SimulatorSimulateALUInstruction(DecoderContext, SimulatorContext, ALU_ADD);
  }
  else if(ECB_IsStringEqual(DecoderContext->InstructionFields.OpCodeMnemonic, "sub"))
  {
    SimulatorSimulateALUInstruction(DecoderContext, SimulatorContext, ALU_SUB);
  }
  else if(ECB_IsStringEqual(DecoderContext->InstructionFields.OpCodeMnemonic, "cmp"))
  {
    SimulatorSimulateALUInstruction(DecoderContext, SimulatorContext, ALU_CMP);
  }
  else
  {
    SimulatorSimulateBranchInstruction(DecoderContext, SimulatorContext);
  }
}
