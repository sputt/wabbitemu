#include "stdafx.h"

#include "guidebug.h"

#include "DBPaneContainer.h"

#include "calc.h"
#include "dbdisasm.h"
#include "dbmem.h"
#include "dbmonitor.h"
#include "dbbreakpoints.h"
#include "dbprofile.h"
#include "dbreg.h"
#include "dbtoolbar.h"
#include "expandpane.h"
#include "registry.h"
#include "fileutilities.h"

extern HINSTANCE g_hInst;
extern HWND hwndLastFocus;

void WriteHumanReadableDump(LPCALC lpCalc, TCHAR *path);

HWND GetDisasmPaneHWND(LPDEBUGWINDOWINFO lpDebugInfo, int index) {
	if (index >= lpDebugInfo->total_disasm_pane) {
		return lpDebugInfo->hdisasmextra[index - lpDebugInfo->total_disasm_pane];
	}
	
	return lpDebugInfo->hdisasmlist[index];
}

HWND GetMemPaneHWND(LPDEBUGWINDOWINFO lpDebugInfo, int index) {
	if (index == lpDebugInfo->total_mem_pane) {
		return lpDebugInfo->hwatch;
	}

	return lpDebugInfo->hmemlist[index];
}

BOOL CALLBACK EnumDebugResize(HWND hwndChild, LPARAM lParam) {
	int idChild;
	RECT rc;
	RECT *rcParent = &rc;
	DEBUGWINDOWINFO *debugInfo = (DEBUGWINDOWINFO *) lParam;
	GetClientRect(debugInfo->hDebug, rcParent);

	// Is it a first level child?
	if (debugInfo->hDebug != GetParent(hwndChild))
		return TRUE;

	idChild = GetWindowID(hwndChild);
	
	switch (idChild) {
		case ID_TOOLBAR:
			MoveWindow(hwndChild, 0, 0, rcParent->right, CY_TOOLBAR, FALSE); 
			break;
		case ID_DISASMTAB: {
			RECT tabRc;
			tabRc.left = 0;
			tabRc.top = CY_TOOLBAR;
			tabRc.right = rcParent->right - REG_PANE_WIDTH;
			tabRc.bottom = debugInfo->cyDisasm;
			MoveWindow(hwndChild, tabRc.left, tabRc.top, tabRc.right - tabRc.left, tabRc.bottom - tabRc.top, FALSE);
			int index = TabCtrl_GetCurSel(hwndChild);
			tabRc.top = 0;
			tabRc.bottom = debugInfo->cyDisasm - CY_TOOLBAR;
			TabCtrl_AdjustRect(hwndChild, FALSE, &tabRc);
			HWND curTab = GetDisasmPaneHWND(debugInfo, index);
			MoveWindow(curTab, tabRc.left, tabRc.top, tabRc.right - tabRc.left, tabRc.bottom - tabRc.top, FALSE);
			SendMessage(curTab, WM_USER, DB_UPDATE, 0);
			break;
		}
		case ID_MEMTAB: {
			MoveWindow(hwndChild, 3, debugInfo->cyDisasm + debugInfo->cyGripper, rcParent->right - 103 - REG_PANE_WIDTH - 8,
						debugInfo->cyMem - debugInfo->cyGripper - 3, FALSE);
			int index = TabCtrl_GetCurSel(hwndChild);
			HWND curTab = GetMemPaneHWND(debugInfo, index);
			MoveWindow(curTab, 3, 24, rcParent->right - REG_PANE_WIDTH - 118, debugInfo->cyMem - debugInfo->cyGripper - 32, FALSE);
			SendMessage(curTab, WM_USER, DB_UPDATE, 0);
			break;
		}
		case ID_STACK:
			MoveWindow(hwndChild, rcParent->right - 110 - REG_PANE_WIDTH, debugInfo->cyDisasm + debugInfo->cyGripper,
						110, debugInfo->cyMem - debugInfo->cyGripper, FALSE);
			break;
		case ID_REG:
			SetWindowPos(hwndChild, HWND_TOP, rcParent->right - REG_PANE_WIDTH, CY_TOOLBAR, REG_PANE_WIDTH, rcParent->bottom - CY_TOOLBAR, 0);
			break;
	}
	SendMessage(hwndChild, WM_USER, DB_UPDATE, 0);
	return TRUE;
}

BOOL CALLBACK EnumDebugUpdate(HWND hwndChild, LPARAM lParam) {
	if (IsWindowVisible(hwndChild)) {
		SendMessage(hwndChild, WM_USER, DB_UPDATE, lParam);
	}
	return TRUE;
}

BOOL CALLBACK EnumDebugResume(HWND hwndChild, LPARAM lParam) {
	SendMessage(hwndChild, WM_USER, DB_UPDATE, lParam);
	SendMessage(hwndChild, WM_USER, DB_RESUME, lParam);
	return TRUE;
}

BOOL CALLBACK EnumDebugDestroy(HWND hwndChild, LPARAM lParam) {
	DestroyWindow(hwndChild);
	return TRUE;
}

int CALLBACK EnumFontFamExProc(
  ENUMLOGFONTEX *lpelfe,    // logical-font data
  NEWTEXTMETRICEX *lpntme,  // physical-font data
  DWORD FontType,           // type of font
  LPARAM lParam             // application-defined data
) {
	HDC hdc = GetDC(NULL);
	LOGFONT *lplf = &lpelfe->elfLogFont;
	lplf->lfHeight = -MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	lplf->lfWidth = 0;
	*((HFONT *) lParam) = CreateFontIndirect(lplf);
	ReleaseDC(NULL, hdc);
	return 0;
}

void AddDisasmTab(LPDEBUGWINDOWINFO lpDebugInfo, ViewType type) {
	int total_disasm_pane = lpDebugInfo->total_disasm_pane;
	if (total_disasm_pane >= MAX_DISASM_TABS) {
		return;
	}
	dp_settings *dps = lpDebugInfo->dps;
	int index = total_disasm_pane + 1;
	ZeroMemory(&dps[index], sizeof(dp_settings));
	dps[index].hdrs[0].nCharsWidth = 10;
	StringCbCopy(dps[index].hdrs[0].pszText, sizeof(dps[index].hdrs[0].pszText), _T("Addr"));
	dps[index].hdrs[0].lpfnCallback = &sprint_addr;
	dps[index].hdrs[1].index = 1;
	dps[index].hdrs[1].nCharsWidth = 11;
	StringCbCopy(dps[index].hdrs[1].pszText, sizeof(dps[index].hdrs[1].pszText), _T("Data"));
	dps[index].hdrs[1].lpfnCallback = &sprint_data;
	dps[index].hdrs[2].index = 2;
	dps[index].hdrs[2].nCharsWidth = 45;
	StringCbCopy(dps[index].hdrs[2].pszText, sizeof(dps[index].hdrs[2].pszText), _T("Disassembly"));
	dps[index].hdrs[2].lpfnCallback = &sprint_command;
	dps[index].hdrs[3].index = 3;
	dps[index].hdrs[3].nCharsWidth = 6;
	StringCbCopy(dps[index].hdrs[3].pszText, sizeof(dps[index].hdrs[3].pszText), _T("Size"));
	dps[index].hdrs[3].uFormat = DT_CENTER;
	dps[index].hdrs[3].lpfnCallback = &sprint_size;
	dps[index].hdrs[4].index = 4;
	dps[index].hdrs[4].nCharsWidth = 8;
	StringCbCopy(dps[index].hdrs[4].pszText, sizeof(dps[index].hdrs[4].pszText), _T("Clocks"));
	dps[index].hdrs[4].uFormat = DT_CENTER;
	dps[index].hdrs[4].lpfnCallback = &sprint_clocks;
	dps[index].hdrs[5].index = -1;
	dps[index].hdrs[5].lpfnCallback = &sprint_addr;
	dps[index].hdrs[6].index = -1;
	dps[index].hdrs[6].lpfnCallback = &sprint_addr;
	dps[index].hdrs[7].index = -1;
	dps[index].hdrs[7].lpfnCallback = &sprint_addr;
	dps[index].type = type;
	dps[index].lpCalc = lpDebugInfo->lpCalc;

	if (type == REGULAR) {
		dps[index].nSel = lpDebugInfo->lpCalc->cpu.pc;
	}

	LPTABWINDOWINFO lpTabInfo = (LPTABWINDOWINFO) malloc(sizeof(TABWINDOWINFO));
	lpTabInfo->lpDebugInfo = lpDebugInfo;
	lpTabInfo->tabInfo = &dps[index];

	lpDebugInfo->hdisasmlist[total_disasm_pane] = CreateWindowEx(
		WS_EX_CONTROLPARENT,
		g_szDisasmName,
		_T("Disasm"),
		WS_VISIBLE | WS_CHILD | WS_VSCROLL,
		3, 20, 100, 10,
		lpDebugInfo->hdisasm,
		(HMENU) ID_DISASM,
		g_hInst, lpTabInfo);
	TCHAR buffer[64];
	switch (type) {
		case REGULAR:
			StringCbPrintf(buffer, sizeof(buffer), _T("Disasm %i"), (total_disasm_pane + 3) / 3);
			break;
		case FLASH:
			if (lpDebugInfo->lpCalc->cpu.pio.model >= TI_73) {
				StringCbPrintf(buffer, sizeof(buffer), _T("Flash %i"), (total_disasm_pane + 3) / 3);
			} else {
				StringCbPrintf(buffer, sizeof(buffer), _T("ROM %i"), (total_disasm_pane + 3) / 3);
			}
			break;
		case RAM:
			StringCbPrintf(buffer, sizeof(buffer), _T("RAM %i"), (total_disasm_pane + 3) / 3);
			break;
	}
	TCITEM tie;
	tie.mask = TCIF_TEXT | TCIF_IMAGE;
	tie.iImage = -1;
	tie.pszText = buffer;
	tie.lParam = (LPARAM)lpDebugInfo->hdisasmlist[total_disasm_pane];
	TabCtrl_InsertItem(lpDebugInfo->hdisasm, total_disasm_pane, &tie);
	TabCtrl_SetCurSel(lpDebugInfo->hdisasm, total_disasm_pane);
	lpDebugInfo->total_disasm_pane++;
}

