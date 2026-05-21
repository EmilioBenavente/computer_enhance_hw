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
SimulatorSimulateMov(decoder_context* DecoderContext, sim_cpu* SimulatorContext)
{
  u16* RegisterPtr = (u16*)&SimulatorContext->A;

  if(DecoderContext->InstructionFields.DataLow.IsExists)
  {
    if(DecoderContext->InstructionFields.Reg.IsExists)
    {
      u8 RegIndex = DecoderContext->InstructionFields.Reg.Value;
      if(DecoderContext->InstructionFields.IsWide.Value)
      {
        RegisterPtr += RegIndex;

        if(RegIndex < 4)
        {
          reg_type* RegisterValueAsRegType = (reg_type*)RegisterPtr;
          RegisterValueAsRegType->Value =
            DecoderContext->InstructionFields.Data;
        }
        else
        {
          *RegisterPtr = DecoderContext->InstructionFields.Data;
        }
      }
      else
      {
        RegisterPtr += (RegIndex % 4);
        reg_type* RegisterValueAsRegType = (reg_type*)RegisterPtr;
        if(RegIndex > 3)
        {
          RegisterValueAsRegType->Low =
            DecoderContext->InstructionFields.Data & 0xF;
        }
        else
        {
          RegisterValueAsRegType->High =
            (DecoderContext->InstructionFields.Data >> 8) & 0xF;
        }
      }
    }
  }

}

file_scope void
SimulatorSimulateSingleInstruction(decoder_context* DecoderContext, sim_cpu* SimulatorContext)
{
  SimulatorUpdatePC(DecoderContext, SimulatorContext);
  if(ECB_IsStringEqual(DecoderContext->InstructionFields.OpCodeMnemonic, "mov"))
  {
    SimulatorSimulateMov(DecoderContext, SimulatorContext);
  }



}
