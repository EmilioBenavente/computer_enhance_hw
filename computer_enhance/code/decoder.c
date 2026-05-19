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
      if(Iter == 3 || Iter == 4)
      {
        InstructionData.Reg.IsExists  = 1;
        InstructionData.Reg.BitCount  = 3;
        InstructionData.Reg.ValueMask = 7;
      }

      if(Iter == 4)
      {
        InstructionData.IsDestination.IsExists  = 1;
        InstructionData.IsDestination.BitCount  = 1;
        InstructionData.IsDestination.ValueMask = 0x1;
        InstructionData.IsDestination.Value = 0x1;
      }


      if(0 && SimISA[Iter].OctalCode.IsExists)
      {
        //@INCOMPLETE(Emilio): Not yet considering OctalCode
      }
      else
      {
        DecoderIncrementBitsBytesRead(Context, SimISA[Iter].OpCode.BitCount);

        InstructionData.OpCodeMnemonic = SimISA[Iter].OpCodeMnemonic;
        InstructionData.OpCode = SimISA[Iter].OpCode;

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

            if(DecoderIter == F_RM)
            {
              if((InstructionData.Mod.Value != 3) ||
                 (InstructionData.Mod.Value == 0 &&
                InstructionData.RM.Value == 6))
              {
                //@INCOMPLETE(Emilio): Process Disp at least 8 bit Field
              }
            }

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
      }
      break;
    }
  }

  Context->InstructionFields = InstructionData;
}

file_scope void
DecoderParseInstructionFromFields(decoder_context* Context)
{
  instruction_fields* FieldData = &Context->InstructionFields;
  char DataString[24];
  char* RMStringPtr = FieldData->RMString;

  Context->InstructionWritePtr = Context->CurrentInstruction;
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

  if(FieldData->DataLow.IsExists)
  {
    char Prefix[5];

    if(!FieldData->Reg.IsExists && FieldData->DispLow.Value)
    {
      if(FieldData->IsWide.Value)
      {
        sprintf(Prefix, "word ");
      }
      else
      {
        sprintf(Prefix, "byte ");
      }
    }
    else
    {
      sprintf(Prefix, "");
    }

    sprintf(DataString, "%s%d", Prefix, FieldData->Data);
  }

  if(FieldData->RM.IsExists)
  {
    if(FieldData->Mod.Value == 0 && FieldData->RM.Value == 6)
    {
      RMStringPtr +=
        sprintf(RMStringPtr, "[%d]", FieldData->Displacement);
    }
    else
    {
      u8 WideOffest =
        FieldData->Mod.Value == 3 && FieldData->IsWide.Value ? 8 : 0;
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

  if(FieldData->DataLow.IsExists && !FieldData->Reg.IsExists)
  {
    sprintf(FieldData->RegString, "%s", DataString);
  }

  b32 IsDestination = FieldData->IsDestination.Value;
  if(FieldData->DataLow.IsExists && FieldData->Reg.IsExists)
  {
    sprintf(FieldData->RMString, "[%s]", DataString);
    IsDestination = !IsDestination;
  }

  if(IsDestination)
  {
    Context->InstructionWritePtr +=
      sprintf(Context->InstructionWritePtr, "%s, %s", FieldData->RegString, FieldData->RMString);
  }
  else
  {
    Context->InstructionWritePtr +=
      sprintf(Context->InstructionWritePtr, "%s, %s", FieldData->RMString, FieldData->RegString);
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
