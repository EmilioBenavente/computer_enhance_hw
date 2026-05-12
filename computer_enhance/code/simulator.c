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
