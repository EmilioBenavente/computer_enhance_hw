#include "decoder.h"

DecoderDecodeInstruction(decoder_context* Context, void* MemoryChipPtr,
                         u32 MemoryAddress)
{
  ECB_ASSERT(MemoryAddress < MegaBytes(1));

  u8 InstructionByte = *((u8*)MemoryChipPtr + MemoryAddress);
  u32 ISAInstructionCount = ArraySize(SimISA);

  for(u32 Iter = 0; Iter < ISAInstructionCount; Iter++)
  {
    u8 ShiftCount = 8 - SimISA[Iter].OpCode.BitCount;
    u8 TestByte = InstructionByte >> ShiftCount;

    //@INCOMPLETE(Emilio): This will collide with the OctalCode ISA Instructions
    if(TestByte == SimISA[Iter].OpCode.Bits)
    {
      Iter++;
      break;
    }
  }


}

//@TODO(Emilio): Print Instruction Function.