void AddMemTab(LPCALC lpCalc, mempane_settings *mps, ViewType type, LPDEBUGWINDOWINFO lpDebugInfo) {
	ShowWindow(lpDebugInfo->hmemlist[TabCtrl_GetCurSel(lpDebugInfo->hmem)], SW_HIDE);
	int total_mem_pane = lpDebugInfo->total_mem_pane;
	if (total_mem_pane >= MAX_MEM_TABS) {
		return;
	}

	int index = total_mem_pane + 1;
	mps[index].addr = 0x0000;
	mps[index].mode = MEM_BYTE;
	mps[index].sel = 0x000;
	mps[index].track = -1;
	mps[index].memNum = total_mem_pane;
	mps[index].type = type;
	mps[index].lpCalc = lpCalc;

	LPTABWINDOWINFO lpTabInfo = (LPTABWINDOWINFO) malloc(sizeof(TABWINDOWINFO));
	lpTabInfo->lpDebugInfo = lpDebugInfo;
	lpTabInfo->tabInfo = &mps[index];

	lpDebugInfo->hmemlist[total_mem_pane] = CreateWindow(
		g_szMemName,
		_T("Memory"),
		WS_VISIBLE | WS_CHILD | WS_VSCROLL,
		3, 20, 100, 10,
		lpDebugInfo->hmem,
		(HMENU) ID_MEM,
		g_hInst, lpTabInfo);
	TCHAR buffer[64];
	switch (type) {
		case REGULAR:
			StringCbPrintf(buffer, sizeof(buffer), _T("Mem %i"), (total_mem_pane + 3) / 3);
			break;
		case FLASH:
			if (lpCalc->cpu.pio.model >= TI_73) {
				StringCbPrintf(buffer, sizeof(buffer), _T("Flash %i"), (total_mem_pane + 3) / 3);
			} else { 
				StringCbPrintf(buffer, sizeof(buffer), _T("ROM %i"), (total_mem_pane + 3) / 3);
			}
			break;
		case RAM:
			StringCbPrintf(buffer, sizeof(buffer), _T("RAM %i"), (total_mem_pane + 3) / 3);
			break;
	}
	TCITEM tie;
	tie.mask = TCIF_TEXT | TCIF_IMAGE;
	tie.iImage = -1;
	tie.pszText = buffer;
	tie.lParam = (LPARAM)lpDebugInfo->hmemlist[total_mem_pane];
	TabCtrl_InsertItem(lpDebugInfo->hmem, total_mem_pane, &tie);
	TabCtrl_SetCurSel(lpDebugInfo->hmem, total_mem_pane);
	lpDebugInfo->total_mem_pane++;
}

void AddWatchTab(LPCALC lpCalc, LPDEBUGWINDOWINFO debugInfo) {	
	if (debugInfo->hwatch) {
		return;
	}

	debugInfo->hwatch = CreateWindow(
		g_szWatchName,
		_T(""),
		WS_VISIBLE | WS_CHILD,
		3, 20, 400, 200,
		debugInfo->hmem,
		(HMENU) ID_WATCH,
		g_hInst, (LPVOID) debugInfo);
	TCITEM tie;
	tie.mask = TCIF_TEXT | TCIF_IMAGE;
	tie.iImage = -1;
	tie.pszText = _T("Watch");
	tie.lParam = (LPARAM)debugInfo->hwatch;
	TabCtrl_InsertItem(debugInfo->hmem, 0, &tie);
	TabCtrl_SetCurSel(debugInfo->hmem, 0);
}

void AddBreakpointsTab(LPCALC lpCalc, LPDEBUGWINDOWINFO debugInfo) {
	if (debugInfo->hdisasmextra[0]) {
		return;
	}

	debugInfo->hdisasmextra[0] = CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_BREAKPOINT),
		debugInfo->hdisasm, (DLGPROC)BreakpointsDialogProc, (LPARAM) debugInfo);

	TCITEM tie;
	tie.mask = TCIF_TEXT | TCIF_IMAGE;
	tie.iImage = -1;
	tie.pszText = _T("Breakpoints");
	tie.lParam = (LPARAM)debugInfo->hdisasmextra[0];
	TabCtrl_InsertItem(debugInfo->hdisasm, 0, &tie);
	TabCtrl_SetCurSel(debugInfo->hdisasm, 0);
}

void AddPortsTab(LPCALC lpCalc, LPDEBUGWINDOWINFO debugInfo) {
	if (debugInfo->hdisasmextra[1]) {
		return;
	}

	debugInfo->hdisasmextra[1] = CreateWindow(
		g_szPortMonitor,
		_T(""),
		WS_VISIBLE | WS_CHILD,
		3, 20, 400, 200,
		debugInfo->hdisasm,
		(HMENU)ID_PORTMON,
		g_hInst, (LPVOID)debugInfo);
	TCITEM tie;
	tie.mask = TCIF_TEXT | TCIF_IMAGE;
	tie.iImage = -1;
	tie.pszText = _T("Port Monitor");
	tie.lParam = (LPARAM)debugInfo->hdisasmextra[1];
	TabCtrl_InsertItem(debugInfo->hdisasm, 0, &tie);
	TabCtrl_SetCurSel(debugInfo->hdisasm, 0);
}

void AddColorLCDTab(LPCALC lpCalc, LPDEBUGWINDOWINFO debugInfo) {
	if (debugInfo->hdisasmextra[2]) {
		return;
	}

	debugInfo->hdisasmextra[2] = CreateWindow(
		g_szLCDMonitor,
		_T(""),
		WS_VISIBLE | WS_CHILD,
		3, 20, 400, 200,
		debugInfo->hdisasm,
		(HMENU)ID_LCDMON,
		g_hInst, (LPVOID)debugInfo);
	TCITEM tie;
	tie.mask = TCIF_TEXT | TCIF_IMAGE;
	tie.iImage = -1;
	tie.pszText = _T("Color LCD Monitor");
	tie.lParam = (LPARAM)debugInfo->hdisasmextra[2];
	TabCtrl_InsertItem(debugInfo->hdisasm, 0, &tie);
	TabCtrl_SetCurSel(debugInfo->hdisasm, 0);
}

void UpdateRunningMenu(LPCALC lpCalc, LPVOID lParam) {
	HMENU hMenu = GetMenu((HWND) lParam);

	if (lpCalc->running) {
		MENUITEMINFO mii;
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_STRING;
		mii.dwTypeData = _T("Stop\tF5");
		SetMenuItemInfo(hMenu, IDM_RUN_RUN, FALSE, &mii);
	} else {
		 MENUITEMINFO mii;
		 mii.cbSize = sizeof(MENUITEMINFO);
		 mii.fMask = MIIM_STRING;
		 mii.dwTypeData = _T("Run\tF5");
		 SetMenuItemInfo(hMenu, IDM_RUN_RUN, FALSE, &mii);
	 }
}

