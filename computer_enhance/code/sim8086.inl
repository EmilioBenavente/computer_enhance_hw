#define INSTRUCTION_BITS(is_exists, bits, bit_count, value_mask) {is_exists, bits, bit_count, value_mask}
#define OP_CODE_BITS(bits, bit_count, value_mask) .OpCode = INSTRUCTION_BITS(1, bits, bit_count, value_mask)
#define D_BIT                    .IsDestination = INSTRUCTION_BITS(1, 0, 1, 0x1)
#define W_BIT                    .IsWide = INSTRUCTION_BITS(1, 0, 1, 0x1)
#define MOD_BITS                .Mod = INSTRUCTION_BITS(1, 0, 2, 0x3)
#define OCTAL_CODE_BITS(bits)         .OctalCode = INSTRUCTION_BITS(1, bits, 3, 0x7)
#define REG_BITS                .Reg = INSTRUCTION_BITS(1, 0, 3, 0x7)
#define SEG_BITS                .Seg = INSTRUCTION_BITS(1, 0, 3, 0x7)
#define RM_BITS                 .RM = INSTRUCTION_BITS(1, 0, 3, 0x7)
#define DISP_LOW                .DispLow = INSTRUCTION_BITS(1, 0, 8, 0xFF)
#define DISP_HIGH               .DispHigh = INSTRUCTION_BITS(1, 0, 8, 0xFF)
#define DATA_LOW                .DataLow = INSTRUCTION_BITS(1, 0, 8, 0xFF)
#define DATA_HIGH               .DataHigh = INSTRUCTION_BITS(1, 0, 8, 0xFF)


//@TODO(Emilio): Theres still 78 more instructions to decode for a full decoder.

//@NOTE(Emilio): mov instructions
#define MOV_RM_TO_FROM_REG {"mov", {{OP_CODE_BITS(0b100010, 6, 0x2F), D_BIT, W_BIT, MOD_BITS, REG_BITS, RM_BITS, DISP_LOW, DISP_HIGH}}}
#define MOV_IMM_TO_RM {"mov", {{OP_CODE_BITS(0b1100011, 7, 0x7F), W_BIT, MOD_BITS, OCTAL_CODE_BITS(0b000), RM_BITS, DISP_LOW, DISP_HIGH, DATA_LOW, DATA_HIGH}}}
#define MOV_IMM_TO_REG {"mov", {{OP_CODE_BITS(0b1011, 4, 0xF), W_BIT, REG_BITS, DATA_LOW, DATA_HIGH}}}
#define MOV_MEM_TO_ACC {"mov", {{OP_CODE_BITS(0b1010000, 7, 0x7F), W_BIT, DATA_LOW, DATA_HIGH}}}
#define MOV_ACC_TO_MEM {"mov", {{OP_CODE_BITS(0b1010001, 7, 0x7F), W_BIT, DATA_LOW, DATA_HIGH}}}
#define MOV_RM_TO_SEG {"mov", {{OP_CODE_BITS(0b10001110, 8, 0xFF), MOD_BITS, SEG_BITS, RM_BITS, DISP_LOW, DISP_HIGH}}}
#define MOV_SEG_TO_RM {"mov", {{OP_CODE_BITS(0b10001100, 8, 0xFF), MOD_BITS, SEG_BITS, RM_BITS, DISP_LOW, DISP_HIGH}}}

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

//@NOTE(Emilio): ret instructions
#define RET_WITHIN_SEG {"ret",{{OP_CODE_BITS(0b11000011, 8, 0xFF)}}}
#define RET_WITHIN_SEG_IMM_TO_SP {"ret",{{OP_CODE_BITS(0b11000010, 8, 0xFF), DATA_LOW, DATA_HIGH}}}
#define RET_ITERSEG {"ret",{{OP_CODE_BITS(0b11001011, 8, 0xFF)}}}
#define RET_ITERSEG_IMM_TO_SP {"ret",{{OP_CODE_BITS(0b11001010, 8, 0xFF), DATA_LOW, DATA_HIGH}}}

