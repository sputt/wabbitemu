#include "stdafx.h"

#include "opcodes.h"

#define NumElm(array) (sizeof (array) / sizeof ((array)[0]))

instr cp_instrs[] = {
	{"IXH", {0xBC, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYH", {0xBC, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"IXL", {0xBD, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYL", {0xBD, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"(IX@)", {0xBE, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"(IY@)", {0xBE, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"B", {0xB8}, 1, 4, 4, false, 0x00, 1},
	{"C", {0xB9}, 1, 4, 4, false, 0x00, 1},
	{"D", {0xBA}, 1, 4, 4, false, 0x00, 1},
	{"E", {0xBB}, 1, 4, 4, false, 0x00, 1},
	{"H", {0xBC}, 1, 4, 4, false, 0x00, 1},
	{"L", {0xBD}, 1, 4, 4, false, 0x00, 1},
	{"(HL)", {0xBE}, 1, 7, 7, false, 0x00, 1},
	{"A", {0xBF}, 1, 4, 4, false, 0x00, 1},
	{"&", {0xFE}, 1, 7, 7, false, 0x00, 2}};

instr di_instrs[] = {
	{"", {0xF3}, 1, 4, 4, false, 0x00, 1}};

instr ei_instrs[] = {
	{"", {0xFB}, 1, 4, 4, false, 0x00, 1}};

instr ex_instrs[] = {
	{"(SP),IX", {0xE3, 0xDD}, 2, 23, 23, false, 0x00, 2},
	{"(SP),IY", {0xE3, 0xFD}, 2, 23, 23, false, 0x00, 2},
	{"AF,AF'", {0x08}, 1, 4, 4, false, 0x00, 1},
	{"(SP),HL", {0xE3}, 1, 19, 19, false, 0x00, 1},
	{"DE,HL", {0xEB}, 1, 4, 4, false, 0x00, 1}};

instr im_instrs[] = {
	{"0", {0x46, 0xED}, 2, 8, 8, false, 0x00, 2},
	{"1", {0x56, 0xED}, 2, 8, 8, false, 0x00, 2},
	{"2", {0x5E, 0xED}, 2, 8, 8, false, 0x00, 2}};

instr in_instrs[] = {
	{"A,(C)", {0x78, 0xED}, 2, 12, 12, false, 0x00, 2},
	{"A,(&)", {0xDB}, 1, 11, 11, false, 0x00, 2},
	{"B,(C)", {0x40, 0xED}, 2, 12, 12, false, 0x00, 2},
	{"C,(C)", {0x48, 0xED}, 2, 12, 12, false, 0x00, 2},
	{"D,(C)", {0x50, 0xED}, 2, 12, 12, false, 0x00, 2},
	{"E,(C)", {0x58, 0xED}, 2, 12, 12, false, 0x00, 2},
	{"H,(C)", {0x60, 0xED}, 2, 12, 12, false, 0x00, 2},
	{"L,(C)", {0x68, 0xED}, 2, 12, 12, false, 0x00, 2},
	{"F,(C)", {0x70, 0xED}, 2, 0, 0, false, 0x00, 2}};

instr jp_instrs[] = {
	{"(IX)", {0xE9, 0xDD}, 2, 8, 8, false, 0x00, 2},
	{"(IY)", {0xE9, 0xFD}, 2, 8, 8, false, 0x00, 2},
	{"NZ,*", {0xC2}, 1, 10, 11, false, 0x00, 3},
	{"Z,*", {0xCA}, 1, 10, 11, false, 0x00, 3},
	{"NC,*", {0xD2}, 1, 10, 11, false, 0x00, 3},
	{"C,*", {0xDA}, 1, 10, 11, false, 0x00, 3},
	{"PO,*", {0xE2}, 1, 10, 11, false, 0x00, 3},
	{"(HL)", {0xE9}, 1, 4, 4, false, 0x00, 1},
	{"PE,*", {0xEA}, 1, 10, 11, false, 0x00, 3},
	{"P,*", {0xF2}, 1, 10, 11, false, 0x00, 3},
	{"M,*", {0xFA}, 1, 10, 11, false, 0x00, 3},
	{"*", {0xC3}, 1, 10, 10, false, 0x00, 3}};

instr jr_instrs[] = {
	{"NZ,%", {0x20}, 1, 7, 12, false, 0x00, 2},
	{"Z,%", {0x28}, 1, 7, 12, false, 0x00, 2},
	{"NC,%", {0x30}, 1, 7, 12, false, 0x00, 2},
	{"C,%", {0x38}, 1, 7, 12, false, 0x00, 2},
	{"%", {0x18}, 1, 7, 12, false, 0x00, 2}};

instr ld_instrs[] = {
	{"SP,HL", {0xF9}, 1, 6, 6, false, 0x00, 1},
	{"A,I", {0x57, 0xED}, 2, 9, 9, false, 0x00, 2},
	{"A,R", {0x5F, 0xED}, 2, 9, 9, false, 0x00, 2},
	{"IXH,B", {0x60, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYH,B", {0x60, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"IXH,C", {0x61, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYH,C", {0x61, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"IXH,D", {0x62, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYH,D", {0x62, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"IXH,E", {0x63, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYH,E", {0x63, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"IXH,IXH", {0x64, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYH,IYH", {0x64, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"IXH,IXL", {0x65, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYH,IYL", {0x65, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"H,(IX@)", {0x66, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"H,(IY@)", {0x66, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"IXH,A", {0x67, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYH,A", {0x67, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"IXL,B", {0x68, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYL,B", {0x68, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"IXL,C", {0x69, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYL,C", {0x69, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"IXL,D", {0x6A, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYL,D", {0x6A, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"IXL,E", {0x6B, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYL,E", {0x6B, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"IXL,IXH", {0x6C, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYL,IYH", {0x6C, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"IXL,IXL", {0x6D, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYL,IYL", {0x6D, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"L,(IX@)", {0x6E, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"L,(IY@)", {0x6E, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"IXL,A", {0x6F, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYL,A", {0x6F, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"(*),IX", {0x22, 0xDD}, 2, 20, 20, false, 0x00, 4},
	{"(*),IY", {0x22, 0xFD}, 2, 20, 20, false, 0x00, 4},
	{"IXH,&", {0x26, 0xDD}, 2, 0, 0, false, 0x00, 3},
	{"IYH,&", {0x26, 0xFD}, 2, 0, 0, false, 0x00, 3},
	{"IX,(*)", {0x2A, 0xDD}, 2, 20, 20, false, 0x00, 4},
	{"IY,(*)", {0x2A, 0xFD}, 2, 20, 20, false, 0x00, 4},
	{"IXL,&", {0x2E, 0xDD}, 2, 0, 0, false, 0x00, 3},
	{"IYL,&", {0x2E, 0xFD}, 2, 0, 0, false, 0x00, 3},
	{"B,IXH", {0x44, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"B,IYH", {0x44, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"B,IXL", {0x45, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"B,IYL", {0x45, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"B,(IX@)", {0x46, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"B,(IY@)", {0x46, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"C,IXH", {0x4C, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"C,IYH", {0x4C, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"C,IXL", {0x4D, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"C,IYL", {0x4D, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"C,(IX@)", {0x4E, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"C,(IY@)", {0x4E, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"D,IXH", {0x54, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"D,IYH", {0x54, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"D,IXL", {0x55, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"D,IYL", {0x55, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"D,(IX@)", {0x56, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"D,(IY@)", {0x56, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"E,IXH", {0x5C, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"E,IYH", {0x5C, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"E,IXL", {0x5D, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"E,IYL", {0x5D, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"E,(IX@)", {0x5E, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"E,(IY@)", {0x5E, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"(IX@),B", {0x70, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"(IY@),B", {0x70, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"(IX@),C", {0x71, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"(IY@),C", {0x71, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"(IX@),D", {0x72, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"(IY@),D", {0x72, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"(IX@),E", {0x73, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"(IY@),E", {0x73, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"(IX@),H", {0x74, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"(IY@),H", {0x74, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"(IX@),L", {0x75, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"(IY@),L", {0x75, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"(IX@),A", {0x77, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"(IY@),A", {0x77, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"A,IXH", {0x7C, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"A,IYH", {0x7C, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"A,IXL", {0x7D, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"A,IYL", {0x7D, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"A,(IX@)", {0x7E, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"A,(IY@)", {0x7E, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"(IX@),&", {0x36, 0xDD}, 2, 19, 19, false, 0x00, 4},
	{"(IY@),&", {0x36, 0xFD}, 2, 19, 19, false, 0x00, 4},
	{"SP,IX", {0xF9, 0xDD}, 2, 10, 10, false, 0x00, 2},
	{"SP,IY", {0xF9, 0xFD}, 2, 10, 10, false, 0x00, 2},
	{"B,B", {0x40}, 1, 4, 4, false, 0x00, 1},
	{"B,C", {0x41}, 1, 4, 4, false, 0x00, 1},
	{"B,D", {0x42}, 1, 4, 4, false, 0x00, 1},
	{"B,E", {0x43}, 1, 4, 4, false, 0x00, 1},
	{"B,H", {0x44}, 1, 4, 4, false, 0x00, 1},
	{"B,L", {0x45}, 1, 4, 4, false, 0x00, 1},
	{"B,(HL)", {0x46}, 1, 7, 7, false, 0x00, 1},
	{"B,A", {0x47}, 1, 4, 4, false, 0x00, 1},
	{"C,B", {0x48}, 1, 4, 4, false, 0x00, 1},
	{"C,C", {0x49}, 1, 4, 4, false, 0x00, 1},
	{"C,D", {0x4A}, 1, 4, 4, false, 0x00, 1},
	{"C,E", {0x4B}, 1, 4, 4, false, 0x00, 1},
	{"C,H", {0x4C}, 1, 4, 4, false, 0x00, 1},
	{"C,L", {0x4D}, 1, 4, 4, false, 0x00, 1},
	{"C,(HL)", {0x4E}, 1, 7, 7, false, 0x00, 1},
	{"C,A", {0x4F}, 1, 4, 4, false, 0x00, 1},
	{"D,B", {0x50}, 1, 4, 4, false, 0x00, 1},
	{"D,C", {0x51}, 1, 4, 4, false, 0x00, 1},
	{"D,D", {0x52}, 1, 4, 4, false, 0x00, 1},
	{"D,E", {0x53}, 1, 4, 4, false, 0x00, 1},
	{"D,H", {0x54}, 1, 4, 4, false, 0x00, 1},
	{"D,L", {0x55}, 1, 4, 4, false, 0x00, 1},
	{"D,(HL)", {0x56}, 1, 7, 7, false, 0x00, 1},
	{"D,A", {0x57}, 1, 4, 4, false, 0x00, 1},
	{"E,B", {0x58}, 1, 4, 4, false, 0x00, 1},
	{"E,C", {0x59}, 1, 4, 4, false, 0x00, 1},
	{"E,D", {0x5A}, 1, 4, 4, false, 0x00, 1},
	{"E,E", {0x5B}, 1, 4, 4, false, 0x00, 1},
	{"E,H", {0x5C}, 1, 4, 4, false, 0x00, 1},
	{"E,L", {0x5D}, 1, 4, 4, false, 0x00, 1},
	{"E,(HL)", {0x5E}, 1, 7, 7, false, 0x00, 1},
	{"E,A", {0x5F}, 1, 4, 4, false, 0x00, 1},
	{"H,B", {0x60}, 1, 4, 4, false, 0x00, 1},
	{"H,C", {0x61}, 1, 4, 4, false, 0x00, 1},
	{"H,D", {0x62}, 1, 4, 4, false, 0x00, 1},
	{"H,E", {0x63}, 1, 4, 4, false, 0x00, 1},
	{"H,H", {0x64}, 1, 4, 4, false, 0x00, 1},
	{"H,L", {0x65}, 1, 4, 4, false, 0x00, 1},
	{"H,(HL)", {0x66}, 1, 7, 7, false, 0x00, 1},
	{"H,A", {0x67}, 1, 4, 4, false, 0x00, 1},
	{"L,B", {0x68}, 1, 4, 4, false, 0x00, 1},
	{"L,C", {0x69}, 1, 4, 4, false, 0x00, 1},
	{"L,D", {0x6A}, 1, 4, 4, false, 0x00, 1},
	{"L,E", {0x6B}, 1, 4, 4, false, 0x00, 1},
	{"L,H", {0x6C}, 1, 4, 4, false, 0x00, 1},
	{"L,L", {0x6D}, 1, 4, 4, false, 0x00, 1},
	{"L,(HL)", {0x6E}, 1, 7, 7, false, 0x00, 1},
	{"L,A", {0x6F}, 1, 4, 4, false, 0x00, 1},
	{"(HL),B", {0x70}, 1, 7, 7, false, 0x00, 1},
	{"(HL),C", {0x71}, 1, 7, 7, false, 0x00, 1},
	{"(HL),D", {0x72}, 1, 7, 7, false, 0x00, 1},
	{"(HL),E", {0x73}, 1, 7, 7, false, 0x00, 1},
	{"(HL),H", {0x74}, 1, 7, 7, false, 0x00, 1},
	{"(HL),L", {0x75}, 1, 7, 7, false, 0x00, 1},
	{"(HL),A", {0x77}, 1, 7, 7, false, 0x00, 1},
	{"A,B", {0x78}, 1, 4, 4, false, 0x00, 1},
	{"A,C", {0x79}, 1, 4, 4, false, 0x00, 1},
	{"A,D", {0x7A}, 1, 4, 4, false, 0x00, 1},
	{"A,E", {0x7B}, 1, 4, 4, false, 0x00, 1},
	{"A,H", {0x7C}, 1, 4, 4, false, 0x00, 1},
	{"A,L", {0x7D}, 1, 4, 4, false, 0x00, 1},
	{"A,(HL)", {0x7E}, 1, 7, 7, false, 0x00, 1},
	{"A,A", {0x7F}, 1, 4, 4, false, 0x00, 1},
	{"(BC),A", {0x02}, 1, 7, 7, false, 0x00, 1},
	{"B,&", {0x06}, 1, 7, 7, false, 0x00, 2},
	{"A,(BC)", {0x0A}, 1, 7, 7, false, 0x00, 1},
	{"C,&", {0x0E}, 1, 7, 7, false, 0x00, 2},
	{"(DE),A", {0x12}, 1, 7, 7, false, 0x00, 1},
	{"D,&", {0x16}, 1, 7, 7, false, 0x00, 2},
	{"A,(DE)", {0x1A}, 1, 7, 7, false, 0x00, 1},
	{"E,&", {0x1E}, 1, 7, 7, false, 0x00, 2},
	{"(*),HL", {0x22}, 1, 16, 16, false, 0x00, 3},
	{"H,&", {0x26}, 1, 7, 7, false, 0x00, 2},
	{"HL,(*)", {0x2A}, 1, 20, 20, false, 0x00, 3},
	{"L,&", {0x2E}, 1, 7, 7, false, 0x00, 2},
	{"(*),A", {0x32}, 1, 13, 13, false, 0x00, 3},
	{"(HL),&", {0x36}, 1, 10, 10, false, 0x00, 2},
	{"A,(*)", {0x3A}, 1, 13, 13, false, 0x00, 3},
	{"A,&", {0x3E}, 1, 7, 7, false, 0x00, 2},
	{"(*),BC", {0x43, 0xED}, 2, 20, 20, false, 0x00, 4},
	{"I,A", {0x47, 0xED}, 2, 9, 9, false, 0x00, 2},
	{"BC,(*)", {0x4B, 0xED}, 2, 20, 20, false, 0x00, 4},
	{"R,A", {0x4F, 0xED}, 2, 9, 9, false, 0x00, 2},
	{"(*),DE", {0x53, 0xED}, 2, 20, 20, false, 0x00, 4},
	{"DE,(*)", {0x5B, 0xED}, 2, 20, 20, false, 0x00, 4},
	{"(*),HL", {0x63, 0xED}, 2, 16, 16, false, 0x00, 4},
	{"HL,(*)", {0x6B, 0xED}, 2, 20, 20, false, 0x00, 4},
	{"(*),SP", {0x73, 0xED}, 2, 20, 20, false, 0x00, 4},
	{"SP,(*)", {0x7B, 0xED}, 2, 20, 20, false, 0x00, 4},
	{"HL,*", {0x21}, 1, 10, 10, false, 0x00, 3},
	{"BC,*", {0x01}, 1, 10, 10, false, 0x00, 3},
	{"DE,*", {0x11}, 1, 10, 10, false, 0x00, 3},
	{"IX,*", {0x21, 0xDD}, 2, 14, 14, false, 0x00, 4},
	{"IY,*", {0x21, 0xFD}, 2, 14, 14, false, 0x00, 4},
	{"SP,*", {0x31}, 1, 10, 10, false, 0x00, 3},
	
};

instr or_instrs[] = {
	{"IXH", {0xB4, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYH", {0xB4, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"IXL", {0xB5, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYL", {0xB5, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"(IX@)", {0xB6, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"(IY@)", {0xB6, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"B", {0xB0}, 1, 4, 4, false, 0x00, 1},
	{"C", {0xB1}, 1, 4, 4, false, 0x00, 1},
	{"D", {0xB2}, 1, 4, 4, false, 0x00, 1},
	{"E", {0xB3}, 1, 4, 4, false, 0x00, 1},
	{"H", {0xB4}, 1, 4, 4, false, 0x00, 1},
	{"L", {0xB5}, 1, 4, 4, false, 0x00, 1},
	{"(HL)", {0xB6}, 1, 7, 7, false, 0x00, 1},
	{"A", {0xB7}, 1, 4, 4, false, 0x00, 1},
	{"&", {0xF6}, 1, 7, 7, false, 0x00, 2}};

instr rl_instrs[] = {
	{"B,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x10, 4},
	{"B,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x10, 4},
	{"C,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x11, 4},
	{"C,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x11, 4},
	{"D,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x12, 4},
	{"D,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x12, 4},
	{"E,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x13, 4},
	{"E,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x13, 4},
	{"H,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x14, 4},
	{"H,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x14, 4},
	{"L,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x15, 4},
	{"L,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x15, 4},
	{"(IX@)", {0xCB, 0xDD}, 2, 23, 23, true, 0x16, 4},
	{"(IY@)", {0xCB, 0xFD}, 2, 23, 23, true, 0x16, 4},
	{"A,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x17, 4},
	{"A,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x17, 4},
	{"B", {0x10, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"C", {0x11, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"D", {0x12, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"E", {0x13, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"H", {0x14, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"L", {0x15, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"(HL)", {0x16, 0xCB}, 2, 15, 15, false, 0x00, 2},
	{"A", {0x17, 0xCB}, 2, 8, 8, false, 0x00, 2}};

instr rr_instrs[] = {
	{"B,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x18, 4},
	{"B,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x18, 4},
	{"C,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x19, 4},
	{"C,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x19, 4},
	{"D,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x1A, 4},
	{"D,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x1A, 4},
	{"E,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x1B, 4},
	{"E,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x1B, 4},
	{"H,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x1C, 4},
	{"H,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x1C, 4},
	{"L,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x1D, 4},
	{"L,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x1D, 4},
	{"(IX@)", {0xCB, 0xDD}, 2, 23, 23, true, 0x1E, 4},
	{"(IY@)", {0xCB, 0xFD}, 2, 23, 23, true, 0x1E, 4},
	{"A,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x1F, 4},
	{"A,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x1F, 4},
	{"B", {0x18, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"C", {0x19, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"D", {0x1A, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"E", {0x1B, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"H", {0x1C, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"L", {0x1D, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"(HL)", {0x1E, 0xCB}, 2, 15, 15, false, 0x00, 2},
	{"A", {0x1F, 0xCB}, 2, 8, 8, false, 0x00, 2}};

instr adc_instrs[] = {
	{"A,IXH", {0x8C, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"A,IYH", {0x8C, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"A,IXL", {0x8D, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"A,IYL", {0x8D, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"A,(IX@)", {0x8E, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"A,(IY@)", {0x8E, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"A,B", {0x88}, 1, 4, 4, false, 0x00, 1},
	{"A,C", {0x89}, 1, 4, 4, false, 0x00, 1},
	{"A,D", {0x8A}, 1, 4, 4, false, 0x00, 1},
	{"A,E", {0x8B}, 1, 4, 4, false, 0x00, 1},
	{"A,H", {0x8C}, 1, 4, 4, false, 0x00, 1},
	{"A,L", {0x8D}, 1, 7, 7, false, 0x00, 1},
	{"A,(HL)", {0x8E}, 1, 7, 7, false, 0x00, 1},
	{"A,A", {0x8F}, 1, 4, 4, false, 0x00, 1},
	{"A,&", {0xCE}, 1, 7, 7, false, 0x00, 2},
	{"HL,BC", {0x4A, 0xED}, 2, 15, 15, false, 0x00, 2},
	{"HL,DE", {0x5A, 0xED}, 2, 15, 15, false, 0x00, 2},
	{"HL,HL", {0x6A, 0xED}, 2, 15, 15, false, 0x00, 2},
	{"HL,SP", {0x7A, 0xED}, 2, 15, 15, false, 0x00, 2}};

instr add_instrs[] = {
	{"IX,BC", {0x09, 0xDD}, 2, 15, 15, false, 0x00, 2},
	{"IY,BC", {0x09, 0xFD}, 2, 15, 15, false, 0x00, 2},
	{"IX,DE", {0x19, 0xDD}, 2, 15, 15, false, 0x00, 2},
	{"IY,DE", {0x19, 0xFD}, 2, 15, 15, false, 0x00, 2},
	{"IX,IX", {0x29, 0xDD}, 2, 15, 15, false, 0x00, 2},
	{"IY,IY", {0x29, 0xFD}, 2, 15, 15, false, 0x00, 2},
	{"IX,SP", {0x39, 0xDD}, 2, 15, 15, false, 0x00, 2},
	{"IY,SP", {0x39, 0xFD}, 2, 15, 15, false, 0x00, 2},
	{"A,IXH", {0x84, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"A,IYH", {0x84, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"A,IXL", {0x85, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"A,IYL", {0x85, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"A,(IX@)", {0x86, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"A,(IY@)", {0x86, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"A,B", {0x80}, 1, 4, 4, false, 0x00, 1},
	{"A,C", {0x81}, 1, 4, 4, false, 0x00, 1},
	{"A,D", {0x82}, 1, 4, 4, false, 0x00, 1},
	{"A,E", {0x83}, 1, 4, 4, false, 0x00, 1},
	{"A,H", {0x84}, 1, 4, 4, false, 0x00, 1},
	{"A,L", {0x85}, 1, 4, 4, false, 0x00, 1},
	{"A,(HL)", {0x86}, 1, 7, 7, false, 0x00, 1},
	{"A,A", {0x87}, 1, 4, 4, false, 0x00, 1},
	{"HL,BC", {0x09}, 1, 11, 11, false, 0x00, 1},
	{"HL,DE", {0x19}, 1, 11, 11, false, 0x00, 1},
	{"HL,HL", {0x29}, 1, 11, 11, false, 0x00, 1},
	{"HL,SP", {0x39}, 1, 11, 11, false, 0x00, 1},
	{"A,&", {0xC6}, 1, 7, 7, false, 0x00, 2}};

instr and_instrs[] = {
	{"IXH", {0xA4, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYH", {0xA4, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"IXL", {0xA5, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYL", {0xA5, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"(IX@)", {0xA6, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"(IY@)", {0xA6, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"B", {0xA0}, 1, 4, 4, false, 0x00, 1},
	{"C", {0xA1}, 1, 4, 4, false, 0x00, 1},
	{"D", {0xA2}, 1, 4, 4, false, 0x00, 1},
	{"E", {0xA3}, 1, 4, 4, false, 0x00, 1},
	{"H", {0xA4}, 1, 4, 4, false, 0x00, 1},
	{"L", {0xA5}, 1, 4, 4, false, 0x00, 1},
	{"(HL)", {0xA6}, 1, 7, 7, false, 0x00, 1},
	{"A", {0xA7}, 1, 4, 4, false, 0x00, 1},
	{"&", {0xE6}, 1, 7, 7, false, 0x00, 2}};

instr bit_instrs[] = {
	{"^,(IX@)", {0xCB, 0xDD}, 2, 20, 20, true, 0x46, 4},
	{"^,(IY@)", {0xCB, 0xFD}, 2, 20, 20, true, 0x46, 4},
	{"^,B", {0xCB}, 1, 8, 8, true, 0x40, 2},
	{"^,C", {0xCB}, 1, 8, 8, true, 0x41, 2},
	{"^,D", {0xCB}, 1, 8, 8, true, 0x42, 2},
	{"^,E", {0xCB}, 1, 8, 8, true, 0x43, 2},
	{"^,H", {0xCB}, 1, 8, 8, true, 0x44, 2},
	{"^,L", {0xCB}, 1, 8, 8, true, 0x45, 2},
	{"^,(HL)", {0xCB}, 1, 12, 12, true, 0x46, 2},
	{"^,A", {0xCB}, 1, 8, 8, true, 0x47, 2}};

instr ccf_instrs[] = {
	{"", {0x3F}, 1, 4, 4, false, 0x00, 1}};

instr cpd_instrs[] = {
	{"", {0xA9, 0xED}, 2, 16, 16, false, 0x00, 2}};

instr cpi_instrs[] = {
	{"", {0xA1, 0xED}, 2, 16, 16, false, 0x00, 2}};

instr cpl_instrs[] = {
	{"", {0x2F}, 1, 4, 4, false, 0x00, 1}};

instr daa_instrs[] = {
	{"", {0x27}, 1, 4, 4, false, 0x00, 1}};

instr dec_instrs[] = {
	{"IXH", {0x25, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYH", {0x25, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"IX", {0x2B, 0xDD}, 2, 10, 10, false, 0x00, 2},
	{"IY", {0x2B, 0xFD}, 2, 10, 10, false, 0x00, 2},
	{"IXL", {0x2D, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYL", {0x2D, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"(IX@)", {0x35, 0xDD}, 2, 23, 23, false, 0x00, 3},
	{"(IY@)", {0x35, 0xFD}, 2, 23, 23, false, 0x00, 3},
	{"B", {0x05}, 1, 4, 4, false, 0x00, 1},
	{"BC", {0x0B}, 1, 6, 6, false, 0x00, 1},
	{"C", {0x0D}, 1, 4, 4, false, 0x00, 1},
	{"D", {0x15}, 1, 4, 4, false, 0x00, 1},
	{"DE", {0x1B}, 1, 6, 6, false, 0x00, 1},
	{"E", {0x1D}, 1, 4, 4, false, 0x00, 1},
	{"H", {0x25}, 1, 4, 4, false, 0x00, 1},
	{"HL", {0x2B}, 1, 6, 6, false, 0x00, 1},
	{"L", {0x2D}, 1, 4, 4, false, 0x00, 1},
	{"(HL)", {0x35}, 1, 11, 11, false, 0x00, 1},
	{"SP", {0x3B}, 1, 6, 6, false, 0x00, 1},
	{"A", {0x3D}, 1, 4, 4, false, 0x00, 1}};

instr exx_instrs[] = {
	{"", {0xD9}, 1, 4, 4, false, 0x00, 1}};

instr inc_instrs[] = {
	{"IX", {0x23, 0xDD}, 2, 10, 10, false, 0x00, 2},
	{"IY", {0x23, 0xFD}, 2, 10, 10, false, 0x00, 2},
	{"IXH", {0x24, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYH", {0x24, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"IXL", {0x2C, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYL", {0x2C, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"(IX@)", {0x34, 0xDD}, 2, 23, 23, false, 0x00, 3},
	{"(IY@)", {0x34, 0xFD}, 2, 23, 23, false, 0x00, 3},
	{"BC", {0x03}, 1, 6, 6, false, 0x00, 1},
	{"B", {0x04}, 1, 4, 4, false, 0x00, 1},
	{"C", {0x0C}, 1, 4, 4, false, 0x00, 1},
	{"DE", {0x13}, 1, 6, 6, false, 0x00, 1},
	{"D", {0x14}, 1, 4, 4, false, 0x00, 1},
	{"E", {0x1C}, 1, 4, 4, false, 0x00, 1},
	{"HL", {0x23}, 1, 6, 6, false, 0x00, 1},
	{"H", {0x24}, 1, 4, 4, false, 0x00, 1},
	{"L", {0x2C}, 1, 4, 4, false, 0x00, 1},
	{"SP", {0x33}, 1, 6, 6, false, 0x00, 1},
	{"(HL)", {0x34}, 1, 11, 11, false, 0x00, 1},
	{"A", {0x3C}, 1, 4, 4, false, 0x00, 1}};

instr ind_instrs[] = {
	{"", {0xAA, 0xED}, 2, 16, 16, false, 0x00, 2}};

instr ini_instrs[] = {
	{"", {0xA2, 0xED}, 2, 16, 16, false, 0x00, 2}};

instr ldd_instrs[] = {
	{"", {0xA8, 0xED}, 2, 16, 16, false, 0x00, 2}};

instr ldi_instrs[] = {
	{"", {0xA0, 0xED}, 2, 16, 16, false, 0x00, 2}};

instr neg_instrs[] = {
	{"", {0x44, 0xED}, 2, 8, 8, false, 0x00, 2}};

instr nop_instrs[] = {
	{"", {0x00}, 1, 4, 4, false, 0x00, 1}};

instr out_instrs[] = {
	{"(C),A", {0x79, 0xED}, 2, 12, 12, false, 0x00, 2},
	{"(&),A", {0xD3}, 1, 11, 11, false, 0x00, 2},
	{"(C),B", {0x41, 0xED}, 2, 12, 12, false, 0x00, 2},
	{"(C),C", {0x49, 0xED}, 2, 12, 12, false, 0x00, 2},
	{"(C),D", {0x51, 0xED}, 2, 12, 12, false, 0x00, 2},
	{"(C),E", {0x59, 0xED}, 2, 12, 12, false, 0x00, 2},
	{"(C),H", {0x61, 0xED}, 2, 12, 12, false, 0x00, 2},
	{"(C),L", {0x69, 0xED}, 2, 12, 12, false, 0x00, 2},
	{"(C),0", {0x71, 0xED}, 2, 0, 0, false, 0x00, 2}};

instr pop_instrs[] = {
	{"IX", {0xE1, 0xDD}, 2, 14, 14, false, 0x00, 2},
	{"IY", {0xE1, 0xFD}, 2, 14, 14, false, 0x00, 2},
	{"BC", {0xC1}, 1, 10, 10, false, 0x00, 1},
	{"DE", {0xD1}, 1, 10, 10, false, 0x00, 1},
	{"HL", {0xE1}, 1, 10, 10, false, 0x00, 1},
	{"AF", {0xF1}, 1, 10, 10, false, 0x00, 1}};

instr res_instrs[] = {
	{"B,^,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x80, 4},
	{"B,^,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x80, 4},
	{"C,^,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x81, 4},
	{"C,^,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x81, 4},
	{"D,^,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x82, 4},
	{"D,^,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x82, 4},
	{"E,^,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x83, 4},
	{"E,^,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x83, 4},
	{"H,^,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x84, 4},
	{"H,^,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x84, 4},
	{"L,^,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x85, 4},
	{"L,^,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x85, 4},
	{"^,(IX@)", {0xCB, 0xDD}, 2, 23, 23, true, 0x86, 4},
	{"^,(IY@)", {0xCB, 0xFD}, 2, 23, 23, true, 0x86, 4},
	{"A,^,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x87, 4},
	{"A,^,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x87, 4},
	{"^,B", {0xCB}, 1, 8, 8, true, 0x80, 2},
	{"^,C", {0xCB}, 1, 8, 8, true, 0x81, 2},
	{"^,D", {0xCB}, 1, 8, 8, true, 0x82, 2},
	{"^,E", {0xCB}, 1, 8, 8, true, 0x83, 2},
	{"^,H", {0xCB}, 1, 8, 8, true, 0x84, 2},
	{"^,L", {0xCB}, 1, 8, 8, true, 0x85, 2},
	{"^,(HL)", {0xCB}, 1, 15, 15, true, 0x86, 2},
	{"^,A", {0xCB}, 1, 8, 8, true, 0x87, 2}};

instr ret_instrs[] = {
	{"", {0xC9}, 1, 0, 0, false, 0x00, 1},
	{"NZ", {0xC0}, 1, 5, 11, false, 0x00, 1},
	{"Z", {0xC8}, 1, 5, 11, false, 0x00, 1},
	{"NC", {0xD0}, 1, 5, 11, false, 0x00, 1},
	{"C", {0xD8}, 1, 5, 11, false, 0x00, 1},
	{"PO", {0xE0}, 1, 5, 11, false, 0x00, 1},
	{"PE", {0xE8}, 1, 5, 11, false, 0x00, 1},
	{"P", {0xF0}, 1, 5, 11, false, 0x00, 1},
	{"M", {0xF8}, 1, 5, 11, false, 0x00, 1}};

instr rla_instrs[] = {
	{"", {0x17}, 1, 4, 4, false, 0x00, 1}};

instr rlc_instrs[] = {
	{"B,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x00, 4},
	{"B,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x00, 4},
	{"C,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x01, 4},
	{"C,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x01, 4},
	{"D,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x02, 4},
	{"D,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x02, 4},
	{"E,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x03, 4},
	{"E,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x03, 4},
	{"H,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x04, 4},
	{"H,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x04, 4},
	{"L,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x05, 4},
	{"L,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x05, 4},
	{"(IX@)", {0xCB, 0xDD}, 2, 23, 23, true, 0x06, 4},
	{"(IY@)", {0xCB, 0xFD}, 2, 23, 23, true, 0x06, 4},
	{"A,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x07, 4},
	{"A,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x07, 4},
	{"B", {0x00, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"C", {0x01, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"D", {0x02, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"E", {0x03, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"H", {0x04, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"L", {0x05, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"(HL)", {0x06, 0xCB}, 2, 15, 15, false, 0x00, 2},
	{"A", {0x07, 0xCB}, 2, 8, 8, false, 0x00, 2}};

instr rld_instrs[] = {
	{"", {0x6F, 0xED}, 2, 18, 18, false, 0x00, 2}};

instr rra_instrs[] = {
	{"", {0x1F}, 1, 4, 4, false, 0x00, 1}};

instr rrc_instrs[] = {
	{"B,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x08, 4},
	{"B,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x08, 4},
	{"C,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x09, 4},
	{"C,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x09, 4},
	{"D,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x0A, 4},
	{"D,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x0A, 4},
	{"E,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x0B, 4},
	{"E,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x0B, 4},
	{"H,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x0C, 4},
	{"H,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x0C, 4},
	{"L,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x0D, 4},
	{"L,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x0D, 4},
	{"(IX@)", {0xCB, 0xDD}, 2, 23, 23, true, 0x0E, 4},
	{"(IY@)", {0xCB, 0xFD}, 2, 23, 23, true, 0x0E, 4},
	{"A,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x0F, 4},
	{"A,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x0F, 4},
	{"B", {0x08, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"C", {0x09, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"D", {0x0A, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"E", {0x0B, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"H", {0x0C, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"L", {0x0D, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"(HL)", {0x0E, 0xCB}, 2, 15, 15, false, 0x00, 2},
	{"A", {0x0F, 0xCB}, 2, 8, 8, false, 0x00, 2}};

instr rrd_instrs[] = {
	{"", {0x67, 0xED}, 2, 18, 18, false, 0x00, 2}};

instr rst_instrs[] = {
	{"00H", {0xC7}, 1, 11, 11, false, 0x00, 1},
	{"08H", {0xCF}, 1, 11, 11, false, 0x00, 1},
	{"10H", {0xD7}, 1, 11, 11, false, 0x00, 1},
	{"18H", {0xDF}, 1, 11, 11, false, 0x00, 1},
	{"20H", {0xE7}, 1, 11, 11, false, 0x00, 1},
	{"28H", {0xEF}, 1, 11, 11, false, 0x00, 1},
	{"30H", {0xF7}, 1, 11, 11, false, 0x00, 1},
	{"38H", {0xFF}, 1, 11, 11, false, 0x00, 1},
	{"#",   {0xFF}, 0, 11, 11, false, 0x00, 1}};

instr sbc_instrs[] = {
	{"A,IXH", {0x9C, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"A,IYH", {0x9C, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"A,IXL", {0x9D, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"A,IYL", {0x9D, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"A,(IX@)", {0x9E, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"A,(IY@)", {0x9E, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"A,B", {0x98}, 1, 4, 4, false, 0x00, 1},
	{"A,C", {0x99}, 1, 4, 4, false, 0x00, 1},
	{"A,D", {0x9A}, 1, 4, 4, false, 0x00, 1},
	{"A,E", {0x9B}, 1, 4, 4, false, 0x00, 1},
	{"A,H", {0x9C}, 1, 4, 4, false, 0x00, 1},
	{"A,L", {0x9D}, 1, 4, 4, false, 0x00, 1},
	{"A,(HL)", {0x9E}, 1, 7, 7, false, 0x00, 1},
	{"A,A", {0x9F}, 1, 4, 4, false, 0x00, 1},
	{"A,&", {0xDE}, 1, 7, 7, false, 0x00, 2},
	{"HL,BC", {0x42, 0xED}, 2, 15, 15, false, 0x00, 2},
	{"HL,DE", {0x52, 0xED}, 2, 15, 15, false, 0x00, 2},
	{"HL,HL", {0x62, 0xED}, 2, 15, 15, false, 0x00, 2},
	{"HL,SP", {0x72, 0xED}, 2, 15, 15, false, 0x00, 2}};

instr scf_instrs[] = {
	{"", {0x37}, 1, 4, 4, false, 0x00, 1}};

instr set_instrs[] = {
	{"B,^,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0xC0, 4},
	{"B,^,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0xC0, 4},
	{"C,^,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0xC1, 4},
	{"C,^,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0xC1, 4},
	{"D,^,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0xC2, 4},
	{"D,^,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0xC2, 4},
	{"E,^,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0xC3, 4},
	{"E,^,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0xC3, 4},
	{"H,^,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0xC4, 4},
	{"H,^,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0xC4, 4},
	{"L,^,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0xC5, 4},
	{"L,^,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0xC5, 4},
	{"^,(IX@)", {0xCB, 0xDD}, 2, 23, 23, true, 0xC6, 4},
	{"^,(IY@)", {0xCB, 0xFD}, 2, 23, 23, true, 0xC6, 4},
	{"A,^,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0xC7, 4},
	{"A,^,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0xC7, 4},
	{"^,B", {0xCB}, 1, 8, 8, true, 0xC0, 2},
	{"^,C", {0xCB}, 1, 8, 8, true, 0xC1, 2},
	{"^,D", {0xCB}, 1, 8, 8, true, 0xC2, 2},
	{"^,E", {0xCB}, 1, 8, 8, true, 0xC3, 2},
	{"^,H", {0xCB}, 1, 8, 8, true, 0xC4, 2},
	{"^,L", {0xCB}, 1, 8, 8, true, 0xC5, 2},
	{"^,(HL)", {0xCB}, 1, 15, 15, true, 0xC6, 2},
	{"^,A", {0xCB}, 1, 8, 8, true, 0xC7, 2}};

instr sl1_instrs[] = {
	{"B,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x30, 4},
	{"B,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x30, 4},
	{"C,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x31, 4},
	{"C,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x31, 4},
	{"D,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x32, 4},
	{"D,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x32, 4},
	{"E,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x33, 4},
	{"E,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x33, 4},
	{"H,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x34, 4},
	{"H,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x34, 4},
	{"L,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x35, 4},
	{"L,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x35, 4},
	{"(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x36, 4},
	{"(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x36, 4},
	{"A,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x37, 4},
	{"A,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x37, 4},
	{"B", {0x30, 0xCB}, 2, 0, 0, false, 0x00, 2},
	{"C", {0x31, 0xCB}, 2, 0, 0, false, 0x00, 2},
	{"D", {0x32, 0xCB}, 2, 0, 0, false, 0x00, 2},
	{"E", {0x33, 0xCB}, 2, 0, 0, false, 0x00, 2},
	{"H", {0x34, 0xCB}, 2, 0, 0, false, 0x00, 2},
	{"L", {0x35, 0xCB}, 2, 0, 0, false, 0x00, 2},
	{"(HL)", {0x36, 0xCB}, 2, 0, 0, false, 0x00, 2},
	{"A", {0x37, 0xCB}, 2, 0, 0, false, 0x00, 2}};

instr sla_instrs[] = {
	{"B,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x20, 4},
	{"B,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x20, 4},
	{"C,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x21, 4},
	{"C,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x21, 4},
	{"D,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x22, 4},
	{"D,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x22, 4},
	{"E,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x23, 4},
	{"E,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x23, 4},
	{"H,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x24, 4},
	{"H,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x24, 4},
	{"L,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x25, 4},
	{"L,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x25, 4},
	{"(IX@)", {0xCB, 0xDD}, 2, 23, 23, true, 0x26, 4},
	{"(IY@)", {0xCB, 0xFD}, 2, 23, 23, true, 0x26, 4},
	{"A,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x27, 4},
	{"A,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x27, 4},
	{"B", {0x20, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"C", {0x21, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"D", {0x22, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"E", {0x23, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"H", {0x24, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"L", {0x25, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"(HL)", {0x26, 0xCB}, 2, 15, 15, false, 0x00, 2},
	{"A", {0x27, 0xCB}, 2, 8, 8, false, 0x00, 2}};

instr sll_instrs[] = {
	{"B,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x30, 4},
	{"B,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x30, 4},
	{"C,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x31, 4},
	{"C,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x31, 4},
	{"D,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x32, 4},
	{"D,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x32, 4},
	{"E,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x33, 4},
	{"E,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x33, 4},
	{"H,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x34, 4},
	{"H,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x34, 4},
	{"L,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x35, 4},
	{"L,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x35, 4},
	{"(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x36, 4},
	{"(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x36, 4},
	{"A,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x37, 4},
	{"A,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x37, 4},
	{"B", {0x30, 0xCB}, 2, 0, 0, false, 0x00, 2},
	{"C", {0x31, 0xCB}, 2, 0, 0, false, 0x00, 2},
	{"D", {0x32, 0xCB}, 2, 0, 0, false, 0x00, 2},
	{"E", {0x33, 0xCB}, 2, 0, 0, false, 0x00, 2},
	{"H", {0x34, 0xCB}, 2, 0, 0, false, 0x00, 2},
	{"L", {0x35, 0xCB}, 2, 0, 0, false, 0x00, 2},
	{"(HL)", {0x36, 0xCB}, 2, 0, 0, false, 0x00, 2},
	{"A", {0x37, 0xCB}, 2, 0, 0, false, 0x00, 2}};

instr sra_instrs[] = {
	{"B,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x28, 4},
	{"B,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x28, 4},
	{"C,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x29, 4},
	{"C,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x29, 4},
	{"D,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x2A, 4},
	{"D,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x2A, 4},
	{"E,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x2B, 4},
	{"E,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x2B, 4},
	{"H,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x2C, 4},
	{"H,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x2C, 4},
	{"L,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x2D, 4},
	{"L,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x2D, 4},
	{"(IX@)", {0xCB, 0xDD}, 2, 23, 23, true, 0x2E, 4},
	{"(IY@)", {0xCB, 0xFD}, 2, 23, 23, true, 0x2E, 4},
	{"A,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x2F, 4},
	{"A,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x2F, 4},
	{"B", {0x28, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"C", {0x29, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"D", {0x2A, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"E", {0x2B, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"H", {0x2C, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"L", {0x2D, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"(HL)", {0x2E, 0xCB}, 2, 15, 15, false, 0x00, 2},
	{"A", {0x2F, 0xCB}, 2, 8, 8, false, 0x00, 2}};

instr srl_instrs[] = {
	{"B,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x38, 4},
	{"B,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x38, 4},
	{"C,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x39, 4},
	{"C,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x39, 4},
	{"D,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x3A, 4},
	{"D,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x3A, 4},
	{"E,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x3B, 4},
	{"E,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x3B, 4},
	{"H,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x3C, 4},
	{"H,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x3C, 4},
	{"L,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x3D, 4},
	{"L,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x3D, 4},
	{"(IX@)", {0xCB, 0xDD}, 2, 23, 23, true, 0x3E, 4},
	{"(IY@)", {0xCB, 0xFD}, 2, 23, 23, true, 0x3E, 4},
	{"A,(IX@)", {0xCB, 0xDD}, 2, 0, 0, true, 0x3F, 4},
	{"A,(IY@)", {0xCB, 0xFD}, 2, 0, 0, true, 0x3F, 4},
	{"B", {0x38, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"C", {0x39, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"D", {0x3A, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"E", {0x3B, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"H", {0x3C, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"L", {0x3D, 0xCB}, 2, 8, 8, false, 0x00, 2},
	{"(HL)", {0x3E, 0xCB}, 2, 15, 15, false, 0x00, 2},
	{"A", {0x3F, 0xCB}, 2, 8, 8, false, 0x00, 2}};

instr sub_instrs[] = {
	{"IXH", {0x94, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYH", {0x94, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"IXL", {0x95, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYL", {0x95, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"(IX@)", {0x96, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"(IY@)", {0x96, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"B", {0x90}, 1, 4, 4, false, 0x00, 1},
	{"C", {0x91}, 1, 4, 4, false, 0x00, 1},
	{"D", {0x92}, 1, 4, 4, false, 0x00, 1},
	{"E", {0x93}, 1, 4, 4, false, 0x00, 1},
	{"H", {0x94}, 1, 4, 4, false, 0x00, 1},
	{"L", {0x95}, 1, 4, 4, false, 0x00, 1},
	{"(HL)", {0x96}, 1, 7, 7, false, 0x00, 1},
	{"A", {0x97}, 1, 4, 4, false, 0x00, 1},
	{"&", {0xD6}, 1, 7, 7, false, 0x00, 2}};

instr xor_instrs[] = {
	{"IXH", {0xAC, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYH", {0xAC, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"IXL", {0xAD, 0xDD}, 2, 0, 0, false, 0x00, 2},
	{"IYL", {0xAD, 0xFD}, 2, 0, 0, false, 0x00, 2},
	{"(IX@)", {0xAE, 0xDD}, 2, 19, 19, false, 0x00, 3},
	{"(IY@)", {0xAE, 0xFD}, 2, 19, 19, false, 0x00, 3},
	{"A", {0xAF}, 1, 4, 4, false, 0x00, 1},
	{"B", {0xA8}, 1, 4, 4, false, 0x00, 1},
	{"C", {0xA9}, 1, 4, 4, false, 0x00, 1},
	{"D", {0xAA}, 1, 4, 4, false, 0x00, 1},
	{"E", {0xAB}, 1, 4, 4, false, 0x00, 1},
	{"H", {0xAC}, 1, 4, 4, false, 0x00, 1},
	{"L", {0xAD}, 1, 4, 4, false, 0x00, 1},
	{"(HL)", {0xAE}, 1, 7, 7, false, 0x00, 1},
	{"&", {0xEE}, 1, 7, 7, false, 0x00, 2}};

instr call_instrs[] = {
	{"NZ,*", {0xC4}, 1, 10, 17, false, 0x00, 3},
	{"Z,*", {0xCC}, 1, 10, 17, false, 0x00, 3},
	{"NC,*", {0xD4}, 1, 10, 17, false, 0x00, 3},
	{"C,*", {0xDC}, 1, 10, 17, false, 0x00, 3},
	{"PO,*", {0xE4}, 1, 10, 17, false, 0x00, 3},
	{"PE,*", {0xEC}, 1, 10, 17, false, 0x00, 3},
	{"P,*", {0xF4}, 1, 10, 17, false, 0x00, 3},
	{"M,*", {0xFC}, 1, 10, 17, false, 0x00, 3},
	{"*", {0xCD}, 1, 17, 17, false, 0x00, 3}};

instr cpdr_instrs[] = {
	{"", {0xB9, 0xED}, 2, 21, 21, false, 0x00, 2}};

instr cpir_instrs[] = {
	{"", {0xB1, 0xED}, 2, 21, 21, false, 0x00, 2}};

instr djnz_instrs[] = {
	{"%", {0x10}, 1, 8, 13, false, 0x00, 2}};

instr halt_instrs[] = {
	{"", {0x76}, 1, 4, 4, false, 0x00, 1}};

instr indr_instrs[] = {
	{"", {0xBA, 0xED}, 2, 21, 21, false, 0x00, 2}};

instr inir_instrs[] = {
	{"", {0xB2, 0xED}, 2, 21, 21, false, 0x00, 2}};

instr lddr_instrs[] = {
	{"", {0xB8, 0xED}, 2, 21, 21, false, 0x00, 2}};

instr ldir_instrs[] = {
	{"", {0xB0, 0xED}, 2, 21, 21, false, 0x00, 2}};

instr otdr_instrs[] = {
	{"", {0xBB, 0xED}, 2, 21, 21, false, 0x00, 2}};

instr otir_instrs[] = {
	{"", {0xB3, 0xED}, 2, 21, 21, false, 0x00, 2}};

instr outd_instrs[] = {
	{"", {0xAB, 0xED}, 2, 16, 16, false, 0x00, 2}};

instr outi_instrs[] = {
	{"", {0xA3, 0xED}, 2, 16, 16, false, 0x00, 2}};

instr push_instrs[] = {
	{"IX", {0xE5, 0xDD}, 2, 15, 15, false, 0x00, 2},
	{"IY", {0xE5, 0xFD}, 2, 15, 15, false, 0x00, 2},
	{"BC", {0xC5}, 1, 11, 11, false, 0x00, 1},
	{"DE", {0xD5}, 1, 11, 11, false, 0x00, 1},
	{"HL", {0xE5}, 1, 11, 11, false, 0x00, 1},
	{"AF", {0xF5}, 1, 11, 11, false, 0x00, 1}};

instr reti_instrs[] = {
	{"", {0x4D, 0xED}, 2, 14, 14, false, 0x00, 2}};

instr retn_instrs[] = {
	{"", {0x45, 0xED}, 2, 14, 14, false, 0x00, 2}};

instr rlca_instrs[] = {
	{"", {0x07}, 1, 4, 4, false, 0x00, 1}};

instr rrca_instrs[] = {
	{"", {0x0F}, 1, 4, 4, false, 0x00, 1}};

opcode opcode_list[] = {
		{"LD",	ld_instrs,	NumElm(ld_instrs),	0,	&opcode_list[1], false},
		{"CALL",	call_instrs,	NumElm(call_instrs),	0,	&opcode_list[2], false},
		{"RET",	ret_instrs,	NumElm(ret_instrs),	0,	&opcode_list[3], false},
		{"JR",	jr_instrs,	NumElm(jr_instrs),	0,	&opcode_list[4], false},
		{"JP",	jp_instrs,	NumElm(jp_instrs),	0,	&opcode_list[5], false},
		{"ADD",	add_instrs,	NumElm(add_instrs),	0,	&opcode_list[6], false},
		{"INC",	inc_instrs,	NumElm(inc_instrs),	0,	&opcode_list[7], false},
		{"POP",	pop_instrs,	NumElm(pop_instrs),	0,	&opcode_list[8], false},
		{"PUSH",	push_instrs,	NumElm(push_instrs),	0,	&opcode_list[9], false},
		{"DEC",	dec_instrs,	NumElm(dec_instrs),	0,	&opcode_list[10], false},
		{"CP",	cp_instrs,	NumElm(cp_instrs),	0,	&opcode_list[11], false},
		{"OR",	or_instrs,	NumElm(or_instrs),	0,	&opcode_list[12], false},
		{"EX",	ex_instrs,	NumElm(ex_instrs),	0,	&opcode_list[13], false},
		{"XOR",	xor_instrs,	NumElm(xor_instrs),	0,	&opcode_list[14], false},
		{"EXX",	exx_instrs,	NumElm(exx_instrs),	0,	&opcode_list[15], false},
		{"SUB",	sub_instrs,	NumElm(sub_instrs),	0,	&opcode_list[16], false},
		{"AND",	and_instrs,	NumElm(and_instrs),	0,	&opcode_list[17], false},
		{"SET",	set_instrs,	NumElm(set_instrs),	0,	&opcode_list[18], false},
		{"BIT",	bit_instrs,	NumElm(bit_instrs),	0,	&opcode_list[19], false},
		{"DJNZ",	djnz_instrs,	NumElm(djnz_instrs),	0,	&opcode_list[20], false},
		{"RRA",	rra_instrs,	NumElm(rra_instrs),	0,	&opcode_list[21], false},
		{"RR",	rr_instrs,	NumElm(rr_instrs),	0,	&opcode_list[22], false},
		{"SRA",	sra_instrs,	NumElm(sra_instrs),	0,	&opcode_list[23], false},
		{"RES",	res_instrs,	NumElm(res_instrs),	0,	&opcode_list[24], false},
		{"LDIR",	ldir_instrs,	NumElm(ldir_instrs),	0,	&opcode_list[25], false},
		{"RLA",	rla_instrs,	NumElm(rla_instrs),	0,	&opcode_list[26], false},
		{"OUT",	out_instrs,	NumElm(out_instrs),	0,	&opcode_list[27], false},
		{"SBC",	sbc_instrs,	NumElm(sbc_instrs),	0,	&opcode_list[28], false},
		{"CPL",	cpl_instrs,	NumElm(cpl_instrs),	0,	&opcode_list[29], false},
		{"RL",	rl_instrs,	NumElm(rl_instrs),	0,	&opcode_list[30], false},
		{"NEG",	neg_instrs,	NumElm(neg_instrs),	0,	&opcode_list[31], false},
		{"RLC",	rlc_instrs,	NumElm(rlc_instrs),	0,	&opcode_list[32], false},
		{"SRL",	srl_instrs,	NumElm(srl_instrs),	0,	&opcode_list[33], false},
		{"RST",	rst_instrs,	NumElm(rst_instrs),	0,	&opcode_list[34], false},
		{"IN",	in_instrs,	NumElm(in_instrs),	0,	&opcode_list[35], false},
		{"LDDR",	lddr_instrs,	NumElm(lddr_instrs),	0,	&opcode_list[36], false},
		{"EI",	ei_instrs,	NumElm(ei_instrs),	0,	&opcode_list[37], false},
		{"IM",	im_instrs,	NumElm(im_instrs),	0,	&opcode_list[38], false},
		{"ADC",	adc_instrs,	NumElm(adc_instrs),	0,	&opcode_list[39], false},
		{"DAA",	daa_instrs,	NumElm(daa_instrs),	0,	&opcode_list[40], false},
		{"DI",	di_instrs,	NumElm(di_instrs),	0,	&opcode_list[41], false},
		{"NOP",	nop_instrs,	NumElm(nop_instrs),	0,	&opcode_list[42], false},
		{"RLCA",	rlca_instrs,	NumElm(rlca_instrs),	0,	&opcode_list[43], false},
		{"SLA",	sla_instrs,	NumElm(sla_instrs),	0,	&opcode_list[44], false},
		{"LDI",	ldi_instrs,	NumElm(ldi_instrs),	0,	&opcode_list[45], false},
		{"CCF",	ccf_instrs,	NumElm(ccf_instrs),	0,	&opcode_list[46], false},
		{"SCF",	scf_instrs,	NumElm(scf_instrs),	0,	&opcode_list[47], false},
		{"RRC",	rrc_instrs,	NumElm(rrc_instrs),	0,	&opcode_list[48], false},
		{"HALT",	halt_instrs,	NumElm(halt_instrs),	0,	&opcode_list[49], false},
		{"RLD",	rld_instrs,	NumElm(rld_instrs),	0,	&opcode_list[50], false},
		{"SL1",	sl1_instrs,	NumElm(sl1_instrs),	0,	&opcode_list[51], false},
		{"CPD",	cpd_instrs,	NumElm(cpd_instrs),	0,	&opcode_list[52], false},
		{"CPDR",	cpdr_instrs,	NumElm(cpdr_instrs),	0,	&opcode_list[53], false},
		{"CPI",	cpi_instrs,	NumElm(cpi_instrs),	0,	&opcode_list[54], false},
		{"CPIR",	cpir_instrs,	NumElm(cpir_instrs),	0,	&opcode_list[55], false},
		{"IND",	ind_instrs,	NumElm(ind_instrs),	0,	&opcode_list[56], false},
		{"INDR",	indr_instrs,	NumElm(indr_instrs),	0,	&opcode_list[57], false},
		{"INI",	ini_instrs,	NumElm(ini_instrs),	0,	&opcode_list[58], false},
		{"INIR",	inir_instrs,	NumElm(inir_instrs),	0,	&opcode_list[59], false},
		{"LDD",	ldd_instrs,	NumElm(ldd_instrs),	0,	&opcode_list[60], false},
		{"OTDR",	otdr_instrs,	NumElm(otdr_instrs),	0,	&opcode_list[61], false},
		{"OTIR",	otir_instrs,	NumElm(otir_instrs),	0,	&opcode_list[62], false},
		{"OUTD",	outd_instrs,	NumElm(outd_instrs),	0,	&opcode_list[63], false},
		{"OUTI",	outi_instrs,	NumElm(outi_instrs),	0,	&opcode_list[64], false},
		{"RETI",	reti_instrs,	NumElm(reti_instrs),	0,	&opcode_list[65], false},
		{"RETN",	retn_instrs,	NumElm(retn_instrs),	0,	&opcode_list[66], false},
		{"RRCA",	rrca_instrs,	NumElm(rrca_instrs),	0,	&opcode_list[67], false},
		{"RRD",	rrd_instrs,	NumElm(rrd_instrs),	0,	&opcode_list[68], false},
		{"SLL",	sll_instrs,	NumElm(sll_instrs),	0,	NULL, false},
};

opcode *all_opcodes = opcode_list;
