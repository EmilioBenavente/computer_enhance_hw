#define INSTRUCTION_BITS(is_exists, bits, bit_count, value_mask) {is_exists, bits, bit_count, value_mask}
#define OP_CODE_BITS(bits, bit_count, value_mask) .OpCode = INSTRUCTION_BITS(1, bits, bit_count, value_mask)
#define D_BIT(bit)                    .IsDisplacement = INSTRUCTION_BITS(1, bit, 1, 0x1)
#define W_BIT(bit)                    .IsWide = INSTRUCTION_BITS(1, bit, 1, 0x1)
#define MOD_BITS(bits)                .Mod = INSTRUCTION_BITS(1, bits, 2, 0x3)
#define OCTAL_CODE_BITS(bits)         .OctalCode = INSTRUCTION_BITS(1, bits, 3, 0x7)
#define REG_BITS(bits)                .Reg = INSTRUCTION_BITS(1, bits, 3, 0x7)
#define RM_BITS(bits)                 .RM = INSTRUCTION_BITS(1, bits, 3, 0x7)
#define DISP_LOW(bits)                .DispLow = INSTRUCTION_BITS(1, bits, 8, 0xFF)
#define DISP_HIGH(bits)               .DispHigh = INSTRUCTION_BITS(1, bits, 8, 0xFF)
#define DATA_LOW(bits)                .DataLow = INSTRUCTION_BITS(1, bits, 8, 0xFF)
#define DATA_HIGH(bits)               .DataHigh = INSTRUCTION_BITS(1, bits, 8, 0xFF)


#define MOV_RM_TO_FROM_REG {"mov", {{OP_CODE_BITS(0b100010, 6, 0x2F), D_BIT(0), W_BIT(0), MOD_BITS(0), REG_BITS(0), RM_BITS(0), DISP_LOW(0), DISP_HIGH(0)}}}
#define MOV_IMM_TO_RM {"mov", {{OP_CODE_BITS(0b1100011, 7, 0x7F), W_BIT(0), MOD_BITS(0), OCTAL_CODE_BITS(0b000), RM_BITS(0), DISP_LOW(0), DISP_HIGH(0), DATA_LOW(0), DATA_HIGH(0)}}}
#define MOV_IMM_TO_REG {"mov", {{OP_CODE_BITS(0b1011, 4, 0xF), W_BIT(0), REG_BITS(0), DATA_LOW(0), DATA_HIGH(0)}}}
#define MOV_MEM_TO_ACC {"mov", {{OP_CODE_BITS(0b1010000, 7, 0x7F), W_BIT(0), DATA_LOW(0), DATA_HIGH(0)}}}
#define MOV_ACC_TO_MEM {"mov", {{OP_CODE_BITS(0b1010001, 7, 0x7F), W_BIT(0), DATA_LOW(0), DATA_HIGH(0)}}}
#define MOV_RM_TO_SEG {"mov", {{OP_CODE_BITS(0b10001110, 8, 0xFF), MOD_BITS(0), REG_BITS(0), DISP_LOW(0), DISP_HIGH(0)}}}
#define MOV_SEG_TO_RM {"mov", {{OP_CODE_BITS(0b10001100, 8, 0xFF), MOD_BITS(0), REG_BITS(0), DISP_LOW(0), DISP_HIGH(0)}}}