LRESULT CALLBACK DebugProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static const TCHAR* MemPaneString = _T("NumMemPane");
	static const TCHAR* DisasmPaneString = _T("NumDisasmPane");
	static const TCHAR* MemSelIndexString = _T("MemSelIndex");
	static const TCHAR* DisasmSelIndexString = _T("DiasmSelIndex");

	LPDEBUGWINDOWINFO lpDebugInfo = lpDebugInfo = (LPDEBUGWINDOWINFO)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (Message) {
		case WM_CREATE:
		{
			lpDebugInfo = (LPDEBUGWINDOWINFO) malloc(sizeof(DEBUGWINDOWINFO));
			ZeroMemory(lpDebugInfo, sizeof(DEBUGWINDOWINFO));
			LPCALC lpCalc = (LPCALC) ((LPCREATESTRUCT) lParam)->lpCreateParams;
			lpDebugInfo->lpCalc = lpCalc;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LPARAM) lpDebugInfo);

			lpDebugInfo->cyGripper = 10;
			lpDebugInfo->cyDisasm = 350;
			lpDebugInfo->hDebug = hwnd;
			lpDebugInfo->code_count_tstates = -1;
			lpDebugInfo->dispType = (DISPLAY_BASE) QueryDebugKey((TCHAR *) DisplayTypeString);

			LOGFONT lf;
			memset(&lf, 0, sizeof(LOGFONT));
			StringCbCopy(lf.lfFaceName, sizeof(lf.lfFaceName), _T("Lucida Console"));
			HDC hdc = GetDC(NULL);
			EnumFontFamiliesEx(
					hdc,
					&lf,
					(FONTENUMPROC) EnumFontFamExProc,
					(LPARAM) &lpDebugInfo->hfontLucida,
					0);

			GetObject(lpDebugInfo->hfontLucida, sizeof(LOGFONT), &lf);
			lf.lfWeight = FW_BOLD;

			lpDebugInfo->hfontLucidaBold = CreateFontIndirect(&lf);
			ReleaseDC(NULL, hdc);

			LOGFONT lfSegoe;
			memset(&lfSegoe, 0, sizeof(LOGFONT));
			StringCbCopy(lfSegoe.lfFaceName, sizeof(lfSegoe.lfFaceName), _T("Segoe UI"));

			hdc = GetDC(NULL);
			if (EnumFontFamiliesEx(hdc, &lfSegoe, (FONTENUMPROC) EnumFontFamExProc, (LPARAM) &lpDebugInfo->hfontSegoe, 0) != 0) {
				StringCbCopy(lfSegoe.lfFaceName, sizeof(lfSegoe.lfFaceName), _T("MS Shell Dlg"));
				ReleaseDC(NULL, hdc);
				hdc = GetDC(NULL);
				EnumFontFamiliesEx(hdc, &lfSegoe, (FONTENUMPROC) EnumFontFamExProc, (LPARAM) &lpDebugInfo->hfontSegoe, 0);
				ReleaseDC(NULL, hdc);
			}

			//need to do this before we add the tabs (sizes)
			RECT rc;
			GetClientRect(hwnd, &rc);
			lpDebugInfo->ratioDisasm = (double) lpDebugInfo->cyDisasm / rc.bottom;

			switch (lpDebugInfo->dispType) {
				case HEX:
					CheckMenuItem(GetMenu(hwnd), IDM_DISPLAYBASE_HEX, MF_BYCOMMAND | MF_CHECKED);
					break;
				case DEC:
					CheckMenuItem(GetMenu(hwnd), IDM_DISPLAYBASE_DEC, MF_BYCOMMAND | MF_CHECKED);
					break;
				case BIN:
					CheckMenuItem(GetMenu(hwnd), IDM_DISPLAYBASE_BIN, MF_BYCOMMAND | MF_CHECKED);
					break;
			}

			lpDebugInfo->hdisasm = CreateWindowEx(
				WS_EX_CONTROLPARENT,
				WC_TABCONTROL, _T(""),
			    WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
			    0, 0, 1, 1,
			    hwnd,
			    (HMENU) ID_DISASMTAB,
			    g_hInst, NULL);
			SetWindowFont(lpDebugInfo->hdisasm, lpDebugInfo->hfontSegoe, TRUE);

			if (lpCalc->model >= TI_84PCSE) {
				AddColorLCDTab(lpCalc, lpDebugInfo);
			}
			AddPortsTab(lpCalc, lpDebugInfo);
			AddBreakpointsTab(lpCalc, lpDebugInfo);

			/* Create disassembly window */
			lpDebugInfo->total_disasm_pane = 0;
			int panes_to_add = max(1, (int) (QueryDebugKey((TCHAR *) DisasmPaneString)));
			while (panes_to_add > 0) {
				SendMessage(hwnd, WM_COMMAND, IDM_VIEW_ADDDISASM, 0);
				panes_to_add--;
			}

			int selIndex = (int) QueryDebugKey((TCHAR *) DisasmSelIndexString);
			TabCtrl_SetCurSel(lpDebugInfo->hdisasm, selIndex);
			NMHDR hdr;
			hdr.code = TCN_SELCHANGE;
			hdr.idFrom = 1; // not needed
			hdr.hwndFrom = lpDebugInfo->hdisasm;
			SendMessage(hwnd, WM_NOTIFY, MAKEWPARAM(0, 0), (LPARAM) &hdr);

			lpDebugInfo->htoolbar = CreateWindowEx(
				WS_EX_CONTROLPARENT,
				g_szToolbar,
				_T("toolbar"),
				WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
				0, 0, 1, 1,
				hwnd,
				(HMENU) ID_TOOLBAR,
				g_hInst, lpDebugInfo);

			lpDebugInfo->hmem =
			CreateWindowEx(
				WS_EX_CONTROLPARENT,
				WC_TABCONTROL, _T(""),
			    WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
			    0, 0, 1, 1,
			    hwnd,
			    (HMENU) ID_MEMTAB,
			    g_hInst, NULL);
			SetWindowFont(lpDebugInfo->hmem, lpDebugInfo->hfontSegoe, TRUE);

			AddWatchTab(lpCalc, lpDebugInfo);

			lpDebugInfo->total_mem_pane = 0;
			panes_to_add = max(1, (int) (QueryDebugKey((TCHAR *) MemPaneString)));
			while (panes_to_add > 0) {
				SendMessage(hwnd, WM_COMMAND, IDM_VIEW_ADDMEM, 0);
				panes_to_add--;
			}

			selIndex = (int) QueryDebugKey((TCHAR *) MemSelIndexString);
			TabCtrl_SetCurSel(lpDebugInfo->hmem, selIndex);
			hdr.code = TCN_SELCHANGE;
			hdr.idFrom = 1; // not needed
			hdr.hwndFrom = lpDebugInfo->hmem;
			SendMessage(hwnd, WM_NOTIFY, MAKEWPARAM(0, 0), (LPARAM) &hdr);

			lpDebugInfo->hreg =
			CreateWindow(
				g_szRegName,
				_T("reg"),
				WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL,
				0, 0, 100, 300,
				hwnd,
				(HMENU) ID_REG,
				g_hInst, lpDebugInfo);

			//CreatePaneContainer(hwnd);

			lpDebugInfo->mps[0].addr = lpCalc->cpu.sp;
			lpDebugInfo->mps[0].mode = MEM_WORD;
			lpDebugInfo->mps[0].display = HEX;
			lpDebugInfo->mps[0].sel = lpDebugInfo->mps[1].addr;
			lpDebugInfo->mps[0].track = offsetof(struct CPU, sp);
			lpDebugInfo->mps[0].memNum = -1;
			lpDebugInfo->mps[0].lpCalc = lpCalc;

			LPTABWINDOWINFO lpTabInfo = (LPTABWINDOWINFO) malloc(sizeof(TABWINDOWINFO));
			lpTabInfo->lpDebugInfo = lpDebugInfo;
			lpTabInfo->tabInfo = &lpDebugInfo->mps[0];

			CreateWindow(
				g_szMemName,
				_T("Stack"),
				WS_VISIBLE | WS_CHILD,
				0, 0, 1, 1,
				hwnd,
				(HMENU) ID_STACK,
				g_hInst, lpTabInfo);

			if (lpDebugInfo->expand_pane_state.total != 0) {
				SetExpandPaneState(lpDebugInfo, &lpDebugInfo->expand_pane_state);
				SendMessage(lpDebugInfo->hreg, WM_SIZE, 0, 0); // didn't help
			}

			if (lpCalc->profiler.running) {
				CheckMenuItem(GetSubMenu(GetMenu(hwnd), 3), IDM_TOOLS_PROFILE, MF_BYCOMMAND | MF_CHECKED);
			}
			if (lpDebugInfo->code_count_tstates != -1) {
				CheckMenuItem(GetSubMenu(GetMenu(hwnd), 3), IDM_TOOLS_COUNT, MF_BYCOMMAND | MF_CHECKED);
			}

			calc_register_event(lpCalc, ROM_RUNNING_EVENT, &UpdateRunningMenu, hwnd);


			int index = TabCtrl_GetCurSel(lpDebugInfo->hdisasm);
			HWND hTab = GetDisasmPaneHWND(lpDebugInfo, index);
			SetFocus(hTab);
			lpDebugInfo->is_ready = TRUE;
			return 0;
		}
		case WM_SIZING:
		{
			RECT *r = (RECT *) lParam;
			int cyCaption = GetSystemMetrics(SM_CYCAPTION);
			int cyBottomFrame = GetSystemMetrics(SM_CYSIZEFRAME);
			int minHeight = CY_TOOLBAR + lpDebugInfo->cyGripper + cyCaption + cyBottomFrame*2;
			if (r->bottom - r->top < minHeight) {
				r->bottom = r->top + minHeight;
				return TRUE;
			}

			return FALSE;
		}
		case WM_SIZE: {
			RECT rc;
			GetClientRect(hwnd, &rc);

			if (!lpDebugInfo->bDrag) {
				if (lpDebugInfo->top_locked) {
					lpDebugInfo->cyDisasm = CY_TOOLBAR;
				} else if (lpDebugInfo->bottom_locked) {
					lpDebugInfo->cyDisasm = rc.bottom - lpDebugInfo->cyGripper;
				} else {
					u_int y = (u_int) (lpDebugInfo->ratioDisasm * rc.bottom);
					if (y < CY_TOOLBAR) y = CY_TOOLBAR;
					if (y > rc.bottom - lpDebugInfo->cyGripper) y = rc.bottom - lpDebugInfo->cyGripper;
					lpDebugInfo->cyDisasm = y;
				}
				lpDebugInfo->cyMem = rc.bottom - lpDebugInfo->cyDisasm;
			}

			EnumChildWindows(hwnd, EnumDebugResize, (LPARAM) lpDebugInfo);
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
			return 0;
		}
		case WM_MOUSEMOVE: {
			u_int y = GET_Y_LPARAM(lParam) + lpDebugInfo->offset_click;
			RECT rc;
			GetClientRect(hwnd, &rc);

			if (lpDebugInfo->bDrag) {
				if (y < CY_TOOLBAR + lpDebugInfo->cyGripper) {
					lpDebugInfo->top_locked = TRUE;
					lpDebugInfo->bottom_locked = FALSE;
					y = CY_TOOLBAR;
				}
				else if (y > rc.bottom - lpDebugInfo->cyGripper) {
					lpDebugInfo->bottom_locked = TRUE;
					lpDebugInfo->top_locked = FALSE;
					y = rc.bottom - lpDebugInfo->cyGripper;
				} else {
					lpDebugInfo->top_locked = FALSE;
					lpDebugInfo->bottom_locked = FALSE;
				}
				lpDebugInfo->cyDisasm = y;
				lpDebugInfo->cyMem = rc.bottom - lpDebugInfo->cyDisasm;
				SendMessage(hwnd, WM_SIZE, 0, 0);
			}
			HCURSOR hcursor = LoadCursor(NULL, IDC_SIZENS);
			int dy = abs((int)(y - (lpDebugInfo->cyDisasm - (lpDebugInfo->cyGripper/2))));

			if (dy < 16) {
				SetCursor(hcursor);
				if (lpDebugInfo->bHot == FALSE) {
					lpDebugInfo->bHot = TRUE;
					RECT r;
					GetClientRect(hwnd, &r);
					r.left = 0; r.right -= REG_PANE_WIDTH;
					r.top = lpDebugInfo->cyDisasm;
					r.bottom = r.top + lpDebugInfo->cyGripper;
					InvalidateRect(hwnd, &r, FALSE);
				}

				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(tme);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hwnd;
				tme.dwHoverTime = 1;
				TrackMouseEvent(&tme);
			} else if (lpDebugInfo->bHot) {
				lpDebugInfo->bHot = FALSE;
				RECT r;
				GetClientRect(hwnd, &r);
				r.left = 0; r.right -= REG_PANE_WIDTH;
				r.top = lpDebugInfo->cyDisasm;
				r.bottom = r.top + lpDebugInfo->cyGripper;
				InvalidateRect(hwnd, &r, FALSE);
			}

			lpDebugInfo->ratioDisasm = (double) lpDebugInfo->cyDisasm / (double) rc.bottom;
			return 0;
		}
		case WM_MOUSELEAVE: {
			lpDebugInfo->bHot = FALSE;
			RECT r;
			GetClientRect(hwnd, &r);
			r.left = 0; r.right -= REG_PANE_WIDTH;
			r.top = lpDebugInfo->cyDisasm;
			r.bottom = r.top + lpDebugInfo->cyGripper;
			InvalidateRect(hwnd, &r, FALSE);
			return 0;
		}
		case WM_LBUTTONDOWN: {
			int y = GET_Y_LPARAM(lParam);
			int dy = abs((int) (y - (lpDebugInfo->cyDisasm + (lpDebugInfo->cyGripper/2))));

			if (dy < 16) {
				lpDebugInfo->bDrag = TRUE;
				lpDebugInfo->offset_click = lpDebugInfo->cyDisasm - y;
				SetCapture(hwnd);
				HCURSOR hcursor = LoadCursor(NULL, IDC_SIZENS);
				SetCursor(hcursor);
				RECT r;
				GetClientRect(hwnd, &r);
				r.left = 0; r.right -= REG_PANE_WIDTH;
				r.top = lpDebugInfo->cyDisasm;
				r.bottom = r.top + lpDebugInfo->cyGripper;
				InvalidateRect(hwnd, &r, FALSE);
			}
			return 0;
		}
		case WM_LBUTTONUP:  {
			ReleaseCapture();
			lpDebugInfo->bDrag = FALSE;
			RECT r;
			GetClientRect(hwnd, &r);
			r.left = 0; r.right -= REG_PANE_WIDTH;
			r.top = lpDebugInfo->cyDisasm;
			r.bottom = r.top + lpDebugInfo->cyGripper;
			InvalidateRect(hwnd, &r, FALSE);
			return 0;
		}
		case WM_LBUTTONDBLCLK: {
			int y = GET_Y_LPARAM(lParam);
			int dy = abs((int) (y - (lpDebugInfo->cyDisasm + (lpDebugInfo->cyGripper/2))));

			if (dy < 16) {
				lpDebugInfo->ratioDisasm = 0.5;
				SendMessage(hwnd, WM_SIZE, 0, 0);
			}
			return 0;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
			case IDM_FILE_RESET: {
				LPCALC lpCalc = lpDebugInfo->lpCalc;
				calc_reset(lpCalc);
				HWND hTab = GetDisasmPaneHWND(lpDebugInfo, TabCtrl_GetCurSel(lpDebugInfo->hdisasm));
				DisasmGotoAddress(hTab, 0);
				Debug_UpdateWindow(hwnd);
				break;
			}
			case IDM_FILE_EXIT:
				DestroyWindow(hwnd);
				break;
			case IDM_TOOLS_COUNT: {
				HMENU hmenu = GetMenu(hwnd);
				LPCALC lpCalc = lpDebugInfo->lpCalc;
				if (lpDebugInfo->code_count_tstates == -1) {
					lpDebugInfo->code_count_tstates = lpCalc->cpu.timer_c->tstates;
					CheckMenuItem(GetSubMenu(hmenu, 3), IDM_TOOLS_COUNT, MF_BYCOMMAND | MF_CHECKED);
				} else {
					TCHAR buffer[256];
					StringCbPrintf(buffer, sizeof(buffer), _T("%i T-States"),
						(int)(lpCalc->cpu.timer_c->tstates - lpDebugInfo->code_count_tstates));
					MessageBox(NULL, buffer, _T("Code Counter"), MB_OK);
					lpDebugInfo->code_count_tstates = -1;
					CheckMenuItem(GetSubMenu(hmenu, 3), IDM_TOOLS_COUNT, MF_BYCOMMAND | MF_UNCHECKED);
				}
				break;
			}
			case IDM_TOOLS_DUMP: {
				TCHAR path[MAX_PATH];
				if (!SaveFile(path, _T("Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0"), _T("Save Dump"), _T("txt"), OFN_PATHMUSTEXIST, 0)) {
					WriteHumanReadableDump(lpDebugInfo->lpCalc, path);
				}
				break;
			}
			case IDM_TOOLS_PROFILE: {
				LPCALC lpCalc = lpDebugInfo->lpCalc;
				lpCalc->profiler.running = !lpCalc->profiler.running;
				HMENU hmenu = GetMenu(hwnd);
				if (lpCalc->profiler.running) {
					int result = (int) DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_DLGPROFILE), hwnd, (DLGPROC) ProfileDialogProc, (LPARAM) lpCalc);
					if (result == IDCANCEL) {
						lpCalc->profiler.running = !lpCalc->profiler.running;
					} else {
						ZeroMemory(lpCalc->profiler.flash_data, sizeof(lpCalc->profiler.flash_data));
						ZeroMemory(lpCalc->profiler.ram_data, sizeof(lpCalc->profiler.ram_data));
						CheckMenuItem(GetSubMenu(hmenu, 3), IDM_TOOLS_PROFILE, MF_BYCOMMAND | MF_CHECKED);
					}
				} else {
					FILE* file;
					double data;
					TCHAR buffer[MAX_PATH];
					if (BrowseFile(buffer, _T("	Text file  (*.txt)\0*.txt\0	All Files (*.*)\0*.*\0\0"),
						_T("Wabbitemu Save Profile"), _T("txt"), 0, 1)) {
						//make the profiler running again
						lpCalc->profiler.running = TRUE;
						break;
					}
					profiler_t *profiler = &lpCalc->profiler;
					_tfopen_s(&file, buffer, _T("wb"));
					_ftprintf_s(file, _T("Total Tstates: %i\r\n"), profiler->totalTime);
					_ftprintf_s(file, _T("Flash Memory:\r\n"));
					for (int j = 0; j < lpCalc->cpu.mem_c->flash_pages; j++)
					{
						for (int i = 0; i < PAGE_SIZE / profiler->blockSize; i++) 
						{
							data = (double) profiler->flash_data[j][i] / (double) profiler->totalTime;
							if (data != 0.0) 
							{
								_ftprintf_s(file, _T("%02X: $%04X - $%04X:\t%f%%\t%d tstates\r\n"), 
												j,
												i * profiler->blockSize, 
												((i + 1) * profiler->blockSize) - 1,
												data * 100,
												profiler->flash_data[j][i]);
							}
						}
					}
					_ftprintf_s(file, _T("\r\nRAM:\r\n"));
					for (int j = 0; j < lpCalc->cpu.mem_c->ram_pages; j++)
					{
						for (int i = 0; i < PAGE_SIZE / profiler->blockSize; i++) 
						{
							data = (double) profiler->ram_data[j][i] / (double) profiler->totalTime;
							if (data != 0.0) 
							{
								_ftprintf_s(file, _T("$%02X: $%04X - $%04X:\t%f%%\t%d tstates\r\n"), 
												j,
												i * profiler->blockSize, 
												((i + 1) * profiler->blockSize) - 1,
												data,
												profiler->ram_data[j][i]);
							}
						}
					}
					fclose(file);
					CheckMenuItem(GetSubMenu(hmenu, 3), IDM_TOOLS_PROFILE, MF_BYCOMMAND | MF_UNCHECKED);
				}
				break;
			}
			case IDM_DISPLAYBASE_HEX:
				CheckMenuItem(GetMenu(hwnd), IDM_DISPLAYBASE_HEX, MF_BYCOMMAND | MF_CHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_DISPLAYBASE_DEC, MF_BYCOMMAND | MF_UNCHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_DISPLAYBASE_BIN, MF_BYCOMMAND | MF_UNCHECKED);
				lpDebugInfo->dispType = HEX;
				lpDebugInfo->mps[TabCtrl_GetCurSel(lpDebugInfo->hmem) + 1].display = lpDebugInfo->dispType;
				Debug_UpdateWindow(hwnd);
				break;
			case IDM_DISPLAYBASE_BIN:
				CheckMenuItem(GetMenu(hwnd), IDM_DISPLAYBASE_HEX, MF_BYCOMMAND | MF_UNCHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_DISPLAYBASE_DEC, MF_BYCOMMAND | MF_UNCHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_DISPLAYBASE_BIN, MF_BYCOMMAND | MF_CHECKED);
				lpDebugInfo->dispType = BIN;
				lpDebugInfo->mps[TabCtrl_GetCurSel(lpDebugInfo->hmem) + 1].display = lpDebugInfo->dispType;
				Debug_UpdateWindow(hwnd);
				break;
			case IDM_DISPLAYBASE_DEC:
				CheckMenuItem(GetMenu(hwnd), IDM_DISPLAYBASE_HEX, MF_BYCOMMAND | MF_UNCHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_DISPLAYBASE_DEC, MF_BYCOMMAND | MF_CHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_DISPLAYBASE_BIN, MF_BYCOMMAND | MF_UNCHECKED);
				lpDebugInfo->dispType = DEC;
				lpDebugInfo->mps[TabCtrl_GetCurSel(lpDebugInfo->hmem) + 1].display = lpDebugInfo->dispType;
				Debug_UpdateWindow(hwnd);
				break;
			case IDM_VIEW_BYTE: {
				CheckMenuItem(GetMenu(hwnd), IDM_VIEW_BYTE, MF_BYCOMMAND | MF_CHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_VIEW_WORD, MF_BYCOMMAND | MF_UNCHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_VIEW_CHAR, MF_BYCOMMAND | MF_UNCHECKED);
				int index = TabCtrl_GetCurSel(lpDebugInfo->hmem) + 1;
				lpDebugInfo->mps[index].mode = MEM_BYTE;
				lpDebugInfo->mps[index].bText = FALSE;
				Debug_UpdateWindow(hwnd);
				break;
			}
			case IDM_VIEW_WORD: {
				CheckMenuItem(GetMenu(hwnd), IDM_VIEW_BYTE, MF_BYCOMMAND | MF_UNCHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_VIEW_WORD, MF_BYCOMMAND | MF_CHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_VIEW_CHAR, MF_BYCOMMAND | MF_UNCHECKED);
				int index = TabCtrl_GetCurSel(lpDebugInfo->hmem) + 1;
				lpDebugInfo->mps[index].mode = MEM_WORD;
				lpDebugInfo->mps[index].bText = FALSE;
				Debug_UpdateWindow(hwnd);
				break;
			}
			case IDM_VIEW_CHAR: {
				CheckMenuItem(GetMenu(hwnd), IDM_VIEW_BYTE, MF_BYCOMMAND | MF_UNCHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_VIEW_WORD, MF_BYCOMMAND | MF_UNCHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_VIEW_CHAR, MF_BYCOMMAND | MF_CHECKED);
				int index = TabCtrl_GetCurSel(lpDebugInfo->hmem) + 1;
				lpDebugInfo->mps[index].mode = MEM_BYTE;
				lpDebugInfo->mps[index].bText = TRUE;
				Debug_UpdateWindow(hwnd);
				break;
			}
			case IDM_VIEW_ADDMEM: {
				HMENU hMenu = GetMenu(hwnd);
				if (lpDebugInfo->total_mem_pane > MAX_MEM_TABS) {
					break;
				}
				AddMemTab(lpDebugInfo->lpCalc, lpDebugInfo->mps, REGULAR, lpDebugInfo);
				AddMemTab(lpDebugInfo->lpCalc, lpDebugInfo->mps, FLASH, lpDebugInfo);
				AddMemTab(lpDebugInfo->lpCalc, lpDebugInfo->mps, RAM, lpDebugInfo);
				if (lpDebugInfo->total_mem_pane == MAX_MEM_TABS) {
					EnableMenuItem(hMenu, IDM_VIEW_ADDMEM, MF_BYCOMMAND | MF_DISABLED);
				}
				if (lpDebugInfo->total_mem_pane <= 4) {
					EnableMenuItem(hMenu, IDM_VIEW_DELMEM, MF_BYCOMMAND | MF_DISABLED);
				} else {
					EnableMenuItem(hMenu, IDM_VIEW_DELMEM, MF_BYCOMMAND | MF_ENABLED);
				}

				LPNMHDR pnmhdr = (LPNMHDR) malloc(sizeof(NMHDR));
				pnmhdr->code = TCN_SELCHANGE;
				pnmhdr->hwndFrom = lpDebugInfo->hmem;
				SendMessage(hwnd, WM_NOTIFY, TCN_SELCHANGE, (LPARAM) pnmhdr);
				free(pnmhdr);
				InvalidateRect(lpDebugInfo->hmem, NULL, FALSE);
				Debug_UpdateWindow(hwnd);
				break;
			}
			case IDM_VIEW_DELMEM: {
				HMENU hMenu = GetMenu(hwnd);
				EnableMenuItem(hMenu, IDM_VIEW_ADDMEM, MF_BYCOMMAND | MF_ENABLED);
				if (lpDebugInfo->total_mem_pane == 3) {
					break;
				}
				lpDebugInfo->total_mem_pane -= 3;
				if (lpDebugInfo->total_mem_pane == 3) {
					EnableMenuItem(hMenu, IDM_VIEW_DELMEM, MF_BYCOMMAND | MF_DISABLED);
				}
				if (TabCtrl_GetCurSel(lpDebugInfo->hmem) >= lpDebugInfo->total_mem_pane) {
					TabCtrl_SetCurSel(lpDebugInfo->hmem, lpDebugInfo->total_mem_pane - 1);
				}
				TabCtrl_DeleteItem(lpDebugInfo->hmem, lpDebugInfo->total_mem_pane + 2);
				TabCtrl_DeleteItem(lpDebugInfo->hmem, lpDebugInfo->total_mem_pane + 1);
				TabCtrl_DeleteItem(lpDebugInfo->hmem, lpDebugInfo->total_mem_pane);
				DestroyWindow(lpDebugInfo->hmemlist[lpDebugInfo->total_mem_pane + 2]);
				DestroyWindow(lpDebugInfo->hmemlist[lpDebugInfo->total_mem_pane + 1]);
				DestroyWindow(lpDebugInfo->hmemlist[lpDebugInfo->total_mem_pane]);
				lpDebugInfo->hmemlist[lpDebugInfo->total_mem_pane] = NULL;
				lpDebugInfo->hmemlist[lpDebugInfo->total_mem_pane + 1] = NULL;
				lpDebugInfo->hmemlist[lpDebugInfo->total_mem_pane + 2] = NULL;

				LPNMHDR pnmhdr = (LPNMHDR) malloc(sizeof(NMHDR));
				pnmhdr->code = TCN_SELCHANGE;
				pnmhdr->hwndFrom = lpDebugInfo->hmem;
				SendMessage(hwnd, WM_NOTIFY, TCN_SELCHANGE, (LPARAM) pnmhdr);
				InvalidateRect(lpDebugInfo->hmem, NULL, FALSE);
				Debug_UpdateWindow(hwnd);
				break;
			}
			case IDM_VIEW_ADDDISASM: {
				HMENU hMenu = GetMenu(hwnd);
				if (lpDebugInfo->total_disasm_pane + 1 > MAX_DISASM_TABS) {
					break;
				}
				AddDisasmTab(lpDebugInfo, REGULAR);
				AddDisasmTab(lpDebugInfo, FLASH);
				AddDisasmTab(lpDebugInfo, RAM);
				if (lpDebugInfo->total_disasm_pane == MAX_DISASM_TABS) {
					EnableMenuItem(hMenu, IDM_VIEW_ADDDISASM, MF_BYCOMMAND | MF_DISABLED);
				}
				if (lpDebugInfo->total_disasm_pane == 3) {
					EnableMenuItem(hMenu, IDM_VIEW_DELDISASM, MF_BYCOMMAND | MF_DISABLED);
				} else {
					EnableMenuItem(hMenu, IDM_VIEW_DELDISASM, MF_BYCOMMAND | MF_ENABLED);
				}

				LPNMHDR pnmhdr = (LPNMHDR) malloc(sizeof(NMHDR));
				pnmhdr->code = TCN_SELCHANGE;
				pnmhdr->hwndFrom = lpDebugInfo->hdisasm;
				SendMessage(hwnd, WM_NOTIFY, TCN_SELCHANGE, (LPARAM) pnmhdr);
				free(pnmhdr);
				Debug_UpdateWindow(hwnd);
				break;
			}
			case IDM_VIEW_DELDISASM: {
				HMENU hMenu = GetMenu(hwnd);
				EnableMenuItem(hMenu, IDM_VIEW_ADDDISASM, MF_BYCOMMAND | MF_ENABLED);
				if (lpDebugInfo->total_disasm_pane == 3) {
					break;
				}
				lpDebugInfo->total_disasm_pane -= 3;
				if (lpDebugInfo->total_disasm_pane == 3) {
					EnableMenuItem(hMenu, IDM_VIEW_DELDISASM, MF_BYCOMMAND | MF_DISABLED);
				}
				if (TabCtrl_GetCurSel(lpDebugInfo->hdisasm) >= lpDebugInfo->total_disasm_pane) {
					TabCtrl_SetCurSel(lpDebugInfo->hdisasm, lpDebugInfo->total_disasm_pane - 1);
				}
				TabCtrl_DeleteItem(lpDebugInfo->hdisasm, lpDebugInfo->total_disasm_pane + 2);
				TabCtrl_DeleteItem(lpDebugInfo->hdisasm, lpDebugInfo->total_disasm_pane + 1);
				TabCtrl_DeleteItem(lpDebugInfo->hdisasm, lpDebugInfo->total_disasm_pane);
				DestroyWindow(lpDebugInfo->hdisasmlist[lpDebugInfo->total_disasm_pane + 2]);
				DestroyWindow(lpDebugInfo->hdisasmlist[lpDebugInfo->total_disasm_pane + 1]);
				DestroyWindow(lpDebugInfo->hdisasmlist[lpDebugInfo->total_disasm_pane]);
				lpDebugInfo->hdisasmlist[lpDebugInfo->total_disasm_pane + 2] = NULL;
				lpDebugInfo->hdisasmlist[lpDebugInfo->total_disasm_pane + 1] = NULL;
				lpDebugInfo->hdisasmlist[lpDebugInfo->total_disasm_pane] = NULL;

				LPNMHDR pnmhdr = (LPNMHDR) malloc(sizeof(NMHDR));
				pnmhdr->code = TCN_SELCHANGE;
				pnmhdr->hwndFrom = lpDebugInfo->hdisasm;
				SendMessage(hwnd, WM_NOTIFY, TCN_SELCHANGE, (LPARAM) pnmhdr);
				free(pnmhdr);
				Debug_UpdateWindow(hwnd);
				break;
			}
			case DB_BREAKPOINTS:
			case IDM_VIEW_BREAKPOINTS: {
				if (IsWindow(lpDebugInfo->hBreakpoints)) {
					SwitchToThisWindow(lpDebugInfo->hBreakpoints, TRUE);
				} else {
					lpDebugInfo->hBreakpoints = CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_BREAKPOINT),
						hwnd, (DLGPROC) BreakpointsDialogProc, (LPARAM) lpDebugInfo->lpCalc);
					ShowWindow(lpDebugInfo->hBreakpoints, SW_SHOW);
				}
				break;
			}
			case DB_REGULAR_DISASM_GOTO_ADDR: {
				LPNMHDR pnmhdr = (LPNMHDR) malloc(sizeof(NMHDR));
				pnmhdr->code = TCN_SELCHANGE;
				pnmhdr->hwndFrom = lpDebugInfo->hdisasm;
				//first mapped disassembly tab
				TabCtrl_SetCurSel(lpDebugInfo->hdisasm, 0);
				SendMessage(hwnd, WM_NOTIFY, TCN_SELCHANGE, (LPARAM) pnmhdr);
				free(pnmhdr);
				HWND hTab = GetDisasmPaneHWND(lpDebugInfo, TabCtrl_GetCurSel(lpDebugInfo->hdisasm));
				DisasmGotoAddress(hTab, (int) lParam);
				break;
			}
			case DB_REGULAR_MEM_GOTO_ADDR: {
				LPNMHDR pnmhdr = (LPNMHDR) malloc(sizeof(NMHDR));
				pnmhdr->code = TCN_SELCHANGE;
				pnmhdr->hwndFrom = lpDebugInfo->hdisasm;
				//first mapped mem tab
				TabCtrl_SetCurSel(lpDebugInfo->hdisasm, 0);
				SendMessage(hwnd, WM_NOTIFY, TCN_SELCHANGE, (LPARAM) pnmhdr);
				free(pnmhdr);
				HWND hTab = GetMemPaneHWND(lpDebugInfo, TabCtrl_GetCurSel(lpDebugInfo->hdisasm));
				MemGotoAddress(hTab, (int) lParam);
				break;
			}
			case DB_DISASM_GOTO_ADDR: {
				waddr_t *waddr = (waddr_t *) lParam;
				//whose dumbass idea was this? you need to manually send the SELCHANGE notify :|
				LPNMHDR pnmhdr = (LPNMHDR) malloc(sizeof(NMHDR));
				pnmhdr->code = TCN_SELCHANGE;
				pnmhdr->hwndFrom = lpDebugInfo->hdisasm;
				if (waddr->is_ram) {
					//first ram tab
					TabCtrl_SetCurSel(lpDebugInfo->hdisasm, 2); 
				} else {
					//first flash tab
					TabCtrl_SetCurSel(lpDebugInfo->hdisasm, 1);
				}
				SendMessage(hwnd, WM_NOTIFY, TCN_SELCHANGE, (LPARAM) pnmhdr);
				free(pnmhdr);
				int addr = (waddr->addr % PAGE_SIZE) + waddr->page * PAGE_SIZE;
				DisasmGotoAddress(lpDebugInfo->hdisasmlist[TabCtrl_GetCurSel(lpDebugInfo->hdisasm)], addr);
				break;
			}
			case DB_MEM_GOTO_ADDR: {
				waddr_t *waddr = (waddr_t *) lParam;
				LPNMHDR pnmhdr = (LPNMHDR) malloc(sizeof(NMHDR));
				pnmhdr->code = TCN_SELCHANGE;
				pnmhdr->hwndFrom = lpDebugInfo->hmem;
				if (waddr->is_ram) {
					//first ram tab
					TabCtrl_SetCurSel(lpDebugInfo->hmem, 2); 
				} else {
					//first flash tab
					TabCtrl_SetCurSel(lpDebugInfo->hmem, 1);
				}
				SendMessage(hwnd, WM_NOTIFY, TCN_SELCHANGE, (LPARAM) pnmhdr);
				free(pnmhdr);
				int addr = (waddr->addr % PAGE_SIZE) + waddr->page * PAGE_SIZE;
				MemGotoAddress(lpDebugInfo->hmemlist[TabCtrl_GetCurSel(lpDebugInfo->hmem)], addr);
				break;
			}
			case DB_BREAKPOINT:
			case DB_MEMPOINT_READ:
			case DB_MEMPOINT_WRITE: {
				int memIndex = TabCtrl_GetCurSel(lpDebugInfo->hmem);
				int disasmIndex = TabCtrl_GetCurSel(lpDebugInfo->hdisasm);
				HWND hMemTab = GetMemPaneHWND(lpDebugInfo, memIndex);
				HWND hDisasmTab = GetDisasmPaneHWND(lpDebugInfo, disasmIndex);
				if (hMemTab == hwndLastFocus) {
					SendMessage(hMemTab, Message, wParam, lParam);
				} else if (hDisasmTab == hwndLastFocus) {
					SendMessage(hDisasmTab, Message, wParam, lParam);
				}
				Debug_UpdateWindow(hwnd);
				break;
			}
			case IDM_RUN_RUN: {
				LPCALC lpCalc = lpDebugInfo->lpCalc;
				if (lpCalc->running) {
					SendMessage(hwnd, WM_COMMAND, DB_STOP, 0);
				} else {
					SendMessage(hwnd, WM_COMMAND, DB_RUN, 0);
				}
				break;
			}
			case DB_RUN: {
				LPCALC lpCalc = lpDebugInfo->lpCalc;
				CPU_step(&lpCalc->cpu);
				calc_set_running(lpCalc, TRUE);
				Debug_UpdateWindow(hwnd);
				break;
			}
			case DB_STOP: {
				LPCALC lpCalc = lpDebugInfo->lpCalc;
				calc_set_running(lpCalc, FALSE);
				Debug_UpdateWindow(hwnd);
				break;
			}
			default: {
				int index = TabCtrl_GetCurSel(lpDebugInfo->hdisasm);
				HWND hTab = GetDisasmPaneHWND(lpDebugInfo, index);
				SendMessage(hTab, Message, wParam, lParam);
				break;
			}
			}
			break;
		}
		case WM_NOTIFY: {
			LPNMHDR header = (LPNMHDR) lParam;
			switch (header->code) {
				case TCN_SELCHANGE: {
					if (header->hwndFrom == lpDebugInfo->hmem) {
						int i;
						int index = TabCtrl_GetCurSel(lpDebugInfo->hmem);
						for (i = 0; i < lpDebugInfo->total_mem_pane + EXTRA_MEM_PANES; i++) {
							HWND hTab = GetMemPaneHWND(lpDebugInfo, i);
							if (i == index) {
								ShowWindow(hTab, SW_SHOW);
							} else {
								ShowWindow(hTab, SW_HIDE);
							}
						}

						SendMessage(hwnd, WM_SIZE, 0 , 0);
					} else if (header->hwndFrom == lpDebugInfo->hdisasm) {
						int i;
						int index = TabCtrl_GetCurSel(lpDebugInfo->hdisasm);
						for (i = 0; i < lpDebugInfo->total_disasm_pane + EXTRA_DISASM_PANES; i++) {
							HWND hTab = GetDisasmPaneHWND(lpDebugInfo, i);
							if (i == index) {
								ShowWindow(hTab, SW_SHOW);
							} else {
								ShowWindow(hTab, SW_HIDE);
							}
						}

						SendMessage(hwnd, WM_SIZE, 0 , 0);
					}
				}
			}
			break;
		}
		case WM_PAINT: {
			// Paint the background and the gripper bar between disassembly and memory
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);

			RECT rectDot, rc;
			GetClientRect(hwnd, &rc);

			TRIVERTEX vert[2];
			GRADIENT_RECT gRect;
			gRect.UpperLeft  = 0;
			gRect.LowerRight = 1;

			COLORREF DarkEdge, LightEdge;

