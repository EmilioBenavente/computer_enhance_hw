#include "8086_decoder.h"

file_scope void
DecoderGetFieldValueAndUpdateBitCount(decoder_field *Field, decoder_stream_pointer *StreamPtr)
{
  if(((Field->StateFlags & FIELD_IS_PADDING) == FIELD_IS_PADDING) ||
    ((Field->StateFlags  & FIELD_IS_IMMPLIED) == FIELD_IS_IMMPLIED))
  {
    StreamPtr->BitCount += Field->FieldBitCount;
  }
  else
  {
    u32 ShiftValue = 8 - (StreamPtr->BitCount + Field->FieldBitCount);
    if((Field->StateFlags & FIELD_EXISTS) == FIELD_EXISTS)
    {
      Field->FieldValue = (*StreamPtr->Pointer >> ShiftValue) & Field->FieldMask;
      StreamPtr->BitCount += Field->FieldBitCount;
    }
  }

  if(StreamPtr->BitCount > 7)
  {
    StreamPtr->BitCount -= 8;
    StreamPtr->Pointer++;
  }
}

file_scope void
DecoderExtractValuesFromField(decoder_opcode *Fields, decoder_stream_pointer *StreamPtr)
{
  StreamPtr->BitCount += Fields->OpCodeStats.BitCount;


  if((Fields->OpCodeStats.StateFlags & OPCODE_IS_16BIT) == OPCODE_IS_16BIT)
  {
    StreamPtr->Pointer += 2;
    StreamPtr->BitCount = 0;
  }
  else if((Fields->OpCodeStats.StateFlags & OPCODE_HAS_ZERO_FIELDS) == OPCODE_HAS_ZERO_FIELDS)
  {
    StreamPtr->Pointer++;
    StreamPtr->BitCount = 0;
  }
  else
  {
    DecoderGetFieldValueAndUpdateBitCount(&Fields->DestinationFlag, StreamPtr);
    DecoderGetFieldValueAndUpdateBitCount(&Fields->SignExtendFlag, StreamPtr);
    DecoderGetFieldValueAndUpdateBitCount(&Fields->RotateCLFlag, StreamPtr);
    DecoderGetFieldValueAndUpdateBitCount(&Fields->ZeroFlag, StreamPtr);
    DecoderGetFieldValueAndUpdateBitCount(&Fields->WideFlag, StreamPtr);
    DecoderGetFieldValueAndUpdateBitCount(&Fields->Mod, StreamPtr);
    DecoderGetFieldValueAndUpdateBitCount(&Fields->Reg, StreamPtr);
    DecoderGetFieldValueAndUpdateBitCount(&Fields->SegmentReg, StreamPtr);
    DecoderGetFieldValueAndUpdateBitCount(&Fields->RM, StreamPtr);

    b32 IsDisplacementExist = (((Fields->RM.StateFlags & FIELD_EXISTS) == FIELD_EXISTS) &&
                               (Fields->Mod.FieldValue == 1 || Fields->Mod.FieldValue == 2 ||
                                (Fields->Mod.FieldValue == 0 && Fields->RM.FieldValue == RM_16BIT_IMM_CASE)) ||
                                ((Fields->Displacement.StateFlags & FIELD_EXISTS) == FIELD_EXISTS));
    if(IsDisplacementExist)
    {
      Fields->Displacement.StateFlags |= FIELD_EXISTS;
      Fields->Displacement.FieldValue = (s32)((s8)*StreamPtr->Pointer);
      StreamPtr->Pointer++;

      if((Fields->Mod.FieldValue == 0x2)                      ||
        (Fields->Mod.StateFlags == FIELD_DOES_NOT_EXIST)      ||
        (Fields->Mod.FieldValue == 0x0 && Fields->RM.FieldValue == RM_16BIT_IMM_CASE))
      {
        s32 OriginalValue = (Fields->Displacement.FieldValue & 0xFF);
        Fields->Displacement.FieldValue &= 0xFF;
        Fields->Displacement.FieldValue |= (*StreamPtr->Pointer << 8);
        StreamPtr->Pointer++;
      }
    }
  }

  if((Fields->Data.StateFlags & FIELD_EXISTS) == FIELD_EXISTS)
  {
    if((Fields->Data.StateFlags & FIELD_IS_UNSIGNED_DATA) == FIELD_IS_UNSIGNED_DATA)
    {
      Fields->Data.FieldValue = (u32)((u8)*StreamPtr->Pointer);
    }
    else
    {
      Fields->Data.FieldValue = (s32)((s8)*StreamPtr->Pointer);
    }
    StreamPtr->Pointer++;

    if(Fields->WideFlag.FieldValue &&
      ((Fields->Data.StateFlags & FIELD_IS_PORT_DATA) != FIELD_IS_PORT_DATA) ||
      ((Fields->Data.StateFlags & FIELD_IS_EXPLICITLY_16BITS) == FIELD_IS_EXPLICITLY_16BITS))
    {
      if((Fields->SignExtendFlag.StateFlags == FIELD_DOES_NOT_EXIST)      ||
        ((Fields->SignExtendFlag.StateFlags & FIELD_EXISTS == FIELD_EXISTS) && (Fields->SignExtendFlag.FieldValue == 0)))
      {
        s32 OriginalValue = (Fields->Data.FieldValue & 0xFF);
        Fields->Data.FieldValue &= 0xFF;
        Fields->Data.FieldValue |= (*StreamPtr->Pointer << 8);
        StreamPtr->Pointer++;
      }
    }
  }
}

