#ifndef DBREG_H
#define DBREG_H

#include "gui.h"
#include "core.h"
#include "calc.h"

#define coff(rreg,rname) {(unsigned int) (offsetof(struct CPU, rreg)), rname}
#define reg16(offs) (*((unsigned short*) (&calcs[DebuggerSlot].cpu) + (offs/sizeof(short))))
#define reg8(offs) (*((unsigned char*) (&calcs[DebuggerSlot].cpu) + (offs/sizeof(char))))
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


LRESULT CALLBACK RegProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
void HandleEditMessages(HWND hwnd, WPARAM wParam, LPARAM lParam);

#endif