#define DARKEN_AMOUNT 0x303030

			if (lpDebugInfo->bDrag) {
				DarkEdge = GetSysColor(COLOR_3DDKSHADOW) - DARKEN_AMOUNT;
				LightEdge = GetSysColor(COLOR_3DHILIGHT) - DARKEN_AMOUNT;
			} else if (lpDebugInfo->bHot) {
				DarkEdge = GetSysColor(COLOR_BTNFACE) - DARKEN_AMOUNT;
				LightEdge = GetSysColor(COLOR_BTNFACE);
			} else {
				DarkEdge = GetSysColor(COLOR_BTNSHADOW);
				LightEdge = GetSysColor(COLOR_BTNFACE);
			}

			vert[0].x = 0;
			// 40%
			vert[0].y = lpDebugInfo->cyDisasm + lpDebugInfo->cyGripper * 4 / 10;
			vert[0].Red = GetRValue(LightEdge) << 8;
			vert[0].Green = GetGValue(LightEdge) << 8;
			vert[0].Blue = GetBValue(LightEdge) << 8;

			vert[1].x = rc.right - REG_PANE_WIDTH;
			vert[1].y = lpDebugInfo->cyDisasm + lpDebugInfo->cyGripper;
			vert[1].Red = GetRValue(DarkEdge) << 8;
			vert[1].Green = GetGValue(DarkEdge) << 8;
			vert[1].Blue = GetBValue(DarkEdge) << 8;

			GradientFill(hdc,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);

