#include "win32_enhance.h"

#include "8086_decoder.c"

s32 WINAPI
wWinMain(HINSTANCE Instance, HINSTANCE PrevInstance, PWSTR CmdLine, s32 CmdShow)
{

  DecoderReadByteStream();


  //@NOTE(Emilio): The only thing we will need at the start is MessageBox
  MessageBoxExA(0, "Hello World", 0, 0, 0);

  return 0;
}
