#ifndef DISASSEMBLE_H
#define DISASSEMBLE_H

#include "core.h"
#include "types.h"

#define mem_read16(mc, adr) (unsigned int) ((addr += 2, mem_read(memc, addr-2) + (mem_read(memc, addr-1) << 8)) & 0xFFFF)

#define DA_NOP	0
#define DA_EX_AF_AF_	1
#define DA_DJNZ_X	2
#define DA_JR_X	3
#define DA_JR_CC_X	4
#define DA_LD_RP_X	5
#define DA_ADD_HL_RP	6
#define DA_LD__BC__A	7
#define DA_LD_A__BC_	8
#define DA_LD__DE__A	9
#define DA_LD_A__DE_	10
#define DA_LD__X__HL	11
#define DA_LD_HL__X_	12
#define DA_LD__X__A	13
#define DA_LD_A__X_	14
#define DA_INC_RP	15
#define DA_DEC_RP	16
#define DA_INC_R	17
#define DA_DEC_R	18
#define DA_LD_R_X	19
#define DA_RLCA	20
#define DA_RRCA	21
#define DA_RLA	22
#define DA_RRA	23
#define DA_DAA	24
#define DA_CPL	25
#define DA_SCF	26
#define DA_CCF	27
#define DA_LD_R_R	28
#define DA_HALT	29
#define DA_ALU	30
#define DA_ALU_A	31
#define DA_RET_CC	32
#define DA_POP_RP	33
#define DA_RET	34
#define DA_EXX	35
#define DA_JP_HL	36
#define DA_LD_SP_HL	37
#define DA_JP_CC_X	38
#define DA_JP_X	39
#define DA_OUT__X__A	40
#define DA_IN_A__X_	41
#define DA_EX__SP__HL	42
#define DA_EX_DE_HL	43
#define DA_DI	44
#define DA_EI	45
#define DA_CALL_CC_X	46
#define DA_PUSH_RP	47
#define DA_CALL_X	48
#define DA_ALU_X	49
#define DA_ALU_A_X	50
#define DA_RST_X	51
#define DA_ROT	52
#define DA_BIT	53
#define DA_RES	54
#define DA_SET	55
#define DA_IN_R__C_	56
#define DA_OUT__C__R	57
#define DA_SBC_HL_RP	58
#define DA_ADC_HL_RP	59
#define DA_LD__X__RP	60
#define DA_LD_RP__X_	61
#define DA_NEG	62
#define DA_RETN	63
#define DA_RETI	64
#define DA_IM_X	65
#define DA_LD_I_A	66
#define DA_LD_R_A	67
#define DA_LD_A_I	68
#define DA_LD_A_R	69
#define DA_RRD	70
#define DA_RLD	71
#define DA_NOP_ED	72
#define DA_BLI	73
#define DA_ROT_R	74
#define DA_BIT_R	75
#define DA_RES_R	76
#define DA_SET_R	77
#define DA_ROT_I	78
#define DA_BIT_I	79
#define DA_RES_I	80
#define DA_SET_I	81
#define DA_ADD_RI_RP	82
#define DA_LD_RI__X_	83
#define DA_LD__X__RI	84
#define DA_INC_RI	85
#define DA_DEC_RI	86
#define DA_LD_RI_X	87
#define DA_LD_RI_R	88
#define DA_LD_R_RI	89
#define DA_ALU_RI	90
#define DA_ALU_A_RI	91
#define DA_JP_RI	92
#define DA_LD_SP_RI	93
#define DA_EX__SP__RI	94
#define DA_LABEL	95
#define DA_BCALL	96
#define DA_BCALL_N	97
#define DA_BIT_RF	98
#define DA_RES_RF	99
#define DA_SET_RF	100
#define DA_BIT_IF	101
#define DA_RES_IF	102
#define DA_SET_IF	103
#define DA_BJUMP	104
#define DA_BJUMP_N	105
#define DA_LD__HL__X	DA_BJUMP_N + 1
#define DA_LD__HL__R	DA_LD__HL__X + 1
#define DA_LD_R__HL_	DA_LD__HL__R
#define DA_INC__HL_	DA_LD_R__HL_ + 1
#define DA_DEC__HL_	DA_INC__HL_ + 1
#define DA_BIT__HL_	DA_DEC__HL_ + 1
#define DA_RES__HL_	DA_BIT__HL_ + 1
#define DA_SET__HL_	DA_RES__HL_ + 1


typedef struct Z80_info {
	int index;
	union {
		struct {
			INT_PTR a1, a2, a3, a4;
		};
		INT_PTR a[4];
	};
	int size;			/* Size of command */
	unsigned short addr;
} Z80_info_t;

typedef struct Z80_command {
	TCHAR format[32];			/* printf formatted string */
	TCHAR clocks;				/* clocks to complete */
	TCHAR clocks_cond;			/* Conditional clocks to complete */
#ifdef da_ready
	TCHAR flag_effects[8];		/* Flag effects for all 8 bits */
	TCHAR *flag_description;		/* optional description of flag effects */
#endif
} Z80_com_t;

int disassemble(memory_context_t *memc, unsigned short addr, int count, Z80_info_t *result);


#endif /* #ifndef DISASSEMBLE_H */
