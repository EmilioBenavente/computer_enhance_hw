#include "decoder.h"

file_scope void
DecoderAddCommentToEndOfInstruction(decoder_context* Context, char* String)
{
  Context->CommentPtr +=
    sprintf(Context->CommentPtr, "%s", String);
}

file_scope void
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

file_scope void
DecoderParseInstructionFields(decoder_context* Context, u8* InstructionPtr)
{
  u8 InstructionByte = *InstructionPtr;
  u32 ISAInstructionCount = ArraySize(SimISA);
  u32 DecoderFieldCount = ArraySize(SimISA[0].InstructionBits);

  instruction_fields InstructionData = {};

  for(u32 Iter = 0; Iter < ISAInstructionCount; Iter++)
  {
    u8 ShiftCount = 8 - SimISA[Iter].OpCode.BitCount;
    u8 TestByte = InstructionByte >> ShiftCount;

    if(TestByte == SimISA[Iter].OpCode.Value)
    {

      if(SimISA[Iter].OctalCode.IsExists)
      {
        u8 TestByteOctal = (*(InstructionPtr+1) >> 3) & 0x7;

        if(TestByteOctal != SimISA[Iter].OctalCode.Value)
        {
          Iter++;
          continue;
        }
      }

      //@SPEED(Emilio): A lot of this code was pushed in without without
      //  thought on performance, look at compacting the branching statements.
      InstructionData.IsExplicitSize = 1;

      if(Iter == 1 || Iter == 4 || Iter == 6 ||
        Iter == 8 || Iter == 11 || Iter == 14)
      {
        InstructionData.IsExplicitSize = 0;
      }

      if(Iter == 3 || Iter == 4 || Iter == 9 || Iter == 12 || Iter == 15)
      {
        InstructionData.Reg.IsExists  = 1;
        InstructionData.Reg.BitCount  = 3;
        InstructionData.Reg.ValueMask = 7;

        InstructionData.IsImpliedAccReg = 1;
      }

      if(Iter == 2 || Iter == 9 || Iter == 12 || Iter == 15)
      {
        InstructionData.IsImpliedAccReg = 1;
        InstructionData.IsImpliedImm = 1;
      }

      if(Iter >= 20 && Iter <= 39)
      {
        InstructionData.IsBranchInstruction = 1;
      }

      DecoderIncrementBitsBytesRead(Context, SimISA[Iter].OpCode.BitCount);

      InstructionData.OpCodeMnemonic = SimISA[Iter].OpCodeMnemonic;
      InstructionData.OpCode = SimISA[Iter].OpCode;

      if(Context->BytesRead)
      {
        InstructionPtr++;
        InstructionByte = *InstructionPtr;
      }

      u32 DecoderIter = F_DisplacementBit;
      while(DecoderIter < DecoderFieldCount)
      {
        if(SimISA[Iter].InstructionBits[DecoderIter].IsExists)
        {
          s32 PrevBytesRead = Context->BytesRead;
          InstructionData.InstructionBits[DecoderIter].IsExists = 1;
          InstructionData.InstructionBits[DecoderIter].BitCount =
            SimISA[Iter].InstructionBits[DecoderIter].BitCount;
          InstructionData.InstructionBits[DecoderIter].ValueMask =
            SimISA[Iter].InstructionBits[DecoderIter].ValueMask;


          u8 FieldOffset =
            (Context->BitsReadInByte +
            SimISA[Iter].InstructionBits[DecoderIter].BitCount);
          u8 DataPos = 8 - FieldOffset;
          u8 DataValue = (InstructionByte &
            (SimISA[Iter].InstructionBits[DecoderIter].ValueMask <<
            DataPos)) >> (DataPos);

          if(DecoderIter == F_DispLow)
          {
            if((InstructionData.Mod.Value == 1) ||
               (InstructionData.Mod.Value == 2) ||
               (InstructionData.Mod.Value == 0 &&
              InstructionData.RM.Value == 6))
            {
              //@INCOMPLETE(Emilio): Process Disp 16 bit Field
            }
            else
            {
              DecoderIter = F_DataLow;
              continue;
            }
          }


          if(DecoderIter == F_DispHigh)
          {
            if((InstructionData.Mod.Value == 2) ||
               (InstructionData.Mod.Value == 0 &&
              InstructionData.RM.Value == 6))
            {
              //@INCOMPLETE(Emilio): Process Disp 16 bit Field
            }
            else
            {
              DecoderIter = F_DataLow;
              continue;
            }
          }

          if(DecoderIter == F_DataHigh &&
            (InstructionData.IsWide.Value == 0 ||
            InstructionData.IsDestination.Value == 1))
          {
              DecoderIter++;
              continue;
          }

          InstructionData.InstructionBits[DecoderIter].Value = DataValue;

          DecoderIncrementBitsBytesRead(Context,
            SimISA[Iter].InstructionBits[DecoderIter].BitCount);

          if(Context->BytesRead > PrevBytesRead)
          {
            InstructionPtr++;
            InstructionByte = *InstructionPtr;
          }
        }

        DecoderIter++;
      }
      //@NOTE(Emilio): We found the correct instruction break out of the for loop!
      break;
    }
  }

  Context->InstructionFields = InstructionData;
}