file_scope decoder_opcode
DecoderGetOpCodeFromStream(u8 *InputStream)
{
  ECB_ASSERT(OpCodes8086Table[OP_CODE_TABLE_8086_SIZE - 1].OpCodeStats.OpCode);

  decoder_opcode Result = {};

  s32 OpCodeIndex = 0;
  while(OpCodeIndex < OP_CODE_TABLE_8086_SIZE)
  {
    decoder_opcode *TableOpCodePtr = &OpCodes8086Table[OpCodeIndex];
    char TestOpCode = *InputStream;

    decoder_opcode_stats TableOpCode = TableOpCodePtr->OpCodeStats;
    s32 TestOpCodeValue = (TestOpCode >> TableOpCode.Shift) & TableOpCode.Mask;
    if((TableOpCode.StateFlags & OPCODE_IS_16BIT) == OPCODE_IS_16BIT)
    {
      TestOpCodeValue = ((*InputStream) | ((*(InputStream+1)) << 8));
    }

    b32 TestOpCodeResult =
      TestOpCodeValue == TableOpCode.OpCode;

    if(TestOpCodeResult)
    {
      char OpCodeExtended = *(InputStream+1);

      if((TableOpCodePtr->Reg.StateFlags & FIELD_IS_OPCODE_EXTENDED))
      {
        s32 ShiftValue = 3;

        decoder_field TableOpField = TableOpCodePtr->Reg;
        if(TableOpField.FieldBitCount == 8)
        {
          ShiftValue = 0;
        }

        OpCodeExtended = (OpCodeExtended >> ShiftValue);
        OpCodeExtended = OpCodeExtended & TableOpField.FieldMask;

        TestOpCodeResult =
          (OpCodeExtended == TableOpField.FieldValue);
      }
      else if((TableOpCodePtr->RM.StateFlags & FIELD_IS_OPCODE_EXTENDED))
      {
        ECB_ASSERT(TableOpCodePtr->SegmentReg.StateFlags & FIELD_EXISTS);

        OpCodeExtended = *InputStream;

        decoder_field TableOpField = TableOpCodePtr->RM;

        OpCodeExtended = OpCodeExtended & TableOpField.FieldMask;

        TestOpCodeResult =
          (OpCodeExtended == TableOpField.FieldValue);
      }
    }

    if(TestOpCodeResult)
    {
      if(TableOpCodePtr->OpCodeStats.OpCode == 0x21)
      {
        printf("Hello World\n");
      }

      Result.OpCodeStats      = TableOpCodePtr->OpCodeStats;
      Result.DestinationFlag  = TableOpCodePtr->DestinationFlag;
      Result.SignExtendFlag   = TableOpCodePtr->SignExtendFlag;
      Result.RotateCLFlag     = TableOpCodePtr->RotateCLFlag;
      Result.ZeroFlag         = TableOpCodePtr->ZeroFlag;
      Result.WideFlag         = TableOpCodePtr->WideFlag;
      Result.Mod              = TableOpCodePtr->Mod;
      Result.Reg              = TableOpCodePtr->Reg;
      Result.RM               = TableOpCodePtr->RM;
      Result.SegmentReg       = TableOpCodePtr->SegmentReg;
      Result.Displacement     = TableOpCodePtr->Displacement;
      Result.Data             = TableOpCodePtr->Data;
      break;
    }

    OpCodeIndex++;
  }

  return Result;
}

