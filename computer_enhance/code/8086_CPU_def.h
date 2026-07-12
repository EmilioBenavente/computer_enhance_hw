#if !defined(_8086_CPU_H_)
#define _8086_CPU_H_

enum OP_CODE_TABLE_8086
{
  MOV_RM_RM_8086,
  MOV_IMM_RM_8086,

  OP_CODE_TABLE_8086_SIZE
};

enum REG_TABLE_8086
{
  REG_AL_8086,
  REG_CL_8086,
  REG_DL_8086,
  REG_BL_8086,
  REG_AH_8086,
  REG_CH_8086,
  REG_DH_8086,
  REG_BH_8086,

  //@NOTE(Emilio): (REG % 8) + 8
  REG_AX_8086,
  REG_CX_8086,
  REG_DX_8086,
  REG_BX_8086,
  REG_SP_8086,
  REG_BP_8086,
  REG_SI_8086,
  REG_DI_8086,

  REG_TABLE_8086_SIZE
};

enum RM_TABLE_8086
{
  RM_BX_SI_8086,
  RM_BX_DI_8086,
  RM_BP_SI_8086,
  RM_BP_DI_8086,
  RM_SI_8086,
  RM_DI_8086,
  RM_D16_8086,
  RM_BX_8086,

  //@NOTE(Emilio): RM = RM + (8 * MOD);
  RM_BX_SI_D8_8086,
  RM_BX_DI_D8_8086,
  RM_BP_SI_D8_8086,
  RM_BP_DI_D8_8086,
  RM_SI_D8_8086,
  RM_DI_D8_8086,
  RM_BP_D8_8086,
  RM_BX_D8_8086,

  RM_BX_SI_D16_8086,
  RM_BX_DI_D16_8086,
  RM_BP_SI_D16_8086,
  RM_BP_DI_D16_8086,
  RM_SI_D16_8086,
  RM_DI_D16_8086,
  RM_BP_D16_8086,
  RM_BX_D16_8086,

  RM_AL_MOD3_8086,
  RM_CL_MOD3_8086,
  RM_DL_MOD3_8086,
  RM_BL_MOD3_8086,
  RM_AH_MOD3_8086,
  RM_CH_MOD3_8086,
  RM_DH_MOD3_8086,
  RM_BH_MOD3_8086,

  //@NOTE(Emilio): Only if Wide Instruction
  RM_AX_MOD3_8086,
  RM_CX_MOD3_8086,
  RM_DX_MOD3_8086,
  RM_BX_MOD3_8086,
  RM_SP_MOD3_8086,
  RM_BP_MOD3_8086,
  RM_SI_MOD3_8086,
  RM_DI_MOD3_8086,

  RM_TABLE_8086_SIZE
};



#if 0
enum OP_CODES_8086
{
  MOV_RM_RM_8086,
  MOV_IMM_RM_8086,
  MOV_IMM_REG_8086,
  MOV_MEM_TO_ACC_8086,
  MOV_ACC_TO_MEM_8086,
  MOV_RM_TO_SEG_8086,
  MOV_SEG_TO_RM_8086,

  OP_CODE_8086_SIZE
};
#endif

#define EMPTY_FIELD                                 {0, 0, 0, 0}
#define OP_CODE_FIELD(_mnemonic, _opcode, _mask, _shift, _bitcount)       {_mnemonic, _opcode, _mask, _shift, _bitcount}

#define D_FIELD                                     {1, 0x1, 1, 0}
#define W_FIELD                                     {1, 0x1, 1, 0}
#define MOD_FIELD                                   {1, 0x3, 2, 0}
#define REG_FIELD                                   {1, 0x7, 3, 0}
#define RM_FIELD                                    {1, 0x7, 3, 0}
#define DATA_FIELD                                  {1, 0x7, 3, 0}
#define FIXED_FIELD(_mask, _bitcount, _num)         {1, _mask, _bitcount, _num}

#define OP_MOV_RM_RM    {OP_CODE_FIELD("mov", 0x22, 0x3F, 2, 6), D_FIELD, W_FIELD, MOD_FIELD, REG_FIELD, RM_FIELD, EMPTY_FIELD, EMPTY_FIELD}
#define OP_MOV_IMM_RM   {OP_CODE_FIELD("mov", 0x63, 0x7F, 1, 7), EMPTY_FIELD, W_FIELD, MOD_FIELD, EMPTY_FIELD, RM_FIELD, EMPTY_FIELD, DATA_FIELD}

#endif /* _8086_CPU_H_ */