//@NOTE(Emilio): jmp instructions
#define JMP_EQ {"je",{{OP_CODE_BITS(0b01110100, 8, 0xFF), DATA_LOW}}}
#define JMP_LESS_THAN {"jl",{{OP_CODE_BITS(0b01111100, 8, 0xFF), DATA_LOW}}}
#define JMP_LESS_EQUAL {"jle",{{OP_CODE_BITS(0b01111110, 8, 0xFF), DATA_LOW}}}
#define JMP_BELOW {"jb",{{OP_CODE_BITS(0b01110010, 8, 0xFF), DATA_LOW}}}
#define JMP_BELOW_EQUAL {"jbe",{{OP_CODE_BITS(0b01110110, 8, 0xFF), DATA_LOW}}}
#define JMP_PARITY {"jp",{{OP_CODE_BITS(0b01111010, 8, 0xFF), DATA_LOW}}}
#define JMP_OVERFLOW {"jo",{{OP_CODE_BITS(0b01110000, 8, 0xFF), DATA_LOW}}}
#define JMP_SIGN {"js",{{OP_CODE_BITS(0b01111000, 8, 0xFF), DATA_LOW}}}
#define JMP_NOT_EQUAL {"jnz",{{OP_CODE_BITS(0b01110101, 8, 0xFF), DATA_LOW}}}
#define JMP_NOT_LESS {"jnl",{{OP_CODE_BITS(0b01111101, 8, 0xFF), DATA_LOW}}}
#define JMP_NOT_LESS_EQUAL {"jg",{{OP_CODE_BITS(0b01111111, 8, 0xFF), DATA_LOW}}}
#define JMP_NOT_BELOW {"jnb",{{OP_CODE_BITS(0b01110011, 8, 0xFF), DATA_LOW}}}
#define JMP_NOT_BELOW_EQUAL {"ja",{{OP_CODE_BITS(0b01110111, 8, 0xFF), DATA_LOW}}}
#define JMP_NOT_PARITY {"jnp",{{OP_CODE_BITS(0b01111011, 8, 0xFF), DATA_LOW}}}
#define JMP_NOT_OVERFLOW {"jno",{{OP_CODE_BITS(0b01110001, 8, 0xFF), DATA_LOW}}}
#define JMP_NOT_SIGN {"jns",{{OP_CODE_BITS(0b01111001, 8, 0xFF), DATA_LOW}}}
#define LOOP {"loop",{{OP_CODE_BITS(0b11100010, 8, 0xFF), DATA_LOW}}}
#define LOOP_ZERO {"loopz",{{OP_CODE_BITS(0b11100001, 8, 0xFF), DATA_LOW}}}
#define LOOP_NOT_ZERO {"loopnz",{{OP_CODE_BITS(0b11100000, 8, 0xFF), DATA_LOW}}}
#define JMP_ON_CX_ZERO {"jcxz",{{OP_CODE_BITS(0b11100011, 8, 0xFF), DATA_LOW}}}

//@NOTE(Emilio): int instructions
#define INT_SPEC {"int",{{OP_CODE_BITS(0b11001101, 8, 0xFF), DATA_LOW}}}
#define INT_3 {"int",{{OP_CODE_BITS(0b11001100, 8, 0xFF)}}}
#define INT_OVERFLOW {"into",{{OP_CODE_BITS(0b11001110, 8, 0xFF)}}}
#define INT_RETURN {"iret",{{OP_CODE_BITS(0b11001111, 8, 0xFF)}}}

//@NOTE(Emilio): processor control instructions
#define CLEAR_CARRY {"clc",{{OP_CODE_BITS(0b11111000, 8, 0xFF)}}}
#define COMPLEMENT_CARRY {"cmc",{{OP_CODE_BITS(0b11110101, 8, 0xFF)}}}
#define SET_CARRY {"stc",{{OP_CODE_BITS(0b11111001, 8, 0xFF)}}}
#define CLEAR_DIRECTION {"cld",{{OP_CODE_BITS(0b11111100, 8, 0xFF)}}}
#define SET_DIRECTION {"std",{{OP_CODE_BITS(0b11111101, 8, 0xFF)}}}
#define CLEAR_INTERRUPT {"cli",{{OP_CODE_BITS(0b11111010, 8, 0xFF)}}}
#define SET_INTERRUPT {"sti",{{OP_CODE_BITS(0b11111011, 8, 0xFF)}}}
#define HALT {"hlt",{{OP_CODE_BITS(0b11110100, 8, 0xFF)}}}
#define WAIT {"wait",{{OP_CODE_BITS(0b10011011, 8, 0xFF)}}}
#define ESCAPE {"esc",{{OP_CODE_BITS(0b11011, 5, 0x1F), MOD_BITS, REG_BITS, RM_BITS, DISP_LOW, DISP_HIGH}}}
#define LOCK {"lock",{{OP_CODE_BITS(0b11110000, 8, 0xFF)}}}
#define SEGMENT {"segment",{{OP_CODE_BITS(0b001, 3, 0x7), REG_BITS, OCTAL_CODE_BITS(0b110)}}}

