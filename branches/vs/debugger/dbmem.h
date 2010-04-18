#ifndef DBMEM_H
#define DBMEM_H

#include "gui.h"
#include "calc.h"


typedef struct mempane_settings {
	int addr;			// Top left address
	int mode;						// current display mode
	int sel;						// current selection
	int xSel, ySel;					// position
	int track;						// register to track
	int iAddr, iData;
	double diff;
	HWND hwndHeader;
	HWND cmbMode;
	HWND hwndTip;
	int addrTrack;
	BOOL bText;
	HFONT hfontAddr, hfontData;
	int nRows, nCols;
	int cxMem;
	int cyRow;
} mempane_settings_t;

typedef mempane_settings_t mp_settings;


LRESULT CALLBACK MemProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

#endif