//@NOTE(Emilio): Returns the number bytes printed.
file_scope u32
DecoderPrintInstructionFromFields(char *WritePtr, decoder_opcode *Fields)
{
  u32 Result = 0;

  char PrintBuffer[TEMP_PRINT_BUFFER_SIZE];
  char* PrintPtr = PrintBuffer;
  PrintPtr += sprintf(PrintPtr, "%s", Fields->OpCodeStats.OpCodeString);

  b32 IsWide = Fields->WideFlag.FieldValue;
  b32 IsCallOp = ECB_IsStringEqual(Fields->OpCodeStats.OpCodeString, "call");

  if((Fields->ZeroFlag.StateFlags & FIELD_EXISTS) == FIELD_EXISTS)
  {
    char* ImplicitSize = Fields->SecondOpCode->WideFlag.FieldValue ? "w" :"b";
    PrintPtr += sprintf(PrintPtr, " %s%s", Fields->SecondOpCode->OpCodeStats.OpCodeString, ImplicitSize);
  }
  else if(Fields->OpCodeStats.OpCode == 0xF0)
  {
    PrintPtr += sprintf(PrintPtr, " %s", Fields->SecondOpCode->OpCodeStats.OpCodeString);
  }
  else if((Fields->OpCodeStats.StateFlags & OPCODE_ONLY_PRINT_OP) != OPCODE_ONLY_PRINT_OP)
  {
    PrintPtr += sprintf(PrintPtr, " ");

    char* RegString = 0;
    if((Fields->Reg.StateFlags & FIELD_EXISTS) == FIELD_EXISTS)
    {
      u32 RegIndex = Fields->Reg.FieldValue + (8*IsWide);
      RegString = RegTable[RegIndex];
    }
    else if((Fields->SegmentReg.StateFlags & FIELD_EXISTS) == FIELD_EXISTS)
    {
      u32 SegIndex = Fields->SegmentReg.FieldValue;
      RegString = SegTable[SegIndex];
    }


    char RMString[64] = {0};
    if(((Fields->RM.StateFlags & FIELD_EXISTS) == FIELD_EXISTS) ||
       ((Fields->Displacement.StateFlags & FIELD_EXISTS) == FIELD_EXISTS))
    {
      u32 RMIndex = Fields->RM.FieldValue + (8*Fields->Mod.FieldValue);
      if(Fields->Mod.StateFlags == FIELD_DOES_NOT_EXIST)
      {
        s16 DisplacementValue = (s16)Fields->Displacement.FieldValue;
        if(DisplacementValue > 0)
        {
          sprintf(RMString, "[%d]", DisplacementValue);
        }
        else
        {
          sprintf(RMString, "[-%d]", (DisplacementValue * -1));
        }
      }
      else if(Fields->Mod.FieldValue == 0x3)
      {
        if((Fields->WideFlag.StateFlags & FIELD_EXISTS) == FIELD_EXISTS)
        {
          RMIndex += (8*IsWide);
        }
        else
        {
          RMIndex = (32+Fields->RM.FieldValue);
        }

        sprintf(RMString, "%s", RMTable[RMIndex]);
      }
      else if(IsCallOp)
      {
        sprintf(RMString, "%s", RMTable[RMIndex]);

        if((Fields->Displacement.StateFlags & FIELD_EXISTS) == FIELD_EXISTS)
        {
          s16 DisplacementValue = Fields->Displacement.FieldValue;

          if((Fields->Mod.FieldValue == 0x0) &&
            (Fields->RM.FieldValue == RM_16BIT_IMM_CASE))
          {
            sprintf(RMString, "[%d]", (u16)DisplacementValue);
          }
          else
          {
            if(DisplacementValue == 0)
            {
              sprintf(RMString, "[%s]", RMTable[RMIndex]);
            }
            else if(DisplacementValue > 0)
            {
              sprintf(RMString, "[%s + %d]", RMTable[RMIndex], DisplacementValue);
            }
            else
            {
              sprintf(RMString, "[%s - %d]", RMTable[RMIndex], (DisplacementValue * -1));
            }
          }
        }
      }
      else
      {
        sprintf(RMString, "[%s]", RMTable[RMIndex]);

        if((Fields->Displacement.StateFlags & FIELD_EXISTS) == FIELD_EXISTS)
        {
          s16 DisplacementValue = (s16)Fields->Displacement.FieldValue;
          if((Fields->Mod.FieldValue == 0x0) &&
            (Fields->RM.FieldValue == RM_16BIT_IMM_CASE))
          {
            if(DisplacementValue > 0)
            {
              sprintf(RMString, "[%d]", DisplacementValue);
            }
            else
            {
              sprintf(RMString, "[-%d]", (DisplacementValue * -1));
            }
          }
          else
          {
            if(DisplacementValue == 0)
            {
              sprintf(RMString, "[%s]", RMTable[RMIndex]);
            }
            else if(DisplacementValue > 0)
            {
              sprintf(RMString, "[%s + %d]", RMTable[RMIndex], DisplacementValue);
            }
            else
            {
              sprintf(RMString, "[%s - %d]", RMTable[RMIndex], (DisplacementValue * -1));
            }
          }
        }
      }
    }

    if((Fields->Data.StateFlags & FIELD_EXISTS) == FIELD_EXISTS)
    {
      s16 DataValue = (s16)Fields->Data.FieldValue;
      if((Fields->Data.StateFlags & FIELD_IS_JMP_DATA) == FIELD_IS_JMP_DATA)
      {
        DataValue += 2;
        if(DataValue >= 0)
        {
          PrintPtr += sprintf(PrintPtr, "$+%d", DataValue);
        }
        else
        {
          PrintPtr += sprintf(PrintPtr, "$-%d", (DataValue * -1));
        }
      }
      else if((Fields->Data.StateFlags & FIELD_IS_PORT_DATA) == FIELD_IS_PORT_DATA)
      {
        //@TODO @NOTE(Emilio): At this point we probably can refactor this whole section
        //  by overwritting the RegString and RMString
        DataValue = (u8)Fields->Data.FieldValue;
        if(Fields->DestinationFlag.FieldValue)
        {
          PrintPtr += sprintf(PrintPtr, "%d, %s", DataValue, RegString);
        }
        else
        {
          PrintPtr += sprintf(PrintPtr, "%s, %d", RegString, DataValue);
        }
      }
      else if((Fields->Reg.StateFlags & FIELD_EXISTS) == FIELD_EXISTS)
      {
        if(DataValue > 0)
        {
          PrintPtr += sprintf(PrintPtr, "%s, %d", RegString, DataValue);
        }
        else
        {
          PrintPtr += sprintf(PrintPtr, "%s, -%d", RegString, (DataValue * -1));
        }
      }
      else if((Fields->SignExtendFlag.StateFlags & FIELD_EXISTS) == FIELD_EXISTS)
      {
        if((Fields->Mod.FieldValue != 0x3))
        {
          char* ImplicitSize = IsWide ? "word" :"byte";
          PrintPtr += sprintf(PrintPtr, "%s ", ImplicitSize);
        }

        if(DataValue > 0)
        {
          PrintPtr += sprintf(PrintPtr, "%s, %d", RMString, DataValue);
        }
        else
        {
          PrintPtr += sprintf(PrintPtr, "%s, -%d", RMString, (DataValue * -1));
        }
      }
      else if((Fields->Data.StateFlags & FIELD_IS_LOGIC_DATA) == FIELD_IS_LOGIC_DATA)
      {
        char* ImplicitSize = IsWide ? "word" :"byte";
        if(DataValue > 0)
        {
          PrintPtr += sprintf(PrintPtr, "%s %s, %d", ImplicitSize, RMString, DataValue);
        }
        else
        {
          PrintPtr += sprintf(PrintPtr, "%s %s, -%d", ImplicitSize, RMString, (DataValue * -1));
        }
      }
      else if((Fields->Data.StateFlags & FIELD_IS_UNSIGNED_TEST_DATA) == FIELD_IS_UNSIGNED_TEST_DATA)
      {
        if((Fields->Mod.FieldValue != 0x3))
        {
          char* ImplicitSize = IsWide ? "word" :"byte";
          PrintPtr += sprintf(PrintPtr, "%s ", ImplicitSize);
        }

        if(DataValue > 0)
        {
          PrintPtr += sprintf(PrintPtr, "%s, %d", RMString, DataValue);
        }
        else
        {
          PrintPtr += sprintf(PrintPtr, "%s, -%d", RMString, (DataValue * -1));
        }
      }
      else if(Fields->WideFlag.StateFlags == FIELD_DOES_NOT_EXIST)
      {
        if(DataValue > 0)
        {
          PrintPtr += sprintf(PrintPtr, "%d", DataValue);
        }
        else
        {
          PrintPtr += sprintf(PrintPtr, "-%d", (DataValue * -1));
        }
      }
      else
      {
        char* ImplicitSize = IsWide ? "word" :"byte";
        if(DataValue > 0)
        {
          PrintPtr += sprintf(PrintPtr, "%s, %s %d", RMString, ImplicitSize, DataValue);
        }
        else
        {
          PrintPtr += sprintf(PrintPtr, "%s, %s -%d", RMString, ImplicitSize, (DataValue * -1));
        }
      }
    }
    //@HARDCODE(Emilio): PUSH/POP need "word", find better solution later
    else if((Fields->RotateCLFlag.StateFlags & FIELD_EXISTS) == FIELD_EXISTS)
    {
      if(Fields->Mod.FieldValue == 0x3)
      {
        if(Fields->RotateCLFlag.FieldValue)
        {
          PrintPtr += sprintf(PrintPtr, "%s, cl", RMString);
        }
        else
        {
          PrintPtr += sprintf(PrintPtr, "%s, 1", RMString);
        }
      }
      else
      {
        char* ImplicitSize = IsWide ? "word" :"byte";
        if(Fields->RotateCLFlag.FieldValue)
        {
          PrintPtr += sprintf(PrintPtr, "%s %s, cl", ImplicitSize, RMString);
        }
        else
        {
          PrintPtr += sprintf(PrintPtr, "%s %s, 1", ImplicitSize, RMString);
        }
      }
    }
    //@HARDCODE(Emilio): PUSH/POP need "word", find better solution later
    else if((Fields->OpCodeStats.OpCode == 0xFF && Fields->Reg.FieldValue == 0x6) ||
            (Fields->OpCodeStats.OpCode == 0x8F))
    {
      PrintPtr += sprintf(PrintPtr, "word %s", RMString);
    }
    //@HARDCODE(Emilio): These seem to be the only instructions that don't obey the table rules.
    else if((Fields->OpCodeStats.OpCode == 0x76) ||
            (Fields->OpCodeStats.OpCode == 0x77))
    {
      if(Fields->DestinationFlag.FieldValue)
      {
        PrintPtr += sprintf(PrintPtr, "dx, %s", RegString);
      }
      else
      {
        PrintPtr += sprintf(PrintPtr, "%s, dx", RegString);
      }
    }
    else if(((Fields->RM.StateFlags & FIELD_EXISTS) == FIELD_DOES_NOT_EXIST) &&
            ((Fields->Displacement.StateFlags & FIELD_IS_IMMPLIED) != FIELD_IS_IMMPLIED))
    {
      PrintPtr += sprintf(PrintPtr, "%s", RegString);
    }
    //@TODO @SPEED(Emilio): We need to clean up this if nesting pretty soon.
    //@NOTE(Emilio): This one is for the call functions.
    else if(IsCallOp)
    {
      PrintPtr += sprintf(PrintPtr, "%s", RMString);
    }
    else if((Fields->Reg.StateFlags & FIELD_EXISTS) == FIELD_DOES_NOT_EXIST)
    {
      if((Fields->Mod.FieldValue == 0x3) ||
        (Fields->WideFlag.StateFlags == FIELD_DOES_NOT_EXIST))
      {
        PrintPtr += sprintf(PrintPtr, "%s", RMString);
      }
      else
      {
        char* ImplicitSize = IsWide ? "word" :"byte";
        PrintPtr += sprintf(PrintPtr, "%s %s", ImplicitSize, RMString);
      }
    }
    else
    {
    //@NOTE(Emilio): The basic case allows us to switch REG and RM field
    //  based on DestinationFlag, but when both registers aren't present
    //  we need to force one of the registers to act like the other
    //  making this scheme useless.
      if(Fields->DestinationFlag.FieldValue)
      {
        PrintPtr += sprintf(PrintPtr, "%s, %s", RegString, RMString);
      }
      else
      {
        PrintPtr += sprintf(PrintPtr, "%s, %s", RMString, RegString);
      }
    }
  }

  if(WritePtr)
  {
    //@HARDCODE(Emilio): We should make functions for new line and comments
    Result = sprintf(WritePtr, "%s\n", PrintBuffer);
  }
  else
  {
    //@INCOMPLETE @TODO(Emilio): printf is not a cross compatible function, thank you windows..
  }

  return Result;
}

