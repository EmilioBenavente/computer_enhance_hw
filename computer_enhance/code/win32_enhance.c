#include "win32_enhance.h"

#include "8086_decoder.c"

s32 WINAPI
wWinMain(HINSTANCE Instance, HINSTANCE PrevInstance, PWSTR CmdLine, s32 CmdShow)
{
  ecb_file_result InputStream = Win32ReadEntireFile(TEST_FILE);
  DecoderReadByteStream(0, (char*)InputStream.Content);

  Win32WriteEntireFile(&InputStream, "dump.txt");
  Win32FreeFile(&InputStream);

  return 0;
}
