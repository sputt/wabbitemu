#ifndef DBREG_H
#define DBREG_H

#include "gui.h"
#include "core.h"
#include "calc.h"

#define coff(rreg,rname) {(unsigned int) (offsetof(struct CPU, rreg)), rname}
#define reg16(offs) (*((unsigned short*) (&lpCalc->cpu) + (offs/sizeof(short))))
#define reg8(offs) (*((unsigned char*) (&lpCalc->cpu) + (offs/sizeof(char))))
#define REG16_COLS 	2
#define REG8_COLS	3
#define REG16_ROWS	6
#define REG8_ROWS	1
#define REG_TOP		2*kRegRow
#define BTN_SWP_WIDTH

#define EN_SUBMIT	99

#define IDC_LCD_ON	0x1000
#define IDC_LCD_X	(IDC_LCD_ON+1)
#define IDC_LCD_Y	(IDC_LCD_ON+2)
#define IDC_LCD_Z	(IDC_LCD_ON+3)
#define IDC_LCD_MODE (IDC_LCD_ON+4)
#define IDC_LCD_CONTRAST	(IDC_LCD_ON+5)

// These are numbered strangely to match VICE's values
typedef enum {
	REGISTER_INVALID = -1,

	REGISTER_A = 0x00,
	REGISTER_PC = 0x03,
	REGISTER_SP = 0x04,
	REGISTER_FLAGS = 0x05,

	REGISTER_AF = 0x06,
	REGISTER_BC = 0x07,
	REGISTER_DE = 0x08,
	REGISTER_HL = 0x09,

	REGISTER_IX = 0x0a,
	REGISTER_IY = 0x0b,
	REGISTER_I = 0x0c,
	REGISTER_R = 0x0d,

	REGISTER_AFP = 0x0e,
	REGISTER_BCP = 0x0f,
	REGISTER_DEP = 0x10,
	REGISTER_HLP = 0x11,

	REGISTER_B = 0x21,
	REGISTER_C = 0x22,
	REGISTER_D = 0x26,
	REGISTER_E = 0x29,

	REGISTER_H = 0x2f,
	REGISTER_L = 0x30,

	REGISTER_IXL = 0x31,
	REGISTER_IXH = 0x32,

	REGISTER_IYL = 0x33,
	REGISTER_IYH = 0x34,
} REGISTER;

typedef struct register_info {
	REGISTER id;
	TCHAR* name;
	void *data;
	size_t size;
} register_info_t;

LRESULT CALLBACK RegProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
register_info_t* GetAllRegisters(LPCALC lpCalc);
void HandleEditMessages(HWND hwnd, WPARAM wParam, LPARAM lParam);

#endif