file_scope void
DecoderPrintSingleInstruction(ecb_string *WriteBuffer, decoder_opcode *Fields)
{
  u32 BytesRead =
    DecoderPrintInstructionFromFields(WriteBuffer->Content, Fields);

  WriteBuffer->ContentSize += BytesRead;
  WriteBuffer->Content = (u8 *)WriteBuffer->Content + BytesRead;

}

//@NOTE(Emilio): Single instruction, also implying that for now, you must
//  call this function twice to complete an instruction with two sets of
//  op code fields like the rep instruction.
file_scope decoder_opcode
DecoderReadSingleInstruction(u8 *InputStream, u32 *ReturnNewInstructionOffset)
{
  ECB_ASSERT(InputStream);

  decoder_opcode Result = {};

  Result = DecoderGetOpCodeFromStream(InputStream);

  if(Result.OpCodeStats.OpCodeString)
  {
    decoder_stream_pointer StreamPtr = {};
    StreamPtr.Pointer  = (u8*)InputStream;
    StreamPtr.BitCount = 0;

    DecoderExtractValuesFromField(&Result, &StreamPtr);

    if(ReturnNewInstructionOffset)
    {
      *ReturnNewInstructionOffset = StreamPtr.Pointer - InputStream;
    }
  }

  return Result;
}

