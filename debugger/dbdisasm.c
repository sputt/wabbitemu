#include "stdafx.h"

#include "guidebug.h"
#include "dbdisasm.h"
#include "dbtoolbar.h"
#include "dbcommon.h"
#include "dbfinddialog.h"
#include "core.h"
#include "guicontext.h"
#include "calc.h"
#include "disassemble.h"
#include "dbcommon.h"
#include "print.h"

#define COLUMN_X_OFFSET 7

#define COLOR_PC_R				180
#define COLOR_PC_G				180
#define COLOR_PC_B				180
#define COLOR_BREAKPOINT_R		230
#define COLOR_BREAKPOINT_G		160
#define COLOR_BREAKPOINT_B		180
#define COLOR_MEMPOINT_WRITE_R	255
#define COLOR_MEMPOINT_WRITE_G	177
#define COLOR_MEMPOINT_WRITE_B	100
#define COLOR_MEMPOINT_READ_R	255
#define COLOR_MEMPOINT_READ_B	250
#define COLOR_MEMPOINT_READ_G	145
#define COLOR_HALT_R			200
#define COLOR_HALT_G			200
#define COLOR_HALT_B			100

#define DISASM_LINE_MAX_LEN 2048

extern Z80_com_t da_opcode[256];

extern HINSTANCE g_hInst;