file_scope void
DecoderParseInstructionFromFields(decoder_context* Context)
{
  instruction_fields* FieldData = &Context->InstructionFields;
  Context->InstructionWritePtr = Context->CurrentInstruction;

   if(FieldData->IsBranchInstruction)
  {
    s8 Value = FieldData->DataLow.Value;
    if(Value < 0)
    {
      Value = -Value;
      Context->InstructionWritePtr +=
        sprintf(Context->InstructionWritePtr, "%s, $-%hhd",
          FieldData->OpCodeMnemonic, Value);
    }
    else
    {
      Context->InstructionWritePtr +=
        sprintf(Context->InstructionWritePtr, "%s, $+%hhd",
          FieldData->OpCodeMnemonic, Value);
    }


    return;
  }

  char DataString[24];
  char* RMStringPtr = FieldData->RMString;
  sprintf(DataString, "");

  Context->InstructionWritePtr +=
    sprintf(Context->InstructionWritePtr, "%s ", FieldData->OpCodeMnemonic);


  FieldData->Displacement = (s8)FieldData->DispLow.Value;
  if(FieldData->DispHigh.Value ||
    (FieldData->Mod.Value == 0 && FieldData->RM.Value == 7) ||
    (FieldData->Mod.Value == 0x2))
  {
    FieldData->Displacement = (s16)(((u8)FieldData->DispHigh.Value << 8) |
                                    (u8)FieldData->Displacement);
  }

  FieldData->Data = (s8)FieldData->DataLow.Value;
  if(FieldData->DataHigh.Value &&
    ((FieldData->IsWide.Value && !FieldData->IsDestination.Value) ||
    (FieldData->IsWide.Value)))
  {
    FieldData->Data = (s16)(((u8)FieldData->DataHigh.Value << 8) |
                 (u8)FieldData->Data);
  }

  if(FieldData->Reg.IsExists)
  {
    sprintf(FieldData->RegString, "%s",
            RegTable[FieldData->Reg.Value + (8 * FieldData->IsWide.Value)]);
  }

  if(FieldData->Seg.IsExists)
  {
    sprintf(FieldData->RegString, "%s",
            SegTable[FieldData->Seg.Value]);
  }


  char PrefixString[5];
  sprintf(PrefixString, "");
  if(FieldData->DataLow.IsExists)
  {

    if(FieldData->IsExplicitSize == 0 && FieldData->Mod.Value != 3)
    {
      if(FieldData->IsWide.Value)
      {
        sprintf(PrefixString, "word ");
      }
      else
      {
        sprintf(PrefixString, "byte ");
      }
    }

    sprintf(DataString, "%d", FieldData->Data);
  }

  if(FieldData->RM.IsExists)
  {
    u8 WideOffest =
      ((FieldData->Mod.Value == 3 && FieldData->IsWide.Value) ||
       (FieldData->OpCode.Value == SimISA[5].OpCode.Value)) ? 8 : 0;

    if(FieldData->Mod.Value == 0 && FieldData->RM.Value == 6)
    {
      RMStringPtr +=
        sprintf(RMStringPtr, "[%d]", FieldData->Displacement);
    }
    else
    {
       RMStringPtr +=
        sprintf(RMStringPtr, "%s",
        RMTable[(FieldData->RM.Value + (8 * FieldData->Mod.Value)) +
                WideOffest]);

      if(FieldData->Mod.Value != 3)
      {
        if(FieldData->Displacement == 0)
        {
          RMStringPtr += sprintf(RMStringPtr, "]");
        }
        else if(FieldData->Displacement < 0)
        {
          RMStringPtr += sprintf(RMStringPtr, " - %d]", -FieldData->Displacement);
        }
        else
        {
          RMStringPtr += sprintf(RMStringPtr, " + %d]", FieldData->Displacement);
        }
      }
    }
  }

  b32 IsDestination = FieldData->IsDestination.Value;
  if(FieldData->DataLow.IsExists && FieldData->RM.IsExists)
  {
    sprintf(FieldData->RegString, "%s", DataString);
    IsDestination = 0;
  }
  else if(FieldData->DataLow.IsExists && FieldData->RM.IsExists == 0)
  {
    if(FieldData->IsImpliedImm)
    {
      sprintf(FieldData->RMString, "%s", DataString);
    }
    else
    {
      sprintf(FieldData->RMString, "[%s]", DataString);
    }

    IsDestination = 1;
  }

  if(FieldData->OpCode.Value == SimISA[5].OpCode.Value)
  {
    IsDestination = 1;
  }

  if(IsDestination)
  {
    //@IMPORTANT @INCOMPLETE(Emilio): This is hardcoded for now.
    FieldData->IsDestination.IsExists = 1;
    FieldData->IsDestination.Value = 1;
    FieldData->IsDestination.BitCount = 1;
    FieldData->IsDestination.ValueMask = 0x1;





    Context->InstructionWritePtr +=
      sprintf(Context->InstructionWritePtr, "%s%s, %s",
              PrefixString, FieldData->RegString, FieldData->RMString);
  }
  else
  {
    Context->InstructionWritePtr +=
      sprintf(Context->InstructionWritePtr, "%s%s, %s",
              PrefixString, FieldData->RMString, FieldData->RegString);
  }

}

file_scope void
DecoderDecodeInstruction(decoder_context* Context, void* MemoryChipPtr,
                         u32 MemoryAddress)
{
  ECB_ASSERT(MemoryAddress < MegaBytes(1));

  Context->CommentPtr = Context->Comments;
  DecoderParseInstructionFields(Context, ((u8*)MemoryChipPtr + MemoryAddress));
  DecoderParseInstructionFromFields(Context);

  char Comment[256];
  sprintf(Comment, "This is Instruction at Mem[0x%X]", MemoryAddress); DecoderAddCommentToEndOfInstruction(Context, Comment);
}
