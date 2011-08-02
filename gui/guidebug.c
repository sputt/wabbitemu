#include "stdafx.h"

#include "guidebug.h"

#include "DBPaneContainer.h"

#include "calc.h"
#include "dbmem.h"
#include "dbcommon.h"
#include "dbdisasm.h"
#include "dbmonitor.h"
#include "dbbreakpoints.h"
#include "dbprofile.h"
#include "dbreg.h"
#include "expandpane.h"
#include "registry.h"
#include "resource.h"
#include "fileutilities.h"
#include "gifhandle.h"


extern HINSTANCE g_hInst;

void WriteHumanReadableDump(LPCALC lpCalc, TCHAR *path);

WINDOWPLACEMENT db_placement = { NULL };
BOOL db_maximized = FALSE;
DISPLAY_BASE dispType = HEX;
HFONT hfontSegoe, hfontLucida, hfontLucidaBold;

#define CY_TOOLBAR 32
static unsigned int cyGripper = 10;
static unsigned int cyDisasm = 350, cyMem;

#define MAX_TABS 5
#define MAX_MEM_TABS MAX_TABS*3
#define MAX_DISASM_TABS MAX_TABS*3

static ep_state expand_pane_state = {0};
HWND htoolbar, hdisasm, hreg, hmem, hwatch, hPortMon, hBreakpoints;
static int total_mem_pane, total_disasm_pane;
static HWND hmemlist[MAX_MEM_TABS];
static HWND hdisasmlist[MAX_DISASM_TABS];
static long long code_count_tstates = -1;


BOOL CALLBACK EnumDebugResize(HWND hwndChild, LPARAM lParam) {
	int idChild;
	RECT rc;
	RECT *rcParent = &rc;
	GetClientRect((HWND) lParam, rcParent);

	// Is it a first level child?
	if ((HWND) lParam != GetParent(hwndChild))
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
		tabRc.bottom = cyDisasm;
		MoveWindow(hwndChild, tabRc.left, tabRc.top, tabRc.right - tabRc.left, tabRc.bottom - tabRc.top, FALSE);
		int index = TabCtrl_GetCurSel(hwndChild);
		tabRc.top = 0;
		tabRc.bottom = cyDisasm - CY_TOOLBAR;
		TabCtrl_AdjustRect(hwndChild, FALSE, &tabRc);
		HWND curTab = hdisasmlist[index];
		MoveWindow(curTab, tabRc.left, tabRc.top, tabRc.right - tabRc.left, tabRc.bottom - tabRc.top, FALSE);
		SendMessage(curTab, WM_USER, DB_UPDATE, 0);
		break;
	}
	/*case ID_DISASM:
		MoveWindow(hwndChild, 0, CY_TOOLBAR, rcParent->right - REG_PANE_WIDTH, cyDisasm - CY_TOOLBAR, FALSE);
		break;*/
	case ID_MEMTAB: {
		MoveWindow(hwndChild, 3, cyDisasm + cyGripper, rcParent->right - 103 - REG_PANE_WIDTH - 8, cyMem - cyGripper - 3, FALSE);
		int index = TabCtrl_GetCurSel(hwndChild);
		HWND curTab = index == total_mem_pane ? hwatch : hmemlist[index];
		MoveWindow(curTab, 3, 24, rcParent->right - REG_PANE_WIDTH - 118, cyMem - cyGripper - 32, FALSE);
		SendMessage(curTab, WM_USER, DB_UPDATE, 0);
		break;
	}
	/*case ID_MEM:
		MoveWindow(hwndChild, 0, 0, rcParent->right, rcParent->bottom, TRUE);
		break;*/
	case ID_STACK:
		MoveWindow(hwndChild, rcParent->right - 110 - REG_PANE_WIDTH, cyDisasm + cyGripper, 110, cyMem- cyGripper, FALSE);
		break;
	case ID_REG:
		printf("Reg: left: %d, top:% d, height: %d\n", rcParent->right - REG_PANE_WIDTH, CY_TOOLBAR, rcParent->bottom);
		SetWindowPos(hwndChild, HWND_TOP, rcParent->right - REG_PANE_WIDTH, CY_TOOLBAR, REG_PANE_WIDTH, rcParent->bottom - CY_TOOLBAR, 0);
		MoveWindow(hwndChild, rcParent->right - REG_PANE_WIDTH, CY_TOOLBAR, REG_PANE_WIDTH, rcParent->bottom, FALSE);
		//ShowWindow(hwndChild, SW_HIDE);
		break;
	/*case ID_PANECONTAINER:
		{
			SetWindowPos(hwndChild, HWND_TOP, rcParent->right - REG_PANE_WIDTH, CY_TOOLBAR, REG_PANE_WIDTH, rcParent->bottom - CY_TOOLBAR, 0);
			break;
		}*/
	}
	SendMessage(hwndChild, WM_USER, DB_UPDATE, 0);
	return TRUE;
}


BOOL CALLBACK EnumDebugUpdate(HWND hwndChild, LPARAM lParam) {
	SendMessage(hwndChild, WM_USER, DB_UPDATE, lParam);
	return TRUE;
}

