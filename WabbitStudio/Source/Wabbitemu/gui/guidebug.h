#ifndef GUIDEBUG_H
#define GUIDEBUG_H

#include "dbcommon.h"
#include "dbdisasm.h"
#include "dbwatch.h"
#include "dbmem.h"

LRESULT CALLBACK DebugProc(HWND, UINT, WPARAM, LPARAM);
int BrowseTxtFile(TCHAR *);
#define REG_PANE_WIDTH 200

typedef enum {
	MEM_BYTE = 1,
	MEM_WORD = 2,
	MEM_DWORD = 4,
};

#define ID_DISASM 		0
#define ID_REG 			1
#define ID_MEMTAB		2
#define ID_MEM			3
#define ID_STACK		4
#define ID_WATCH		5
#define ID_DISASMSIZE	6
#define ID_SIZE			6
#define ID_TOOLBAR		7
#define ID_SUBBAR		8
#define ID_DISASMTAB	9
#define ID_PANECONTAINER 10
#define ID_BREAKPOINTS	11
#define ID_PORTMON		12
#define ID_LCDMON		13

#define MAX_MEM_TABS MAX_TABS * 3
#define MAX_DISASM_TABS MAX_TABS * 3
#define CY_TOOLBAR 32
#define MAX_TABS 2

#define EXTRA_DISASM_PANES 3
#define EXTRA_MEM_PANES 1

typedef struct tagDEBUGWINDOWINFO
{
	unsigned int cyGripper;
	unsigned int cyDisasm, cyMem;
	
	// how much the regs pane has been scrolled down
	int reg_panes_yoffset;
	// if the height has changed, this is the old height value
	unsigned int reg_pane_old_height;
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
	watchpoint_t watchpoints[MAX_WATCHPOINTS];

	DISPLAY_BASE dispType;
	WINDOWPLACEMENT db_placement;
	HWND hDebug;
	HWND htoolbar;
	HWND hdisasm;
	HWND hreg;
	HWND hmem;
	HWND hwatch;
	HWND hBreakpoints;
	HFONT hfontSegoe, hfontLucida, hfontLucidaBold;
	int total_mem_pane, total_disasm_pane;
	HWND hmemlist[MAX_MEM_TABS];
	HWND hdisasmlist[MAX_DISASM_TABS];
	HWND hdisasmextra[EXTRA_DISASM_PANES];
	HWND hwndLastFocus;
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
	volatile BOOL is_ready;
	BOOL bTIOSDebug;

	LPCALC duplicate_calc;
	int port_map[0xFF];
	WNDPROC wpOrigEditProc;

} DEBUGWINDOWINFO, *LPDEBUGWINDOWINFO;

typedef struct tagTABWINDOWINFO {
	LPDEBUGWINDOWINFO lpDebugInfo;
	void *tabInfo;
} TABWINDOWINFO, *LPTABWINDOWINFO;

#endif
