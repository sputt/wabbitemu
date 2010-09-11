#include "stdafx.h"

#include "dbdisasm.h"
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

extern Z80_com_t da_opcode[256];

extern HINSTANCE g_hInst;
extern unsigned short goto_addr;

void sprint_addr(HDC hdc, Z80_info_t *zinf, RECT *r) {
	char s[64];

	SetTextColor(hdc, RGB(0, 0, 0));
#ifdef WINVER
	sprintf_s(s, "%04X", zinf->addr);
#else
	sprintf(s, "%04X", zinf->addr);
#endif

	r->left += COLUMN_X_OFFSET;
	DrawText(hdc, s, -1, r, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
}

void sprint_data(HDC hdc, Z80_info_t *zinf, RECT *r) {
	char s[64];
	int j;
	SetTextColor(hdc, RGB(0, 0, 0));

	if (zinf->size == 0) return;

	for (j = 0; j < zinf->size; j++) {
#ifdef WINVER
		sprintf_s(s + (j*2), 3, "%02x", mem_read(calcs[DebuggerSlot].cpu.mem_c, zinf->addr+j));
#else
		sprintf(s + (j*2), "%02x", mem_read(calcs[DebuggerSlot].cpu.mem_c, zinf->addr+j));
#endif
	}
	r->left += COLUMN_X_OFFSET;
	DrawText(hdc, s, -1, r, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
}

void sprint_command(HDC hdc, Z80_info_t *zinf, RECT *r) {
	mysprintf(hdc, zinf, r, da_opcode[zinf->index].format, zinf->a1, zinf->a2, zinf->a3, zinf->a4);
}

void sprint_size(HDC hdc, Z80_info_t *zinf, RECT *r) {
	char s[64];
	SetTextColor(hdc, RGB(0, 0, 0));
	if (zinf->size == 0) return;
#ifdef WINVER
	sprintf_s(s, "%d", zinf->size);
#else
	sprintf(s, "%d", zinf->size);
#endif

	r->left += COLUMN_X_OFFSET;
	DrawText(hdc, s, -1, r, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
}

void sprint_clocks(HDC hdc, Z80_info_t *zinf, RECT *r) {
	char s[64];
	SetTextColor(hdc, RGB(0, 0, 0));
	if (da_opcode[zinf->index].clocks != -1) {
		if (da_opcode[zinf->index].clocks_cond) {
#ifdef WINVER
			sprintf_s(s, "%d/%d", da_opcode[zinf->index].clocks, da_opcode[zinf->index].clocks_cond);
#else
			sprintf(s, "%d/%d", da_opcode[zinf->index].clocks, da_opcode[zinf->index].clocks_cond);
#endif
		} else {
#ifdef WINVER
			sprintf_s(s, "%d", da_opcode[zinf->index].clocks);
#else
			sprintf(s, "%d", da_opcode[zinf->index].clocks);
#endif
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

	RECT r;
	RECT hdrRect;

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

	{
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
			disassemble(cpu->mem_c, old_pc.addr, 1, &zinflocal);

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

	disassemble(cpu->mem_c, cpu->pc, 1, &zinflocal);

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
		for (i = 0; i < NumElm(usable_commands); i++) {
			if (zinflocal.index == usable_commands[i]) {
				while ((tc_elapsed(cpu->timer_c) - time) < 15.0 && cpu->pc != (zinflocal.addr + zinflocal.size))
					CPU_step(cpu);
				return;
			}
		}

		CPU_step(cpu);
	}
}



void disasmhdr_show(HWND hwndHeader, disasmhdr_t* hdrs) {
	int lpiNewArray[8];
	int iSize = (int) SendMessage(hwndHeader, HDM_GETITEMCOUNT, 0, 0);
	SendMessage(hwndHeader, HDM_GETORDERARRAY, (WPARAM) iSize, (LPARAM) lpiNewArray);
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

	iSize = (int) SendMessage(hwndHeader, HDM_GETITEMCOUNT, 0, 0);
	return Header_InsertItem(hwndHeader, iSize + 1, &hdi);
}

int disasmhdr_find(HWND hwndHeader, int index) {
	HDITEM hdi;
	int iSize = (int) SendMessage(hwndHeader, HDM_GETITEMCOUNT, 0, 0);
	int i;

	hdi.mask = HDI_LPARAM;

	for (i = 0; i < iSize; i++) {
		SendMessage(hwndHeader, HDM_GETITEM, i, (LPARAM) &hdi);
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


extern HFONT hfontSegoe, hfontLucida, hfontLucidaBold;
static disasmpane_settings_t *dps;
static Z80_info_t zinf[256];

static int addr_to_index(int addr) {
	u_int i = 0;
	for (i = 0; i < dps->nRows; i++) {
		if (zinf[i].addr == addr)
			return i;
	}
	return -1;
}

static int next_select(int addr) {
	int i = addr_to_index(addr);
	if (i > 255)
		return -1;
	return zinf[i+1].addr;
}

static int prev_select(int addr) {
	int i = addr_to_index(addr);
	if (i < 1)
		return -1;
	return zinf[i-1].addr;
}

static void cycle_pcs(dp_settings *dps) {
	int i;
	for (i = PC_TRAILS-1; i > 0; i--) {
		dps->nPCs[i] = dps->nPCs[i-1];
	}

	dps->nPCs[0] = calcs[DebuggerSlot].cpu.pc;
}

LRESULT CALLBACK DisasmProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static TEXTMETRIC tm;
	static BOOL IsDragging = FALSE;
	static POINT MousePoint;
	static HWND hwndTip;
	static TOOLINFO toolInfo;
	static int cyHeader;
	static int max_right;
	static int nClick;

	switch (Message) {
		case WM_SETFOCUS:
		case WM_KILLFOCUS: {
			InvalidateSel(hwnd, dps->iSel);
			UpdateWindow(hwnd);
			return 0;
		}
		case WM_CREATE: {
			HDC hdc = GetDC(hwnd);
			RECT rc;
			int i;
			WINDOWPOS wp;
			HDLAYOUT hdl;
			RECT hdrRect;

			GetClientRect(hwnd, &rc);

			dps = (disasmpane_settings_t *) ((CREATESTRUCT*)lParam)->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) dps);

			dps->hfontDisasm = hfontLucida;
			dps->hfontData = hfontLucida;
			dps->hfontAddr = hfontLucidaBold;

			SelectObject(hdc, dps->hfontDisasm);
			GetTextMetrics(hdc, &tm);
			dps->cyRow = 4*tm.tmHeight/3;
			dps->nPane = dps->nSel;

			// Set column widths
			if (!dps->hdrs[0].cx) {
				dps->hdrs[0].hfont = dps->hfontAddr;
				dps->hdrs[1].hfont = dps->hfontData;
				dps->hdrs[2].hfont = dps->hfontDisasm;
				dps->hdrs[3].hfont = dps->hfontData;
				dps->hdrs[3].hfont = dps->hfontData;
			}

			InitCommonControls();
			// Create the header window
			dps->hwndHeader = CreateWindowEx(0, WC_HEADER, (LPCTSTR) NULL,
                WS_CHILD |  HDS_HORZ | WS_VISIBLE | HDS_FULLDRAG | HDS_DRAGDROP,
                0, 0, 1, 1, hwnd, (HMENU) ID_DISASMSIZE, g_hInst,
                (LPVOID) NULL);

			SendMessage(dps->hwndHeader, WM_SETFONT, (WPARAM) hfontSegoe, TRUE);

			hdl.prc = &rc;
			hdl.pwpos = &wp;
			SendMessage(dps->hwndHeader, HDM_LAYOUT, 0, (LPARAM) &hdl);
			SetWindowPos(dps->hwndHeader, wp.hwndInsertAfter, wp.x, wp.y,
				wp.cx, wp.cy, wp.flags);

			// Add all of the columns

			for (i = 0; i < NumElm(dps->hdrs); i++) {
				if (dps->hdrs[i].index != -1) dps->hdrs[i].index = disasmhdr_insert(dps->hwndHeader, &dps->hdrs[i]);
			}


			hwndTip = CreateWindowEx(
					0,
					TOOLTIPS_CLASS,
					NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
					CW_USEDEFAULT, CW_USEDEFAULT,
					CW_USEDEFAULT, CW_USEDEFAULT,
					hwnd, NULL, g_hInst, NULL);

			SetWindowPos(hwndTip, HWND_TOPMOST,0, 0, 0, 0,
			             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			SendMessage(hwndTip, TTM_ACTIVATE, TRUE, 0);

			toolInfo.cbSize = sizeof(toolInfo);
		    toolInfo.hwnd = hwnd;
		    toolInfo.uFlags = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE;
		    toolInfo.uId = (UINT_PTR)hwnd;
		    toolInfo.lpszText = "";
		    GetClientRect(hwnd, &toolInfo.rect);
		    SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);


			GetWindowRect(dps->hwndHeader, &hdrRect);

			cyHeader = hdrRect.bottom - hdrRect.top;


			dps->nPCs[0] = calcs[DebuggerSlot].cpu.pc;
			for (i = 1; i < PC_TRAILS; i++) {
				dps->nPCs[i] = -1;
			}
			return 0;
		}
		case WM_SIZE:	{
			RECT rc;

			WINDOWPOS wp;
			HDLAYOUT hdl;

			GetClientRect(hwnd, &rc);


			hdl.prc = &rc;
			hdl.pwpos = &wp;
			SendMessage(dps->hwndHeader, HDM_LAYOUT, 0, (LPARAM) &hdl);
			SetWindowPos(dps->hwndHeader, wp.hwndInsertAfter, wp.x, wp.y,
				wp.cx, wp.cy, wp.flags);

			if (rc.bottom < cyHeader) rc.bottom = cyHeader;
			if (dps->cyRow == 0) return 0;
			dps->nRows = (rc.bottom - cyHeader)/dps->cyRow + 1;

			SendMessage(hwnd, WM_COMMAND, DB_DISASM, dps->nPane);
			// Assign page length to include length sum of all commands on screen
			{
				Z80_info_t 	*zfirst 	= &zinf[0],
							*zlast 	= &zinf[dps->nRows-1];

				dps->nPage = zlast->addr + zlast->size - zfirst->addr;
			}
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
			int iSize = (int) SendMessage(((NMHDR*) lParam)->hwndFrom, HDM_GETITEMCOUNT, 0, 0);
			static int lpiArray[8];
			HWND hwndHeader = ((NMHDR*)lParam)->hwndFrom;
			switch (((NMHDR*) lParam)->code) {
				case HDN_BEGINTRACK:
					//if (((NMHEADER*) lParam)->iItem == 0)
					//	return TRUE;
				case HDN_ITEMCHANGING:
				case HDN_ENDTRACK:
				{
					HDITEM *lphdi = ((NMHEADER*) lParam)->pitem;
					static BOOL in_changing = FALSE;
					HDITEM hdi;
					RECT rc;
					int iCol;

					if (in_changing) return FALSE;

					in_changing = TRUE;


 					hdi.mask = HDI_LPARAM;
 					SendMessage(hwndHeader, HDM_GETITEM, ((NMHEADER*) lParam)->iItem, (LPARAM) &hdi);


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

					hmenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_DISASM_HEADER_MENU));
					if (!hmenu) break;
					hmenu = GetSubMenu(hmenu, 0);


					for (i = 0; i < NumElm(dps->hdrs); i++) {
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
					SendMessage(hwndHeader, HDM_GETORDERARRAY, (WPARAM) iSize, (LPARAM) lpiArray);

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
					SendMessage(hwndHeader, HDM_GETORDERARRAY, (WPARAM) iSize, (LPARAM) lpiNewArray);

					if (lpiNewArray[0] != dpsAddr) {
						SendMessage(hwndHeader, HDM_SETORDERARRAY, (WPARAM) iSize, (LPARAM) lpiArray);
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

			GetClientRect(hwnd, &rc);

			hdcDest = BeginPaint(hwnd, &ps);

			hdc = CreateCompatibleDC(hdcDest);
			hbm = CreateCompatibleBitmap(hdcDest, rc.right, rc.bottom);
			SelectObject(hdc, hbm);

			FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));


			iSize = (int) SendMessage(dps->hwndHeader, HDM_GETITEMCOUNT, 0, 0);
			if (iSize == 0) return 0;
			SendMessage(dps->hwndHeader, HDM_GETORDERARRAY, (WPARAM) iSize, (LPARAM) lpiArray);


			hdi.mask = HDI_LPARAM;

			SelectObject(hdc, GetStockObject(DC_PEN));
			SetDCPenColor(hdc, GetSysColor(COLOR_BTNFACE));

			max_right = 0;
			for (iItem = 0; iItem < iSize; iItem++) {
				int iCol;

				SendMessage(dps->hwndHeader, HDM_GETITEM, SendMessage(dps->hwndHeader, HDM_ORDERTOINDEX, iItem, 0), (LPARAM) &hdi);
				iCol = (int) hdi.lParam;
				if (iCol != -1) {
					max_right += dps->hdrs[iCol].cx;
					MoveToEx(hdc, max_right - 1, cyHeader, NULL);
					LineTo(hdc, max_right - 1, rc.bottom);
				}
			}

			if (dps->iSel + dps->NumSel > 0) {
				RECT sr = {COLUMN_X_OFFSET/2, dps->cyRow * dps->iSel, max_right, dps->cyRow * dps->iSel + dps->cyRow*dps->NumSel};
				OffsetRect(&sr, 0, cyHeader);
				DrawItemSelection(hdc, &sr, hwnd == GetFocus(), FALSE, 220);
			}

			if (dps->iHot != -1) {
				RECT sr = {COLUMN_X_OFFSET/2, dps->cyRow * dps->iHot, max_right, dps->cyRow * dps->iHot + dps->cyRow};
				OffsetRect(&sr, 0, cyHeader);
				DrawItemSelection(hdc, &sr, TRUE, FALSE, 130);
			}

			SetBkMode(hdc, TRANSPARENT);

			// Mark selection index as not on screen
			dps->iPC = -1;


			CopyRect(&tr, &rc);
			tr.bottom = dps->cyRow;

			OffsetRect(&tr, 0, cyHeader);



			gRect.UpperLeft  = 0;
			gRect.LowerRight = 1;

			if (IsRectEmpty(&ps.rcPaint))
				return 0;

			/*printf("rcPaint: %ld %ld %ld %ld\n",
					ps.rcPaint.left,
					ps.rcPaint.top,
					ps.rcPaint.right,
					ps.rcPaint.bottom);

			printf("starting i: %d\n",(ps.rcPaint.top - cyHeader)/dps->cyRow);*/

			i = (ps.rcPaint.top - cyHeader)/dps->cyRow;
			OffsetRect(&tr, 0, dps->cyRow * i);

			end_i = (ps.rcPaint.bottom - cyHeader + dps->cyRow - 1)/dps->cyRow;

			for (; i < end_i; i++, OffsetRect(&tr, 0, dps->cyRow)) {
				BOOL do_gradient = FALSE;

				int pc_i;
				for (pc_i = 0; pc_i < PC_TRAILS && !do_gradient; pc_i++) {
					if ((dps->nPCs[pc_i] == zinf[i].addr) && (zinf[i].index != DA_LABEL)) {
						///dps->iPC = i;
						vert[0].Red = 0xFF00;
						vert[0].Green = 0xFF00;
						vert[0].Blue = 0xFF00;

						if (calcs[DebuggerSlot].cpu.halt && pc_i == 0) {
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
				memory_context_t *calc_mem = &calcs[DebuggerSlot].mem_c;
				if (check_break(calc_mem, zinf[i].addr)) {
					vert [0] .Red    = GetRValue(COLOR_BREAKPOINT) << 8;
					vert [0] .Green  = GetGValue(COLOR_BREAKPOINT) << 8;
					vert [0] .Blue   = GetBValue(COLOR_BREAKPOINT) << 8;
					breakpoint = TRUE;
					do_gradient = TRUE;
				}
				if (check_mem_write_break(calc_mem, zinf[i].addr)) {
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
				if (check_mem_read_break(calc_mem, zinf[i].addr)) {
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
					vert[0].x      = tr.left;
					vert[0].y      = tr.top;
					vert[1].x      = tr.right;
					vert[1].y      = tr.bottom;
					GradientFill(hdc,vert,2,&gRect,1,GRADIENT_FILL_RECT_H);
				}

				// Draw the columns

 				for (iItem = 0; iItem < iSize; iItem++, tr.left = tr.right) {
					int iCol;
					SendMessage(dps->hwndHeader, HDM_GETITEM, SendMessage(dps->hwndHeader, HDM_ORDERTOINDEX, iItem, 0), (LPARAM) &hdi);
					iCol = (int) hdi.lParam;
					if (iCol != -1) {
						tr.right = tr.left + dps->hdrs[iCol].cx;
						SelectObject(hdc, dps->hdrs[iCol].hfont);
						dps->hdrs[iCol].lpfnCallback(hdc, &zinf[i], &tr);

					}
				}

 				max_right = tr.right;
				tr.left = 0;
 				tr.right = rc.right;
				if (zinf[i].addr == dps->nKey && hwnd == GetFocus()) {
					//dps->iSel = i;
					RECT fr;
					CopyRect(&fr, &tr);
					fr.left += COLUMN_X_OFFSET/2;
					fr.right = max_right;
					InflateRect(&fr, -1, -1);
					DrawFocusRect(hdc, &fr);
				}
			}

			if (IsDragging == TRUE) {
				POINT p = {MousePoint.x, MousePoint.y};

				RECT sr;
				SetRect(&sr, dps->DragStart.x, dps->DragStart.y, p.x, p.y);
				DrawSelectionRect(hdc, &sr);
			}

			GetClientRect(hwnd, &rc);
			BitBlt(hdcDest, 0, cyHeader, rc.right, rc.bottom, hdc, 0, cyHeader, SRCCOPY);

			EndPaint(hwnd, &ps);

			DeleteObject(hbm);
			DeleteDC(hdc);
			return 0;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case DB_DISASM: {
					u_int addr = (u_int) lParam;
					disassemble(&calcs[DebuggerSlot].mem_c, addr, dps->nRows, zinf);
					break;
				}
				case DB_RUN: {
					//SendMessage(hwnd, WM_COMMAND, DB_STEP, 0);
					CPU_step((&calcs[DebuggerSlot].cpu));
					calcs[DebuggerSlot].running = TRUE;
					/*GetWindowRect(hwnd, &db_rect);

					GetExpandPaneState(&expand_pane_state)*/
					//DestroyWindow(GetParent(hwnd));
					break;
				}
				case DB_STEP: {
					int past_last;
					int before_first;

					CPU_step((&calcs[DebuggerSlot].cpu));
				db_step_finish:
					past_last = calcs[DebuggerSlot].cpu.pc - zinf[dps->nRows-1].addr + zinf[dps->nRows-1].size;
					before_first = zinf[0].addr - calcs[DebuggerSlot].cpu.pc;
					//InvalidateSel(hwnd, dps->iPC);
					InvalidateSel(hwnd, dps->iSel);
					dps->nSel = (&calcs[DebuggerSlot].cpu)->pc;
					if (past_last >= 0 || before_first > 0) {
						int iQ1;
						SendMessage(hwnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBTRACK, calcs[DebuggerSlot].cpu.pc), 0);
						iQ1 = dps->nRows/4;
						if (iQ1 == 0) return 0;
						while (iQ1--) SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
						UpdateWindow(hwnd);
					} else if (past_last > 0) {
						SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
					} else if (before_first > 0) {
						SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
					} else {
						UpdateWindow(hwnd);
					}
					//InvalidateSel(hwnd, dps->iPC);
					cycle_pcs(dps);
					SendMessage(GetParent(hwnd), WM_USER, DB_UPDATE, 0);
					break;
				}
				case DB_STEPOVER: {
					CPU_stepover(&calcs[DebuggerSlot].cpu);
					goto db_step_finish;
				}
				case DB_GOTO: {
					int result;
					result = (int) DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DLGGOTO), hwnd, (DLGPROC)GotoDialogProc);
					if (result == IDOK) SendMessage(hwnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBTRACK, goto_addr), 0);
					dps->nSel = goto_addr;
					SetFocus(hwnd);
					return 0;
				}
				case DB_BREAKPOINT: {
					bank_t *bank = &calcs[DebuggerSlot].mem_c.banks[mc_bank(dps->nSel)];

					if (check_break(&calcs[DebuggerSlot].mem_c, dps->nSel)) {
						clear_break(&calcs[DebuggerSlot].mem_c, bank->ram, bank->page, dps->nSel);
					} else {
						set_break(&calcs[DebuggerSlot].mem_c, bank->ram, bank->page, dps->nSel);
					}
					InvalidateSel(hwnd, dps->iSel);
					break;
				}
				case DB_MEMPOINT_WRITE: {
					bank_t *bank = &calcs[DebuggerSlot].mem_c.banks[mc_bank(dps->nSel)];

					if (check_mem_write_break(&calcs[DebuggerSlot].mem_c, dps->nSel)) {
						clear_mem_write_break(&calcs[DebuggerSlot].mem_c, bank->ram, bank->page, dps->nSel);
					} else {
						set_mem_write_break(&calcs[DebuggerSlot].mem_c, bank->ram, bank->page, dps->nSel);
					}
					InvalidateSel(hwnd, dps->iSel);
					SendMessage(GetParent(hwnd), WM_USER, DB_UPDATE, 0);
					break;
				}
				case DB_MEMPOINT_READ: {
					bank_t *bank = &calcs[DebuggerSlot].mem_c.banks[mc_bank(dps->nSel)];

					if (check_mem_read_break(&calcs[DebuggerSlot].mem_c, dps->nSel)) {
						clear_mem_read_break(&calcs[DebuggerSlot].mem_c, bank->ram, bank->page, dps->nSel);
					} else {
						set_mem_read_break(&calcs[DebuggerSlot].mem_c, bank->ram, bank->page, dps->nSel);
					}
					InvalidateSel(hwnd, dps->iSel);
					SendMessage(GetParent(hwnd), WM_USER, DB_UPDATE, 0);
					break;
				}
				case IDM_DISASMDATA:
				case IDM_DISASMDISASM:
				case IDM_DISASMSIZE:
				case IDM_DISASMCLOCKS: {
					disasmhdr_toggle(dps->hwndHeader,
						&dps->hdrs[LOWORD(wParam) - IDM_DISASMADDR]);

					SendMessage(hwnd, WM_USER, DB_UPDATE, 0);
					break;
				}
				case IDM_DSETPC: {
					calcs[DebuggerSlot].cpu.pc = zinf[dps->iSel].addr;
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

			GetWindowRect(dps->hwndHeader, &hdrRect);

			y = GET_Y_LPARAM(lParam) - (hdrRect.bottom - hdrRect.top);


			GetClientRect(hwnd, &r);
			InvalidateSel(hwnd, dps->iSel);
			r.top = y -  (y % dps->cyRow); r.bottom = r.top + dps->cyRow;
			dps->iSel = y/dps->cyRow;
			dps->nSel = zinf[y/dps->cyRow].addr;
			dps->nKey = dps->nSel;
			nClick = zinf[y/dps->cyRow].addr;
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

			GetWindowRect(dps->hwndHeader, &hdrRect);
			y = GET_Y_LPARAM(lParam) - cyHeader;

			ReleaseCapture();
			InvalidateRect(hwnd, NULL, FALSE);
			IsDragging = FALSE;
			SendMessage(hwndTip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &toolInfo);
			return 0;
		}
		case WM_MOUSELEAVE: {
			InvalidateSel(hwnd, dps->iHot);
			dps->iHot = -1;
			return 0;
		}
		case WM_MOUSEMOVE: {
			static DWORD dwDragCountdown = 0;
			int total = 0, total_cond = 0, total_bytes = 0;

			MousePoint.x = GET_X_LPARAM(lParam);
			MousePoint.y = GET_Y_LPARAM(lParam);

			if (wParam != MK_LBUTTON) {
				dwDragCountdown = 0;
				IsDragging = FALSE;
				SendMessage(hwndTip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &toolInfo);

				InvalidateSel(hwnd, dps->iHot);

				if (MousePoint.x < max_right) {
					TRACKMOUSEEVENT tme;

					dps->iHot = (MousePoint.y - cyHeader) / dps->cyRow;
					InvalidateSel(hwnd, dps->iHot);

					tme.cbSize = sizeof(tme);
					tme.dwFlags = TME_LEAVE;
					tme.hwndTrack = hwnd;
					tme.dwHoverTime = 1;
					TrackMouseEvent(&tme);
				} else {
					dps->iHot = -1;
				}
			} else if (IsDragging == FALSE) {
				if (++dwDragCountdown < (DWORD) GetSystemMetrics(SM_CXDRAG)) return 0;

				IsDragging = TRUE;
			}

			if (IsDragging == TRUE) {

				RECT r, rc;
				char szTip[64];
				int j;

				if (MousePoint.y > dps->DragStart.y) {
					SetRect(&r, dps->DragStart.x, dps->DragStart.y, MousePoint.x, MousePoint.y);
				} else {
					SetRect(&r, dps->DragStart.x, MousePoint.y, MousePoint.x, dps->DragStart.y);
				}

				if (MousePoint.x < dps->DragStart.x) {
					r.left = MousePoint.x;
					r.right = dps->DragStart.x;
				}

				GetClientRect(hwnd, &rc);

				if (r.top < cyHeader) r.top = cyHeader;
				if (r.bottom > rc.bottom) r.bottom = rc.bottom;
				if (r.left < 0) r.left = 0;
				if (r.right > rc.right) r.right = rc.right;

				InvalidateRect(hwnd, &r, FALSE);
				OffsetRect(&r, 0, -cyHeader);

				InvalidateSel(hwnd, dps->iSel);

				dps->nSel = zinf[r.top/dps->cyRow].addr;
				dps->iSel = r.top/dps->cyRow;
				dps->NumSel = r.bottom/dps->cyRow - dps->iSel + 1;

				InvalidateSel(hwnd, dps->iSel);

				SendMessage(hwndTip, TTM_SETMAXTIPWIDTH, 0, 150);



				for (j = dps->iSel; j != dps->iSel + dps->NumSel; j++) {
					total_bytes += zinf[j].size;
					total += da_opcode[zinf[j].index].clocks;
					total_cond += da_opcode[zinf[j].index].clocks_cond;
				}

#ifdef WINVER
				sprintf_s(szTip,"$%04X : $%04X\n%d bytes\n%d/%d clocks\n",
						zinf[dps->iSel].addr, zinf[dps->iSel+dps->NumSel-1].addr,
						total_bytes,
						total, total + total_cond);
#else
				sprintf(szTip,"$%04X : $%04X\n%d bytes\n%d/%d clocks\n",
						zinf[dps->iSel].addr, zinf[dps->iSel+dps->NumSel-1].addr,
						total_bytes,
						total, total + total_cond);
#endif
				toolInfo.lpszText = szTip;
				SendMessage(hwndTip, TTM_SETTOOLINFO, 0, (LPARAM) &toolInfo);

				UpdateWindow(hwnd);

				{
					POINT p = {MousePoint.x, MousePoint.y};
					ClientToScreen(hwnd, &p);
					SendMessage(hwndTip, TTM_TRACKPOSITION, 0, MAKELONG(p.x + 10, p.y - 20));
					SendMessage(hwndTip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &toolInfo);
				}
			}

			return 0;
		}
		case WM_KEYDOWN: {
			int Q1 = dps->nRows/4, Q3 = dps->nRows - dps->nRows/4 - 1;

			BOOL bCenter = FALSE;
			int nSel_old = dps->nSel;

			switch (wParam) {
				case VK_DOWN:
					dps->nKey = next_select(dps->nKey);

					if (GetKeyState(VK_SHIFT) & 0x80000000) {
						dps->nSel = min(nClick, dps->nKey);
						dps->NumSel = abs(addr_to_index(dps->nKey)
											- addr_to_index(nClick)) + 1;
					} else {
						dps->nSel = dps->nKey;
						nClick = dps->nSel;
						dps->NumSel = 1;
					}
					bCenter = TRUE;
					break;
				case VK_UP:
				{
					dps->nKey = prev_select(dps->nKey);

					if (GetKeyState(VK_SHIFT) & 0x80000000) {
						dps->nSel = min(nClick, dps->nKey);
						dps->NumSel = abs(addr_to_index(dps->nKey)
											- addr_to_index(nClick)) + 1;
					} else {
						dps->nSel = dps->nKey;
						nClick = dps->nSel;
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
				case VK_F3: {
					SendMessage(hwnd, WM_COMMAND, DB_MEMPOINT_WRITE, 0);
					break;
				}
				case 'G': {
					SendMessage(hwnd, WM_COMMAND, DB_GOTO, 0);
					break;
				}
			}

			if (bCenter && (dps->nSel < dps->nPane || dps->nSel > dps->nPane + dps->nPage)) {
				dps->nSel = nSel_old;
				SendMessage(hwnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBTRACK, dps->nSel), 0);
				while (Q1--) SendMessage(hwnd, WM_VSCROLL, MAKEWPARAM(SB_LINEUP, 0), 0);
				UpdateWindow(hwnd);
				return 0;
			}

			dps->iSel = addr_to_index(dps->nSel);
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
			static int last_pagedown = 32;

			switch (LOWORD(wParam)) {
				case SB_TOP:			//Home key
					dps->nPane = 0;
					break;
				case SB_BOTTOM:
					dps->nPane = 0x10000 - (4*dps->nRows);
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
						disassemble(calcs[DebuggerSlot].cpu.mem_c, --dps->nPane, LINEUP_DEPTH, zup);
					} while (zup[LINEUP_DEPTH-2].addr > zinf[0].addr && dps->nPane);


					for (i = 0; i < LINEUP_DEPTH && zup[i].addr != zinf[0].addr; i++);
					if (dps->nPane == 0) {
						if (i == 0) return 0;
						dps->nPane = zup[i - 1].addr;
					} else {
						if (i == LINEUP_DEPTH || i == 0) {
							dps->nPane = nPane_old - 1;
						} else {
							dps->nPane = zup[i - 1].addr;
						}
					}
					dps->iSel++;
					break;
				}
				case SB_LINEDOWN:
					if (zinf[0].addr + dps->nPage == 0x10000) return 0;

					if (zinf[0].size) {
						dps->nPane += zinf[0].size;
					} else {
						// label
						dps->nPane += zinf[1].size;
					}
					dps->iSel--;
					break;
				case SB_THUMBTRACK:
					dps->nPane = HIWORD(wParam);
					break;
				case SB_PAGEDOWN:
					last_pagedown = zinf[dps->nRows - 2].addr - dps->nPane;
					dps->nPane += last_pagedown;
					break;
				case SB_PAGEUP:
					dps->nPane -= last_pagedown;
					break;
			}

			SendMessage(hwnd, WM_COMMAND, DB_DISASM, dps->nPane);
			{
				Z80_info_t 	*zfirst 	= &zinf[0],
							*zlast 	= &zinf[dps->nRows-1];

				dps->nPage = zlast->addr + zlast->size - zfirst->addr;
				InvalidateRect(hwnd, NULL, TRUE);
			}
			return dps->nPane;
		}
		case WM_USER: {
			switch (wParam) {
				case DB_UPDATE:
					SendMessage(hwnd, WM_COMMAND, DB_DISASM, dps->nPane);
					InvalidateRect(hwnd, NULL, FALSE);
					UpdateWindow(hwnd);
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