BOOL CALLBACK EnumDebugResume(HWND hwndChild, LPARAM lParam) {
	SendMessage(hwndChild, WM_USER, DB_UPDATE, lParam);
	SendMessage(hwndChild, WM_USER, DB_RESUME, lParam);
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

void AddDisasmTab(dp_settings *dps, ViewType type) {
	if (total_disasm_pane >= MAX_TABS)
		return;
	ZeroMemory(&dps[total_disasm_pane + 1], sizeof(dp_settings));
	dps[total_disasm_pane + 1].hdrs[0].nCharsWidth = 7;
	StringCbCopy(dps[total_disasm_pane + 1].hdrs[0].pszText, sizeof(dps[total_disasm_pane + 1].hdrs[0].pszText), _T("Addr"));
	dps[total_disasm_pane + 1].hdrs[0].lpfnCallback = &sprint_addr;
	dps[total_disasm_pane + 1].hdrs[1].index = 1;
	dps[total_disasm_pane + 1].hdrs[1].nCharsWidth = 11;
	StringCbCopy(dps[total_disasm_pane + 1].hdrs[1].pszText, sizeof(dps[total_disasm_pane + 1].hdrs[1].pszText), _T("Data"));
	dps[total_disasm_pane + 1].hdrs[1].lpfnCallback = &sprint_data;
	dps[total_disasm_pane + 1].hdrs[2].index = 2;
	dps[total_disasm_pane + 1].hdrs[2].nCharsWidth = 45;
	StringCbCopy(dps[total_disasm_pane + 1].hdrs[2].pszText, sizeof(dps[total_disasm_pane + 1].hdrs[2].pszText), _T("Disassembly"));
	dps[total_disasm_pane + 1].hdrs[2].lpfnCallback = &sprint_command;
	dps[total_disasm_pane + 1].hdrs[3].index = 3;
	dps[total_disasm_pane + 1].hdrs[3].nCharsWidth = 6;
	StringCbCopy(dps[total_disasm_pane + 1].hdrs[3].pszText, sizeof(dps[total_disasm_pane + 1].hdrs[3].pszText), _T("Size"));
	dps[total_disasm_pane + 1].hdrs[3].uFormat = DT_CENTER;
	dps[total_disasm_pane + 1].hdrs[3].lpfnCallback = &sprint_size;
	dps[total_disasm_pane + 1].hdrs[4].index = 4;
	dps[total_disasm_pane + 1].hdrs[4].nCharsWidth = 8;
	StringCbCopy(dps[total_disasm_pane + 1].hdrs[4].pszText, sizeof(dps[total_disasm_pane + 1].hdrs[4].pszText), _T("Clocks"));
	dps[total_disasm_pane + 1].hdrs[4].uFormat = DT_CENTER;
	dps[total_disasm_pane + 1].hdrs[4].lpfnCallback = &sprint_clocks;
	dps[total_disasm_pane + 1].hdrs[5].index = -1;
	dps[total_disasm_pane + 1].hdrs[5].lpfnCallback = &sprint_addr;
	dps[total_disasm_pane + 1].hdrs[6].index = -1;
	dps[total_disasm_pane + 1].hdrs[6].lpfnCallback = &sprint_addr;
	dps[total_disasm_pane + 1].hdrs[7].index = -1;
	dps[total_disasm_pane + 1].hdrs[7].lpfnCallback = &sprint_addr;
	dps->type = type;

	if (type == REGULAR)
		dps[total_disasm_pane + 1].nSel = lpDebuggerCalc->cpu.pc;

	hdisasmlist[total_disasm_pane] = CreateWindow(
		g_szDisasmName,
		_T("Disasm"),
		WS_VISIBLE | WS_CHILD | WS_VSCROLL,
		3, 20, 100, 10,
		hdisasm,
		(HMENU) ID_DISASM,
		g_hInst, &dps[total_disasm_pane + 1]);
	TCHAR buffer[64];
	switch (type) {
		case REGULAR:
			StringCbPrintf(buffer, sizeof(buffer), _T("Disasm %i"), (total_disasm_pane + 3) / 3);
			break;
		case FLASH:
			StringCbPrintf(buffer, sizeof(buffer), _T("Flash %i"), (total_disasm_pane + 3) / 3);
			break;
		case RAM:
			StringCbPrintf(buffer, sizeof(buffer), _T("RAM %i"), (total_disasm_pane + 3) / 3);
			break;
	}
	TCITEM tie;
	tie.mask = TCIF_TEXT | TCIF_IMAGE;
	tie.iImage = -1;
	tie.pszText = buffer;
	tie.lParam = (LPARAM)hdisasmlist[total_disasm_pane];
	TabCtrl_InsertItem(hdisasm, total_disasm_pane, &tie);
	TabCtrl_SetCurSel(hdisasm, total_disasm_pane);
	total_disasm_pane++;
}

void AddMemTab(mempane_settings *mps, ViewType type) {
	ShowWindow(hmemlist[TabCtrl_GetCurSel(hmem)], SW_HIDE);
	if (total_mem_pane >= MAX_TABS)
		return;

	mps[total_mem_pane + 1].addr = 0x0000;
	mps[total_mem_pane + 1].mode = MEM_BYTE;
	mps[total_mem_pane + 1].sel = 0x000;
	mps[total_mem_pane + 1].track = -1;
	mps[total_mem_pane + 1].memNum = total_mem_pane;
	mps[total_mem_pane + 1].type = type;

	hmemlist[total_mem_pane] = CreateWindow(
		g_szMemName,
		_T("Memory"),
		WS_VISIBLE | WS_CHILD | WS_VSCROLL,
		3, 20, 100, 10,
		hmem,
		(HMENU) ID_MEM,
		g_hInst, &mps[total_mem_pane + 1]);
	TCHAR buffer[64];
	switch (type) {
		case REGULAR:
			StringCbPrintf(buffer, sizeof(buffer), _T("Mem %i"), (total_mem_pane + 3) / 3);
			break;
		case FLASH:
			StringCbPrintf(buffer, sizeof(buffer), _T("Flash %i"), (total_mem_pane + 3) / 3);
			break;
		case RAM:
			StringCbPrintf(buffer, sizeof(buffer), _T("RAM %i"), (total_mem_pane + 3) / 3);
			break;
	}
	TCITEM tie;
	tie.mask = TCIF_TEXT | TCIF_IMAGE;
	tie.iImage = -1;
	tie.pszText = buffer;
	tie.lParam = (LPARAM)hmemlist[total_mem_pane];
	TabCtrl_InsertItem(hmem, total_mem_pane, &tie);
	TabCtrl_SetCurSel(hmem, total_mem_pane);
	total_mem_pane++;
}

void AddWatchTab(HWND hwnd) {
	if (hwatch)
		return;

	hwatch = CreateWindow(
		g_szWatchName,
		_T(""),
		WS_VISIBLE | WS_CHILD,
		3, 20, 400, 200,
		hmem,
		(HMENU) ID_WATCH,
		g_hInst, NULL);
	TCITEM tie;
	tie.mask = TCIF_TEXT | TCIF_IMAGE;
	tie.iImage = -1;
	tie.pszText = _T("Watch");
	tie.lParam = (LPARAM)hwatch;
	TabCtrl_InsertItem(hmem, 0, &tie);
	TabCtrl_SetCurSel(hmem, 0);
}

extern HWND hwndPrev;

LRESULT CALLBACK DebugProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static double ratioDisasm;

	static mp_settings mps[1 + MAX_TABS];
	static dp_settings dps[1 + MAX_TABS];

	static BOOL bDrag = FALSE, bHot = FALSE;
	static int offset_click;
	static BOOL top_locked = FALSE;
	static BOOL bottom_locked = FALSE;

	static const TCHAR* MemPaneString = _T("NumMemPane");
	static const TCHAR* DisasmPaneString = _T("NumDisasmPane");
	static const TCHAR* MemSelIndexString = _T("MemSelIndex");
	static const TCHAR* DisasmSelIndexString = _T("DiasmSelIndex");

	switch (Message) {
		case WM_CREATE:
		{
			lpDebuggerCalc = (LPCALC) ((LPCREATESTRUCT) lParam)->lpCreateParams;
			keypad_t *keypad = lpDebuggerCalc->cpu.pio.keypad;
			//handle keys already down (just send release)
			//i send the message here so that things like logging are handled
			for (int group = 0; group < 8; group++) {
				for (int bit = 0; bit < 8; bit++) {
					if (keypad->keys[group][bit]) {
						keypad_vk_release(lpDebuggerCalc->hwndFrame, group, bit);
					}
				}
			}

			LOGFONT lf;
			memset(&lf, 0, sizeof(LOGFONT));
			StringCbCopy(lf.lfFaceName, sizeof(lf.lfFaceName), _T("Lucida Console"));
			HDC hdc = GetDC(NULL);
			EnumFontFamiliesEx(
					hdc,
					&lf,
					(FONTENUMPROC) EnumFontFamExProc,
					(LPARAM) &hfontLucida,
					0);

			GetObject(hfontLucida, sizeof(LOGFONT), &lf);
			lf.lfWeight = FW_BOLD;

			hfontLucidaBold = CreateFontIndirect(&lf);
			ReleaseDC(NULL, hdc);

			LOGFONT lfSegoe;
			memset(&lfSegoe, 0, sizeof(LOGFONT));
			StringCbCopy(lfSegoe.lfFaceName, sizeof(lfSegoe.lfFaceName), _T("Segoe UI"));

			hdc = GetDC(NULL);
			if (EnumFontFamiliesEx(hdc, &lfSegoe, (FONTENUMPROC) EnumFontFamExProc, (LPARAM) &hfontSegoe, 0) != 0) {
				StringCbCopy(lfSegoe.lfFaceName, sizeof(lfSegoe.lfFaceName), _T("MS Shell Dlg"));
				ReleaseDC(NULL, hdc);
				hdc = GetDC(NULL);
				EnumFontFamiliesEx(hdc, &lfSegoe, (FONTENUMPROC) EnumFontFamExProc, (LPARAM) &hfontSegoe, 0);
				ReleaseDC(NULL, hdc);
			}

			//need to do this before we add the tabs (sizes)
			RECT rc;
			GetClientRect(hwnd, &rc);
			ratioDisasm = (double) cyDisasm / rc.bottom;

			switch (dispType) {
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

			hdisasm = CreateWindow(
				WC_TABCONTROL, _T(""),
			    WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
			    0, 0, 1, 1,
			    hwnd,
			    (HMENU) ID_DISASMTAB,
			    g_hInst, NULL);
			SetWindowFont(hdisasm, hfontSegoe, TRUE);

			/* Create disassembly window */
			total_disasm_pane = 0;
			int panes_to_add = max(1, (int) (QueryDebugKey((TCHAR *) DisasmPaneString)));
			while (panes_to_add > 0) {
				SendMessage(hwnd, WM_COMMAND, IDM_VIEW_ADDDISASM, 0);
				panes_to_add--;
			}

			int selIndex = (int) QueryDebugKey((TCHAR *) DisasmSelIndexString);
			TabCtrl_SetCurSel(hdisasm, selIndex);
			NMHDR hdr;
			hdr.code = TCN_SELCHANGE;
			hdr.idFrom = 1; // not needed
			hdr.hwndFrom = hdisasm;
			SendMessage(hwnd, WM_NOTIFY, MAKEWPARAM(0, 0), (LPARAM) &hdr);

			htoolbar = CreateWindow(
				g_szToolbar,
				_T("toolbar"),
				WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
				0, 0, 1, 1,
				hwnd,
				(HMENU) ID_TOOLBAR,
				g_hInst, NULL);

			hmem =
			CreateWindow(
				WC_TABCONTROL, _T(""),
			    WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
			    0, 0, 1, 1,
			    hwnd,
			    (HMENU) ID_MEMTAB,
			    g_hInst, NULL);
			SetWindowFont(hmem, hfontSegoe, TRUE);

			AddWatchTab(hwnd);

			total_mem_pane = 0;
			panes_to_add = max(1, (int) (QueryDebugKey((TCHAR *) MemPaneString)));
			while (panes_to_add > 0) {
				SendMessage(hwnd, WM_COMMAND, IDM_VIEW_ADDMEM, 0);
				panes_to_add--;
			}

			selIndex = (int) QueryDebugKey((TCHAR *) MemSelIndexString);
			TabCtrl_SetCurSel(hmem, selIndex);
			hdr.code = TCN_SELCHANGE;
			hdr.idFrom = 1; // not needed
			hdr.hwndFrom = hmem;
			SendMessage(hwnd, WM_NOTIFY, MAKEWPARAM(0, 0), (LPARAM) &hdr);

			hreg =
			CreateWindow(
				g_szRegName,
				_T("reg"),
				WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL,
				0, 0, 100, 300,
				hwnd,
				(HMENU) ID_REG,
				g_hInst, NULL);

			CreatePaneContainer(hwnd);

			mps[0].addr = lpDebuggerCalc->cpu.sp;
			mps[0].mode = MEM_WORD;
			mps[0].display = HEX;
			mps[0].sel = mps[1].addr;
			mps[0].track = offsetof(struct CPU, sp);
			mps[0].memNum = -1;

			CreateWindow(
				g_szMemName,
				_T("Stack"),
				WS_VISIBLE | WS_CHILD,
				0, 0, 1, 1,
				hwnd,
				(HMENU) ID_STACK,
				g_hInst, &mps[0]);

			if (expand_pane_state.total != 0) {
				SetExpandPaneState(&expand_pane_state);
				SendMessage(hreg, WM_SIZE, 0, 0); // didn't help
			}

			if (lpDebuggerCalc->profiler.running)
				CheckMenuItem(GetSubMenu(GetMenu(hwnd), 3), IDM_TOOLS_PROFILE, MF_BYCOMMAND | MF_CHECKED);
			if (code_count_tstates != -1)
				CheckMenuItem(GetSubMenu(GetMenu(hwnd), 3), IDM_TOOLS_COUNT, MF_BYCOMMAND | MF_CHECKED);

			hwndPrev = hdisasm;
			SetFocus(hdisasm);
			SendMessage(hwnd, WM_SIZE, 0, 0);
			Debug_UpdateWindow(hwnd);
			return 0;
		}
		case WM_SIZING:
		{
			RECT *r = (RECT *) lParam;
			int cyCaption = GetSystemMetrics(SM_CYCAPTION);
			int cyBottomFrame = GetSystemMetrics(SM_CYSIZEFRAME);
			int minHeight = CY_TOOLBAR + cyGripper + cyCaption + cyBottomFrame*2;
			if (r->bottom - r->top < minHeight) {
				r->bottom = r->top + minHeight;
				return TRUE;
			}

			return FALSE;
		}
		case WM_SIZE: {
			RECT rc;
			GetClientRect(hwnd, &rc);

			if (!bDrag) {
				if (top_locked) {
					cyDisasm = CY_TOOLBAR;
				} else if (bottom_locked) {
					cyDisasm = rc.bottom - cyGripper;
				} else {
					u_int y = (u_int) (ratioDisasm * rc.bottom);
					if (y < CY_TOOLBAR) y = CY_TOOLBAR;
					if (y > rc.bottom - cyGripper) y = rc.bottom - cyGripper;
					cyDisasm = y;
				}
				cyMem = rc.bottom - cyDisasm;
			}

			EnumChildWindows(hwnd, EnumDebugResize, (LPARAM) hwnd);
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
			return 0;
		}
		case WM_MOUSEMOVE: {
			u_int y = GET_Y_LPARAM(lParam) + offset_click;
			RECT rc;
			GetClientRect(hwnd, &rc);

			if (bDrag) {
				if (y < CY_TOOLBAR + cyGripper) {
					top_locked = TRUE;
					bottom_locked = FALSE;
					y = CY_TOOLBAR;
				}
				else if (y > rc.bottom - cyGripper) {
					bottom_locked = TRUE;
					top_locked = FALSE;
					y = rc.bottom - cyGripper;
				} else {
					top_locked = FALSE;
					bottom_locked = FALSE;
				}
				cyDisasm = y;
				cyMem = rc.bottom - cyDisasm;
				SendMessage(hwnd, WM_SIZE, 0, 0);
			}
			HCURSOR hcursor = LoadCursor(NULL, IDC_SIZENS);
			int dy = abs((int)(y - (cyDisasm - (cyGripper/2))));

			if (dy < 16) {
				SetCursor(hcursor);
				if (bHot == FALSE) {
					bHot = TRUE;
					RECT r;
					GetClientRect(hwnd, &r);
					r.left = 0; r.right -= REG_PANE_WIDTH;
					r.top = cyDisasm;
					r.bottom = r.top + cyGripper;
					InvalidateRect(hwnd, &r, FALSE);
				}

				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(tme);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hwnd;
				tme.dwHoverTime = 1;
				TrackMouseEvent(&tme);
			} else if (bHot) {
				bHot = FALSE;
				RECT r;
				GetClientRect(hwnd, &r);
				r.left = 0; r.right -= REG_PANE_WIDTH;
				r.top = cyDisasm;
				r.bottom = r.top + cyGripper;
				InvalidateRect(hwnd, &r, FALSE);
			}

			ratioDisasm = (double) cyDisasm / (double) rc.bottom;
			return 0;
		}
		case WM_MOUSELEAVE: {
			bHot = FALSE;
			RECT r;
			GetClientRect(hwnd, &r);
			r.left = 0; r.right -= REG_PANE_WIDTH;
			r.top = cyDisasm;
			r.bottom = r.top + cyGripper;
			InvalidateRect(hwnd, &r, FALSE);
			return 0;
		}
		case WM_LBUTTONDOWN: {
			int y = GET_Y_LPARAM(lParam);
			int dy = abs((int) (y - (cyDisasm + (cyGripper/2))));

			if (dy < 16) {
				bDrag = TRUE;
				offset_click = cyDisasm - y;
				SetCapture(hwnd);
				HCURSOR hcursor = LoadCursor(NULL, IDC_SIZENS);
				SetCursor(hcursor);
				RECT r;
				GetClientRect(hwnd, &r);
				r.left = 0; r.right -= REG_PANE_WIDTH;
				r.top = cyDisasm;
				r.bottom = r.top + cyGripper;
				InvalidateRect(hwnd, &r, FALSE);
			}
			return 0;
		}
		case WM_LBUTTONUP:  {
			ReleaseCapture();
			bDrag = FALSE;
			RECT r;
			GetClientRect(hwnd, &r);
			r.left = 0; r.right -= REG_PANE_WIDTH;
			r.top = cyDisasm;
			r.bottom = r.top + cyGripper;
			InvalidateRect(hwnd, &r, FALSE);
			return 0;
		}
		case WM_LBUTTONDBLCLK: {
			int y = GET_Y_LPARAM(lParam);
			int dy = abs((int) (y - (cyDisasm + (cyGripper/2))));

			if (dy < 16) {
				ratioDisasm = 0.5;
				SendMessage(hwnd, WM_SIZE, 0, 0);
			}
			return 0;
		}
		case WM_COMMAND: {
			printf("Got a command\n");
			switch (LOWORD(wParam)) {
			case IDM_DEBUG_EXIT:
				DestroyWindow(hwnd);
				break;
			case IDM_TOOLS_COUNT: {
				HMENU hmenu = GetMenu(hwnd);
				if (code_count_tstates == -1) {
					code_count_tstates = lpDebuggerCalc->cpu.timer_c->tstates;
					CheckMenuItem(GetSubMenu(hmenu, 3), IDM_TOOLS_COUNT, MF_BYCOMMAND | MF_CHECKED);
				} else {
					TCHAR buffer[256];
					StringCbPrintf(buffer, sizeof(buffer), _T("%i T-States"), (int)(lpDebuggerCalc->cpu.timer_c->tstates - code_count_tstates));
					MessageBox(NULL, buffer, _T("Code Counter"), MB_OK);
					code_count_tstates = -1;
					CheckMenuItem(GetSubMenu(hmenu, 3), IDM_TOOLS_COUNT, MF_BYCOMMAND | MF_UNCHECKED);
				}
				break;
			}
			case IDM_TOOLS_DUMP: {
				TCHAR path[MAX_PATH];
				if (!SaveFile(path, _T("Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0"), _T("Save Dump"), _T("txt"), OFN_PATHMUSTEXIST)) {
					WriteHumanReadableDump(lpDebuggerCalc, path);
				}
				break;
			}
			case IDM_TOOLS_PROFILE: {
				lpDebuggerCalc->profiler.running = !lpDebuggerCalc->profiler.running;
				HMENU hmenu = GetMenu(hwnd);
				if (lpDebuggerCalc->profiler.running) {
					int result = (int) DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DLGPROFILE), hwnd, (DLGPROC) ProfileDialogProc);
					if (result == IDCANCEL)
						lpDebuggerCalc->profiler.running = !lpDebuggerCalc->profiler.running;
					else {
						memset(lpDebuggerCalc->profiler.data, 0, MIN_BLOCK_SIZE * sizeof(long));
						CheckMenuItem(GetSubMenu(hmenu, 3), IDM_TOOLS_PROFILE, MF_BYCOMMAND | MF_CHECKED);
					}
				} else {
					FILE* file;
					int i;
					double data;
					TCHAR buffer[MAX_PATH];
					if (BrowseFile(buffer, _T("	Text file  (*.txt)\0*.txt\0	All Files (*.*)\0*.*\0\0"),
									_T("Wabbitemu Save Profile"), _T("txt"))) {
						//make the profiler running again
						lpDebuggerCalc->profiler.running = TRUE;
						break;
					}
					_tfopen_s(&file, buffer, _T("wb"));
					_ftprintf_s(file, _T("Total Tstates: %i\r\n"), lpDebuggerCalc->profiler.totalTime);
					for(i = lpDebuggerCalc->profiler.lowAddress / lpDebuggerCalc->profiler.blockSize;
							i < ARRAYSIZE(lpDebuggerCalc->profiler.data) &&
							i < (lpDebuggerCalc->profiler.highAddress / lpDebuggerCalc->profiler.blockSize); i++) {
						data = (double) lpDebuggerCalc->profiler.data[i] / (double) lpDebuggerCalc->profiler.totalTime;
						if (data != 0.0)
							_ftprintf_s(file, _T("$%04X - $%04X: %f%% %d tstates\r\n"), i * lpDebuggerCalc->profiler.blockSize, ((i + 1) *
											lpDebuggerCalc->profiler.blockSize) - 1, data, lpDebuggerCalc->profiler.data[i]);
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
				dispType = HEX;
				mps[TabCtrl_GetCurSel(hmem) + 1].display = dispType;
				Debug_UpdateWindow(hwnd);
				break;
			case IDM_DISPLAYBASE_BIN:
				CheckMenuItem(GetMenu(hwnd), IDM_DISPLAYBASE_HEX, MF_BYCOMMAND | MF_UNCHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_DISPLAYBASE_DEC, MF_BYCOMMAND | MF_UNCHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_DISPLAYBASE_BIN, MF_BYCOMMAND | MF_CHECKED);
				dispType = BIN;
				mps[TabCtrl_GetCurSel(hmem) + 1].display = dispType;
				Debug_UpdateWindow(hwnd);
				break;
			case IDM_DISPLAYBASE_DEC:
				CheckMenuItem(GetMenu(hwnd), IDM_DISPLAYBASE_HEX, MF_BYCOMMAND | MF_UNCHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_DISPLAYBASE_DEC, MF_BYCOMMAND | MF_CHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_DISPLAYBASE_BIN, MF_BYCOMMAND | MF_UNCHECKED);
				dispType = DEC;
				mps[TabCtrl_GetCurSel(hmem) + 1].display = dispType;
				Debug_UpdateWindow(hwnd);
				break;
			case IDM_VIEW_BYTE: {
				CheckMenuItem(GetMenu(hwnd), IDM_VIEW_BYTE, MF_BYCOMMAND | MF_CHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_VIEW_WORD, MF_BYCOMMAND | MF_UNCHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_VIEW_CHAR, MF_BYCOMMAND | MF_UNCHECKED);
				int index = TabCtrl_GetCurSel(hmem) + 1;
				mps[index].mode = 1;
				mps[index].bText = FALSE;
				Debug_UpdateWindow(hwnd);
				break;
			}
			case IDM_VIEW_WORD: {
				CheckMenuItem(GetMenu(hwnd), IDM_VIEW_BYTE, MF_BYCOMMAND | MF_UNCHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_VIEW_WORD, MF_BYCOMMAND | MF_CHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_VIEW_CHAR, MF_BYCOMMAND | MF_UNCHECKED);
				int index = TabCtrl_GetCurSel(hmem) + 1;
				mps[index].mode = 2;
				mps[index].bText = FALSE;
				Debug_UpdateWindow(hwnd);
				break;
			}
			case IDM_VIEW_CHAR: {
				CheckMenuItem(GetMenu(hwnd), IDM_VIEW_BYTE, MF_BYCOMMAND | MF_UNCHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_VIEW_WORD, MF_BYCOMMAND | MF_UNCHECKED);
				CheckMenuItem(GetMenu(hwnd), IDM_VIEW_CHAR, MF_BYCOMMAND | MF_CHECKED);
				int index = TabCtrl_GetCurSel(hmem) + 1;
				mps[index].mode = 1;
				mps[index].bText = TRUE;
				Debug_UpdateWindow(hwnd);
				break;
			}
			case IDM_VIEW_ADDMEM: {
				HMENU hMenu = GetMenu(hwnd);
				EnableMenuItem(hMenu, IDM_VIEW_DELMEM, MF_BYCOMMAND | MF_ENABLED);
				if (total_mem_pane + 1 > MAX_MEM_TABS)
					break;
				AddMemTab(mps, REGULAR);
				AddMemTab(mps, FLASH);
				AddMemTab(mps, RAM);
				if (total_mem_pane == MAX_TABS)
					EnableMenuItem(hMenu, IDM_VIEW_ADDMEM, MF_BYCOMMAND | MF_DISABLED);
				SendMessage(hwnd, WM_SIZE, 0, 0);
				Debug_UpdateWindow(hwnd);
				break;
			}
			case IDM_VIEW_DELMEM: {
				HMENU hMenu = GetMenu(hwnd);
				EnableMenuItem(hMenu, IDM_VIEW_ADDMEM, MF_BYCOMMAND | MF_ENABLED);
				if (total_mem_pane == 3)
					break;
				total_mem_pane -= 3;
				if (total_mem_pane == 3)
					EnableMenuItem(hMenu, IDM_VIEW_DELMEM, MF_BYCOMMAND | MF_DISABLED);
				if (TabCtrl_GetCurSel(hmem) >= total_mem_pane)
					TabCtrl_SetCurSel(hmem, total_mem_pane - 1);
				TabCtrl_DeleteItem(hmem, total_mem_pane);
				TabCtrl_DeleteItem(hmem, total_mem_pane + 1);
				TabCtrl_DeleteItem(hmem, total_mem_pane + 2);
				hmemlist[total_mem_pane] = NULL;
				hmemlist[total_mem_pane + 1] = NULL;
				hmemlist[total_mem_pane + 2] = NULL;
				Debug_UpdateWindow(hwnd);
				break;
			}
			case IDM_VIEW_ADDDISASM: {
				HMENU hMenu = GetMenu(hwnd);
				EnableMenuItem(hMenu, IDM_VIEW_DELDISASM, MF_BYCOMMAND | MF_ENABLED);
				if (total_disasm_pane + 1 > MAX_DISASM_TABS)
					break;
				AddDisasmTab(dps, REGULAR);
				AddDisasmTab(dps, FLASH);
				AddDisasmTab(dps, RAM);
				if (total_disasm_pane == MAX_TABS)
					EnableMenuItem(hMenu, IDM_VIEW_ADDDISASM, MF_BYCOMMAND | MF_DISABLED);
				SendMessage(hwnd, WM_SIZE, 0, 0);
				Debug_UpdateWindow(hwnd);
				break;
			}
			case IDM_VIEW_DELDISASM: {
				HMENU hMenu = GetMenu(hwnd);
				EnableMenuItem(hMenu, IDM_VIEW_ADDDISASM, MF_BYCOMMAND | MF_ENABLED);
				if (total_disasm_pane == 3)
					break;
				total_disasm_pane -= 3;
				if (total_disasm_pane == 3)
					EnableMenuItem(hMenu, IDM_VIEW_DELDISASM, MF_BYCOMMAND | MF_DISABLED);
				if (TabCtrl_GetCurSel(hdisasm) >= total_disasm_pane)
					TabCtrl_SetCurSel(hdisasm, total_disasm_pane - 1);
				TabCtrl_DeleteItem(hdisasm, total_disasm_pane);
				TabCtrl_DeleteItem(hdisasm, total_disasm_pane + 1);
				TabCtrl_DeleteItem(hdisasm, total_disasm_pane + 2);
				DestroyWindow(hdisasmlist[total_disasm_pane]);
				DestroyWindow(hdisasmlist[total_disasm_pane + 1]);
				DestroyWindow(hdisasmlist[total_disasm_pane + 2]);
				hdisasmlist[total_disasm_pane] = NULL;
				hdisasmlist[total_disasm_pane + 1] = NULL;
				hdisasmlist[total_disasm_pane + 2] = NULL;
				Debug_UpdateWindow(hwnd);
				break;
			}
			case DB_PORTMONITOR:
			case IDM_VIEW_PORTMONITOR: {
				if (IsWindow(hPortMon)) {
					SwitchToThisWindow(hPortMon, TRUE);
				} else {
					hPortMon = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_PORT_MONITOR), hwnd, (DLGPROC) PortMonitorDialogProc);
					ShowWindow(hPortMon, SW_SHOW);
				}
				break;
			}
			case DB_BREAKPOINTS:
			case IDM_VIEW_BREAKPOINTS: {
				if (IsWindow(hBreakpoints)) {
					SwitchToThisWindow(hBreakpoints, TRUE);
				} else {
					hBreakpoints = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_BREAKPOINT), hwnd, (DLGPROC) BreakpointsDialogProc);
					ShowWindow(hBreakpoints, SW_SHOW);
				}
				break;
			}
			extern HWND hwndLastFocus;
			default: {
				if (hmemlist[TabCtrl_GetCurSel(hmem)] == hwndLastFocus)
					SendMessage(hmemlist[TabCtrl_GetCurSel(hmem)], Message, wParam, lParam);
				else if (hdisasmlist[TabCtrl_GetCurSel(hdisasm)] == hwndLastFocus)
					SendMessage(hdisasmlist[TabCtrl_GetCurSel(hdisasm)], Message, wParam, lParam);
				else
					SendMessage(hdisasm, Message, wParam, lParam);
				break;
				}
			}
			break;
		}
		case WM_NOTIFY: {
			LPNMHDR header = (LPNMHDR) lParam;
			switch (header->code) {
				case TCN_SELCHANGE: {
					if(header->hwndFrom == hmem) {
						int i;
						int index = TabCtrl_GetCurSel(hmem);
						for (i = 0; i < total_mem_pane; i++) {
							if (i == index)
								ShowWindow(hmemlist[i], SW_SHOW);
							else
								ShowWindow(hmemlist[i], SW_HIDE);
						}
						if (index == total_mem_pane)
							ShowWindow(hwatch, SW_SHOW);
						else
							ShowWindow(hwatch, SW_HIDE);
						SendMessage(hwnd, WM_SIZE, 0 , 0);
					} else if (header->hwndFrom == hdisasm) {
						int i;
						int index = TabCtrl_GetCurSel(hdisasm);
						for (i = 0; i < total_disasm_pane; i++) {
							if (i == index)
								ShowWindow(hdisasmlist[i], SW_SHOW);
							else
								ShowWindow(hdisasmlist[i], SW_HIDE);
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

			if (bDrag) {
				DarkEdge = GetSysColor(COLOR_3DDKSHADOW) - DARKEN_AMOUNT;
				LightEdge = GetSysColor(COLOR_3DHILIGHT) - DARKEN_AMOUNT;
			} else if (bHot) {
				DarkEdge = GetSysColor(COLOR_BTNFACE) - DARKEN_AMOUNT;
				LightEdge = GetSysColor(COLOR_BTNFACE);
			} else {
				DarkEdge = GetSysColor(COLOR_BTNSHADOW);
				LightEdge = GetSysColor(COLOR_BTNFACE);
			}

			vert[0].x = 0;
			// 40%
			vert[0].y = cyDisasm + cyGripper * 4 / 10;
			vert[0].Red = GetRValue(LightEdge) << 8;
			vert[0].Green = GetGValue(LightEdge) << 8;
			vert[0].Blue = GetBValue(LightEdge) << 8;

			vert[1].x = rc.right - REG_PANE_WIDTH;
			vert[1].y = cyDisasm + cyGripper;
			vert[1].Red = GetRValue(DarkEdge) << 8;
			vert[1].Green = GetGValue(DarkEdge) << 8;
			vert[1].Blue = GetBValue(DarkEdge) << 8;

			GradientFill(hdc,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);

#define DOT_WIDTH	4

			HBRUSH hbr = CreateSolidBrush(DarkEdge); //GetSysColorBrush(COLOR_BTNSHADOW);

			rectDot.left = (rc.right-REG_PANE_WIDTH)/2 - 12;
			rectDot.right = rectDot.left + DOT_WIDTH;

			rectDot.top = cyDisasm + cyGripper/2 - DOT_WIDTH/2;
			rectDot.bottom = rectDot.top + DOT_WIDTH;
			SelectObject(hdc, hbr);
			SelectObject(hdc, GetStockObject(DC_PEN));
			SetDCPenColor(hdc, DarkEdge - DARKEN_AMOUNT);

			if (bDrag)
				OffsetRect(&rectDot, 1, 1);

			int i;
			for (i = 0; i < 3; i ++, OffsetRect(&rectDot, DOT_WIDTH*2, 0)) {
				Ellipse(hdc, rectDot.left, rectDot.top, rectDot.right, rectDot.bottom);
			}
			RECT r;
			r.left = 0; r.right = rc.right - REG_PANE_WIDTH;
			r.top = cyDisasm;
			r.bottom = r.top + cyGripper;

			if (bDrag) {
				DrawEdge(hdc, &r, EDGE_SUNKEN, BF_TOP|BF_SOFT);
			} else {
				DrawEdge(hdc, &r, EDGE_RAISED, BF_TOP|BF_BOTTOM|BF_SOFT);
			}

			SelectObject(hdc, GetStockObject(DC_PEN));
			SetDCPenColor(hdc, GetSysColor(COLOR_BTNSHADOW)); //(147, 176, 194));
			MoveToEx(hdc, rc.right - 102 - REG_PANE_WIDTH, cyDisasm+cyGripper, NULL);
			LineTo(hdc, rc.right - 102 - REG_PANE_WIDTH, rc.bottom);

			DeleteObject(hbr);

			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_USER:
			switch (wParam) {
				case DB_UPDATE:
					EnumChildWindows(hwnd, EnumDebugUpdate, 0);
					if (IsWindow(hPortMon)) {
						SendMessage(hPortMon, WM_USER, DB_UPDATE, 0); 
					}
					if (IsWindow(hBreakpoints)) {
						SendMessage(hBreakpoints, WM_USER, DB_UPDATE, 0); 
					}
					break;
				case DB_RESUME:
					EnumChildWindows(hwnd, EnumDebugResume, 0);
					break;
			}
			return 0;
		case WM_DESTROY: {
			CPU_step(&lpDebuggerCalc->cpu);
			lpDebuggerCalc->running = TRUE;
			calc_unpause_linked();
			GetWindowPlacement(hwnd, &db_placement);
			db_maximized = IsMaximized(hwnd);

			int selIndex = TabCtrl_GetCurSel(hmem);
			int groupIndex = total_mem_pane / 3;
			SaveDebugKey((TCHAR *) MemPaneString, REG_DWORD, &groupIndex);
			SaveDebugKey((TCHAR *) MemSelIndexString, REG_DWORD, &selIndex);
			GetExpandPaneState(&expand_pane_state);
			DeleteObject(hfontLucida);
			hfontLucida = NULL;
			DeleteObject(hfontLucidaBold);
			hfontLucidaBold = NULL;
			DeleteObject(hfontSegoe);
			hfontSegoe = NULL;
			hwatch = NULL;
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
	
	_fputts(_T("\nDisplay:\n"), file);
	uint8_t (*lcd)[LCD_WIDTH] = (uint8_t(*)[LCD_WIDTH]) GIFGREYLCD(lpCalc->cpu.pio.lcd);
	for (int i = 0; i < LCD_HEIGHT * 4; i += 4) {
		for (int j = 0; j < lpCalc->cpu.pio.lcd->width * 2; j += 2) {
			if (lcd[i][j] < lpCalc->cpu.pio.lcd->shades / 4)
				_fputtc(' ', file);
			/*else if (lcd[i][j] < lpCalc->cpu.pio.lcd->shades / 2)
				_fputtc(176, file);
			else if (lcd[i][j] < lpCalc->cpu.pio.lcd->shades * 3 / 4)
				_fputtc(178 , file);*/
			else
				_fputtc('#', file);
		}
		_fputtc('\n', file);
	}
	
	_fputts(_T("\nPorts:\n"), file);
	CPU_t *cpu = CPU_clone(&lpDebuggerCalc->cpu);
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
	for (int page = 0; page < lpDebuggerCalc->cpu.mem_c->flash_pages; page++) {
		for (int addr = 0; addr < PAGE_SIZE; addr += 32) {
			_ftprintf_s(file, _T("%02X %04X: "), page, addr);
			for (int i = 0; i < 32; i++)
				_ftprintf_s(file, _T("%02X "), lpDebuggerCalc->cpu.mem_c->flash[page * PAGE_SIZE + addr + i]);
			_fputtc('\n', file);
		}
	}

	_fputts(_T("\nRAM:\n"), file);
	for (int page = 0; page < lpDebuggerCalc->cpu.mem_c->ram_pages; page++) {
		for (int addr = 0; addr < PAGE_SIZE; addr += 32) {
			_ftprintf_s(file, _T("%02X %04X: "), page, addr);
			for (int i = 0; i < 32; i++)
				_ftprintf_s(file, _T("%02X "), lpDebuggerCalc->cpu.mem_c->ram[page * PAGE_SIZE + addr + i]);
			_fputtc('\n', file);
		}
	}

	fclose(file);
}