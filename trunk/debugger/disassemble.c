#include "disassemble.h"
#include "label.h"
#include "calc.h"
#include <stdio.h>

//http://www.z80.info/decoding.htm


Z80_com_t da_opcode[256] = {
{"  nop", 				4, 	0},
{"  ex %r,%r",			4, 	0},
{"  djnz %g",			13,	8},
{"  jr %g",				10, 0},
{"  jr %c,%g",			12,	7},
{"  ld %r,%x",			10, 0},
{"  add %r,%r",			11, 0},
{"  ld (%r),%r",		7,	0},
{"  ld %r,(%r)",		7,	0},
{"  ld (%r),%r",		7,	0},
{"  ld %r,(%r)",		7,	0},
{"  ld (%a),%r",		16,	0},
{"  ld %r,(%a)",		16,	0},
{"  ld (%a),%r",		13,	0},
{"  ld %r,(%a)",		13,	0},
{"  inc %r",			8,	0},
{"  dec %r",			8,	0},
{"  inc %r",			-1,	0},
{"  dec %r",			-1,	0},
{"  ld %r,%x",			7,	0},
{"  rlca",				4,	0},
{"  rrca",				4,	0},
{"  rla",				4,	0},
{"  rra",				4,	0},
{"  daa",				4,	0},
{"  cpl",				4,	0},
{"  scf",				4,	0},
{"  ccf",				4,	0},
{"  ld %r,%r",			4,	0},
{"  halt",				4,	0},
{"  %s %r",				4,	0},
{"  %s %r,%r",			4,	0},
{"  ret %c",		11,	5},
{"  pop %r",		10,	0},
{"  ret",			10, 0},
{"  exx",			4,	0},
{"  jp (%r)",		4,	0},
{"  ld %r,%r",		6,	0},
{"  jp %c,%a",		10,	0},
{"  jp %a",			10,	0},
{"  out (%x),%r",	11,	0},
{"  in %r,(%x)",		11, 0},
{"  ex (%r),%r",	19, 0},
{"  ex %r,%r",		4,	0},
{"  di",			4,	0},
{"  ei",			4,	0},
{"  call %c,%a",	17,	10},
{"  push %r",		11,	0},
{"  call %a",		17,	0},
{"  %s %d",			4,	0},
{"  %s %r,%d",		4,	0},
{"  rst %xh",		11,	0},
{"  %s %s",			-1,	0},
{"  bit %d,%r",		-1,	0},
{"  res %d,%r",		-1,	0},
{"  set %d,%r",		-1,	0},
{"  in %r,(%r)",		12,	0},
{"  out (%r),%r",	12,	0},
{"  sbc %r,%r",		15,	0},
{"  adc %r,%r",		11,	0},
{"  ld (%a),%r",	20,	0},
{"  ld %r,(%a)",	20,	0},
{"  neg",			8,	0},
{"  retn",			14,	0},
{"  reti",			14,	0},
{"  im %s",			8,	0},
{"  ld i,a",		9,	0},
{"  ld r,a",		9,	0},
{"  ld a,i",		9,	0},
{"  ld a,r",		9,	0},
{"  rrd",			18,	0},
{"  rld",			18, 0},
{"  nop",			8,	0},
{"  %s",			-1,	0},
{"  %s (%r%h)->%r",	23,	0},
{"  bit %d,(%r%h)->%r",	23,	0},
{"  res %d,(%r%h)->%r",	23,	0},
{"  set %d,(%r%h)->%r",	23,	0},
{"  %s (%s%h)",		23,	0},
{"  bit %d,(%r%h)",	20,	0},
{"  res %d,(%r%h)",	20,	0},
{"  set %d,(%r%h)",	20,	0},
{"  add %r,%r",		15,	0},
{"  ld %r,(%a)",	20,	0},
{"  ld (%a),%r",	20,	0},
{"  inc (%r%h)",	23,	0},
{"  dec (%r%h)",	23,	0},
{"  ld (%r%h),%x",	19,	0},
{"  ld (%r%h),%r",	19,	0},
{"  ld %r,(%r%h)",	19,	0},
{"  %s (%r%h)",		19,	0},
{"	%s %r,(%r%h)",	19, 0},
{"  jp %s",			6,	0},
{"  ld %r,%r",		10, 0},
{"  ex (sp),%s",	23,	0},
{"%l:",				-1, 0},
{"  bcall(%l)",		-1,	0},
{"  bcall(%a)",		-1,	0},
{"  bit %l,(%r+%l)->%r",	23,	0},
{"  res %l,(%r+%l)->%r",	23,	0},
{"  set %l,(%r+%l)->%r",	23,	0},
{"  bit %l,(%r+%l)",	20,	0},
{"  res %l,(%r+%l)",	20,	0},
{"  set %l,(%r+%l)",	20,	0},
{"  bjump(%l)",		-1,	0},
{"  bjump(%a)",		-1,	0}
};