file_scope void
DecoderReadByteStream(ecb_string *WriteBuffer, u8 *InputStream, u32 StreamSize)
{
  ECB_ASSERT(InputStream);

  char InstructionByte = 0;
  u32 InstructionCount = 0;
  //@NOTE(Emilio): We are currently debugging logical shift instructions
  u32 DEBUGCount = 0;
  while(StreamSize)
  {
    if(DEBUGCount == 303)
    {
      char* DEBUGString = (WriteBuffer->Content - 50);
      printf("hello world \n");
    }
    decoder_opcode OpCodeFields = DecoderReadSingleInstruction(InputStream, &InstructionCount);
    decoder_opcode SecondOpCodeFields = {};

    if(((OpCodeFields.ZeroFlag.StateFlags & FIELD_EXISTS) == FIELD_EXISTS) ||
      (OpCodeFields.OpCodeStats.OpCode == 0xF0))
    {
      u32 TempCount = InstructionCount;
      SecondOpCodeFields = DecoderReadSingleInstruction((InputStream+TempCount), &InstructionCount);
      OpCodeFields.SecondOpCode = &SecondOpCodeFields;
      InstructionCount += TempCount;
    }


    if(OpCodeFields.OpCodeStats.OpCodeString)
    {
      DecoderPrintSingleInstruction(WriteBuffer, &OpCodeFields);

      InputStream += InstructionCount;
      StreamSize -= InstructionCount;
    }
    else
    {
      return;
    }
    DEBUGCount++;
  }
}

