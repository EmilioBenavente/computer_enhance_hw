#include "linux_enhance.h"

#include "8086_decoder.c"
#include "simulator.c"

s32
main(s32 ArgCount, char **Args)
{
  ecb_file_result InputStream = LinuxReadEntireFile(TEST_FILE);

  char *InitialContent = "bits 16\n";
  ecb_string WriteStream =
    LinuxCreateString(InitialContent, ECB_GetStringLength(InitialContent), MegaBytes(12));

  ecb_string RegisterStream =
    LinuxCreateString("", 0, MegaBytes(12));


  cpu_program ProgramA = {};
  ProgramA.ProgamSize = InputStream.ContentSize;

  SimulatorFlashProgram((u8*)InputStream.Content, InputStream.ContentSize, 0);

  u32 InstructionCount = 0;
  u32 PrintCount = 0;
  while(ProgramA.Registers.IP <
    (ProgramA.MemoryOffset + ProgramA.ProgamSize) % (MegaBytes(1) - 1))
  {
    PrintCount = SimulatorPrintRegisters(RegisterStream.Content, &ProgramA);
    RegisterStream.ContentSize += PrintCount;
    RegisterStream.Content += PrintCount;

    decoder_opcode OpCodeFields = DecoderReadSingleInstruction((InputStream.Content + ProgramA.Registers.IP),
                                                               ProgramA.Registers.IP, &InstructionCount);
    if(OpCodeFields.OpCodeStats.OpCodeString)
    {
      DecoderPrintSingleInstruction(&WriteStream, &OpCodeFields);

      ProgramA.Registers.IP += InstructionCount;
      ProgramA.Registers.IP = ProgramA.Registers.IP % (MegaBytes(1) - 1);
      SimulatorFillProgramWithDecoderFields(&ProgramA, &OpCodeFields);
      SimulatorSingleInstruction(&ProgramA);
    }
  }

  PrintCount = SimulatorPrintRegisters(RegisterStream.Content, &ProgramA);
  RegisterStream.ContentSize += PrintCount;
  RegisterStream.Content += PrintCount;


  ecb_file_result WriteFile = {};
  WriteFile.Content = (u8*)(WriteStream.Content) - WriteStream.ContentSize;
  WriteFile.ContentSize = WriteStream.ContentSize;
  LinuxWriteEntireFile(&WriteFile, "dump.asm");

  WriteFile.Content = (u8*)(RegisterStream.Content) - RegisterStream.ContentSize;
  WriteFile.ContentSize = RegisterStream.ContentSize;
  LinuxWriteEntireFile(&WriteFile, "reg_dump.txt");

  LinuxFreeFile(&InputStream);

  return 0;
}