#define R_B 0
#define R_C 1
#define R_D 2
#define R_E 3
#define R_H 4
#define R_L 5
#define R__HL_ 6
#define R_A 7
#define R_F 8
static char r[9][5]  = 	{"b","c","d","e","h","l","(hl)","a","f"};
static char r8i[2][9][5]  = {
{"b","c","d","e","ixh","ixl","ix","a","f"},
{"b","c","d","e","iyh","iyl","iy","a","f"}};
#define R_BC 0
#define R_DE 1
#define R_HL 2
#define R_SP 3
#define R_AF 3
static char rp[4][4] = 	{"bc","de","hl","sp"};
static char rpi[2][4][4] = {{"bc","de","ix","sp"},{"  bc","de","iy","sp"}};
static char rp2[4][4] = {"bc","de","hl","af"};
static char rp2i[2][4][4] = {{"bc","de","ix","af"},{"  bc","de","iy","af"}};
static char ri[2][4] = 	{"ix","iy"};
static char cc[8][4] = 	{"nz","z","nc","c","po","pe","p","m"};
static char alu[8][4] = {"add","adc","sub","sbc","and","xor","or","cp"};
static char rot[8][4] = {"rlc","rrc","rl","rr","sla","sra","sll","srl"};
static char im[8][4] = 	{"0","0/1","1","2","0","0/1","1","2"};
static char bli[4][4][8] = {
{"ldi", "cpi", "ini", "outi"},
{"ldd", "cpd", "ind", "outd"},
{"ldir","cpir","inir","otir"},
{"lddr","cpdr","indr","otdr"}};

void da_display(Z80_info_t *command) {
	putchar(' ');
	printf(da_opcode[command->index].format, command->a1, command->a2, command->a3, command->a4);
}