#define DOT_WIDTH	4

			HBRUSH hbr = CreateSolidBrush(DarkEdge); //GetSysColorBrush(COLOR_BTNSHADOW);

			rectDot.left = (rc.right-REG_PANE_WIDTH)/2 - 12;
			rectDot.right = rectDot.left + DOT_WIDTH;

			rectDot.top = lpDebugInfo->cyDisasm + lpDebugInfo->cyGripper/2 - DOT_WIDTH/2;
			rectDot.bottom = rectDot.top + DOT_WIDTH;
			SelectObject(hdc, hbr);
			SelectObject(hdc, GetStockObject(DC_PEN));
			SetDCPenColor(hdc, DarkEdge - DARKEN_AMOUNT);

			if (lpDebugInfo->bDrag)
				OffsetRect(&rectDot, 1, 1);

			int i;
			for (i = 0; i < 3; i ++, OffsetRect(&rectDot, DOT_WIDTH*2, 0)) {
				Ellipse(hdc, rectDot.left, rectDot.top, rectDot.right, rectDot.bottom);
			}
			RECT r;
			r.left = 0; r.right = rc.right - REG_PANE_WIDTH;
			r.top = lpDebugInfo->cyDisasm;
			r.bottom = r.top + lpDebugInfo->cyGripper;

			if (lpDebugInfo->bDrag) {
				DrawEdge(hdc, &r, EDGE_SUNKEN, BF_TOP|BF_SOFT);
			} else {
				DrawEdge(hdc, &r, EDGE_RAISED, BF_TOP|BF_BOTTOM|BF_SOFT);
			}

			SelectObject(hdc, GetStockObject(DC_PEN));
			SetDCPenColor(hdc, GetSysColor(COLOR_BTNSHADOW)); //(147, 176, 194));
			MoveToEx(hdc, rc.right - 102 - REG_PANE_WIDTH, lpDebugInfo->cyDisasm + lpDebugInfo->cyGripper, NULL);
			LineTo(hdc, rc.right - 102 - REG_PANE_WIDTH, rc.bottom);

			DeleteObject(hbr);

			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_USER:
			switch (wParam) {
			case DB_UPDATE: {
				EnumChildWindows(hwnd, EnumDebugUpdate, 0);
				break;
			}
			case DB_RESUME:
				EnumChildWindows(hwnd, EnumDebugResume, 0);
				break;
			case DB_GOTO_RESULT:
				SendMessage(hwndLastFocus, WM_USER, DB_GOTO_RESULT, lParam);
				break;
			}
			return 0;
		case WM_DESTROY: {
			LPCALC lpCalc = lpDebugInfo->lpCalc;
			calc_unregister_event(lpCalc, ROM_RUNNING_EVENT, &UpdateRunningMenu, hwnd);
			CPU_step(&lpCalc->cpu);
			calc_set_running(lpCalc, TRUE);

			GetWindowPlacement(hwnd, &lpDebugInfo->db_placement);
			lpDebugInfo->db_maximized = IsMaximized(hwnd);

			int selIndex = TabCtrl_GetCurSel(lpDebugInfo->hmem);
			int groupIndex = lpDebugInfo->total_mem_pane / 3;
			SaveDebugKey((TCHAR *) MemPaneString, REG_DWORD, &groupIndex);
			SaveDebugKey((TCHAR *) MemSelIndexString, REG_DWORD, &selIndex);
			SaveDebugKey((TCHAR *) DisplayTypeString, REG_DWORD, &lpDebugInfo->dispType);
			GetExpandPaneState(lpDebugInfo, &lpDebugInfo->expand_pane_state);
			DeleteObject(lpDebugInfo->hfontLucida);
			lpDebugInfo->hfontLucida = NULL;
			DeleteObject(lpDebugInfo->hfontLucidaBold);
			lpDebugInfo->hfontLucidaBold = NULL;
			DeleteObject(lpDebugInfo->hfontSegoe);
			lpDebugInfo->hfontSegoe = NULL;
			lpDebugInfo->hwatch = NULL;

			EnumChildWindows(hwnd, EnumDebugDestroy, 0);
			free(lpDebugInfo);
			return 0;
		}
	}
	return DefWindowProc(hwnd, Message, wParam, lParam);
}


