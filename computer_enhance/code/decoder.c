#include "decoder.h"

DecoderIncrementBitsBytesRead(decoder_context* Context, u32 BitsRead)
{
  Context->BitsReadInByte += BitsRead;
  Context->BitPosition += BitsRead;

  if(Context->BitsReadInByte > 7)
  {
    Context->BytesRead++;
    Context->BitsReadInByte -= 8;
  }
}

DecoderDecodeInstruction(decoder_context* Context, void* MemoryChipPtr,
                         u32 MemoryAddress)
{
  ECB_ASSERT(MemoryAddress < MegaBytes(1));

  u8 InstructionByte = *((u8*)MemoryChipPtr + MemoryAddress);
  u32 ISAInstructionCount = ArraySize(SimISA);
  u32 DecoderFieldCount = ArraySize(SimISA[0].InstructionBits);

  instruction_type InstructionData = {};

  for(u32 Iter = 0; Iter < ISAInstructionCount; Iter++)
  {
    u8 ShiftCount = 8 - SimISA[Iter].OpCode.BitCount;
    u8 TestByte = InstructionByte >> ShiftCount;

    if(TestByte == SimISA[Iter].OpCode.Bits)
    {
      if(SimISA[Iter].OctalCode.IsExists)
      {
        //@INCOMPLETE(Emilio): Not yet considering OctalCode
      }
      else
      {
        DecoderIncrementBitsBytesRead(Context, SimISA[Iter].OpCode.BitCount);

        InstructionData.OpCode = SimISA[Iter].OpCode;

        for(u32 DecoderIter = 1; DecoderIter < DecoderFieldCount; DecoderIter++)
        {
          if(SimISA[Iter].InstructionBits[DecoderIter].IsExists)
          {
            s32 PrevBytesRead = Context->BytesRead;
            InstructionData.InstructionBits[DecoderIter].IsExists = 1;
            InstructionData.InstructionBits[DecoderIter].BitCount =
              SimISA[Iter].InstructionBits[DecoderIter].BitCount;

            u8 DataPos = 8 - Context->BitPosition - 1;
            u8 DataValue = (InstructionByte & (1 << DataPos)) >> (DataPos);
            InstructionData.InstructionBits[DecoderIter].Bits = DataValue;

            DecoderIncrementBitsBytesRead(Context,
              SimISA[Iter].InstructionBits[DecoderIter].BitCount);

            //@INCOMPLETE(Emilio): We strattle pass the memory we should be reading.
            //  due to not checking if DISP/DATA are allowed via R/M
            if(Context->BytesRead > PrevBytesRead)
            {
              MemoryAddress++;
              InstructionByte = *((u8*)MemoryChipPtr + MemoryAddress);
            }
          }
        }
      }
      break;
    }
  }

  //////////////
  // M     B
  // P     P
  // 011101010
  //////////////
  //  8 - BP - 1 => 2
  //  BP & (1 << 2)
  //  BP >> (2 - 1)
  //////////////


}

//@TODO(Emilio): Print Instruction Function.