/* returns number of bytes read */
int disassemble(memory_context_t *memc, unsigned short addr, int count, Z80_info_t *result) {
	int i, prefix = 0, pi = 0;
	for (i = 0; i < count; i++, result++, prefix = 0) {
		int start_addr = result->addr = addr;


		char* labelname = FindAddressLabel(gslot, calcs[gslot].cpu.mem_c->banks[addr>>14].ram, calcs[gslot].cpu.mem_c->banks[addr>>14].page, addr);

		if (labelname) {
			result->index = DA_LABEL;
			result->a1 = (int) labelname;
			result->size = 0;
			result++;
			result->addr = addr;
		}

		unsigned char data = mem_read(memc, addr++);

		if (data == 0xDD || data == 0xFD) {
			prefix = data;
			pi = (prefix >> 5) & 1;
			data = mem_read(memc, addr++);
		}
		if (data == 0xCB) {
			int offset;
			data = mem_read(memc, addr++);
			if (prefix) {
				offset = (char) data;
				data = mem_read(memc, addr++);
			}
			int x = (data & 0xC0) >> 6;
			int y = (data & 0x38) >> 3;
			int z = (data & 0x07);
			result->a1 = y;
			result->a2 = (int) r[z];

			switch (x) {
				case 0: /* X = 0 */
					result->index = DA_ROT;
					result->a1 = (int) rot[y];
					break;
				case 1:	result->index = DA_BIT; break; /* X = 1 */
				case 2:	result->index = DA_RES; break; /* X = 2 */
				case 3:	result->index = DA_SET; break; /* X = 3 */
			}
			if (prefix) {
				char *flagname, *bitname;
				FindFlags(offset,y,&flagname,&bitname);

				/* Special IY flags*/
				if ((prefix == 0xFD) &&
					(x!=0) &&
					(calcs[gslot].cpu.iy == 0x89F0) &&
					(calcs[gslot].model >= TI_83P) &&
					flagname && bitname) {
					if (z == 6) {
						result->index += (DA_BIT_IF - DA_BIT);
						result->a1 = (int) bitname;
						result->a2 = (int) "iy";
						result->a3 = (int) flagname;
					} else {
						result->index += (DA_BIT_RF - DA_BIT);
						result->a4 = result->a2;	// old register target receives the result
						result->a1 = (int) bitname;
						result->a2 = (int) "iy";
						result->a3 = (int) flagname;
					}
				} else {
					if (z == 6) {
						result->index += (DA_ROT_I - DA_ROT);
						result->a2 = (int) ri[pi];
						result->a3 = offset;
					} else {
						result->index += (DA_ROT_R - DA_ROT);
						result->a4 = result->a2;
						result->a3 = offset;
						result->a2 = (int) ri[pi];
					}
				}
			}
		} else if (data == 0xED) {
			data = mem_read(memc, addr++);
			int x = (data & 0xC0) >> 6;
			int y = (data & 0x38) >> 3;
			int z = (data & 0x07);
			int p = (data & 0x30) >> 4;
			int q = y & 1;

			if (x == 1) {
			/* FOR X  = 1 */
			if (z == 0) {
				if (y == 6) y = 8;
				result->index = DA_IN_R__C_;
				result->a1 = (int) r[y];
				result->a2 = (int) r[R_C];
			} else
			if (z == 1) {
				if (y == 6) y = 8;
				result->index = DA_OUT__C__R;
				result->a1 = (int) r[R_C];
				result->a2 = (int) r[y];
			} else
			if (z == 2) {
				if (q == 0) result->index = DA_SBC_HL_RP;
				else result->index = DA_ADC_HL_RP;
				result->a1 = (int) rp[R_HL];
				result->a2 = (int) rp[p];
			} else
			if (z == 3) {
				if (q == 0) {
					result->index = DA_LD__X__RP;
					result->a1 = mem_read16(memc, addr);
					result->a2 = (int) rp[p];
				} else {
					result->index = DA_LD_RP__X_;
					result->a1 = (int) rp[p];
					result->a2 = mem_read16(memc, addr);
				}
			} else
			if (z == 4) {
				result->index = DA_NEG;
			} else
			if (z == 5) {
				if (y == 1) {
					result->index = DA_RETI;
				} else {
					result->index = DA_RETN;
				}
			} else
			if (z == 6) {
				result->index = DA_IM_X;
				result->a1 = (int) im[y];
			} else
			if (z == 7) {
				switch (y) {
					case 0:	result->index = DA_LD_I_A; break;
					case 1: result->index = DA_LD_R_A; break;
					case 2: result->index = DA_LD_A_I; break;
					case 3: result->index = DA_LD_A_R; break;
					case 4: result->index = DA_RRD; break;
					case 5: result->index = DA_RLD; break;
					default:
						result->index = DA_NOP_ED; break;
				}
			}
			} else
			/* FOR X  = 2 */
			if (x == 2) {
				if (y >= 4) {
					result->index = DA_BLI;
					result->a1 = (int) bli[y-4][z];
				} else {}
			} else {}
		} else {










			int x = (data & 0xC0) >> 6;
			int y = (data & 0x38) >> 3;
			int z = (data & 0x07);
			int p = (data & 0x30) >> 4;
			int q = y & 1;
			int offset = mem_read(memc, addr);

			if (x == 0) {


			/* FOR X  = 0 */
			if (z == 0) {
				switch (y) {
					case 0: result->index = DA_NOP; break;
					case 1:
						result->index = DA_EX_AF_AF_;
						result->a1 = (int) rp2[3];
						result->a2 = (int) "af'";
						break;
					case 2:
						result->index = DA_DJNZ_X;
						result->a1 = mem_read(memc, addr++);
						break;
					case 3:
						result->index = DA_JR_X;
						result->a1 = mem_read(memc, addr++);
						break;
					default:
						result->index = DA_JR_CC_X;
						result->a1 = (int) cc[y-4];
						result->a2 = mem_read(memc, addr++);
						break;
				}
			} else
			if (z == 1) {	/* ix, iy ready */
				if (q == 0) {
					result->index = DA_LD_RP_X;
					result->a2 = mem_read16(memc, addr);
					if (prefix && p == 2) {
						result->a1 = (int) ri[pi];
					} else {
						result->a1 = (int) rp[p];
					}
				} else {
					if (prefix) {
						result->index = DA_ADD_RI_RP;
						result->a1 = (int) ri[pi];
						result->a2 = (int) rpi[pi][p];
					} else {
						result->index = DA_ADD_HL_RP;
						result->a1 = (int) rp[R_HL];
						result->a2 = (int) rp[p];
					}
				}
			} else
			if (z == 2) {	/* ix, iy ready */
				switch (y) {
					case 0:
						result->index = DA_LD__BC__A;
						result->a1 = (int) rp[R_BC];
						result->a2 = (int) r[R_A];
						break;
					case 1:
						result->index = DA_LD_A__BC_;
						result->a1 = (int) r[R_A];
						result->a2 = (int) rp[R_BC];
						break;
					case 2:
						result->index = DA_LD__DE__A;
						result->a1 = (int) rp[R_DE];
						result->a2 = (int) r[R_A];
						break;
					case 3:
						result->index = DA_LD_A__DE_;
						result->a1 = (int) r[R_A];
						result->a2 = (int) rp[R_DE];
						break;
					case 4:
						if (prefix) {
							result->index = DA_LD__X__RI;
							result->a2 = (int) ri[pi];
							result->a1 = mem_read16(memc, addr);
						} else {
							result->index = DA_LD__X__HL;
							result->a1 = mem_read16(memc, addr);
							result->a2 = (int) rp[R_HL];
						}
						break;
					case 5:
						if (prefix) {
							result->index = DA_LD_RI__X_;
							result->a1 = (int) ri[pi];
							result->a2 = mem_read16(memc, addr);
						} else {
							result->index = DA_LD_HL__X_;
							result->a1 = (int) rp[R_HL];
							result->a2 = mem_read16(memc, addr);

						}
						break;
					case 6:	result->index = DA_LD__X__A;
							result->a1 = mem_read16(memc, addr);
							result->a2 = (int) r[R_A];
							break;
					case 7:	result->index = DA_LD_A__X_;
							result->a1 = (int) r[R_A];
							result->a2 = mem_read16(memc, addr);
							break;
				}
			} else
			if (z == 3) {	/* ix, iy ready */
				result->index = (q == 0) ? DA_INC_RP : DA_DEC_RP;
				result->a1 = (int) (prefix ? rpi[pi][p] : rp[p]);
			} else
			if (z < 6) {	/* ix, iy ready */
				result->index = (z == 4) ? DA_INC_R : DA_DEC_R;
				result->a1 = (int) (prefix ? r8i[pi][y] : r[y]);
				if (prefix && y == 6) {
					result->index += (DA_INC_RI - DA_INC_R);
					result->a2 = offset;
					addr++;
				}

			} else
			if (z == 6) {	/* ix, iy ready */
				result->index = DA_LD_R_X;
				result->a1 = (int) (prefix ? r8i[pi][y] : r[y]);
				if (prefix && y == 6) {
					result->index = DA_LD_RI_X;
					result->a2 = offset;
					addr++;
					result->a3 = (unsigned char) mem_read(memc, addr++);
				} else {
					result->a2 = (unsigned char) mem_read(memc, addr++);
				}
			} else {	/* ix, iy ready */
				switch (y) {
					case 0:	result->index = DA_RLCA; break;
					case 1: result->index = DA_RRCA; break;
					case 2: result->index = DA_RLA; break;
					case 3: result->index = DA_RRA; break;
					case 4: result->index = DA_DAA; break;
					case 5: result->index = DA_CPL; break;
					case 6: result->index = DA_SCF; break;
					case 7: result->index = DA_CCF; break;
				}
			}
			} else


			/* FOR X = 1 */
			if (x == 1) {	/* ix, iy ready */
				if (z == 6 && y == 6) {
					result->index = DA_HALT;
				} else {
					result->index = DA_LD_R_R;
					result->a1 = (int) r[y];
					result->a2 = (int) r[z];
					if (prefix) {
						if (y == 6) {
							addr++;
							result->index = DA_LD_RI_R;
							result->a1 = (int) ri[pi];
							result->a3 = result->a2;
							result->a2 = offset;
						} else if (z == 6) {
							addr++;
							result->a2 = (int) ri[pi];
							result->index = DA_LD_R_RI;
							result->a3 = offset;
						}
					}
				}
			} else
			/* FOR X = 2 */
			if (x == 2) {	/* ix, iy ready */
				unsigned int *a = result->a;
				*(a++) = (int) alu[y];
				if (y == 0 || y == 1 || y == 3) {
					*(a++) = (int) r[R_A];
					result->index = DA_ALU_A;
				} else
				result->index = DA_ALU;

				*(a++) = (int) (prefix ? r8i[pi][z] : r[z]);
				if (prefix && z == 6) {
					result->index += (DA_ALU_RI - DA_ALU);
					*(a++) = offset;
					addr++;
				}

			} else
			if (x == 3) {
			/* FOR X = 3 */
			if (z == 0) {
				result->index = DA_RET_CC;
				result->a1 = (int) cc[y];
			} else
			if (z == 1) {
				if (q == 0) {
					result->index = DA_POP_RP;
					result->a1 = (int) (prefix ? rp2i[pi][p] : rp2[p]);
				} else {
					switch (p) {
						case 0:	result->index = DA_RET; break;
						case 1: result->index = DA_EXX; break;
						case 2:
							result->index = prefix ? DA_JP_RI : DA_JP_HL;
							result->a1 = (int) (prefix ? ri[pi] : rp[R_HL]);
							break;
						case 3:
							result->index = DA_LD_SP_HL;
							result->a1 = (int) rp[R_SP];
							result->a2 = (int) (prefix ? ri[pi] : rp[R_HL]);
							break;
					}
				}
			} else
			if (z == 2) {
				result->index = DA_JP_CC_X;
				result->a1 = (int) cc[y];
				result->a2 = mem_read16(memc, addr);
			} else
			if (z == 3) {
				switch (y) {
					case 0:
						result->index = DA_JP_X;
						result->a1 = mem_read16(memc, addr);
						break;
					case 2:
						result->index = DA_OUT__X__A;
						result->a1 = mem_read(memc, addr++);
						result->a2 = (int) r[R_A];
						break;
					case 3:
						result->index = DA_IN_A__X_;
						result->a1 = (int) r[R_A];
						result->a2 = mem_read(memc, addr++);
						break;
					case 4:
						result->index = (int) (prefix ? DA_EX__SP__RI : DA_EX__SP__HL);
						result->a1 = (int) rp[R_SP];
						result->a2 = (int) (prefix ? ri[pi] : rp[R_HL]);
						break;
					case 5:
						result->index = DA_EX_DE_HL;
						result->a1 = (int) rp[R_DE];
						result->a2 = (int) rp[R_HL];
						break;
					case 6: result->index = DA_DI; break;
					case 7: result->index = DA_EI; break;
				}
			} else
			if (z == 4) {
				result->index = DA_CALL_CC_X;
				result->a1 = (int) cc[y];
				result->a2 = mem_read16(memc, addr);
			} else
			if (z == 5) {
				if (q == 0) {
					result->index = DA_PUSH_RP;
					result->a1 = (int) (prefix ? rp2i[pi][p] : rp2[p]);
				} else {
					if (p == 0) {
						result->index = DA_CALL_X;
						result->a1 = mem_read16(memc, addr);

						if ((result->a1 == 0x0050) && calcs[gslot].model >= TI_83P) {
							result->index = DA_BJUMP;
							result->a1 = mem_read16(memc, addr);
							char* Name = FindBcall(result->a1);
							if (Name == NULL) {
								result->index = DA_BJUMP_N;
							} else {
								result->a1 = (int) Name;
							}
						}

					}
				}
			} else
			if (z == 6) {
				switch (y) {
					case 0:
						result->index = DA_ADD_HL_RP;
						result->a1 = (int) r[R_A];
						break;
					case 1:
						result->index = DA_ADC_HL_RP;
						result->a1 = (int) r[R_A];
						break;
					case 3:
						result->index = DA_SBC_HL_RP;
						result->a1 = (int) r[R_A];
						break;
					default:
						result->index = DA_ALU;
						result->a1 = (int) alu[y];
						break;
				}
				result->index = DA_ALU_X;
				result->a1 = (int) alu[y];
				result->a2 = (unsigned char) mem_read(memc, addr++);
			} else
			if (z == 7) {
				if ((y == 5) && (calcs[gslot].model >= TI_83P)) {
					result->index = DA_BCALL;
					int tmp = mem_read16(memc, addr);
					char* Name = FindBcall(tmp);
					if (Name == NULL) {
						result->index = DA_BCALL_N;
						result->a1 = tmp;
					} else {
						result->a1 = (int) Name;
					}
				} else {
					result->index = DA_RST_X;
					result->a1 = y*8;
				}
			}
			}
		}
		result->size = addr - start_addr;
		//printf("%0.4X: ",start_addr); da_display(result); putchar('\n');
	}

	return i;
}
