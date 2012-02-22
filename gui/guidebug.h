#ifndef GUIDEBUG_H
#define GUIDEBUG_H

#include "dbcommon.h"
#include "dbdisasm.h"
#include "dbwatch.h"
#include "dbmem.h"

LRESULT CALLBACK DebugProc(HWND, UINT, WPARAM, LPARAM);
int BrowseTxtFile(TCHAR *);
#define REG_PANE_WIDTH 200

#define ID_DISASM 		0
#define ID_REG 			1
#define ID_MEMTAB		2
#define ID_MEM			3
#define ID_STACK		4
#define ID_WABBIT		5
#define ID_DISASMSIZE	6
#define ID_SIZE			6
#define ID_TOOLBAR		7
#define ID_SUBBAR		8
#define ID_DISASMTAB	9
#define ID_PANECONTAINER 10

#define MAX_MEM_TABS MAX_TABS * 3
#define MAX_DISASM_TABS MAX_TABS * 3
#define CY_TOOLBAR 32
#define MAX_TABS 2

typedef struct tagDEBUGWINDOWINFO
{
	unsigned int cyGripper;
	unsigned int cyDisasm, cyMem;
	
	int regPanesYScroll;
	HWND ExpandPanes[16];
	int TotalPanes;
	int PanesHeight;
	int kRegRow, kRegAddr;
	RECT val_locs[15];

	HWND hwndEditControl;
	int edit_row, edit_col;
	HWND hwndSpriteViewer[MAX_WATCHPOINTS];
	int num_watch;
	HWND hwndListView;
	watchpoint_t * watchpoints[MAX_WATCHPOINTS];

	DISPLAY_BASE dispType;
	WINDOWPLACEMENT db_placement;
	HWND htoolbar, hdisasm, hreg, hmem, hwatch, hPortMon, hBreakpoints, hDebug;
	HFONT hfontSegoe, hfontLucida, hfontLucidaBold;
	int total_mem_pane, total_disasm_pane;
	HWND hmemlist[MAX_MEM_TABS];
	HWND hdisasmlist[MAX_DISASM_TABS];
	double ratioDisasm;
	LPCALC lpCalc;

	void *tabInfo;
	mp_settings mps[1 + MAX_MEM_TABS];
	dp_settings dps[1 + MAX_DISASM_TABS];

	BOOL bDrag, bHot;
	int offset_click;
	BOOL top_locked;
	BOOL bottom_locked;

	BOOL db_maximized;
	ep_state expand_pane_state;	
	
	long long code_count_tstates;
} DEBUGWINDOWINFO, *LPDEBUGWINDOWINFO;

typedef struct tagTABWINDOWINFO {
	LPDEBUGWINDOWINFO lpDebugInfo;
	void *tabInfo;
} TABWINDOWINFO, *LPTABWINDOWINFO;

#endif
