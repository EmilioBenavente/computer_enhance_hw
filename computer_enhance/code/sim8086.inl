#define INSTRUCTION_BITS(is_exists, bits, bit_count, value_mask) {is_exists, bits, bit_count, value_mask}
#define OP_CODE_BITS(bits, bit_count, value_mask) .OpCode = INSTRUCTION_BITS(1, bits, bit_count, value_mask)
#define D_BIT                    .IsDestination = INSTRUCTION_BITS(1, 0, 1, 0x1)
#define W_BIT                    .IsWide = INSTRUCTION_BITS(1, 0, 1, 0x1)
#define MOD_BITS                .Mod = INSTRUCTION_BITS(1, 0, 2, 0x3)
#define OCTAL_CODE_BITS(bits)         .OctalCode = INSTRUCTION_BITS(1, bits, 3, 0x7)
#define REG_BITS                .Reg = INSTRUCTION_BITS(1, 0, 3, 0x7)
#define RM_BITS                 .RM = INSTRUCTION_BITS(1, 0, 3, 0x7)
#define DISP_LOW                .DispLow = INSTRUCTION_BITS(1, 0, 8, 0xFF)
#define DISP_HIGH               .DispHigh = INSTRUCTION_BITS(1, 0, 8, 0xFF)
#define DATA_LOW                .DataLow = INSTRUCTION_BITS(1, 0, 8, 0xFF)
#define DATA_HIGH               .DataHigh = INSTRUCTION_BITS(1, 0, 8, 0xFF)


//@NOTE(Emilio): mov instructions
#define MOV_RM_TO_FROM_REG {"mov", {{OP_CODE_BITS(0b100010, 6, 0x2F), D_BIT, W_BIT, MOD_BITS, REG_BITS, RM_BITS, DISP_LOW, DISP_HIGH}}}
#define MOV_IMM_TO_RM {"mov", {{OP_CODE_BITS(0b1100011, 7, 0x7F), W_BIT, MOD_BITS, OCTAL_CODE_BITS(0b000), RM_BITS, DISP_LOW, DISP_HIGH, DATA_LOW, DATA_HIGH}}}
#define MOV_IMM_TO_REG {"mov", {{OP_CODE_BITS(0b1011, 4, 0xF), W_BIT, REG_BITS, DATA_LOW, DATA_HIGH}}}
#define MOV_MEM_TO_ACC {"mov", {{OP_CODE_BITS(0b1010000, 7, 0x7F), W_BIT, DATA_LOW, DATA_HIGH}}}
#define MOV_ACC_TO_MEM {"mov", {{OP_CODE_BITS(0b1010001, 7, 0x7F), W_BIT, DATA_LOW, DATA_HIGH}}}
#define MOV_RM_TO_SEG {"mov", {{OP_CODE_BITS(0b10001110, 8, 0xFF), MOD_BITS, REG_BITS, DISP_LOW, DISP_HIGH}}}
#define MOV_SEG_TO_RM {"mov", {{OP_CODE_BITS(0b10001100, 8, 0xFF), MOD_BITS, REG_BITS, DISP_LOW, DISP_HIGH}}}

//@NOTE(Emilio): add instructions
#define ADD_RM_WITH_REG {"add", {{OP_CODE_BITS(0b000000, 6, 0x2F), D_BIT, W_BIT, MOD_BITS, REG_BITS, RM_BITS, DISP_LOW, DISP_HIGH}}}
#define ADD_IMM_TO_RM {"add", {{OP_CODE_BITS(0b100000, 6, 0x2F), D_BIT, W_BIT, MOD_BITS, OCTAL_CODE_BITS(0b000), RM_BITS, DISP_LOW, DISP_HIGH, DATA_LOW, DATA_HIGH}}}
#define ADD_IMM_TO_ACC {"add", {{OP_CODE_BITS(0b0000010, 7, 0x3F), W_BIT, DATA_LOW, DATA_HIGH}}}

//@NOTE(Emilio): sub instructions
#define SUB_RM_WITH_REG {"sub", {{OP_CODE_BITS(0b001010, 6, 0x2F), D_BIT, W_BIT, MOD_BITS, REG_BITS, RM_BITS, DISP_LOW, DISP_HIGH}}}
#define SUB_IMM_FROM_RM {"sub", {{OP_CODE_BITS(0b100000, 6, 0x2F), D_BIT, W_BIT, MOD_BITS, OCTAL_CODE_BITS(0b101), RM_BITS, DISP_LOW, DISP_HIGH, DATA_LOW, DATA_HIGH}}}
#define SUB_IMM_FROM_ACC {"sub", {{OP_CODE_BITS(0b0010110, 7, 0x3F), W_BIT, DATA_LOW, DATA_HIGH}}}

//@NOTE(Emilio): cmp instructions
#define CMP_RM_AND_REG {"cmp", {{OP_CODE_BITS(0b001110, 6, 0x2F), D_BIT, W_BIT, MOD_BITS, REG_BITS, RM_BITS, DISP_LOW, DISP_HIGH}}}
#define CMP_IMM_WITH_RM {"cmp", {{OP_CODE_BITS(0b100000, 6, 0x2F), D_BIT, W_BIT, MOD_BITS, OCTAL_CODE_BITS(0b111), RM_BITS, DISP_LOW, DISP_HIGH, DATA_LOW, DATA_HIGH}}}
#define CMP_IMM_WITH_ACC {"cmp", {{OP_CODE_BITS(0b0011110, 7, 0x3F), W_BIT, DATA_LOW, DATA_HIGH}}}


