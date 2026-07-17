#include "linux_enhance.h"

#include "8086_decoder.c"

s32
main(s32 ArgCount, char **Args)
{
  ecb_file_result InputStream = LinuxReadEntireFile(TEST_FILE);

  char *InitialContent = "bits 16\n";
  ecb_string WriteStream =
    LinuxCreateString(InitialContent, ECB_GetStringLength(InitialContent), 512);

  DecoderReadByteStream(&WriteStream, InputStream.Content, InputStream.ContentSize);

  ecb_file_result WriteFile = {};
  WriteFile.Content = (u8*)(WriteStream.Content) - WriteStream.ContentSize;
  WriteFile.ContentSize = WriteStream.ContentSize;
  LinuxWriteEntireFile(&WriteFile, "dump.txt");
  LinuxFreeFile(&InputStream);

  return 0;
}