void sprint_addr(LPCALC lpCalc, HDC hdc, Z80_info_t *zinf, ViewType, RECT *r) {
	TCHAR s[64];

	SetTextColor(hdc, RGB(0, 0, 0));
	int page = zinf->waddr.page;
	if (zinf->waddr.is_ram) {
		switch (lpCalc->cpu.pio.model) {
			case TI_83P:
			case TI_73:
				page += 0x40;
				break;
			case TI_83PSE:
			case TI_84P:
			case TI_84PSE:
				page += 0x80;
				break;
		}
	}
	StringCbPrintf(s, sizeof(s), _T("%02X %04X"), page, zinf->waddr.addr);

	r->left += COLUMN_X_OFFSET;
	DrawText(hdc, s, -1, r, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
}

void sprint_data(LPCALC lpCalc, HDC hdc, Z80_info_t *zinf, ViewType, RECT *r) {
	TCHAR s[64];
	int j;
	SetTextColor(hdc, RGB(0, 0, 0));

	if (zinf->size == 0) return;

	waddr_t waddr = zinf->waddr;
	for (j = 0; j < zinf->size; j++) {
		waddr.addr = (uint16_t)(zinf->waddr.addr + j);
		if (waddr.addr % PAGE_SIZE < zinf->waddr.addr % PAGE_SIZE) {
			waddr.page++;
			// we don't handle ram changes here because things should never cross pages
			// therefore i don't really care
		}
		StringCbPrintf(s + (j*2), sizeof(s), _T("%02x"), wmem_read(lpCalc->cpu.mem_c, waddr));
	}
	r->left += COLUMN_X_OFFSET;
	DrawText(hdc, s, -1, r, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
}

void sprint_command(LPCALC lpCalc, HDC hdc, Z80_info_t *zinf, ViewType type, RECT *r) {
	MyDrawText(lpCalc, hdc, r, zinf, type, da_opcode[zinf->index].format, zinf->a1, zinf->a2, zinf->a3, zinf->a4);
}

void sprint_size(LPCALC, HDC hdc, Z80_info_t *zinf, ViewType, RECT *r) {
	TCHAR s[64];
	SetTextColor(hdc, RGB(0, 0, 0));
	if (zinf->size == 0) return;
	StringCbPrintf(s, sizeof(s), _T("%d"), zinf->size);

	r->left += COLUMN_X_OFFSET;
	DrawText(hdc, s, -1, r, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
}

void sprint_clocks(LPCALC, HDC hdc, Z80_info_t *zinf, ViewType, RECT *r) {
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

static int GetMaxAddr(dp_settings *dps) {
	switch (dps->type) {
		case REGULAR:
			return 0xFFFF;
		case FLASH:
			return dps->lpCalc->cpu.mem_c->flash_size;
		case RAM:
			return dps->lpCalc->cpu.mem_c->ram_size;
	}
	return -1;
}

void InvalidateSel(HWND hwnd, int sel) {
	LPTABWINDOWINFO lpTabInfo = (LPTABWINDOWINFO) GetWindowLongPtr(hwnd, GWLP_USERDATA);
	dp_settings *dps = (dp_settings *) lpTabInfo->tabInfo;
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

static BOOL had_exe_violation = FALSE;
void stepoverout_exe_callback(CPU_t *) {
	had_exe_violation = TRUE;
}

void CPU_stepout(LPCALC lpCalc, BOOL bTIOSDebug) {
	CPU_t *cpu = &lpCalc->cpu;
	uint64_t time = cpu->timer_c->tstates;
	uint16_t old_sp = cpu->sp;

	had_exe_violation = FALSE;
	BOOL backup_exe_violation = break_on_exe_violation;
	break_on_exe_violation = TRUE;
	void (*backupFunction)(CPU_t *) = cpu->exe_violation_callback;
	cpu->exe_violation_callback = stepoverout_exe_callback;

	uint64_t tstates5seconds = 5 * cpu->timer_c->freq;
	while ((cpu->timer_c->tstates - time) < tstates5seconds && !had_exe_violation) {
		waddr_t old_pc = addr16_to_waddr(cpu->mem_c, cpu->pc);
		CPU_step(cpu);

		if (cpu->sp > old_sp) {
			Z80_info_t zinflocal;
			disassemble(lpCalc, REGULAR, addr16_to_waddr(cpu->mem_c, old_pc.addr), 1, bTIOSDebug, &zinflocal);

			if (zinflocal.index == DA_RET 		||
				zinflocal.index == DA_RET_CC 	||
				zinflocal.index == DA_RETI		||
				zinflocal.index == DA_RETN) {
				
				return;
			}

		}
	}
	cpu->exe_violation_callback = backupFunction;
	break_on_exe_violation = backup_exe_violation;
}

/*
 * Repeatedly step until you get to the next command
 */
void CPU_stepover(LPCALC lpCalc, BOOL bTIOSDebug) {
	const int usable_commands[] = { DA_BJUMP, DA_BJUMP_N, DA_BCALL_N, DA_BCALL,
									DA_BLI, DA_CALL_X, DA_CALL_CC_X, DA_HALT, DA_RST_X};
	int i;
	CPU_t *cpu = &lpCalc->cpu;
	double time = cpu->timer_c->elapsed;
	Z80_info_t zinfarray[4];
	Z80_info_t *zinflocal = zinfarray;

	had_exe_violation = FALSE;
	BOOL backup_exe_violation = break_on_exe_violation;
	break_on_exe_violation = TRUE;
	void (*backupFunction)(CPU_t *) = cpu->exe_violation_callback;
	cpu->exe_violation_callback = stepoverout_exe_callback;

	disassemble(lpCalc, REGULAR, addr16_to_waddr(cpu->mem_c, cpu->pc), 1, bTIOSDebug, zinflocal);

	const double FIVE_SECONDS = 5.0;
	if (cpu->halt) {
		if (cpu->iff1) {
			while ((cpu->timer_c->elapsed - time) < FIVE_SECONDS &&
				cpu->halt == TRUE)
			{
				CPU_step(cpu);
			}
		} else {
			cpu->halt = FALSE;
		}
	} else if (zinflocal->index == DA_CALL_X || zinflocal->index == DA_CALL_CC_X) {
		uint16_t old_stack = cpu->sp;
		uint16_t return_pc = cpu->pc + 3;
		CPU_step(cpu);
		if (cpu->sp != old_stack) {
			double time = cpu->timer_c->elapsed;
			uint16_t old_sp = cpu->sp;
			while ((cpu->timer_c->elapsed - time) < FIVE_SECONDS &&
				!had_exe_violation)
			{
				uint16_t old_pc = cpu->pc;
				CPU_step(cpu);

				if ((cpu->sp >= old_sp || (old_sp - cpu->sp) >= 0xFF00) && (cpu->pc >= return_pc && (cpu->pc <= return_pc + 2))) {
					disassemble(lpCalc, REGULAR, addr16_to_waddr(cpu->mem_c, old_pc), 1, bTIOSDebug, zinflocal);

					if (zinflocal->index == DA_RET 		||
						zinflocal->index == DA_RET_CC 	||
						zinflocal->index == DA_RETI		||
						zinflocal->index == DA_RETN) {
				
						return;
					}

				}
			}
		}
	} else {
		for (i = 0; i < ARRAYSIZE(usable_commands); i++) {
			if (zinflocal->index == usable_commands[i]) {
				while ((cpu->timer_c->elapsed - time) < FIVE_SECONDS &&
					cpu->pc != (zinflocal->waddr.addr + zinflocal->size))
				{
					CPU_step(cpu);
				}
				return;
			}
		}

		CPU_step(cpu);
	}

	cpu->exe_violation_callback = backupFunction;
	break_on_exe_violation = backup_exe_violation;
}

int disasmhdr_insert(HWND hwndHeader, disasmhdr_t* dhdr) {
	HDITEM hdi;
	TEXTMETRIC tm;
	int iSize;

	HDC hdc = GetDC(GetParent(hwndHeader));
	SelectObject(hdc, dhdr->hfont);
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

static int addr_to_index(dp_settings *dps, int addr) {
	for (u_int i = 0; i < dps->nRows; i++) {
		if (dps->type == REGULAR) {
			if (dps->zinf[i].waddr.addr == addr)
				return i;
		} else {
			if (dps->zinf[i].waddr.addr == addr % PAGE_SIZE && dps->zinf[i].waddr.page == addr / PAGE_SIZE)
				return i;
		}
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
	for (int i = PC_TRAILS - 1; i > 0; i--) {
		dps->nPCs[i] = dps->nPCs[i - 1];
	}

	dps->nPCs[0] = dps->lpCalc->cpu.pc;
}

void clear_pcs(dp_settings *dps) {
	memset(dps->nPCs, -1, sizeof(dps->nPCs));
}

void reverse_cycle_pcs(dp_settings *dps) {
	for (int i = 0; i < PC_TRAILS - 1; i++) {
		dps->nPCs[i] = dps->nPCs[i + 1];
	}
	dps->nPCs[PC_TRAILS - 1] = -1;
	dps->nPCs[0] = dps->lpCalc->cpu.pc;
}

void invalidate_pcs(HWND hwnd, dp_settings *dps) {
	for (int i = 0; i < PC_TRAILS; i++) {
		InvalidateSel(hwnd, addr_to_index(dps, dps->nPCs[i]));
	}
}

void db_step_finish(HWND hwnd, dp_settings *dps, BOOL step_back = FALSE) {
	short past_last = (short) (dps->lpCalc->cpu.pc - dps->zinf[dps->nRows-1].waddr.addr + dps->zinf[dps->nRows-1].size);
	short before_first = dps->zinf[0].waddr.addr - dps->lpCalc->cpu.pc;
	InvalidateSel(hwnd, dps->iSel);
	BOOL changeScrollbar = TRUE;
	if (dps->type == REGULAR) {
		dps->nKey = dps->nSel = dps->lpCalc->cpu.pc;
		dps->iSel = 0;
	} else {
		waddr_t waddr = addr16_to_waddr(dps->lpCalc->cpu.mem_c, dps->lpCalc->cpu.pc);
		if ((waddr.is_ram && dps->type == RAM) || (!waddr.is_ram && dps->type == FLASH)) {
			dps->nKey = dps->nSel = waddr.page * PAGE_SIZE + (waddr.addr % PAGE_SIZE);
			dps->iSel = 0;
		} else {
			changeScrollbar = FALSE;
		}
	}
	if (changeScrollbar) {
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS;
		si.nPos = dps->nSel;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
	
		if (past_last >= 0 || before_first > 0) {
			int iQ1;
			SendMessage(hwnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBTRACK, dps->lpCalc->cpu.pc), 0);
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
	}

	invalidate_pcs(hwnd, dps);
	if (!step_back) {
		cycle_pcs(dps);
	}
	InvalidateSel(hwnd, addr_to_index(dps, dps->nPCs[0]));
	Debug_UpdateWindow(GetParent(GetParent(hwnd)));
}

static void on_running_changed(LPCALC lpCalc, LPVOID lParam) {
	HWND hDisasm = (HWND)lParam;
	EnableWindow(hDisasm, !lpCalc->running);
	LPTABWINDOWINFO lpTabInfo = (LPTABWINDOWINFO)GetWindowLongPtr(hDisasm, GWLP_USERDATA);
	dp_settings *dps = NULL;
	if (lpTabInfo != NULL) {
		dps = (dp_settings *)lpTabInfo->tabInfo;
		if (lpCalc->running) {
			clear_pcs(dps);
		} else {
			cycle_pcs(dps);
		}

		invalidate_pcs(hDisasm, dps);
	}
	Debug_UpdateWindow(hDisasm);
}

LRESULT CALLBACK DisasmProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	LPTABWINDOWINFO lpTabInfo = (LPTABWINDOWINFO)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	dp_settings *dps = NULL;
	if (lpTabInfo != NULL) {
		dps = (dp_settings *)lpTabInfo->tabInfo;
	}

	switch (Message) {
		case WM_SETFOCUS: {
			lpTabInfo->lpDebugInfo->hwndLastFocus = hwnd;
			if (dps != NULL) {
				InvalidateSel(hwnd, dps->iSel);
			}
			UpdateWindow(hwnd);
			return 0;
		}
		case WM_KILLFOCUS: {
			if (dps != NULL) {
				InvalidateSel(hwnd, dps->iSel);
			}
			UpdateWindow(hwnd);
			return 0;
		}
		case WM_DESTROY: {
			SetWindowLongPtr(hwnd, GWLP_USERDATA, NULL);
			calc_unregister_event(dps->lpCalc, ROM_RUNNING_EVENT, &on_running_changed, hwnd);
			free(lpTabInfo);
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

			lpTabInfo = (LPTABWINDOWINFO) ((LPCREATESTRUCT) lParam)->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) lpTabInfo);
			dps = (dp_settings *) lpTabInfo->tabInfo;

			dps->hfontDisasm = lpTabInfo->lpDebugInfo->hfontLucida;
			dps->hfontData = lpTabInfo->lpDebugInfo->hfontLucida;
			dps->hfontAddr = lpTabInfo->lpDebugInfo->hfontLucidaBold;

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

			SetWindowFont(dps->hwndHeader, lpTabInfo->lpDebugInfo->hfontSegoe, TRUE);

			hdl.prc = &rc;
			hdl.pwpos = &wp;
			Header_Layout(dps->hwndHeader, &hdl);
			SetWindowPos(dps->hwndHeader, wp.hwndInsertAfter, wp.x, wp.y,
				wp.cx, wp.cy, wp.flags);

			// Add all of the columns
			for (i = 0; i < NumElm(dps->hdrs); i++) {
				if (dps->hdrs[i].index != -1) dps->hdrs[i].index = disasmhdr_insert(dps->hwndHeader, &dps->hdrs[i]);
			}

			dps->hwndTip = CreateWindow(
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
			si.nMax = GetMaxAddr(dps);
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

			dps->nPCs[0] = dps->lpCalc->cpu.pc;
			for (i = 1; i < PC_TRAILS; i++) {
				dps->nPCs[i] = -1;
			}
			dps->last_pagedown = 32;

			calc_register_event(dps->lpCalc, ROM_RUNNING_EVENT, &on_running_changed, hwnd);
			return 0;
		}
		case WM_SIZE: {
			RECT rc;
			WINDOWPOS wp;
			HDLAYOUT hdl;

			GetClientRect(hwnd, &rc);
			hdl.prc = &rc;
			hdl.pwpos = &wp;
			Header_Layout(dps->hwndHeader, &hdl);
			SetWindowPos(dps->hwndHeader, wp.hwndInsertAfter, wp.x, wp.y,
				wp.cx, wp.cy, wp.flags);

			if (rc.bottom < dps->cyHeader) {
				rc.bottom = dps->cyHeader;
			}
			if (dps->cyRow == 0) {
				return 0;
			}
			dps->nRows = (rc.bottom - dps->cyHeader) / dps->cyRow + 1;

			SendMessage(hwnd, WM_COMMAND, DB_DISASM, dps->nPane);
			// Assign page length to include length sum of all commands on screen
			Z80_info_t 	*zfirst 	= &dps->zinf[0],
						*zlast 	= &dps->zinf[dps->nRows-1];
			dps->nPage = zlast->waddr.addr + zlast->size - zfirst->waddr.addr;

			// calculate the correct ending page
			int last_top_page_addr;
			Z80_info_t zup[128];
			last_top_page_addr = GetMaxAddr(dps) - (5 * dps->nRows);
			waddr_t waddr;
			if (dps->type == REGULAR) {
				do {
					waddr = addr16_to_waddr(dps->lpCalc->cpu.mem_c, (uint16_t) ++last_top_page_addr);
					disassemble(dps->lpCalc, dps->type, waddr, dps->nRows, lpTabInfo->lpDebugInfo->bTIOSDebug, zup);
				} while (zup[dps->nRows - 1].waddr.addr + zup[dps->nRows - 1].size <= 0xFFFF);
			} else {
				do {
					waddr = addr32_to_waddr(++last_top_page_addr, dps->type == RAM);
					disassemble(dps->lpCalc, dps->type, waddr, dps->nRows, lpTabInfo->lpDebugInfo->bTIOSDebug, zup);
				} while (zup[dps->nRows - 1].waddr.addr + zup[dps->nRows - 1].size <= 4);
					
				}

			int total_size = 0;
			for (u_int i = 0; i < dps->nRows; i++) {
				total_size += zup[i].size;
			}

			// check if the first page is visible from the last page
			if (zfirst->waddr.addr > zlast->waddr.addr) {
				dps->nPane--;
				SendMessage(hwnd, WM_SIZE, 0, 0);
			}

			last_top_page_addr = GetMaxAddr(dps) - total_size + dps->last_pagedown;

			SCROLLINFO si;
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_RANGE;
			si.nMax = last_top_page_addr;
			si.nMin = 0x0000;
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

			position_goto_dialog(dps->hGotoDialog);
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

					InvalidateRect(hwnd, NULL, FALSE);
					Debug_UpdateWindow(GetParent(hwnd));

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
					InvalidateRect(hwnd, NULL, FALSE);
					Debug_UpdateWindow(GetParent(hwnd));
					return 0;
				}
				default:
					return FALSE;
			}
			return FALSE;
		}
		case WM_PAINT:
			{
			unsigned int i;
			RECT rc;
			
			HDITEM hdi = {0};
			int iItem;
			RECT tr;
			TRIVERTEX vert[2];
			u_int end_i;
			GRADIENT_RECT gRect;

			GetClientRect(hwnd, &rc);

			RECT ur;
			GetUpdateRect(hwnd, &ur, FALSE);
			if ((ur.bottom - ur.top) == 0 || (ur.left - ur.right) == 0) {
				return 0;
			}

			int nCols = Header_GetItemCount(dps->hwndHeader);
			if (nCols == 0) {
				return 0;
			}

			PAINTSTRUCT ps;
			HDC hdcDest = BeginPaint(hwnd, &ps);

			HDC hdc = CreateCompatibleDC(hdcDest);
			HBITMAP hbm = CreateCompatibleBitmap(hdcDest, rc.right, rc.bottom);
			SelectObject(hdc, hbm);

			FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));

			int lpiArray[8];
			Header_GetOrderArray(dps->hwndHeader, nCols, lpiArray);
			hdi.mask = HDI_LPARAM;

			SelectObject(hdc, GetStockObject(DC_PEN));
			SetDCPenColor(hdc, GetSysColor(COLOR_BTNFACE));

			if (dps->iSel + dps->NumSel > 0) {
				RECT sr = {COLUMN_X_OFFSET / 2, dps->cyRow * dps->iSel, dps->max_right, dps->cyRow * dps->iSel + dps->cyRow*dps->NumSel};
				OffsetRect(&sr, 0, dps->cyHeader);
				DrawItemSelection(hdc, &sr, hwnd == GetFocus(), FALSE, 220);
			}

			if (dps->iHot != -1) {
				RECT sr = {COLUMN_X_OFFSET / 2, dps->cyRow * dps->iHot, dps->max_right, dps->cyRow * dps->iHot + dps->cyRow};
				OffsetRect(&sr, 0, dps->cyHeader);
				DrawItemSelection(hdc, &sr, TRUE, FALSE, 130);
			}

			SetBkMode(hdc, TRANSPARENT);

			// Mark selection index as not on screen
			dps->iPC = -1;

			CopyRect(&tr, &rc);
			tr.bottom = dps->cyRow;
			
			gRect.UpperLeft  = 0;
			gRect.LowerRight = 1;

			i = max((ur.top - dps->cyHeader) / (int) dps->cyRow, 0);
			OffsetRect(&tr, 0, dps->cyHeader + (i * dps->cyRow));

			end_i = ((int) (ur.bottom - dps->cyHeader + dps->cyRow - 1 - 1)) / (int) dps->cyRow;

			memory_context_t *calc_mem = &dps->lpCalc->mem_c;
			for (; i < end_i; i++, OffsetRect(&tr, 0, dps->cyRow)) {
				BOOL do_gradient = FALSE;

				int pc_i;
				for (pc_i = 0; pc_i < PC_TRAILS && !do_gradient; pc_i++) {
					waddr_t pc_waddr = addr16_to_waddr(calc_mem, (uint16_t) dps->nPCs[pc_i]);
					if ((pc_waddr.addr % PAGE_SIZE == dps->zinf[i].waddr.addr % PAGE_SIZE) && pc_waddr.page == dps->zinf[i].waddr.page
						&& pc_waddr.is_ram == dps->zinf[i].waddr.is_ram && (dps->zinf[i].index != DA_LABEL)) 
					{
						///dps->iPC = i;
						vert[0].Red = 0xFF00;
						vert[0].Green = 0xFF00;
						vert[0].Blue = 0xFF00;

						if (dps->lpCalc->cpu.halt && pc_i == 0) {
							vert[1].Red = COLOR_HALT_R << 8;
							vert[1].Green = COLOR_HALT_G << 8;
							vert[1].Blue = COLOR_HALT_B << 8;
						} else {
							vert[1].Red = (COLOR16)((COLOR_PC_R + (((255 - COLOR_PC_R) / (PC_TRAILS))*(pc_i))) << 8);
							vert[1].Green = (COLOR16)((COLOR_PC_G + (((255 - COLOR_PC_G) / (PC_TRAILS))*(pc_i))) << 8);
							vert[1].Blue = (COLOR16)((COLOR_PC_B + (((255 - COLOR_PC_B) / (PC_TRAILS))*(pc_i))) << 8);
						}
						do_gradient = TRUE;
					} else {
						vert[1].Red = 0xFF00;
						vert[1].Green = 0xFF00;
						vert[1].Blue = 0xFF00;
					}
				}
				BOOL breakpoint = FALSE;
				if (check_break(calc_mem, dps->zinf[i].waddr)) {
					vert [0] .Red    = COLOR_BREAKPOINT_R << 8;
					vert [0] .Green  = COLOR_BREAKPOINT_G << 8;
					vert [0] .Blue   = COLOR_BREAKPOINT_B << 8;
					breakpoint = TRUE;
					do_gradient = TRUE;
				}
				if (check_mem_write_break(calc_mem, dps->zinf[i].waddr)) {
					if (breakpoint) {
						vert [0] .Red    |= COLOR_MEMPOINT_WRITE_R << 8;
						vert [0] .Green  |= COLOR_MEMPOINT_WRITE_G << 8;
						vert [0] .Blue   = COLOR_MEMPOINT_WRITE_B << 8;
					} else {
						vert [0] .Red    = COLOR_MEMPOINT_WRITE_R << 8;
						vert [0] .Green  = COLOR_MEMPOINT_WRITE_G << 8;
						vert [0] .Blue   = COLOR_MEMPOINT_WRITE_B << 8;
					}
					breakpoint = TRUE;
					do_gradient = TRUE;
				}
				if (check_mem_read_break(calc_mem, dps->zinf[i].waddr)) {
					if (breakpoint) {
						vert [0] .Red    |= COLOR_MEMPOINT_READ_R << 8;
						vert [0] .Green  |= COLOR_MEMPOINT_READ_G << 8;
						vert [0] .Blue   = COLOR_MEMPOINT_READ_B << 8;
					} else {
						vert [0] .Red    = COLOR_MEMPOINT_READ_R << 8;
						vert [0] .Green  = COLOR_MEMPOINT_READ_G << 8;
						vert [0] .Blue   = COLOR_MEMPOINT_READ_B << 8;
					}
					do_gradient = TRUE;
				}


				if (do_gradient) {
					int width = dps->max_right;
					vert[0].x      = tr.left;
					vert[0].y      = tr.top;
					vert[1].x      = width;
					vert[1].y      = tr.bottom;
					GradientFill(hdc,vert,2,&gRect,1,GRADIENT_FILL_RECT_H);
				}

				// Draw the columns
				for (iItem = 0; iItem < nCols; iItem++, tr.left = tr.right) {
					int iCol;
					Header_GetItem(dps->hwndHeader, Header_OrderToIndex(dps->hwndHeader, iItem), &hdi);
					iCol = (int) hdi.lParam;
					if (iCol != -1) {
						tr.right = tr.left + dps->hdrs[iCol].cx;
						SelectObject(hdc, dps->hdrs[iCol].hfont);
						dps->hdrs[iCol].lpfnCallback(dps->lpCalc, hdc, &dps->zinf[i], dps->type, &tr);
					}
				}

				dps->max_right = tr.right;
				tr.left = 0;
				tr.right = rc.right;
				
				if (dps->zinf[i].waddr.page * PAGE_SIZE + (dps->zinf[i].waddr.addr % PAGE_SIZE) == dps->nKey && hwnd == GetFocus()) {
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
			if (IsWindowEnabled(hwnd)) {
				BitBlt(hdcDest, 0, dps->cyHeader, rc.right, rc.bottom, hdc, 0, dps->cyHeader, SRCCOPY);
			} else {
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
					TCHAR *disassembly = (TCHAR *) LocalAlloc(LMEM_FIXED, DISASM_LINE_MAX_LEN);
					ZeroMemory(disassembly, 2048);
					TCHAR copy_line[1024] = {0};
					TCHAR buf[1024] = {0};
					for (i = 0; i < dps->NumSel; i++) {
						zinf_line = &dps->zinf[dps->iSel + i];
						//print the address
						StringCbPrintf(copy_line, sizeof(copy_line), _T("%04X"), zinf_line->waddr.addr);
						StringCbCat(copy_line, sizeof(copy_line), _T(":"));
						//print the data
						for (j = 0; j < zinf_line->size; j++) {
							StringCbPrintf(buf + (j*2), sizeof(buf), _T("%02x"), 
								mem_read(dps->lpCalc->cpu.mem_c, (unsigned short) (zinf_line->waddr.addr+j)));
						}
						StringCbCat(copy_line, sizeof(copy_line), buf);
						StringCbCat(copy_line, sizeof(copy_line), _T(":\t"));
						if (zinf_line->size <= 2) {
							StringCbCat(copy_line, sizeof(copy_line), _T("\t"));
						}
						//print the command
						TCHAR commandString[255];
						mysprintf(dps->lpCalc, commandString, sizeof(commandString), zinf_line, REGULAR,
							da_opcode[zinf_line->index].format, zinf_line->a1, zinf_line->a2, zinf_line->a3, zinf_line->a4);
						StringCbCat(copy_line, sizeof(copy_line), commandString);
						StringCbCat(copy_line, sizeof(copy_line), _T("\t\t"));
						size_t lineLength;
						StringCbLength(copy_line, sizeof(copy_line), &lineLength);
						if (lineLength <= 11) {
							StringCbCat(copy_line, sizeof(copy_line), _T("\t"));
						}

						if (lineLength <= 7) {
							StringCbCat(copy_line, sizeof(copy_line), _T("\t"));
						}

						StringCbCat(copy_line, sizeof(copy_line), _T(";"));
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
						StringCbCat(disassembly, DISASM_LINE_MAX_LEN, copy_line);
					}

					BOOL success = OpenClipboard(hwnd);
					if (!success) {
						MessageBox(hwnd, _T("Unable to open clipboard"), _T("Error"), MB_OK);
						break;
					}

					success = EmptyClipboard();
					if (!success) {
						MessageBox(hwnd, _T("Unable to empty clipboard"), _T("Error"), MB_OK);
						break;
					}

#ifdef UNICODE
					HANDLE hData = SetClipboardData(CF_UNICODETEXT, (HANDLE)disassembly);
#else
					HANDLE hData = SetClipboardData(CF_TEXT, (HANDLE)disassembly);
#endif
					if (hData == NULL) {
						MessageBox(hwnd, _T("Unable to set clipboard data"), _T("Error"), MB_OK);
						break;
					}

					success = CloseClipboard();
					if (!success) {
						MessageBox(hwnd, _T("Unable to open clipboard"), _T("Error"), MB_OK);
						break;
					}
					break;
				}
				case DB_DISASM: {
					u_int addr = (u_int) lParam;
					waddr_t waddr;
					if (dps->type == REGULAR) {
						waddr = addr16_to_waddr(dps->lpCalc->cpu.mem_c, (uint16_t) addr);
					} else {
						waddr = addr32_to_waddr(addr, dps->type == RAM);
					}
					disassemble(dps->lpCalc, dps->type, waddr, dps->nRows, lpTabInfo->lpDebugInfo->bTIOSDebug, dps->zinf);
					break;
				}
				case IDM_RUN_STEP:
				case DB_STEP: {
					CPU_step(&dps->lpCalc->cpu);
					db_step_finish(hwnd, dps);
					break;
				}
				case IDM_RUN_STEPOVER:
				case DB_STEPOVER: {
					CPU_stepover(dps->lpCalc, lpTabInfo->lpDebugInfo->bTIOSDebug);
					db_step_finish(hwnd, dps);
					break;
				}
				case IDM_RUN_STEPOUT: {
					CPU_stepout(dps->lpCalc, lpTabInfo->lpDebugInfo->bTIOSDebug);
					db_step_finish(hwnd, dps);
					break;
				}
#ifdef WITH_REVERSE
				case DB_STEPBACK: {
					LPTABWINDOWINFO lpTabInfo = (LPTABWINDOWINFO) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					dp_settings *dps = (dp_settings *) lpTabInfo->tabInfo;
					CPU_step_reverse(&dps->lpCalc->cpu);
					reverse_cycle_pcs(dps);
					db_step_finish(hwnd, dps, TRUE);
					break;
				}
#endif
				case DB_CYCLEPCS: {
					cycle_pcs(dps);
					break;
				}
				case DB_DISASM_GOTO:
				case DB_GOTO: {
					if (dps->hGotoDialog != NULL) {
						SetFocus(dps->hGotoDialog);
					} else {
						dps->hGotoDialog = CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_DLGGOTO), hwnd,
							(DLGPROC)GotoDialogProc, (LPARAM)lpTabInfo->lpDebugInfo);
						position_goto_dialog(dps->hGotoDialog);
					}
					return 0;
				}
				case DB_FIND_NEXT: {
					int global_addr = (int)lParam;
					dps->nPane = global_addr;
					dps->nSel = global_addr;
					SendMessage(hwnd, WM_COMMAND, DB_DISASM, global_addr);
					InvalidateRect(hwnd, NULL, FALSE);
					UpdateWindow(hwnd);
					return 0;
				}
				case DB_OPEN_FIND: {
					if (dps->hFindDialog) {
						SetFocus(dps->hFindDialog);
						break;
					}

					find_dialog_params_t params = { 0 };
					params.hwndParent = hwnd;
					params.lpCalc = dps->lpCalc;
					params.start_addr = dps->nSel;
					params.type = dps->type;
					dps->hFindDialog = CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_DLGFIND), hwnd, (DLGPROC)FindDialogProc, (LPARAM)&params);
					ShowWindow(dps->hFindDialog, SW_SHOW);
					break;
				}
				case DB_FIND_CLOSE: {
					dps->hFindDialog = NULL;
					break;
				}
				case DB_BREAKPOINT: {
					waddr_t waddr;
					if (dps->type == REGULAR) {
						waddr = addr16_to_waddr(&dps->lpCalc->mem_c, (uint16_t)dps->nSel);
					} else {
						waddr = addr32_to_waddr(dps->nSel, dps->type == RAM);
					}

					if (check_break(&dps->lpCalc->mem_c, waddr)) {
						clear_break(&dps->lpCalc->mem_c, waddr);
					} else {
						set_break(&dps->lpCalc->mem_c, waddr);
					}
					InvalidateSel(hwnd, dps->iSel);
					Debug_UpdateWindow(GetParent(hwnd));
					break;
				}
				case DB_MEMPOINT_WRITE: {
					waddr_t waddr;
					if (dps->type == REGULAR) {
						waddr = addr16_to_waddr(&dps->lpCalc->mem_c, (uint16_t)dps->nSel);
					} else {
						waddr = addr32_to_waddr(dps->nSel, dps->type == RAM);
					}

					if (check_mem_write_break(&dps->lpCalc->mem_c, waddr)) {
						clear_mem_write_break(&dps->lpCalc->mem_c, waddr);
					} else {
						set_mem_write_break(&dps->lpCalc->mem_c, waddr);
					}
					InvalidateSel(hwnd, dps->iSel);
					Debug_UpdateWindow(GetParent(hwnd));
					break;
				}
				case DB_MEMPOINT_READ: {
				
					waddr_t waddr;
					if (dps->type == REGULAR) {
						waddr = addr16_to_waddr(&dps->lpCalc->mem_c, (uint16_t) dps->nSel);
					} else {
						waddr = addr32_to_waddr(dps->nSel, dps->type == RAM);
					}

					if (check_mem_read_break(&dps->lpCalc->mem_c, waddr)) {
						clear_mem_read_break(&dps->lpCalc->mem_c, waddr);
					} else {
						set_mem_read_break(&dps->lpCalc->mem_c, waddr);
					}
					InvalidateSel(hwnd, dps->iSel);
					Debug_UpdateWindow(GetParent(hwnd));
					break;
				}
				case IDM_DISASMDATA:
				case IDM_DISASMDISASM:
				case IDM_DISASMSIZE:
				case IDM_DISASMCLOCKS: {
					disasmhdr_toggle(dps->hwndHeader, &dps->hdrs[LOWORD(wParam) - IDM_DISASMADDR]);

					Debug_UpdateWindow(hwnd);
					break;
				}
				case DB_SET_PC: {
					if (dps->type == REGULAR) {
						dps->lpCalc->cpu.pc = dps->zinf[dps->iSel].waddr.addr;
					} else {
						waddr_t waddr = dps->zinf[dps->iSel].waddr;
						u_int i;
						for (i = 0; i < 4; i++) {
							bank_t bank = dps->lpCalc->mem_c.banks[i];
							if (bank.page == waddr.page && waddr.is_ram == bank.ram) {
								dps->lpCalc->cpu.pc = (uint16_t)(mc_base(waddr.addr) + i * PAGE_SIZE);
								break;
							}
						}

						if (i == 4) {
							TCHAR *pageType = waddr.is_ram ? _T("RAM") : _T("Flash");
							TCHAR msg[256];
							int bank = waddr.is_ram ? 2 : 1;
							StringCbPrintf(msg, sizeof(msg), _T("Swap %s page %02X into bank %d?"), pageType, waddr.page, bank);
							if (MessageBox(hwnd, msg, _T("Swap page?"), MB_YESNO | MB_ICONQUESTION) == IDYES) {
								change_page(&dps->lpCalc->mem_c, bank, waddr.page, waddr.is_ram);
								dps->lpCalc->cpu.pc = (uint16_t)(mc_base(waddr.addr) + bank * PAGE_SIZE);
							}
						}
					}
					
					cycle_pcs(dps);
					Debug_UpdateWindow(hwnd);
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
			if (y < 0) {
				return 0;
			}


			GetClientRect(hwnd, &r);
			InvalidateSel(hwnd, dps->iSel);
			r.top = y -  (y % dps->cyRow); r.bottom = r.top + dps->cyRow;
			dps->iSel = y/dps->cyRow;
			if (dps->type == REGULAR) {
				dps->nSel = dps->zinf[y/dps->cyRow].waddr.addr;
			} else {
				dps->nSel = dps->zinf[y/dps->cyRow].waddr.addr % PAGE_SIZE + dps->zinf[y/dps->cyRow].waddr.page * PAGE_SIZE;
			}
			dps->nKey = dps->nSel;
			dps->nClick = dps->zinf[y/dps->cyRow].waddr.addr;
			dps->NumSel = 1;
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
			y = GET_Y_LPARAM(lParam) - dps->cyHeader;

			ReleaseCapture();
			InvalidateRect(hwnd, NULL, FALSE);
			dps->IsDragging = FALSE;
			dps->toolInfo.lParam = FALSE;
			SendMessage(dps->hwndTip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &dps->toolInfo);
			return 0;
		}
		case WM_MOUSELEAVE: {
			InvalidateSel(hwnd, dps->iHot);
			dps->iHot = -1;
			return 0;
		}
		case WM_MOUSEMOVE: {
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

				if (dps->type == REGULAR) {
					dps->nSel = dps->zinf[r.top/dps->cyRow].waddr.addr;
				} else {
					dps->nSel = dps->zinf[r.top/dps->cyRow].waddr.addr % PAGE_SIZE + dps->zinf[r.top/dps->cyRow].waddr.page * PAGE_SIZE;
				}
				dps->iSel = r.top/dps->cyRow;
				dps->NumSel = r.bottom/dps->cyRow - dps->iSel + 1;

				InvalidateSel(hwnd, dps->iSel);

				//update tooltip text
				int total_bytes = 0, total = 0, total_cond = 0;
				TCHAR szTip[64], szOldTip[64];

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
					return 0;
				case VK_PRIOR:
					SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
					return 0;
				case VK_F3:
					SendMessage(hwnd, WM_COMMAND, DB_MEMPOINT_WRITE, 0);
					return 0;
				case 'G':
					SendMessage(hwnd, WM_COMMAND, DB_GOTO, 0);
					return 0;
				case 'F':
					SendMessage(hwnd, WM_COMMAND, DB_OPEN_FIND, 0);
					break;
				default:
					return 0;
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
			if (zDelta > 0) {
				sbtype = SB_LINEUP;
			} else {
				sbtype = SB_LINEDOWN;
				}


			for (i = 0; i < abs(zDelta * 2); i += WHEEL_DELTA)
				SendMessage(hwnd, WM_VSCROLL, sbtype, 0);

			return 0;
		}
		case WM_VSCROLL: {
			SCROLLINFO si;
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_RANGE | SIF_TRACKPOS;
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

					waddr_t waddr;
					// Disasm 6 commands such that the 6th is equal to the first
					// visible command
					do {
						dps->nPane--;
						if (dps->type == REGULAR) {
							waddr = addr16_to_waddr(dps->lpCalc->cpu.mem_c, (uint16_t)dps->nPane);
						} else {
							waddr = addr32_to_waddr(dps->nPane, dps->type == RAM);
						}
						disassemble(dps->lpCalc, dps->type, waddr, LINEUP_DEPTH, lpTabInfo->lpDebugInfo->bTIOSDebug, zup);
					} while (zup[LINEUP_DEPTH - 2].waddr.addr > dps->zinf[0].waddr.addr && dps->nPane);


					for (i = 0; i < LINEUP_DEPTH && zup[i].waddr.addr != dps->zinf[0].waddr.addr; i++);
					if (dps->nPane == 0) {
						if (i == 0) return 0;
						if (dps->type == REGULAR) {
							dps->nPane = zup[i - 1].waddr.addr;
						} else {
							dps->nPane = zup[i - 1].waddr.page * PAGE_SIZE + zup[i - 1].waddr.addr % PAGE_SIZE;
						}
					} else {
						if (i == LINEUP_DEPTH || i == 0) {
							dps->nPane = nPane_old - 1;
						} else {
							if (dps->type == REGULAR) {
								dps->nPane = zup[i - 1].waddr.addr;
							} else {
								dps->nPane = zup[i - 1].waddr.page * PAGE_SIZE + zup[i - 1].waddr.addr % PAGE_SIZE;
							}
						}
					}
					dps->iSel++;
					break;
				}
				case SB_LINEDOWN: {
					if (dps->type == REGULAR) {
						if (dps->zinf[dps->nRows - 1].waddr.addr < dps->zinf[0].waddr.addr) {
							return 0;
						}
					} else {
						int page = dps->type == FLASH ? dps->lpCalc->mem_c.flash_pages : dps->lpCalc->mem_c.ram_pages;
						if (dps->zinf[dps->nRows - 1].waddr.page >= page) {
							return 0;
						}
					}

					if (dps->zinf[0].size) {
						dps->nPane += dps->zinf[0].size;
					} else {
						// label
						dps->nPane += dps->zinf[1].size;
					}
					dps->iSel--;
					break;
				}
				case SB_THUMBTRACK:
					dps->iSel += dps->nPane - si.nTrackPos;
					//we need a 32 bit int so we use the value grabbed during GetScrollInfo
					//I think its better to grab it always then make a separate call here
					dps->nPane = si.nTrackPos;
					break;
				case SB_PAGEDOWN: {
					dps->iSel -= dps->last_pagedown;
					int newPageDown;
					if (dps->type == REGULAR) {
						newPageDown = dps->zinf[dps->nRows - 2].waddr.addr + PAGE_SIZE - dps->nPane;
					} else {
						newPageDown = (dps->zinf[dps->nRows - 2].waddr.addr % PAGE_SIZE) +
										dps->zinf[dps->nRows - 2].waddr.page * PAGE_SIZE - dps->nPane;
					}
					int global_addr;
					if (dps->type == REGULAR) {
						global_addr = dps->zinf[dps->nRows - 1].waddr.addr;
					} else {
						global_addr = (dps->zinf[dps->nRows - 1].waddr.addr % PAGE_SIZE) +
										dps->zinf[dps->nRows - 1].waddr.page * PAGE_SIZE;
					}
					if (newPageDown < 0 || global_addr + newPageDown >= GetMaxAddr(dps)) {
						newPageDown -= dps->zinf[dps->nRows - 1].waddr.addr + newPageDown;
					} else {
						dps->last_pagedown = newPageDown;
					}
					dps->nPane += newPageDown;
					break;
				}
				case SB_PAGEUP:
					if ((int) dps->nPane - dps->last_pagedown >= 0) {
						dps->iSel +=dps->last_pagedown;
						dps->nPane -= dps->last_pagedown;
					} else {
						dps->iSel += dps->nPane;
						dps->nPane = 0;
					}
					break;
			}

			si.fMask = SIF_POS  | SIF_PAGE;
			si.nPage = dps->last_pagedown;
			si.nPos = dps->nPane;
			si.nTrackPos = dps->nPane;
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

			SendMessage(hwnd, WM_COMMAND, DB_DISASM, dps->nPane);
			Z80_info_t *zfirst = &dps->zinf[0]; 
			Z80_info_t *zlast = &dps->zinf[dps->nRows-1];

			dps->nPage = zlast->waddr.addr + zlast->size - zfirst->waddr.addr;
			InvalidateRect(hwnd, NULL, TRUE);
			return dps->nPane;
		}
		case WM_USER: {
			switch (wParam) {
				case DB_UPDATE:
					SendMessage(hwnd, WM_COMMAND, DB_DISASM, dps->nPane);
					InvalidateRect(hwnd, NULL, FALSE);
					UpdateWindow(hwnd);
					break;
				case DB_RESUME: {
					dps->nPCs[0] = dps->lpCalc->cpu.pc;
					EnableWindow(hwnd, TRUE);

					Debug_UpdateWindow(hwnd);
					break;
				}
				case DB_GOTO_RESULT: {
					dps->hGotoDialog = NULL;
					int goto_addr = (int) lParam;
					if (goto_addr == -1) {
						break;
					}
					switch (dps->type) {
					case REGULAR:
						goto_addr = goto_addr & 0xFFFF;
						break;
					case FLASH:
						goto_addr = ((goto_addr & 0xFFFF) % PAGE_SIZE) + ((goto_addr >> 16) * PAGE_SIZE);
						if (goto_addr > GetMaxAddr(dps))
							goto_addr = GetMaxAddr(dps) - 1;
						break;
					case RAM: {
						goto_addr = ((goto_addr & 0xFFFF) % PAGE_SIZE) + (((goto_addr >> 16) & 0x7) * PAGE_SIZE);
						if (goto_addr > GetMaxAddr(dps))
							goto_addr = GetMaxAddr(dps) - 1;
						break;
					}
					}

					Debug_GotoAddr(hwnd, goto_addr);
					break;
				}
				case DB_GOTO_ADDR: {
					int addr = (int)lParam;
					SCROLLINFO si;
					si.cbSize = sizeof(SCROLLINFO);
					si.fMask = SIF_POS;
					si.nPos = addr;
					SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

					SendMessage(hwnd, WM_VSCROLL, MAKEWPARAM(SB_THUMBTRACK, addr), 0);
					dps->nSel = addr;
					Debug_UpdateWindow(hwnd);
					break;
				}
			}
			return 0;
		}
		default: {
			return DefWindowProc(hwnd, Message, wParam, lParam);
		}
	}
	return 0;
}
