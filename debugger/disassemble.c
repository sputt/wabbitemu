#include "stdafx.h"

#include "disassemble.h"
#include "label.h"
#include "calc.h"

//http://www.z80.info/decoding.htm


Z80_com_t da_opcode[256] = {
{_T("  nop"), 				4, 	0},
{_T("  ex %r,%r"),			4, 	0},
{_T("  djnz %g"),			13,	8},
{_T("  jr %g"),				12, 0},
{_T("  jr %c,%g"),			12,	7},
{_T("  ld %r,%x"),			10, 0},
{_T("  add %r,%r"),			11, 0},
{_T("  ld (%r),%r"),		7,	0},
{_T("  ld %r,(%r)"),		7,	0},
{_T("  ld (%r),%r"),		7,	0},
{_T("  ld %r,(%r)"),		7,	0},
{_T("  ld (%a),%r"),		16,	0},
{_T("  ld %r,(%a)"),		16,	0},
{_T("  ld (%a),%r"),		13,	0},
{_T("  ld %r,(%a)"),		13,	0},
{_T("  inc %r"),			6,	0},
{_T("  dec %r"),			6,	0},
{_T("  inc %r"),			4,	0},
{_T("  dec %r"),			4,	0},
{_T("  ld %r,%x"),			7,	0},
{_T("  rlca"),				4,	0},
{_T("  rrca"),				4,	0},
{_T("  rla"),				4,	0},
{_T("  rra"),				4,	0},
{_T("  daa"),				4,	0},
{_T("  cpl"),				4,	0},
{_T("  scf"),				4,	0},
{_T("  ccf"),				4,	0},
{_T("  ld %r,%r"),			4,	0},
{_T("  halt"),				4,	0},
{_T("  %s %r"),			4,	0},
{_T("  %s %r,%r"),			4,	0},
{_T("  ret %c"),		11,	5},
{_T("  pop %r"),		10,	0},
{_T("  ret"),			10, 0},
{_T("  exx"),			4,	0},
{_T("  jp (%r)"),		4,	0},
{_T("  ld %r,%r"),		6,	0},
{_T("  jp %c,%a"),		10,	0},
{_T("  jp %a"),			10,	0},
{_T("  out (%x),%r"),	11,	0},
{_T("  in %r,(%x)"),		11, 0},
{_T("  ex (%r),%r"),	19, 0},
{_T("  ex %r,%r"),		4,	0},
{_T("  di"),			4,	0},
{_T("  ei"),			4,	0},
{_T("  call %c,%a"),	17,	10},
{_T("  push %r"),		11,	0},
{_T("  call %a"),		17,	0},
{_T("  %s %d"),			4,	0},
{_T("  %s %r,%d"),		4,	0},
{_T("  rst %xh"),		11,	0},
{_T("  %s %s"),			-1,	0},
{_T("  bit %d,%r"),		8,	0},
{_T("  res %d,%r"),		8,	0},
{_T("  set %d,%r"),		8,	0},
{_T("  in %r,(%r)"),	12,	0},
{_T("  out (%r),%r"),	12,	0},
{_T("  sbc %r,%r"),		15,	0},
{_T("  adc %r,%r"),		15,	0},
{_T("  ld (%a),%r"),	20,	0},
{_T("  ld %r,(%a)"),	20,	0},
{_T("  neg"),			8,	0},
{_T("  retn"),			14,	0},
{_T("  reti"),			14,	0},
{_T("  im %s"),			8,	0},
{_T("  ld i,a"),		9,	0},
{_T("  ld r,a"),		9,	0},
{_T("  ld a,i"),		9,	0},
{_T("  ld a,r"),		9,	0},
{_T("  rrd"),			18,	0},
{_T("  rld"),			18, 0},
{_T("  nop"),			8,	0},
{_T("  %s"),			-2,	0},
{_T("  %s (%r%h)->%r"),	23,	0},
{_T("  bit %d,(%r%h)->%r"),	23,	0},
{_T("  res %d,(%r%h)->%r"),	23,	0},
{_T("  set %d,(%r%h)->%r"),	23,	0},
{_T("  %s (%s%h)"),		23,	0},
{_T("  bit %d,(%r%h)"),	20,	0},
{_T("  res %d,(%r%h)"),	20,	0},
{_T("  set %d,(%r%h)"),	20,	0},
{_T("  add %r,%r"),		15,	0},
{_T("  ld %r,(%a)"),	20,	0},
{_T("  ld (%a),%r"),	20,	0},
{_T("  inc (%r%h)"),	23,	0},
{_T("  dec (%r%h)"),	23,	0},
{_T("  ld (%r%h),%x"),	19,	0},
{_T("  ld (%r%h),%r"),	19,	0},
{_T("  ld %r,(%r%h)"),	19,	0},
{_T("  %s (%r%h)"),		19,	0},
{_T("	%s %r,(%r%h)"),	19, 0},
{_T("  jp %s"),			6,	0},
{_T("  ld %r,%r"),		10, 0},
{_T("  ex (sp),%s"),	23,	0},
{_T("%l:"),				-1, 0},
{_T("  bcall(%l)"),		-1,	0},
{_T("  bcall(%a)"),		-1,	0},
{_T("  bit %l,(%r+%l)->%r"),	23,	0},
{_T("  res %l,(%r+%l)->%r"),	23,	0},
{_T("  set %l,(%r+%l)->%r"),	23,	0},
{_T("  bit %l,(%r+%l)"),	20,	0},
{_T("  res %l,(%r+%l)"),	20,	0},
{_T("  set %l,(%r+%l)"),	20,	0},
{_T("  bjump(%l)"),			-1,	0},
{_T("  bjump(%a)"),			-1,	0},
{_T("  ld %r,%x"),			10,	0},
{_T("  ld %r,%r"),			7,	0},
{_T("  inc %r"),			11,	0},
{_T("  dec %r"),			11,	0},
{_T("  bit %d,%r"),			12,	0},
{_T("  res %d,%r"),			15,	0},
{_T("  set %d,%r"),			15,	0},
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
static TCHAR r[9][5]		= {_T("b"),_T("c"),_T("d"),_T("e"),_T("h"),_T("l"),_T("(hl)"),_T("a"),_T("f")};
static TCHAR r8i[2][9][5]	= {
{_T("b"),_T("c"),_T("d"),_T("e"),_T("ixh"),_T("ixl"),_T("ix"),_T("a"),_T("f")},
{_T("b"),_T("c"),_T("d"),_T("e"),_T("iyh"),_T("iyl"),_T("iy"),_T("a"),_T("f")}};
#define R_BC 0
#define R_DE 1
#define R_HL 2
#define R_SP 3
#define R_AF 3
static TCHAR rp[4][4]		= {_T("bc"),_T("de"),_T("hl"),_T("sp")};
static TCHAR rpi[2][4][4]	= {{_T("bc"),_T("de"),_T("ix"),_T("sp")},{_T("bc"),_T("de"),_T("iy"),_T("sp")}};
static TCHAR rp2[4][4]		= {_T("bc"),_T("de"),_T("hl"),_T("af")};
static TCHAR rp2i[2][4][4]	= {{_T("bc"),_T("de"),_T("ix"),_T("af")},{_T("bc"),_T("de"),_T("iy"),_T("af")}};
static TCHAR ri[2][4]		= {_T("ix"),_T("iy")};
static TCHAR cc[8][4]		= {_T("nz"),_T("z"),_T("nc"),_T("c"),_T("po"),_T("pe"),_T("p"),_T("m")};
static TCHAR alu[8][4]		= {_T("add"),_T("adc"),_T("sub"),_T("sbc"),_T("and"),_T("xor"),_T("or"),_T("cp")};
static TCHAR rot[8][4]		= {_T("rlc"),_T("rrc"),_T("rl"),_T("rr"),_T("sla"),_T("sra"),_T("sll"),_T("srl")};
static TCHAR im[8][4]		= {_T("0"),_T("0/1"),_T("1"),_T("2"),_T("0"),_T("0/1"),_T("1"),_T("2")};
static TCHAR bli[4][4][8]	= {
{_T("ldi"), _T("cpi"), _T("ini"), _T("outi")},
{_T("ldd"), _T("cpd"), _T("ind"), _T("outd")},
{_T("ldir"),_T("cpir"),_T("inir"),_T("otir")},
{_T("lddr"),_T("cpdr"),_T("indr"),_T("otdr")}};

void da_display(Z80_info_t *command) {
	_puttchar(' ');
	_tprintf_s(da_opcode[command->index].format, command->a1, command->a2, command->a3, command->a4); 
}

/* returns number of bytes read */
int disassemble(memory_context_t *memc, unsigned short addr, int count, Z80_info_t *result) {
	int i, prefix = 0, pi = 0;
	for (i = 0; i < count; i++, result++, prefix = 0) {
		int start_addr = result->addr = addr;


		TCHAR* labelname = FindAddressLabel(lpDebuggerCalc, lpDebuggerCalc->cpu.mem_c->banks[addr >> 14].ram,
											lpDebuggerCalc->cpu.mem_c->banks[addr>>14].page, addr);

		if (labelname) {
			result->index = DA_LABEL;
			result->a1 = (INT_PTR) labelname;
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
			result->a2 = (INT_PTR) r[z];
			
			switch (x) {
				case 0: /* X = 0 */
					result->index = DA_ROT;
					result->a1 = (INT_PTR) rot[y];
					break;
				case 1:	result->index = (result->a2 == (INT_PTR) r[R__HL_] && !prefix) ? DA_BIT__HL_ : DA_BIT; break; /* X = 1 */
				case 2:	result->index = (result->a2 == (INT_PTR) r[R__HL_] && !prefix) ? DA_RES__HL_ : DA_RES; break; /* X = 2 */
				case 3:	result->index = (result->a2 == (INT_PTR) r[R__HL_] && !prefix) ? DA_SET__HL_ : DA_SET; break; /* X = 3 */
			}
			if (prefix) {
				TCHAR *flagname, *bitname;
				FindFlags(offset, y, &flagname, &bitname);

				/* Special IY flags*/
				if ((prefix == 0xFD) &&
					(x!=0) &&
					(lpDebuggerCalc->cpu.iy == 0x89F0) &&
					(lpDebuggerCalc->model >= TI_83P) &&
					flagname && bitname) {
					if (z == 6) {
						result->index += (DA_BIT_IF - DA_BIT);
						result->a1 = (INT_PTR) bitname;
						result->a2 = (INT_PTR) _T("iy");
						result->a3 = (INT_PTR) flagname;
					} else {
						result->index += (DA_BIT_RF - DA_BIT);
						result->a4 = result->a2;	// old register target receives the result
						result->a1 = (INT_PTR) bitname;
						result->a2 = (INT_PTR) _T("iy");
						result->a3 = (INT_PTR) flagname;
					}
				} else {
					if (z == 6) {
						result->index += (DA_ROT_I - DA_ROT);
						result->a2 = (INT_PTR) ri[pi];
						result->a3 = offset;
					} else {
						result->index += (DA_ROT_R - DA_ROT);
						result->a4 = result->a2;
						result->a3 = offset;
						result->a2 = (INT_PTR) ri[pi];
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
				result->a1 = (INT_PTR) r[y];
				result->a2 = (INT_PTR) r[R_C];
			} else
			if (z == 1) {
				if (y == 6) y = 8;
				result->index = DA_OUT__C__R;
				result->a1 = (INT_PTR) r[R_C];
				result->a2 = (INT_PTR) r[y];
			} else
			if (z == 2) {
				if (q == 0) result->index = DA_SBC_HL_RP;
				else result->index = DA_ADC_HL_RP;
				result->a1 = (INT_PTR) rp[R_HL];
				result->a2 = (INT_PTR) rp[p];
			} else
			if (z == 3) {
				if (q == 0) {
					result->index = DA_LD__X__RP;
					result->a1 = mem_read16(memc, addr);
					result->a2 = (INT_PTR) rp[p];
				} else {
					result->index = DA_LD_RP__X_;
					result->a1 = (INT_PTR) rp[p];
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
				result->a1 = (INT_PTR) im[y];
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
					result->a1 = (INT_PTR) bli[y-4][z];
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
						result->a1 = (INT_PTR) rp2[3];
						result->a2 = (INT_PTR) _T("af'");
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
						result->a1 = (INT_PTR) cc[y-4];
						result->a2 = mem_read(memc, addr++);
						break;
				}
			} else
			if (z == 1) {	/* ix, iy ready */
				if (q == 0) {
					result->index = DA_LD_RP_X;
					result->a2 = mem_read16(memc, addr);
					if (prefix && p == 2) {
						result->a1 = (INT_PTR) ri[pi];
					} else {
						result->a1 = (INT_PTR) rp[p];
					}
				} else {
					if (prefix) {
						result->index = DA_ADD_RI_RP;
						result->a1 = (INT_PTR) ri[pi];
						result->a2 = (INT_PTR) rpi[pi][p];
					} else {
						result->index = DA_ADD_HL_RP;
						result->a1 = (INT_PTR) rp[R_HL];
						result->a2 = (INT_PTR) rp[p];
					}
				}
			} else
			if (z == 2) {	/* ix, iy ready */
				switch (y) {
					case 0: 
						result->index = DA_LD__BC__A; 
						result->a1 = (INT_PTR) rp[R_BC];
						result->a2 = (INT_PTR) r[R_A];
						break;
					case 1: 
						result->index = DA_LD_A__BC_;
						result->a1 = (INT_PTR) r[R_A];
						result->a2 = (INT_PTR) rp[R_BC];
						break;
					case 2: 
						result->index = DA_LD__DE__A; 
						result->a1 = (INT_PTR) rp[R_DE];
						result->a2 = (INT_PTR) r[R_A];
						break;
					case 3: 
						result->index = DA_LD_A__DE_; 
						result->a1 = (INT_PTR) r[R_A];
						result->a2 = (INT_PTR) rp[R_DE];
						break;
					case 4: 
						if (prefix) {
							result->index = DA_LD__X__RI;
							result->a2 = (INT_PTR) ri[pi];
							result->a1 = mem_read16(memc, addr);
						} else {
							result->index = DA_LD__X__HL;
							result->a1 = mem_read16(memc, addr);
							result->a2 = (INT_PTR) rp[R_HL];
						}
						break;							
					case 5:
						if (prefix) {
							result->index = DA_LD_RI__X_;
							result->a1 = (INT_PTR) ri[pi];
							result->a2 = mem_read16(memc, addr);
						} else {
							result->index = DA_LD_HL__X_;
							result->a1 = (INT_PTR) rp[R_HL];
							result->a2 = mem_read16(memc, addr);
							
						}
						break;
					case 6:	result->index = DA_LD__X__A;
							result->a1 = mem_read16(memc, addr);
							result->a2 = (INT_PTR) r[R_A];
							break;
					case 7:	result->index = DA_LD_A__X_;
							result->a1 = (INT_PTR) r[R_A];
							result->a2 = mem_read16(memc, addr);
							break;	
				}				
			} else
			if (z == 3) {	/* ix, iy ready */
				result->index = (q == 0) ? DA_INC_RP : DA_DEC_RP;
				result->a1 = (INT_PTR) (prefix ? rpi[pi][p] : rp[p]);
			} else
			if (z < 6) {	/* ix, iy ready */
				result->index = (z == 4) ? DA_INC_R : DA_DEC_R;
				result->a1 = (INT_PTR) (prefix ? r8i[pi][y] : r[y]);
				if (result->a1 == (INT_PTR) r[R__HL_])
					result->index = (z == 4) ? DA_INC__HL_ : DA_DEC__HL_;
				if (prefix && y == 6) {
					result->index += (DA_INC_RI - DA_INC_R);
					result->a2 = offset;
					addr++;
				}
					
			} else
			if (z == 6) {	/* ix, iy ready */
				result->index = DA_LD_R_X;
				result->a1 = (INT_PTR) (prefix ? r8i[pi][y] : r[y]);
				if (result->a1 == (INT_PTR) r[R__HL_])
					result->index = DA_LD__HL__X;
				if (prefix && y == 6) {
					result->index = DA_LD_RI_X;
					result->a2 = offset;
					addr++;
					result->a3 = (INT_PTR) mem_read(memc, addr++);
				} else {
					result->a2 = (INT_PTR) mem_read(memc, addr++);
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
					result->a1 = (INT_PTR) r[y];
					if (result->a1 == (INT_PTR) r[R__HL_])
						result->index = DA_LD__HL__R;
					result->a2 = (INT_PTR) r[z];
					if (result->a2 == (INT_PTR) r[R__HL_])
						result->index = DA_LD_R__HL_;
					if (prefix) {
						if (y == 6) {
							addr++;
							result->index = DA_LD_RI_R;
							result->a1 = (INT_PTR) ri[pi];
							result->a3 = result->a2;
							result->a2 = offset;
						} else if (z == 6) {
							addr++;
							result->a2 = (INT_PTR) ri[pi];
							result->index = DA_LD_R_RI;
							result->a3 = offset;
						}
					}			
				}
			} else
			/* FOR X = 2 */
			if (x == 2) {	/* ix, iy ready */
				INT_PTR *a = result->a;
				*(a++) = (INT_PTR) alu[y];
				if (y == 0 || y == 1 || y == 3) {
					*(a++) = (INT_PTR) r[R_A];
					result->index = DA_ALU_A;
				} else
				result->index = DA_ALU;
				
				*(a++) = (INT_PTR) (prefix ? r8i[pi][z] : r[z]);
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
				result->a1 = (INT_PTR) cc[y];
			} else
			if (z == 1) {
				if (q == 0) {
					result->index = DA_POP_RP;
					result->a1 = (INT_PTR) (prefix ? rp2i[pi][p] : rp2[p]);
				} else {
					switch (p) {
						case 0:	result->index = DA_RET; break;
						case 1: result->index = DA_EXX; break;
						case 2:
							result->index = prefix ? DA_JP_RI : DA_JP_HL;
							result->a1 = (INT_PTR) (prefix ? ri[pi] : rp[R_HL]);
							break;
						case 3: 
							result->index = DA_LD_SP_HL;
							result->a1 = (INT_PTR) rp[R_SP];
							result->a2 = (INT_PTR) (prefix ? ri[pi] : rp[R_HL]);
							break;
					}
				}
			} else
			if (z == 2) {
				result->index = DA_JP_CC_X;
				result->a1 = (INT_PTR) cc[y];
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
						result->a2 = (INT_PTR) r[R_A];
						break;
					case 3:
						result->index = DA_IN_A__X_;
						result->a1 = (INT_PTR) r[R_A];
						result->a2 = mem_read(memc, addr++);
						break;
					case 4:	
						result->index = (prefix ? DA_EX__SP__RI : DA_EX__SP__HL);
						result->a1 = (INT_PTR) rp[R_SP];
						result->a2 = (INT_PTR) (prefix ? ri[pi] : rp[R_HL]);
						break;
					case 5: 
						result->index = DA_EX_DE_HL; 
						result->a1 = (INT_PTR) rp[R_DE];
						result->a2 = (INT_PTR) rp[R_HL];
						break;
					case 6: result->index = DA_DI; break;
					case 7: result->index = DA_EI; break;
				}
			} else
			if (z == 4) {
				result->index = DA_CALL_CC_X;
				result->a1 = (INT_PTR) cc[y];
				result->a2 = mem_read16(memc, addr);
			} else
			if (z == 5) {
				if (q == 0) {
					result->index = DA_PUSH_RP;
					result->a1 = (INT_PTR) (prefix ? rp2i[pi][p] : rp2[p]);
				} else {
					if (p == 0) {
						result->index = DA_CALL_X;
						result->a1 = mem_read16(memc, addr);

						if ((result->a1 == 0x0050) && lpDebuggerCalc->model >= TI_83P) {
							result->index = DA_BJUMP;
							result->a1 = mem_read16(memc, addr);
							TCHAR* Name = FindBcall((int) result->a1);
							if (Name == NULL) {
								result->index = DA_BJUMP_N;
							} else {
								result->a1 = (INT_PTR) Name;
							}
						}

					}
				}					
			} else
			if (z == 6) {
				switch (y) {
					case 0:
						result->index = DA_ADD_HL_RP;
						result->a1 = (INT_PTR) r[R_A];
						break;
					case 1:
						result->index = DA_ADC_HL_RP;
						result->a1 = (INT_PTR) r[R_A];
						break;
					case 3:
						result->index = DA_SBC_HL_RP;
						result->a1 = (INT_PTR) r[R_A];
						break;
					default:
						result->index = DA_ALU;
						result->a1 = (INT_PTR) alu[y];
						break;
				}
				result->index = DA_ALU_X;
				result->a1 = (INT_PTR) alu[y];
				result->a2 = (INT_PTR) mem_read(memc, addr++);
			} else
			if (z == 7) {
				if ((y == 5) && (lpDebuggerCalc->model >= TI_83P)) {
					result->index = DA_BCALL;
					int tmp = mem_read16(memc, addr);
					TCHAR* Name = FindBcall(tmp);
					if (Name == NULL) {
						result->index = DA_BCALL_N;
						result->a1 = tmp;
					} else {
						result->a1 = (INT_PTR) Name;
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
