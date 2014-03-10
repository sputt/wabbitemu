#ifndef DBDISASM_H
#define DBDISASM_H

#include "gui.h"
#include "core.h"
#include "disassemble.h"


#define PC_TRAILS 4
typedef struct disasmhdr {
	int index;
	unsigned int cx;
	unsigned int nCharsWidth;
	TCHAR pszText[32];
	void (*lpfnCallback)(LPCALC, HDC, Z80_info_t*, RECT*);
	UINT uFormat;
	HFONT hfont;
} disasmhdr_t;

typedef struct disasmpane_settings {
	LPCALC lpCalc;
	unsigned int nSel, nPane;
	int iSel, iPC;
	int iHot;
	int nKey;
	int NumSel;
	POINT DragStart;
	//nRows = number of visible rows (including partially visible ones)
	unsigned int cyRow, nRows, nPage;
	HFONT hfontAddr, hfontData, hfontDisasm, hfontHeader;
	disasmhdr_t hdrs[8];
	HWND hwndHeader;
	int nPCs[PC_TRAILS];
	//converted from static
	HWND hwndTip;
	BOOL IsDragging;
	int last_pagedown;
	DWORD dwDragCountdown;
	Z80_info_t zinf[256];
	int nClick;
	int max_right;
	int cyHeader;
	POINT MousePoint;
	TOOLINFO toolInfo;
	ViewType type;
	HWND hGotoDialog;
	HWND hFindDialog;
} disasmpane_settings_t, dp_settings;

LRESULT CALLBACK DisasmProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
void sprint_addr(LPCALC, HDC, Z80_info_t *, RECT *);
void sprint_command(LPCALC, HDC, Z80_info_t *, RECT *);
void sprint_data(LPCALC, HDC, Z80_info_t *, RECT *);
void sprint_size(LPCALC, HDC, Z80_info_t *, RECT *);
void sprint_clocks(LPCALC, HDC, Z80_info_t *, RECT *);
void CPU_stepout(LPCALC, BOOL tiosDebug);
void CPU_stepover(LPCALC, BOOL tiosDebug);
void cycle_pcs(dp_settings *);

#define dpsAddr		0
#define dpsData		1
#define dpsDisasm	2
#define dpsSize		3
#define dpsClocks	4

#define DB_DISASM		40

void cycle_pcs(dp_settings *dps);

#endif
