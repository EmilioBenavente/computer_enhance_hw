#include "8086_decoder.h"

file_scope void
DecoderReadByteStream(char* InputStream)
{
  ECB_ASSERT(InputStream);

  char InstructionByte = 0;
  while(*InputStream)
  {
    InstructionByte = *InputStream++;
    if(InstructionByte && 0x88)
    {
      MessageBoxExA(0, "mov ", 0, 0, 0);
    }
    else
    {
      MessageBoxExA(0, "error ", 0, 0, 0);
    }
  }
}