void WriteHumanReadableDump(LPCALC lpCalc, TCHAR *path) {
	FILE *file;
	_tfopen_s(&file, path, _T("w"));
	
	//Write CPU output
	_fputts(_T("CPU Registers:\n"), file);
	_ftprintf_s(file, _T("AF: %04X\tAF': %04X\n"), lpCalc->cpu.af, lpCalc->cpu.afp);
	_ftprintf_s(file, _T("BC: %04X\tBC': %04X\n"), lpCalc->cpu.bc, lpCalc->cpu.bcp);
	_ftprintf_s(file, _T("DE: %04X\tDE': %04X\n"), lpCalc->cpu.de, lpCalc->cpu.dep);
	_ftprintf_s(file, _T("HL: %04X\tHL': %04X\n"), lpCalc->cpu.hl, lpCalc->cpu.hlp);
	_ftprintf_s(file, _T("IX: %04X\tIY: %04X\n"), lpCalc->cpu.ix, lpCalc->cpu.iy);
	_ftprintf_s(file, _T("PC: %04X\tSP: %04X\n"), lpCalc->cpu.pc, lpCalc->cpu.sp);
	
	_fputts(_T("\nCPU Status:\n"), file);
	_ftprintf_s(file, _T("Bus: %02X\n"), lpCalc->cpu.bus);
	_ftprintf(file, _T("%s %s"), _T("Halted:"), lpCalc->cpu.halt ? _T("True\n") : _T("False\n"));
	_ftprintf_s(file, _T("Freq: %d\n"), lpCalc->cpu.timer_c->freq);
	_ftprintf_s(file, _T("T-States: %d\n"), lpCalc->cpu.timer_c->tstates);
	
	_fputts(_T("\nInterrupt Status:\n"), file);
	_ftprintf(file, _T("%s %s"), _T("Interrupt:"), lpCalc->cpu.interrupt ? _T("True\n") : _T("False\n"));
	_ftprintf(file, _T("%s %s"), _T("IFF1:"), lpCalc->cpu.iff1 ? _T("True\n") : _T("False\n"));
	_ftprintf(file, _T("%s %s"), _T("IFF2:"), lpCalc->cpu.iff2 ? _T("True\n") : _T("False\n"));
	_ftprintf_s(file, _T("IM %d\n"), lpCalc->cpu.imode);
	
	if (lpCalc->model < TI_84PCSE) {
		_fputts(_T("\nDisplay:\n"), file);
		LCD_t *lcd = (LCD_t *)lpCalc->cpu.pio.lcd;
		uint8_t(*lcdData)[LCD_WIDTH] = (uint8_t(*)[LCD_WIDTH]) lcd->base.image(&lcd->base);
		for (int i = 0; i < LCD_HEIGHT * 4; i += 4) {
			for (int j = 0; j < lpCalc->cpu.pio.lcd->width * 2; j += 2) {
				if (lcdData[i][j] < lcd->shades / 4) {
					_fputtc(' ', file);
				}
				/*else if (lcd[i][j] < lpCalc->cpu.pio.lcd->shades / 2)
					_fputtc(176, file);
					else if (lcd[i][j] < lpCalc->cpu.pio.lcd->shades * 3 / 4)
					_fputtc(178 , file);*/
				else {
					_fputtc('#', file);
				}
			}
			_fputtc('\n', file);
		}
	}
	
	_fputts(_T("\nPorts:\n"), file);
	CPU_t *cpu = CPU_clone(&lpCalc->cpu);
	for (int i = 0; i < 256; i++) {
		device_t *device = &cpu->pio.devices[i];
		if (device->active)
		{
			cpu->input = TRUE;
			device->code(cpu, device);
			_ftprintf_s(file, _T("Port %02X: %02X\n"), i, cpu->bus);
		}
	}
	free(cpu);

	_fputts(_T("\nFlash:\n"), file);
	for (int page = 0; page < lpCalc->cpu.mem_c->flash_pages; page++) {
		for (int addr = 0; addr < PAGE_SIZE; addr += 32) {
			_ftprintf_s(file, _T("%02X %04X: "), page, addr);
			for (int i = 0; i < 32; i++)
				_ftprintf_s(file, _T("%02X "), lpCalc->cpu.mem_c->flash[page * PAGE_SIZE + addr + i]);
			_fputtc('\n', file);
		}
	}

	_fputts(_T("\nRAM:\n"), file);
	for (int page = 0; page < lpCalc->cpu.mem_c->ram_pages; page++) {
		for (int addr = 0; addr < PAGE_SIZE; addr += 32) {
			_ftprintf_s(file, _T("%02X %04X: "), page, addr);
			for (int i = 0; i < 32; i++)
				_ftprintf_s(file, _T("%02X "), lpCalc->cpu.mem_c->ram[page * PAGE_SIZE + addr + i]);
			_fputtc('\n', file);
		}
	}

	fclose(file);
}