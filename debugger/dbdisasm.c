#include "stdafx.h"

#include "dbdisasm.h"
#include "dbtoolbar.h"
#include "core.h"
#include "guicontext.h"
#include "calc.h"
#include "disassemble.h"
#include <commctrl.h>
#include "dbcommon.h"
#include "resource.h"
#include "print.h"

#define COLUMN_X_OFFSET 7

#define COLOR_PC				(RGB(180, 180, 180))
#define COLOR_BREAKPOINT		(RGB(230, 160, 180))
#define COLOR_MEMPOINT_WRITE	(RGB(255, 177, 100))
#define COLOR_MEMPOINT_READ		(RGB(255, 250, 145))
#define COLOR_HALT				(RGB(200, 200, 100))

extern HWND hwndLastFocus;
extern Z80_com_t da_opcode[256];

extern HINSTANCE g_hInst;
extern unsigned int goto_addr;
extern int find_value;
extern BOOL search_backwards;

void sprint_addr(HDC hdc, Z80_info_t *zinf, RECT *r) {
	TCHAR s[64];

	SetTextColor(hdc, RGB(0, 0, 0));
	_stprintf_s(s, _T("%04X"), zinf->waddr.addr);

	r->left += COLUMN_X_OFFSET;
	DrawText(hdc, s, -1, r, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
}

void sprint_data(HDC hdc, Z80_info_t *zinf, RECT *r) {
	TCHAR s[64];
	int j;
	SetTextColor(hdc, RGB(0, 0, 0));

	if (zinf->size == 0) return;

	for (j = 0; j < zinf->size; j++) {
		StringCbPrintf(s + (j*2), sizeof(s), _T("%02x"), mem_read(lpDebuggerCalc->cpu.mem_c, zinf->waddr.addr + j));
	}
	r->left += COLUMN_X_OFFSET;
	DrawText(hdc, s, -1, r, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
}

void sprint_command(HDC hdc, Z80_info_t *zinf, RECT *r) {
	MyDrawText(lpDebuggerCalc, hdc, r, zinf, REGULAR, da_opcode[zinf->index].format, zinf->a1, zinf->a2, zinf->a3, zinf->a4);
}

void sprint_size(HDC hdc, Z80_info_t *zinf, RECT *r) {
	TCHAR s[64];
	SetTextColor(hdc, RGB(0, 0, 0));
	if (zinf->size == 0) return;
	StringCbPrintf(s, sizeof(s), _T("%d"), zinf->size);

	r->left += COLUMN_X_OFFSET;
	DrawText(hdc, s, -1, r, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
}

void sprint_clocks(HDC hdc, Z80_info_t *zinf, RECT *r) {
	TCHAR s[64];
	SetTextColor(hdc, RGB(0, 0, 0));
	if (da_opcode[zinf->index].clocks != -1) {
		if (da_opcode[zinf->index].clocks_cond) {
			StringCbPrintf(s, sizeof(s), _T("%d/%d"), da_opcode[zinf->index].clocks, da_opcode[zinf->index].clocks_cond);
		} else {
			StringCbPrintf(s, sizeof(s), _T("%d"), da_opcode[zinf->index].clocks);
		}
	} else {
		*s = '\0';
	}
	r->left += COLUMN_X_OFFSET;
	DrawText(hdc, s, -1, r, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
}



void InvalidateSel(HWND hwnd, int sel) {
	dp_settings *dps = (dp_settings*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
	HDC hdc = GetDC(hwnd);

	RECT r, hdrRect;

	GetClientRect(hwnd, &r);
	r.top = sel*dps->cyRow; r.bottom = r.top + (dps->cyRow * dps->NumSel);


	GetWindowRect(dps->hwndHeader, &hdrRect);
	OffsetRect(&r, 0, hdrRect.bottom - hdrRect.top);
	InvalidateRect(hwnd, &r, TRUE);

	ReleaseDC(hwnd, hdc);
}

/*
 * Draws a translucent selection rectangle to HDC
 */
void DrawSelectionRect(HDC hdc, RECT *r) {
	BLENDFUNCTION bf;

	if (r->right < r->left) {
		LONG swap = r->left;
		r->left = r->right;
		r->right = swap;
	}

	if (r->bottom < r->top) {
		LONG swap = r->top;
		r->top = r->bottom;
		r->bottom = swap;
	}


	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.AlphaFormat = 0;
	bf.SourceConstantAlpha = 100;

	SelectObject(hdc, GetStockObject(DC_PEN));
	SetDCPenColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
	SelectObject(hdc, GetStockObject(NULL_BRUSH));

	Rectangle(hdc, r->left, r->top, r->right, r->bottom);
	HDC hdcSel = CreateCompatibleDC(hdc);
	HBITMAP hbmSel = CreateCompatibleBitmap(hdc, 1, 1);
	SelectObject(hdcSel, hbmSel);

	SetPixel(hdcSel, 0, 0, GetSysColor(COLOR_HIGHLIGHT));

	AlphaBlend(	hdc, r->left+1, r->top+1, r->right - r->left - 2, r->bottom - r->top - 2,
				hdcSel, 0, 0, 1, 1,
				bf);

	DeleteObject(hbmSel);
	DeleteDC(hdcSel);
}

void DrawItemSelection(HDC hdc, RECT *r, BOOL active, COLORREF breakpoint, int opacity) {

	HDC hdcSel = CreateCompatibleDC(hdc);
	HBITMAP hbmSel = CreateCompatibleBitmap(hdc, r->right - r->left, r->bottom - r->top);
	TRIVERTEX vert[2];
	GRADIENT_RECT gRect;
	COLORREF rgbSel;
	BLENDFUNCTION bf;
	COLORREF tl, tr, br, bl;

	SelectObject(hdcSel, hbmSel);

	SelectObject(hdc, GetStockObject(WHITE_BRUSH));
	SelectObject(hdcSel, GetStockObject(DC_PEN));
	SetDCPenColor(hdcSel, RGB(255, 255, 255));
	Rectangle(hdcSel, 0, 0, r->right - r->left, r->bottom - r->top);

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;


	if (active == TRUE) {
		rgbSel = RGB(153, 222, 253);
		//rgbSel = RGB(24, 153, 255);
	} else {
		rgbSel = RGB(190, 190, 190);
	}

	if (breakpoint)
		rgbSel = breakpoint;

	vert[0].x = 2;
	vert[0].y = 1;
	vert[0].Red    = 0xff00;
	vert[0].Green  = 0xff00;
	vert[0].Blue   = 0xff00;

	vert[1].x = r->right - r->left - 2;
	vert[1].y = (r->bottom - r->top - 2) * 2;
	vert[1].Red 	= GetRValue(rgbSel) << 8;
	vert[1].Green 	= GetGValue(rgbSel) << 8;
	vert[1].Blue 	= GetBValue(rgbSel) << 8;

	GradientFill(hdcSel,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);


	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = opacity;
	bf.AlphaFormat = 0;

	SelectObject(hdcSel, GetStockObject(NULL_BRUSH));
	SetDCPenColor(hdcSel, rgbSel);

	RoundRect(hdcSel, 0, 0, r->right - r->left, r->bottom - r->top, 5, 5);

	SetDCPenColor(hdcSel, GetPixel(hdcSel, 3, (r->bottom - r->top)/3));
	RoundRect(hdcSel, 1, 1, r->right - r->left-1, r->bottom - r->top-1, 5, 5);


	tl = GetPixel(hdc, r->left, r->top);
	tr = GetPixel(hdc, r->right - 1, r->top);
	br = GetPixel(hdc, r->right - 1, r->bottom - 1);
	bl = GetPixel(hdc, r->left, r->bottom - 1);

	AlphaBlend(	hdc, r->left, r->top, r->right - r->left, r->bottom - r->top,
				hdcSel, 0, 0, r->right - r->left, r->bottom - r->top,
				bf);

	SetPixel(hdc, r->left, r->top, tl);
	SetPixel(hdc, r->left, r->top + 1, tl);
	SetPixel(hdc, r->right - 1, r->top, tr);
	SetPixel(hdc, r->right - 1, r->top + 1, tr);
	SetPixel(hdc, r->right - 1, r->bottom - 1, br);
	SetPixel(hdc, r->right - 1, r->bottom - 2, br);
	SetPixel(hdc, r->left, r->bottom - 1, bl);
	SetPixel(hdc, r->left, r->bottom - 2, bl);


	DeleteObject(hbmSel);
	DeleteDC(hdcSel);


}

void CPU_stepout(CPU_t *cpu) {
	double time = tc_elapsed(cpu->timer_c);
	uint16_t old_sp = cpu->sp;

	while ((tc_elapsed(cpu->timer_c) - time) < 15.0) {
		waddr_t old_pc = addr_to_waddr(cpu->mem_c, cpu->pc);
		CPU_step(cpu);

		if (cpu->sp > old_sp) {
			Z80_info_t zinflocal;
			disassemble(cpu->mem_c, REGULAR, addr_to_waddr(cpu->mem_c, old_pc.addr), 1, &zinflocal);

			if (zinflocal.index == DA_RET 		||
				zinflocal.index == DA_RET_CC 	||
				zinflocal.index == DA_RETI		||
				zinflocal.index == DA_RETN) {

				return;
			}

		}
	}
}

/*
 * Repeatedly step until you get to the next command
 */
void CPU_stepover(CPU_t *cpu) {
	const int usable_commands[] = { DA_BJUMP, DA_BJUMP_N, DA_BCALL_N, DA_BCALL,
									DA_BLI, DA_CALL_X, DA_CALL_CC_X, DA_HALT, DA_RST_X};
	int i;
	double time = tc_elapsed(cpu->timer_c);
	Z80_info_t zinflocal;

	disassemble(cpu->mem_c, REGULAR, addr_to_waddr(cpu->mem_c, cpu->pc), 1, &zinflocal);

	if (cpu->halt) {
		if (cpu->iff1) {
			while ((tc_elapsed(cpu->timer_c) - time) < 15.0 && cpu->halt == TRUE )
				CPU_step(cpu);
		} else {
			cpu->halt = FALSE;
		}
	} else if (zinflocal.index == DA_CALL_X || zinflocal.index == DA_CALL_CC_X) {
		uint16_t old_stack = cpu->sp;
		CPU_step(cpu);
		if (cpu->sp != old_stack)
			CPU_stepout(cpu);
	} else {
		for (i = 0; i < ARRAYSIZE(usable_commands); i++) {
			if (zinflocal.index == usable_commands[i]) {
				while ((tc_elapsed(cpu->timer_c) - time) < 15.0 && cpu->pc != (zinflocal.waddr.addr + zinflocal.size))
					CPU_step(cpu);
				return;
			}
		}

		CPU_step(cpu);
	}
}


void disasmhdr_show(HWND hwndHeader, disasmhdr_t* hdrs) {
	int lpiNewArray[8];
	int iSize = Header_GetItemCount(hwndHeader);
	Header_GetOrderArray(hwndHeader, iSize, lpiNewArray);
}

int disasmhdr_insert(HWND hwndHeader, disasmhdr_t* dhdr) {
	HDITEM hdi;
	TEXTMETRIC tm;
	int iSize;

	HDC hdc = GetDC(GetParent(hwndHeader));
	GetTextMetrics(hdc, &tm);
	ReleaseDC(hwndHeader, hdc);

	hdi.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH | HDI_LPARAM;
	hdi.pszText = dhdr->pszText;
	hdi.cxy = dhdr->cx = dhdr->nCharsWidth * tm.tmAveCharWidth;
	hdi.cchTextMax = sizeof(hdi.pszText)/sizeof(hdi.pszText[0]);
	hdi.fmt = HDF_LEFT | HDF_STRING;
	hdi.lParam = dhdr->index;

	iSize = Header_GetItemCount(hwndHeader);
	return Header_InsertItem(hwndHeader, iSize + 1, &hdi);
}

int disasmhdr_find(HWND hwndHeader, int index) {
	HDITEM hdi;
	int iSize = Header_GetItemCount(hwndHeader);
	int i;

	hdi.mask = HDI_LPARAM;

	for (i = 0; i < iSize; i++) {
		Header_GetItem(hwndHeader, i, &hdi);
		if (hdi.lParam == index) return i;
	}
	return -1;
}


int disasmhdr_toggle(HWND hwndHeader, disasmhdr_t* dhdr) {
	int iOrder = disasmhdr_find(hwndHeader, dhdr->index);
	if (iOrder != -1) {
		return Header_DeleteItem(hwndHeader, iOrder);
	} else {
		return disasmhdr_insert(hwndHeader, dhdr);
	}
}

static int GetMaxAddr(dp_settings *dps) {
	switch (dps->type) {
		case REGULAR:
			return 0xFFFF;
		case FLASH:
			return lpDebuggerCalc->cpu.mem_c->flash_size;
		case RAM:
			return lpDebuggerCalc->cpu.mem_c->ram_size;
	}
}

extern HFONT hfontSegoe, hfontLucida, hfontLucidaBold;

static int addr_to_index(dp_settings *dps, int addr) {
	u_int i = 0;
	for (i = 0; i < dps->nRows; i++) {
		if (dps->zinf[i].waddr.addr == addr)
			return i;
	}
	return -1;
}

static int next_select(dp_settings *dps, int addr) {
	int i = addr_to_index(dps, addr);
	if (i > 255)
		return -1;
	return dps->zinf[i+1].waddr.addr;
}

static int prev_select(dp_settings *dps, int addr) {
	int i = addr_to_index(dps, addr);
	if (i < 1)
		return -1;
	return dps->zinf[i-1].waddr.addr;
}

void cycle_pcs(dp_settings *dps) {
	int i;
	for (i = PC_TRAILS-1; i > 0; i--) {
		dps->nPCs[i] = dps->nPCs[i-1];
	}

	dps->nPCs[0] = lpDebuggerCalc->cpu.pc;
}

void db_step_finish(HWND hwnd, dp_settings *dps) {
	unsigned short past_last = lpDebuggerCalc->cpu.pc - dps->zinf[dps->nRows-1].waddr.addr + dps->zinf[dps->nRows-1].size;
	unsigned short before_first = dps->zinf[0].waddr.addr - lpDebuggerCalc->cpu.pc;
	InvalidateSel(hwnd, dps->iSel);
	dps->nSel = (&lpDebuggerCalc->cpu)->pc;
	if (past_last >= 0 || before_first > 0) {
		int iQ1;
		SendMessage(hwnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBTRACK, lpDebuggerCalc->cpu.pc), 0);
		iQ1 = dps->nRows/4;
		if (iQ1 == 0) 
			return;
		while (iQ1--) SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
		UpdateWindow(hwnd);
	} else if (past_last > 0) {
		SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
	} else if (before_first > 0) {
		SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
	} else {
		UpdateWindow(hwnd);
	}

	cycle_pcs(dps);
	SendMessage(GetParent(hwnd), WM_USER, DB_UPDATE, 0);
}

LRESULT CALLBACK DisasmProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {

	switch (Message) {
		case WM_SETFOCUS: {
			dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			hwndLastFocus = hwnd;
			InvalidateSel(hwnd, dps->iSel);
			UpdateWindow(hwnd);
			return 0;
		}
		case WM_KILLFOCUS: {
			dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			InvalidateSel(hwnd, dps->iSel);
			UpdateWindow(hwnd);
			return 0;
		}
		case WM_CREATE: {
			TEXTMETRIC tm;
			HDC hdc = GetDC(hwnd);
			RECT rc;
			int i;
			WINDOWPOS wp;
			HDLAYOUT hdl;
			RECT hdrRect;

			GetClientRect(hwnd, &rc);

			dp_settings *dps = (dp_settings *) ((LPCREATESTRUCT) lParam)->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) dps);

			dps->hfontDisasm = hfontLucida;
			dps->hfontData = hfontLucida;
			dps->hfontAddr = hfontLucidaBold;

			SelectObject(hdc, dps->hfontDisasm);
			GetTextMetrics(hdc, &tm);
			dps->cyRow = 4 * tm.tmHeight / 3;
			dps->nPane = dps->nSel;

			// Set column fonts (need to reset so that the newly allocated fonts work)
			dps->hdrs[0].hfont = dps->hfontAddr;
			dps->hdrs[1].hfont = dps->hfontData;
			dps->hdrs[2].hfont = dps->hfontDisasm;
			dps->hdrs[3].hfont = dps->hfontData;
			dps->hdrs[3].hfont = dps->hfontData;

			InitCommonControls();
			// Create the header window
			dps->hwndHeader = CreateWindowEx(0, WC_HEADER, (LPCTSTR) NULL,
				WS_CHILD |  HDS_HORZ | WS_VISIBLE | HDS_FULLDRAG | HDS_DRAGDROP,
				0, 0, 1, 1, hwnd, (HMENU) ID_DISASMSIZE, g_hInst,
				(LPVOID) NULL);

			SetWindowFont(dps->hwndHeader, hfontSegoe, TRUE);

			hdl.prc = &rc;
			hdl.pwpos = &wp;
			Header_Layout(dps->hwndHeader, &hdl);
			SetWindowPos(dps->hwndHeader, wp.hwndInsertAfter, wp.x, wp.y,
				wp.cx, wp.cy, wp.flags);

			// Add all of the columns
			for (i = 0; i < NumElm(dps->hdrs); i++) {
				if (dps->hdrs[i].index != -1) dps->hdrs[i].index = disasmhdr_insert(dps->hwndHeader, &dps->hdrs[i]);
			}

			dps->hwndTip = CreateWindowEx(
					0,
					TOOLTIPS_CLASS,
					NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
					CW_USEDEFAULT, CW_USEDEFAULT,
					CW_USEDEFAULT, CW_USEDEFAULT,
					hwnd, NULL, g_hInst, NULL);
			SendMessage(dps->hwndTip, TTM_ACTIVATE, TRUE, 0);

			dps->toolInfo.cbSize = sizeof(TOOLINFO);
			dps->toolInfo.hwnd = hwnd;
			dps->toolInfo.uFlags = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE;
			dps->toolInfo.uId = (UINT_PTR)hwnd;
			dps->toolInfo.lpszText = _T("");
			SendMessage(dps->hwndTip, TTM_ADDTOOL, 0, (LPARAM) &dps->toolInfo);

			GetWindowRect(dps->hwndHeader, &hdrRect);
			dps->cyHeader = hdrRect.bottom - hdrRect.top;

			SCROLLINFO si;
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_POS | SIF_PAGE | SIF_RANGE;
			si.nPage = 32;
			si.nPos = dps->nPane;
			si.nTrackPos = dps->nPane;
			si.nMin = 0;
			si.nMax = 0xFFFF;
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

			dps->nPCs[0] = lpDebuggerCalc->cpu.pc;
			for (i = 1; i < PC_TRAILS; i++) {
				dps->nPCs[i] = -1;
			}
			dps->last_pagedown = 32;
			return 0;
		}
		case WM_SIZE:	{
			RECT rc;
			WINDOWPOS wp;
			HDLAYOUT hdl;
			dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);

			GetClientRect(hwnd, &rc);
			hdl.prc = &rc;
			hdl.pwpos = &wp;
			Header_Layout(dps->hwndHeader, &hdl);
			SetWindowPos(dps->hwndHeader, wp.hwndInsertAfter, wp.x, wp.y,
				wp.cx, wp.cy, wp.flags);

			if (rc.bottom < dps->cyHeader) rc.bottom = dps->cyHeader;
			if (dps->cyRow == 0) return 0;
			dps->nRows = (rc.bottom - dps->cyHeader)/dps->cyRow + 1;

			SendMessage(hwnd, WM_COMMAND, DB_DISASM, dps->nPane);
			// Assign page length to include length sum of all commands on screen
			Z80_info_t 	*zfirst 	= &dps->zinf[0],
						*zlast 	= &dps->zinf[dps->nRows-1];
			dps->nPage = zlast->waddr.addr + zlast->size - zfirst->waddr.addr;

			//calculate the correct ending page
			int last_top_page_addr;
			Z80_info_t zup[128];
			int nPane_old = dps->nPane;
			last_top_page_addr = 0xFFFF - (5 * dps->nRows);
			do {
				disassemble(lpDebuggerCalc->cpu.mem_c, dps->type, 
					addr_to_waddr(lpDebuggerCalc->cpu.mem_c, ++last_top_page_addr), dps->nRows, zup);
			} while ((int) zup[dps->nRows - 1].waddr.addr + zup[dps->nRows - 1].size <= GetMaxAddr(dps));

			last_top_page_addr = zup[0].waddr.addr + dps->last_pagedown;

			SCROLLINFO si;
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_RANGE;
			si.nMax = last_top_page_addr;
			si.nMin = 0x0000;
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
			return 0;
		}
		case WM_CONTEXTMENU: {
			RECT rc;
			POINT p;
			HMENU hmenu;

			p.x = GET_X_LPARAM(lParam);
			p.y = GET_Y_LPARAM(lParam);

			GetClientRect(hwnd, &rc);
			ScreenToClient(hwnd, &p);

			SendMessage(hwnd, WM_LBUTTONDOWN, 0, MAKELPARAM(p.x, p.y));

			hmenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_DISASM_MENU));
			if (!hmenu) break;
			hmenu = GetSubMenu(hmenu, 0);

			if (!OnContextMenu(hwnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), hmenu)) {
				DefWindowProc(hwnd, Message, wParam, lParam);
			}

			DestroyMenu(hmenu);
			return 0;
		}
		case WM_NOTIFY: {
			int iSize = Header_GetItemCount(((NMHDR*) lParam)->hwndFrom);
			static int lpiArray[8];
			HWND hwndHeader = ((NMHDR*)lParam)->hwndFrom;
			switch (((NMHDR*) lParam)->code) {
				case HDN_BEGINTRACK:
				case HDN_ITEMCHANGING:
				case HDN_ENDTRACK:
				{
					dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					HDITEM *lphdi = ((NMHEADER*) lParam)->pitem;
					static BOOL in_changing = FALSE;
					HDITEM hdi;
					RECT rc;
					int iCol;

					if (in_changing) return FALSE;

					in_changing = TRUE;


					hdi.mask = HDI_LPARAM;
					Header_GetItem(hwndHeader, ((NMHEADER*) lParam)->iItem, &hdi);


					GetClientRect(hwnd, &rc);
					iCol = (int) hdi.lParam;
					if (lphdi->cxy > rc.right - rc.left) lphdi->cxy = rc.right - rc.left - 6;

					if (iCol == dpsDisasm && lphdi->cxy < COLUMN_X_OFFSET*5) {
						in_changing = FALSE;
						return TRUE;
					} else if (lphdi->cxy < COLUMN_X_OFFSET*3) {
						in_changing = FALSE;
						return TRUE;
					} else {
						dps->hdrs[iCol].cx = lphdi->cxy;
					}

					SendMessage(GetParent(hwnd), WM_USER, DB_UPDATE, 0);

					in_changing = FALSE;
					return FALSE;
				}
				case NM_RCLICK:
				{
					HMENU hmenu;
					POINT p;
					int i;
					dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);

					hmenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_DISASM_HEADER_MENU));
					if (!hmenu) break;
					hmenu = GetSubMenu(hmenu, 0);


					for (i = 0; i < ARRAYSIZE(dps->hdrs); i++) {
						UINT check = MF_BYPOSITION;
						int iHdr = disasmhdr_find(dps->hwndHeader, i);
						if (iHdr != -1) {
							check |= MF_CHECKED;
						} else {
							check |= MF_UNCHECKED;
						}
						CheckMenuItem(hmenu, i, check);
					}

					GetCursorPos(&p);
					if (!OnContextMenu(hwnd, p.x, p.y, hmenu)) {
						DefWindowProc(hwnd, Message, wParam, lParam);
					}
					DestroyMenu(hmenu);
					return TRUE;
				}
				case HDN_BEGINDRAG:
					Header_GetOrderArray(hwndHeader, iSize, lpiArray);
					if (((NMHEADER*) lParam)->iItem == 0) {
						// Stop the drag operation
						SendMessage(hwndHeader, WM_LBUTTONUP, 0, 0);
						return TRUE;
					}
					return FALSE;
				case HDN_ENDDRAG: {
					return FALSE;
				}
				case NM_RELEASEDCAPTURE: {
					// If the addr column isn't in the leftmost anymore, restore the old layout
					int lpiNewArray[8];
					Header_GetOrderArray(hwndHeader, iSize, lpiNewArray);

					if (lpiNewArray[0] != dpsAddr) {
						Header_SetOrderArray(hwndHeader, iSize, lpiArray);
						return TRUE;
					}
					SendMessage(GetParent(hwnd), WM_USER, DB_UPDATE, 0);
					return 0;
				}
				default:
					return FALSE;
			}
			return FALSE;
		}
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc, hdcDest;
			unsigned int i;
			HBITMAP hbm;
			RECT rc;
			int lpiArray[8];
			int iSize;
			HDITEM hdi = {0};
			int iItem;
			RECT tr;
			TRIVERTEX vert[2];
			u_int end_i;
			GRADIENT_RECT gRect;
			dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);

			GetClientRect(hwnd, &rc);

			hdcDest = BeginPaint(hwnd, &ps);

			hdc = CreateCompatibleDC(hdcDest);
			hbm = CreateCompatibleBitmap(hdcDest, rc.right, rc.bottom);
			SelectObject(hdc, hbm);

			FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));


			iSize = Header_GetItemCount(dps->hwndHeader);
			if (iSize == 0) {
				EndPaint(hwnd, &ps);

				DeleteObject(hbm);
				DeleteDC(hdc);
				return 0;
			}
			Header_GetOrderArray(dps->hwndHeader, iSize, lpiArray);


			hdi.mask = HDI_LPARAM;

			SelectObject(hdc, GetStockObject(DC_PEN));
			SetDCPenColor(hdc, GetSysColor(COLOR_BTNFACE));

			dps->max_right = 0;
			for (iItem = 0; iItem < iSize; iItem++) {
				int iCol;

				Header_GetItem(dps->hwndHeader, Header_OrderToIndex(dps->hwndHeader, iItem), &hdi);
				iCol = (int) hdi.lParam;
				if (iCol != -1) {
					dps->max_right += dps->hdrs[iCol].cx;
					MoveToEx(hdc, dps->max_right - 1, dps->cyHeader, NULL);
					LineTo(hdc, dps->max_right - 1, rc.bottom);
				}
			}

			if (dps->iSel + dps->NumSel > 0) {
				RECT sr = {COLUMN_X_OFFSET/2, dps->cyRow * dps->iSel, dps->max_right, dps->cyRow * dps->iSel + dps->cyRow*dps->NumSel};
				OffsetRect(&sr, 0, dps->cyHeader);
				DrawItemSelection(hdc, &sr, hwnd == GetFocus(), FALSE, 220);
			}

			if (dps->iHot != -1) {
				RECT sr = {COLUMN_X_OFFSET/2, dps->cyRow * dps->iHot, dps->max_right, dps->cyRow * dps->iHot + dps->cyRow};
				OffsetRect(&sr, 0, dps->cyHeader);
				DrawItemSelection(hdc, &sr, TRUE, FALSE, 130);
			}

			SetBkMode(hdc, TRANSPARENT);

			// Mark selection index as not on screen
			dps->iPC = -1;

			CopyRect(&tr, &rc);
			tr.bottom = dps->cyRow;

			OffsetRect(&tr, 0, dps->cyHeader);

			gRect.UpperLeft  = 0;
			gRect.LowerRight = 1;

			if (IsRectEmpty(&ps.rcPaint)) {
				EndPaint(hwnd, &ps);

				DeleteObject(hbm);
				DeleteDC(hdc);
				return 0;
			}

			i = (ps.rcPaint.top /*- dps->cyHeader*/) / dps->cyRow;
			//HACK: fixes problem with updating, not sure why
			if (i > 0)
				i--;
			OffsetRect(&tr, 0, dps->cyRow * i);

			end_i = (ps.rcPaint.bottom /*- dps->cyHeader*/ + dps->cyRow - 1) / dps->cyRow;

			for (; i < end_i; i++, OffsetRect(&tr, 0, dps->cyRow)) {
				BOOL do_gradient = FALSE;

				int pc_i;
				for (pc_i = 0; pc_i < PC_TRAILS && !do_gradient; pc_i++) {
					if ((dps->nPCs[pc_i] == dps->zinf[i].waddr.addr) && (dps->zinf[i].index != DA_LABEL)) {
						///dps->iPC = i;
						vert[0].Red = 0xFF00;
						vert[0].Green = 0xFF00;
						vert[0].Blue = 0xFF00;

						if (lpDebuggerCalc->cpu.halt && pc_i == 0) {
							vert [1] .Red    = GetRValue(COLOR_HALT) << 8;
							vert [1] .Green  = GetGValue(COLOR_HALT) << 8;
							vert [1] .Blue   = GetBValue(COLOR_HALT) << 8;
						} else {
							vert [1] .Red    = (GetRValue(COLOR_PC)+(((255-GetRValue(COLOR_PC))/(PC_TRAILS))*(pc_i))) << 8;
							vert [1] .Green  = (GetGValue(COLOR_PC)+(((255-GetGValue(COLOR_PC))/(PC_TRAILS))*(pc_i))) << 8;
							vert [1] .Blue   = (GetBValue(COLOR_PC)+(((255-GetBValue(COLOR_PC))/(PC_TRAILS))*(pc_i))) << 8;
						}
						do_gradient = TRUE;
					} else {
						vert[1].Red = 0xFF00;
						vert[1].Green = 0xFF00;
						vert[1].Blue = 0xFF00;
					}
				}
				BOOL breakpoint = FALSE;
				memory_context_t *calc_mem = &lpDebuggerCalc->mem_c;
				if (check_break(calc_mem, dps->zinf[i].waddr)) {
					vert [0] .Red    = GetRValue(COLOR_BREAKPOINT) << 8;
					vert [0] .Green  = GetGValue(COLOR_BREAKPOINT) << 8;
					vert [0] .Blue   = GetBValue(COLOR_BREAKPOINT) << 8;
					breakpoint = TRUE;
					do_gradient = TRUE;
				}
				if (check_mem_write_break(calc_mem, dps->zinf[i].waddr)) {
					if (breakpoint) {
						vert [0] .Red    |= GetRValue(COLOR_MEMPOINT_WRITE) << 8;
						vert [0] .Green  |= GetGValue(COLOR_MEMPOINT_WRITE) << 8;
						vert [0] .Blue   = GetBValue(COLOR_MEMPOINT_WRITE) << 8;
					} else {
						vert [0] .Red    = GetRValue(COLOR_MEMPOINT_WRITE) << 8;
						vert [0] .Green  = GetGValue(COLOR_MEMPOINT_WRITE) << 8;
						vert [0] .Blue   = GetBValue(COLOR_MEMPOINT_WRITE) << 8;
					}
					breakpoint = TRUE;
					do_gradient = TRUE;
				}
				if (check_mem_read_break(calc_mem, dps->zinf[i].waddr)) {
					if (breakpoint) {
						vert [0] .Red    |= GetRValue(COLOR_MEMPOINT_READ) << 8;
						vert [0] .Green  |= GetGValue(COLOR_MEMPOINT_READ) << 8;
						vert [0] .Blue   = GetBValue(COLOR_MEMPOINT_READ) << 8;
					} else {
						vert [0] .Red    = GetRValue(COLOR_MEMPOINT_READ) << 8;
						vert [0] .Green  = GetGValue(COLOR_MEMPOINT_READ) << 8;
						vert [0] .Blue   = GetBValue(COLOR_MEMPOINT_READ) << 8;
					}
					do_gradient = TRUE;
				}


				if (do_gradient) {
					int i, width = 0;
					for (i = 0; i < ARRAYSIZE(dps->hdrs); i++) {
						width += dps->hdrs[i].cx;
					}
					vert[0].x      = tr.left;
					vert[0].y      = tr.top;
					vert[1].x      = width;
					vert[1].y      = tr.bottom;
					GradientFill(hdc,vert,2,&gRect,1,GRADIENT_FILL_RECT_H);
				}

				// Draw the columns
				for (iItem = 0; iItem < iSize; iItem++, tr.left = tr.right) {
					int iCol;
					Header_GetItem(dps->hwndHeader, Header_OrderToIndex(dps->hwndHeader, iItem), &hdi);
					iCol = (int) hdi.lParam;
					if (iCol != -1) {
						tr.right = tr.left + dps->hdrs[iCol].cx;
						SelectObject(hdc, dps->hdrs[iCol].hfont);
						dps->hdrs[iCol].lpfnCallback(hdc, &dps->zinf[i], &tr);

					}
				}

				dps->max_right = tr.right;
				tr.left = 0;
				tr.right = rc.right;
				if (dps->zinf[i].waddr.addr == dps->nKey && hwnd == GetFocus()) {
					//dps->iSel = i;
					RECT fr;
					CopyRect(&fr, &tr);
					fr.left += COLUMN_X_OFFSET / 2;
					fr.right = dps->max_right;
					InflateRect(&fr, -1, -1);
					DrawFocusRect(hdc, &fr);
				}
			}

			if (dps->IsDragging == TRUE) {
				POINT p = {dps->MousePoint.x, dps->MousePoint.y};

				RECT sr;
				SetRect(&sr, dps->DragStart.x, dps->DragStart.y, p.x, p.y);
				DrawSelectionRect(hdc, &sr);
			}

			GetClientRect(hwnd, &rc);
			if (IsWindowEnabled(hwnd))
				BitBlt(hdcDest, 0, dps->cyHeader, rc.right, rc.bottom, hdc, 0, dps->cyHeader, SRCCOPY);
			else {
				HBITMAP hbmSizer = CreateCompatibleBitmap(NULL, rc.right - rc.left, rc.bottom - rc.top);
				SelectObject(hdcDest, hbmSizer);
				FillRect(hdcDest, &rc, (HBRUSH) GetStockObject(GRAY_BRUSH));
				BLENDFUNCTION bf;
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				bf.SourceConstantAlpha = 100;
				bf.AlphaFormat = AC_SRC_ALPHA;
				AlphaBlend(hdcDest, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
					hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, bf);
				DeleteObject(hbmSizer);
			}

			EndPaint(hwnd, &ps);

			DeleteObject(hbm);
			DeleteDC(hdc);
			return 0;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case DB_COPYLINE: {
					int i, j;
					Z80_info *zinf_line;
					dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					TCHAR *disassembly = (TCHAR *) LocalAlloc(LMEM_FIXED, 1024);
					memset(disassembly, 0, 1024);
					TCHAR copy_line[512];
					TCHAR buf[512];
					for (i = 0; i < dps->NumSel; i++) {
						zinf_line = &dps->zinf[dps->iSel + i];
						//print the address
						StringCbPrintf(copy_line, sizeof(copy_line), _T("%04X"), zinf_line->waddr.addr);
						StringCbCat(copy_line, sizeof(copy_line), _T(":"));
						//print the data
						for (j = 0; j < zinf_line->size; j++) {
							StringCbPrintf(buf + (j*2), sizeof(buf), _T("%02x"), 
								mem_read(lpDebuggerCalc->cpu.mem_c, zinf_line->waddr.addr+j));
						}
						StringCbCat(copy_line, sizeof(copy_line), buf);
						StringCbCat(copy_line, sizeof(copy_line), _T(": "));
						//print the command
						TCHAR *test_string = mysprintf(lpDebuggerCalc, zinf_line, REGULAR, da_opcode[zinf_line->index].format,
														zinf_line->a1, zinf_line->a2, zinf_line->a3, zinf_line->a4);
						StringCbCat(copy_line, sizeof(copy_line), test_string);
						StringCbCat(copy_line, sizeof(copy_line), _T("\t;"));
						//print the size
						if (zinf_line->size > 0) {
							StringCbPrintf(buf, sizeof(buf), _T("%d"), zinf_line->size);
							StringCbCat(copy_line, sizeof(copy_line), buf);
							StringCbCat(copy_line, sizeof(copy_line), _T(" bytes "));
						}
						//print the clocks
						if (da_opcode[zinf_line->index].clocks != -1) {
							if (da_opcode[zinf_line->index].clocks_cond) {
								StringCbPrintf(buf, sizeof(buf), _T("%d/%d"), da_opcode[zinf_line->index].clocks, da_opcode[zinf_line->index].clocks_cond);
							} else {
								StringCbPrintf(buf, sizeof(buf), _T("%d"), da_opcode[zinf_line->index].clocks);
							}
							StringCbCat(copy_line, sizeof(copy_line), buf);
							StringCbCat(copy_line, sizeof(copy_line), _T(" clocks"));
						}
						StringCbCat(copy_line, sizeof(copy_line), _T("\r\n"));
						StringCbCat(disassembly, 1024, copy_line);
					}
					OpenClipboard(hwnd);
					EmptyClipboard();
					SetClipboardData(CF_TEXT, (HLOCAL) disassembly);
					CloseClipboard();
					break;
				}
				case DB_DISASM: {
					dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					u_int addr = (u_int) lParam;
					disassemble(&lpDebuggerCalc->mem_c, REGULAR, addr_to_waddr(lpDebuggerCalc->cpu.mem_c, addr), dps->nRows, dps->zinf);
					break;
				}
				case IDM_RUN_RUN:
				case DB_RUN: {
					int i;
					dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					EnableWindow(hwnd, FALSE);
					SendMessage(hwnd, WM_USER, DB_UPDATE, 0);
					for (i = 0; i < PC_TRAILS; i++) {
						dps->nPCs[i] = -1;
					}
					CPU_step((&lpDebuggerCalc->cpu));
					calc_unpause_linked();
					lpDebuggerCalc->running = TRUE;
					break;
				}
				case DB_STOP: {
					int i;
					dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					EnableWindow(hwnd, TRUE);
					SendMessage(hwnd, WM_COMMAND, DB_DISASM, lpDebuggerCalc->cpu.pc);
					db_step_finish(hwnd, dps);
					lpDebuggerCalc->running = FALSE;
					SendMessage(hwnd, WM_USER, DB_UPDATE, 0);
					break;
				}
				case IDM_RUN_STEP:
				case DB_STEP: {
					dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					CPU_step(&lpDebuggerCalc->cpu);
					db_step_finish(hwnd, dps);
					break;
				}
				case IDM_RUN_STEPOVER:
				case DB_STEPOVER: {
					dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					CPU_stepover(&lpDebuggerCalc->cpu);
					db_step_finish(hwnd, dps);
					break;
				}
				case IDM_RUN_STEPOUT: {
					dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					CPU_stepout(&lpDebuggerCalc->cpu);
					db_step_finish(hwnd, dps);
					break;
				}
				case DB_DISASM_GOTO:
				case DB_GOTO: {
					dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					int result;
					result = (int) DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DLGGOTO), hwnd, (DLGPROC) GotoDialogProc);
					if (result == IDOK) SendMessage(hwnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBTRACK, goto_addr & 0xFFFF), 0);
					dps->nSel = goto_addr & 0xFFFF;
					SetFocus(hwnd);
					return 0;
				}
				case DB_FIND_NEXT: {
					dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					unsigned short value = find_value - 1;
					int addr = search_backwards ? dps->nSel - 1 : dps->nSel + 1;
					while (addr <= 0xFFFF && addr >= 0x0000 && value != find_value) {
						if (search_backwards)
							addr--;
						else
							addr++;
						if (find_value > 0xFF)
							value = mem_read16(&lpDebuggerCalc->mem_c, addr);
						else
							value = mem_read(&lpDebuggerCalc->mem_c, addr);
					}
					if (addr > 0xFFFF || addr < 0x0000) {
						MessageBox(NULL, _T("Value not found"), _T("Find results"), MB_OK);
						break;
					}
					dps->nSel = addr;
					SetFocus(hwnd);
					SendMessage(hwnd, WM_COMMAND, DB_DISASM, addr);
					InvalidateRect(hwnd, NULL, FALSE);
					UpdateWindow(hwnd);
					return 0;
				}
				case DB_OPEN_FIND: {
					HWND hwndDialog = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DLGFIND), hwnd, (DLGPROC)FindDialogProc);
					ShowWindow(hwndDialog, SW_SHOW);
					break;
				}
				case DB_BREAKPOINT: {
					dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					waddr_t waddr = addr_to_waddr(&lpDebuggerCalc->mem_c, dps->nSel);

					if (check_break(&lpDebuggerCalc->mem_c, waddr)) {
						clear_break(&lpDebuggerCalc->mem_c, waddr);
					} else {
						set_break(&lpDebuggerCalc->mem_c, waddr);
					}
					InvalidateSel(hwnd, dps->iSel);
					SendMessage(GetParent(hwnd), WM_USER, DB_UPDATE, 0);
					break;
				}
				case DB_MEMPOINT_WRITE: {
					dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					waddr_t waddr = addr_to_waddr(&lpDebuggerCalc->mem_c, dps->nSel);

					if (check_mem_write_break(&lpDebuggerCalc->mem_c, waddr)) {
						clear_mem_write_break(&lpDebuggerCalc->mem_c, waddr);
					} else {
						set_mem_write_break(&lpDebuggerCalc->mem_c, waddr);
					}
					InvalidateSel(hwnd, dps->iSel);
					SendMessage(GetParent(hwnd), WM_USER, DB_UPDATE, 0);
					break;
				}
				case DB_MEMPOINT_READ: {
					dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					waddr_t waddr = addr_to_waddr(&lpDebuggerCalc->mem_c, dps->nSel);

					if (check_mem_read_break(&lpDebuggerCalc->mem_c, waddr)) {
						clear_mem_read_break(&lpDebuggerCalc->mem_c, waddr);
					} else {
						set_mem_read_break(&lpDebuggerCalc->mem_c, waddr);
					}
					InvalidateSel(hwnd, dps->iSel);
					SendMessage(GetParent(hwnd), WM_USER, DB_UPDATE, 0);
					break;
				}
				case IDM_DISASMDATA:
				case IDM_DISASMDISASM:
				case IDM_DISASMSIZE:
				case IDM_DISASMCLOCKS: {
					dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					disasmhdr_toggle(dps->hwndHeader, &dps->hdrs[LOWORD(wParam) - IDM_DISASMADDR]);

					SendMessage(hwnd, WM_USER, DB_UPDATE, 0);
					break;
				}
				case DB_SET_PC: {
					dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					lpDebuggerCalc->cpu.pc = dps->zinf[dps->iSel].waddr.addr;
					cycle_pcs(dps);
					SendMessage(hwnd, WM_USER, DB_UPDATE, 0);
					break;
				}
			}
			break;
		}
		case WM_LBUTTONDOWN: {
			RECT hdrRect;
			RECT r;
			int y;
			dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);

			GetWindowRect(dps->hwndHeader, &hdrRect);

			y = GET_Y_LPARAM(lParam) - (hdrRect.bottom - hdrRect.top);


			GetClientRect(hwnd, &r);
			InvalidateSel(hwnd, dps->iSel);
			r.top = y -  (y % dps->cyRow); r.bottom = r.top + dps->cyRow;
			dps->iSel = y/dps->cyRow;
			dps->nSel = dps->zinf[y/dps->cyRow].waddr.addr;
			dps->nKey = dps->nSel;
			dps->nClick = dps->zinf[y/dps->cyRow].waddr.addr;
			dps->NumSel = 1;
			SendMessage(hwnd, WM_PAINT, 0, 0);
			InvalidateSel(hwnd, dps->iSel);
			UpdateWindow(hwnd);

			dps->DragStart.x = GET_X_LPARAM(lParam);
			dps->DragStart.y = GET_Y_LPARAM(lParam);
			SetFocus(hwnd);
			SetCapture(hwnd);
			return 0;
		}
		case WM_LBUTTONUP: {
			RECT hdrRect;
			int y;
			dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);

			GetWindowRect(dps->hwndHeader, &hdrRect);
			y = GET_Y_LPARAM(lParam) - dps->cyHeader;

			ReleaseCapture();
			InvalidateRect(hwnd, NULL, FALSE);
			dps->IsDragging = FALSE;
			dps->toolInfo.lParam = FALSE;
			SendMessage(dps->hwndTip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &dps->toolInfo);
			return 0;
		}
		case WM_MOUSELEAVE: {
			dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			InvalidateSel(hwnd, dps->iHot);
			dps->iHot = -1;
			return 0;
		}
		case WM_MOUSEMOVE: {
			dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			dps->MousePoint.x = GET_X_LPARAM(lParam);
			dps->MousePoint.y = GET_Y_LPARAM(lParam);

			if (wParam != MK_LBUTTON) {
				dps->dwDragCountdown = 0;
				dps->IsDragging = FALSE;
				dps->toolInfo.lParam = FALSE;
				SendMessage(dps->hwndTip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &dps->toolInfo);

				InvalidateSel(hwnd, dps->iHot);

				if (dps->MousePoint.x < dps->max_right) {
					TRACKMOUSEEVENT tme;

					dps->iHot = (dps->MousePoint.y - dps->cyHeader) / dps->cyRow;
					InvalidateSel(hwnd, dps->iHot);

					tme.cbSize = sizeof(tme);
					tme.dwFlags = TME_LEAVE;
					tme.hwndTrack = hwnd;
					tme.dwHoverTime = 1;
					TrackMouseEvent(&tme);
				} else {
					dps->iHot = -1;
				}
			} else if (dps->IsDragging == FALSE) {
				if (++dps->dwDragCountdown < (DWORD) GetSystemMetrics(SM_CXDRAG)) return 0;

				dps->IsDragging = TRUE;
			}

			if (dps->IsDragging == TRUE) {
				RECT r, rc;

				if (dps->MousePoint.y > dps->DragStart.y) {
					SetRect(&r, dps->DragStart.x, dps->DragStart.y, dps->MousePoint.x, dps->MousePoint.y);
				} else {
					SetRect(&r, dps->DragStart.x, dps->MousePoint.y, dps->MousePoint.x, dps->DragStart.y);
				}

				if (dps->MousePoint.x < dps->DragStart.x) {
					r.left = dps->MousePoint.x;
					r.right = dps->DragStart.x;
				}

				GetClientRect(hwnd, &rc);

				if (r.top < dps->cyHeader) r.top = dps->cyHeader;
				if (r.bottom > rc.bottom) r.bottom = rc.bottom;
				if (r.left < 0) r.left = 0;
				if (r.right > rc.right) r.right = rc.right;

				InvalidateRect(hwnd, &r, FALSE);
				OffsetRect(&r, 0, -dps->cyHeader);

				InvalidateSel(hwnd, dps->iSel);

				dps->nSel = dps->zinf[r.top/dps->cyRow].waddr.addr;
				dps->iSel = r.top/dps->cyRow;
				dps->NumSel = r.bottom/dps->cyRow - dps->iSel + 1;

				InvalidateSel(hwnd, dps->iSel);

				//update tooltip text
				int total_bytes = 0, total = 0, total_cond = 0;
				TCHAR szTip[64], szOldTip[64];
				dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);

				for (int j = dps->iSel; j != dps->iSel + dps->NumSel; j++) {
					total_bytes += dps->zinf[j].size;
					total += da_opcode[dps->zinf[j].index].clocks;
					total_cond += da_opcode[dps->zinf[j].index].clocks_cond;
				}

				dps->toolInfo.lpszText = szOldTip;
				SendMessage(dps->hwndTip, TTM_GETTEXT, ARRAYSIZE(szOldTip), (LPARAM) &dps->toolInfo);
				StringCbPrintf(szTip, sizeof(szTip), _T("$%04X : $%04X\n%d bytes\n%d/%d clocks\n"),
					dps->zinf[dps->iSel].waddr.addr, dps->zinf[dps->iSel+dps->NumSel-1].waddr.addr,
					total_bytes,
					total, total + total_cond);
				if (_tcsicmp(szTip, dps->toolInfo.lpszText)) {
					dps->toolInfo.lpszText = szTip;
					SendMessage(dps->hwndTip, TTM_UPDATETIPTEXT, 0, (LPARAM) &dps->toolInfo);
				}

				UpdateWindow(hwnd);
				POINT p = {dps->MousePoint.x, dps->MousePoint.y};
				ClientToScreen(hwnd, &p);
				SendMessage(dps->hwndTip, TTM_TRACKPOSITION, 0, MAKELONG(p.x + 10, p.y - 20));
				if (!dps->toolInfo.lParam) {
					dps->toolInfo.lpszText = szTip;
					SendMessage(dps->hwndTip, TTM_SETTOOLINFO, 0, (LPARAM) &dps->toolInfo);
					SendMessage(dps->hwndTip, TTM_SETMAXTIPWIDTH, 0, 150);
					SendMessage(dps->hwndTip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &dps->toolInfo);
					dps->toolInfo.lParam = TRUE;
				}
			}

			return 0;
		}
		case WM_KEYDOWN: {
			dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			int Q1 = dps->nRows/4, Q3 = dps->nRows - dps->nRows/4 - 1;

			BOOL bCenter = FALSE;
			int nSel_old = dps->nSel;

			switch (wParam) {
				case VK_DOWN:
					dps->nKey = next_select(dps, dps->nKey);

					if (GetKeyState(VK_SHIFT) & 0x80000000) {
						dps->nSel = min(dps->nClick, dps->nKey);
						dps->NumSel = abs(addr_to_index(dps, dps->nKey) - addr_to_index(dps, dps->nClick)) + 1;
					} else {
						dps->nSel = dps->nKey;
						dps->nClick = dps->nSel;
						dps->NumSel = 1;
					}
					bCenter = TRUE;
					break;
				case VK_UP:
				{
					dps->nKey = prev_select(dps, dps->nKey);

					if (GetKeyState(VK_SHIFT) & 0x80000000) {
						dps->nSel = min(dps->nClick, dps->nKey);
						dps->NumSel = abs(addr_to_index(dps, dps->nKey) - addr_to_index(dps, dps->nClick)) + 1;
					} else {
						dps->nSel = dps->nKey;
						dps->nClick = dps->nSel;
						dps->NumSel = 1;
					}
					bCenter = TRUE;
					break;
				}
				case VK_NEXT:
					SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
					break;
				case VK_PRIOR:
					SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
					break;
				case VK_F3:
					SendMessage(hwnd, WM_COMMAND, DB_MEMPOINT_WRITE, 0);
					break;
				case 'G':
					SendMessage(hwnd, WM_COMMAND, DB_GOTO, 0);
					break;
				case 'F':
					SendMessage(hwnd, WM_COMMAND, DB_OPEN_FIND, 0);
					break;
				case 'P':
					SendMessage(GetParent(hwnd), WM_COMMAND, IDM_VIEW_PORTMONITOR, 0);
					break;
				case 'B':
					SendMessage(GetParent(hwnd), WM_COMMAND, IDM_VIEW_BREAKPOINTS, 0);
					break;
			}

			if (bCenter && (dps->nSel < dps->nPane || dps->nSel > dps->nPane + dps->nPage)) {
				dps->nSel = nSel_old;
				SendMessage(hwnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBTRACK, dps->nSel), 0);
				while (Q1--) SendMessage(hwnd, WM_VSCROLL, MAKEWPARAM(SB_LINEUP, 0), 0);
				UpdateWindow(hwnd);
				return 0;
			}

			dps->iSel = addr_to_index(dps, dps->nSel);
			// it's on the screen
			if (dps->iSel < Q1) SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
			else if (dps->iSel > Q3) SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);

			InvalidateRect(hwnd, NULL, FALSE);
			UpdateWindow(hwnd);
			return 0;
		}
		case WM_MOUSEWHEEL: {
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			int i;

			WPARAM sbtype;
			if (zDelta > 0) sbtype = SB_LINEUP;
			else sbtype = SB_LINEDOWN;


			for (i = 0; i < abs(zDelta); i += WHEEL_DELTA)
				SendMessage(hwnd, WM_VSCROLL, sbtype, 0);

			return 0;
		}
		case WM_VSCROLL: {
			dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			SCROLLINFO si;
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_RANGE;
			GetScrollInfo(hwnd, SB_VERT, &si);
			switch (LOWORD(wParam)) {
				case SB_TOP:			//Home key
					dps->nPane = 0;
					break;
				case SB_BOTTOM:
					dps->nPane = si.nMax - dps->last_pagedown;
					break;
				case SB_LINEUP:
				{
#define LINEUP_DEPTH	12
					Z80_info_t zup[LINEUP_DEPTH];
					unsigned int i;
					int nPane_old = dps->nPane;
					if (dps->nPane == 0) return 0;

					// Disasm 6 commands such that the 6th is equal to the first
					// visible command
					do {
						disassemble(lpDebuggerCalc->cpu.mem_c, dps->type,
							addr_to_waddr(lpDebuggerCalc->cpu.mem_c, --dps->nPane), LINEUP_DEPTH, zup);
					} while (zup[LINEUP_DEPTH-2].waddr.addr > dps->zinf[0].waddr.addr && dps->nPane);


					for (i = 0; i < LINEUP_DEPTH && zup[i].waddr.addr != dps->zinf[0].waddr.addr; i++);
					if (dps->nPane == 0) {
						if (i == 0) return 0;
						dps->nPane = zup[i - 1].waddr.addr;
					} else {
						if (i == LINEUP_DEPTH || i == 0) {
							dps->nPane = nPane_old - 1;
						} else {
							dps->nPane = zup[i - 1].waddr.addr;
						}
					}
					dps->iSel++;
					break;
				}
				case SB_LINEDOWN:
					if (dps->zinf[0].waddr.addr + dps->nPage == GetMaxAddr(dps)) return 0;

					if (dps->zinf[0].size) {
						dps->nPane += dps->zinf[0].size;
					} else {
						// label
						dps->nPane += dps->zinf[1].size;
					}
					dps->iSel--;
					break;
				case SB_THUMBTRACK:
					dps->nPane = HIWORD(wParam);
					break;
				case SB_PAGEDOWN:
					dps->last_pagedown = dps->zinf[dps->nRows - 2].waddr.addr - dps->nPane;
					dps->nPane += dps->last_pagedown;
					break;
				case SB_PAGEUP:
					dps->nPane -= dps->last_pagedown;
					break;
			}

			si.fMask = SIF_POS  | SIF_PAGE;
			si.nPage = dps->last_pagedown;
			si.nPos = dps->nPane;
			si.nTrackPos = dps->nPane;
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

			SendMessage(hwnd, WM_COMMAND, DB_DISASM, dps->nPane);
			{
				Z80_info_t 	*zfirst = &dps->zinf[0],
							*zlast 	= &dps->zinf[dps->nRows-1];

				dps->nPage = zlast->waddr.addr + zlast->size - zfirst->waddr.addr;
				InvalidateRect(hwnd, NULL, TRUE);
			}
			return dps->nPane;
		}
		case WM_USER: {
			dp_settings *dps = (dp_settings *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			switch (wParam) {
				case DB_UPDATE:
					SendMessage(hwnd, WM_COMMAND, DB_DISASM, dps->nPane);
					InvalidateRect(hwnd, NULL, FALSE);
					UpdateWindow(hwnd);
					break;
				case DB_RESUME:
					dps->nPCs[0] = lpDebuggerCalc->cpu.pc;
					EnableWindow(hwnd, TRUE);
					//this will change so the run/stop button says Run
					extern HWND htoolbar;
					HWND hButton = FindWindowEx(htoolbar, NULL, _T("BUTTON"), _T("Stop"));
					TBBTN *tbb = (TBBTN *) GetWindowLongPtr(hButton, GWLP_USERDATA);
					ChangeRunButtonIconAndText(tbb);
					
					SendMessage(hwnd, WM_USER, DB_UPDATE, 0);
					break;
			}
			return 0;
		}
		default: {
			return DefWindowProc(hwnd, Message, wParam, lParam);
		}
	}
	return 0;
}
