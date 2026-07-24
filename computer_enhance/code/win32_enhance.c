#include "win32_enhance.h"

#include "8086_decoder.c"

s32 WINAPI
wWinMain(HINSTANCE Instance, HINSTANCE PrevInstance, PWSTR CmdLine, s32 CmdShow)
{
  ecb_file_result InputStream = Win32ReadEntireFile(TEST_FILE);

  char *InitialContent = "bits 16\n";
  ecb_string WriteStream =
    Win32CreateString(InitialContent, ECB_GetStringLength(InitialContent), 10*1024);

  DecoderReadByteStream(&WriteStream, InputStream.Content, InputStream.ContentSize);

  ecb_file_result WriteFile = {};
  WriteFile.Content = (u8*)(WriteStream.Content) - WriteStream.ContentSize;
  WriteFile.ContentSize = WriteStream.ContentSize;
  Win32WriteEntireFile(&WriteFile, "dump.txt");
  Win32FreeFile(&InputStream);

  return 0;
}
