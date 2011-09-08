#include "stdafx.h"

#include "dbreg.h"
#include "ti_stdint.h"
#include "alu.h"
#include "guidebug.h"
#include "print.h"
#include "expandpane.h"
#include "dbvalue.h"
#include "dbdisasm.h"


extern HINSTANCE g_hInst;
extern HFONT hfontSegoe, hfontLucida, hfontLucidaBold;
extern HWND hdisasm;

#define DBREG_ORGX	12
#define DBREG_ORGY	0
static SCROLLINFO si;
int regPanesYScroll;

struct db_reg {
	unsigned int offset;
	char name[8];
};

static const struct db_reg reg_offset[] = {
	coff(af, "af"), coff(afp, "af'"),
	coff(bc, "bc"), coff(bcp, "bc'"),
	coff(de, "de"), coff(dep, "de'"),
	coff(hl, "hl"), coff(hlp, "hl'"),
	coff(ix, "ix"), coff(sp, "sp"),
	coff(iy, "iy"), coff(pc, "pc"),
	coff(i, "i"), coff(imode, "im"), coff(r, "r")};

static RECT val_locs[NumElm(reg_offset)];
static int kRegRow, kRegAddr;

void ValueDraw(HDC hdc, RECT *dr, int i) {
	TCHAR szRegVal[16];

	dr->right = dr->left + kRegAddr;
	StringCbPrintf(szRegVal, sizeof(szRegVal), _T("%s"), reg_offset[i].name);
	SelectObject(hdc, hfontLucida);
	SetTextColor(hdc, DBCOLOR_HILIGHT);
	DrawText(hdc, szRegVal, -1, dr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	SetTextColor(hdc, RGB(0,0,0));
	dr->left = dr->right;
	dr->right += kRegAddr;
	SelectObject(hdc, hfontLucida);

	if (i < REG16_ROWS * REG16_COLS) {
		StringCbPrintf(szRegVal, sizeof(szRegVal), _T("%04X"), reg16(reg_offset[i].offset));
		DrawText(hdc, szRegVal, -1, dr, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
		StringCbPrintf(szRegVal, sizeof(szRegVal), _T("%02X"), reg8(reg_offset[i].offset));
		DrawText(hdc, szRegVal, -1, dr, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}
	val_locs[i] = *dr;

}

static TEXTMETRIC tm;
static HWND hwndVal = NULL;
static int vi;

void HandleEditMessages(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	switch (HIWORD(wParam)) {
		case EN_CHANGE:
		case 256:
		case EN_UPDATE:
		case EN_MAXTEXT:
			break;

		case EN_KILLFOCUS:
			if (GetFocus() == hwnd) break;
		case EN_SUBMIT:

		default:
		if (vi != -1) {
			if (vi < REG16_ROWS*REG16_COLS) {
				ValueSubmit(hwndVal, ((TCHAR *) (&lpDebuggerCalc->cpu)) + reg_offset[vi].offset, 2);
			} else {
				ValueSubmit(hwndVal, ((TCHAR *) (&lpDebuggerCalc->cpu)) + reg_offset[vi].offset, 1);

			}
			Debug_UpdateWindow(GetParent(hwnd));
		}
		hwndVal = NULL;
		return;
	}
	return;
}

void ClearEditField(HWND hwnd) {

	if (hwndVal) {
		SendMessage(hwnd, WM_COMMAND, EN_SUBMIT<<16, (LPARAM) hwndVal);
	}

}

void CreateEditField(HWND hwnd, POINT p) {

	if (hwndVal) {
		SendMessage(hwnd, WM_COMMAND, EN_SUBMIT<<16, (LPARAM) hwndVal);
	}

	unsigned int i;
	for (i = 0; i < NumElm(reg_offset); i++) {
		if (PtInRect(&val_locs[i], p)) {
			vi = i;
			break;
		}
	}

	if (i == NumElm(reg_offset)) {
		vi = -1;
		return;
	}
	RECT r;
	GetClientRect(hwnd, &r);

	hwndVal = NULL;
	TCHAR rval[8];
	int edit_width = 4;
	if (vi < REG16_ROWS * REG16_COLS) {
		StringCbPrintf(rval, sizeof(rval), _T("%04X"), reg16(reg_offset[vi].offset));
		edit_width = 4;
	} else {
		StringCbPrintf(rval, sizeof(rval), _T("%02X"), reg8(reg_offset[vi].offset));
		edit_width = 2;
	}


	RECT rrc;
	GetWindowRect(hwnd, &rrc);

	hwndVal =
	CreateWindow(_T("EDIT"), rval,
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_MULTILINE,
		val_locs[vi].left-2,
		val_locs[vi].top,
		(edit_width*kRegAddr/4)+4,
		kRegRow,
		hwnd,
		0, g_hInst, NULL);

	SubclassEdit(hwndVal, edit_width, HEX4);
}

static BOOL CALLBACK EnumDeselectChildren(HWND hwndChild, LPARAM lParam) {
	SendMessage(hwndChild, WM_USER, VF_DESELECT, 0);
	return TRUE;
}

LRESULT CALLBACK DBRegProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {

	switch (Message) {
	case WM_CREATE:
	{
		struct {TCHAR *name; void *data; size_t size;} reg[] =
		{
				{_T("af"), &lpDebuggerCalc->cpu.af, 2}, {_T("af'"), &lpDebuggerCalc->cpu.afp, 2},
				{_T("bc"), &lpDebuggerCalc->cpu.bc, 2}, {_T("bc'"), &lpDebuggerCalc->cpu.bcp, 2},
				{_T("de"), &lpDebuggerCalc->cpu.de, 2}, {_T("de'"), &lpDebuggerCalc->cpu.dep, 2},
				{_T("hl"), &lpDebuggerCalc->cpu.hl, 2}, {_T("hl'"), &lpDebuggerCalc->cpu.hlp, 2},
				{_T("ix"), &lpDebuggerCalc->cpu.ix, 2}, {_T("sp"),  &lpDebuggerCalc->cpu.sp, 2},
				{_T("iy"), &lpDebuggerCalc->cpu.iy, 2}, {_T("pc"),  &lpDebuggerCalc->cpu.pc, 2},
		};

		// Create all of the value fields
		int i;
		for (i = 0; i < NumElm(reg); i++) {
			HWND hwndValue = CreateValueField(hwnd, reg[i].name, kRegAddr, reg[i].data, reg[i].size, 4, HEX4);
			SetWindowPos(hwndValue, NULL, (i % 2) * kRegAddr*3, kRegRow * (i / 2), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			SendMessage(hwndValue, WM_SIZE, 0, 0);
		}
		return 0;
	}
	case WM_COMMAND:
	{
		value_field_settings *vfs = (value_field_settings *) GetWindowLongPtr((HWND) lParam, GWLP_USERDATA);
		dp_settings *dps = (dp_settings *) GetWindowLongPtr(hdisasm, GWLP_USERDATA);
		if (!_tcscmp(vfs->szName, _T("pc")))
			cycle_pcs(dps);
		Debug_UpdateWindow(lpDebuggerCalc->hwndDebug);
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;

		hdc = BeginPaint(hwnd, &ps);

		RECT rc;
		GetClientRect(hwnd, &rc);

		FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_SIZE:
	{
		SetWindowPos(hwnd, NULL, 0, 0, kRegAddr * 6, kRegRow * 8, SWP_NOMOVE | SWP_NOZORDER);
		return 0;
	}
	case WM_USER:
		switch (wParam) {
		case VF_DESELECT_CHILDREN:
			EnumChildWindows(hwnd, EnumDeselectChildren, (LPARAM) hwnd);
			break;
		default:
			break;
		}
		return 0;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
}

LRESULT CALLBACK DBMemMapProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND chkRO[4];
	static HWND rdoType[8];

	switch (Message) {
	case WM_CREATE:
	{

		HWND hwndValue;
		int i;
		for (i = 0; i < 4; i++) {
			int row_y = kRegRow / 4 + kRegRow * (i + 1);

			hwndValue = CreateValueField(hwnd, _T(""), 0, &lpDebuggerCalc->cpu.mem_c->banks[i].page, 1, 3, HEX2, lpDebuggerCalc->mem_c.flash_pages - 1);
			SetWindowPos(hwndValue, NULL, kRegAddr * 4, row_y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			SendMessage(hwndValue, WM_SIZE, 0, 0);

			rdoType[2 * i] =
				CreateWindow(
					_T("BUTTON"),
					_T(""),
					WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
					kRegAddr*3/2+kRegAddr/8, row_y, kRegAddr/2, kRegRow,
					hwnd, (HMENU) (20+2*i), g_hInst, NULL);
			rdoType[2*i+1] =
				CreateWindow(
					_T("BUTTON"),
					_T(""),
					WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
					kRegAddr * 5 / 2 - kRegAddr / 8, row_y, kRegAddr / 2, kRegRow,
					hwnd, (HMENU) (20 + 2 * i + 1), g_hInst, NULL);
			// not quite the right thing, but close enough
			DWORD check_width = GetSystemMetrics(SM_CYMENUCHECK);
			chkRO[i] =
			CreateWindow(
				_T("BUTTON"),
				_T(""),
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				kRegAddr * 3 + ((kRegAddr - check_width) / 2), row_y, kRegAddr / 2, kRegRow,
				hwnd, (HMENU) (30 + i), g_hInst, NULL);
			SetWindowFont(chkRO[i], hfontSegoe, TRUE);
		}

		return 0;
	}
	case WM_COMMAND:
	{
		switch (HIWORD(wParam)) {
		case BN_CLICKED:
			if ((LOWORD(wParam) >= 20) && (LOWORD(wParam) < 28)) {
				Button_SetCheck(rdoType[((LOWORD(wParam) - 20) / 2) * 2 + 0], BST_UNCHECKED);
				Button_SetCheck(rdoType[((LOWORD(wParam) - 20) / 2) * 2 + 1], BST_UNCHECKED);
				bank_state_t *bank = &lpDebuggerCalc->mem_c.banks[(LOWORD(wParam) - 20) / 2];
				bank->ram = !(LOWORD(wParam) % 2);
			} else if (LOWORD(wParam) >= 30 && LOWORD(wParam) < 34) {
				lpDebuggerCalc->mem_c.banks[LOWORD(wParam) - 30].read_only = !lpDebuggerCalc->mem_c.banks[LOWORD(wParam) - 30].read_only;
			}
			break;
		}
		Debug_UpdateWindow(hwnd);
		Debug_UpdateWindow(lpDebuggerCalc->hwndDebug);
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;

		hdc = BeginPaint(hwnd, &ps);

		RECT rc;
		GetClientRect(hwnd, &rc);

		FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));

		rc.bottom = rc.top + kRegRow;
		rc.left = kRegAddr * 2;
		rc.right = kRegAddr * 3;
		SelectObject(hdc, hfontSegoe);
		SetRect(&rc, kRegAddr * 3 / 2 + kRegAddr / 8, 0, kRegAddr * 3 / 2 + kRegAddr / 8 + kRegAddr / 2, kRegRow);
		DrawTextA(hdc, "R", -1, &rc, DT_CENTER);

		SetRect(&rc, kRegAddr * 3 / 2 + kRegAddr / 8, 0, kRegAddr * 5 / 2 - kRegAddr / 8 + kRegAddr / 2, kRegRow);
		DrawTextA(hdc, "-", -1, &rc, DT_CENTER);

		SetRect(&rc, kRegAddr * 5 / 2 - kRegAddr / 8, 0, kRegAddr * 5 / 2 - kRegAddr / 8 + kRegAddr / 2, kRegRow);
		DrawTextA(hdc, "F", -1, &rc, DT_CENTER);

		SetRect(&rc, kRegAddr * 3, 0, kRegAddr * 4, kRegRow);
		DrawTextA(hdc, "RO", -1, &rc, DT_CENTER);

		OffsetRect(&rc, kRegAddr, 0);
		DrawTextA(hdc, "Page", -1, &rc, DT_CENTER);

		SetRect(&rc, 0, kRegRow / 4 + kRegRow, kRegAddr * 3 / 2, kRegRow / 4 + kRegRow * 2);
		int i;
		for (i = 0; i < 4; i++)
		{
			char bank[16];
			sprintf_s(bank, "Bank %d", i);
			DrawTextA(hdc, bank, -1, &rc, DT_LEFT);
			OffsetRect(&rc, 0, kRegRow);
		}

		SelectObject(hdc, GetStockObject(DC_PEN));
		SetDCPenColor(hdc, GetSysColor(COLOR_BTNFACE));

		GetClientRect(hwnd, &rc);
		MoveToEx(hdc, kRegAddr * 3 / 2, 0, NULL);
		LineTo(hdc,  kRegAddr * 3 / 2, rc.bottom - kRegRow * 2);
		MoveToEx(hdc, 3 * kRegAddr, 0, NULL);
		LineTo(hdc,  3 * kRegAddr, rc.bottom - kRegRow*2);
		MoveToEx(hdc, 4 * kRegAddr - kRegRow / 8, 0, NULL);
		LineTo(hdc,  4 * kRegAddr - kRegRow / 8, rc.bottom - kRegRow * 2);
		MoveToEx(hdc, 5 * kRegAddr, 0, NULL);
		LineTo(hdc,  5 * kRegAddr, rc.bottom - kRegRow * 2);
		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_SIZE:
	{
		SetWindowPos(hwnd, NULL, 0, 0, kRegAddr * 6, kRegRow / 4 + kRegRow * 2 + kRegRow * 5, SWP_NOMOVE | SWP_NOZORDER);
		return 0;
	}
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
	{
		return (LRESULT) GetStockObject(WHITE_BRUSH);
	}
	case WM_USER:
		switch (wParam) {
		case DB_UPDATE:
		{
			int i;
			for (i = 0; i < 4; i++)
			{
				bank_state_t *bank = &lpDebuggerCalc->mem_c.banks[i];
				Button_SetCheck(rdoType[2 * i + (bank->ram ? 0 : 1)], BST_CHECKED);
				Button_SetCheck(chkRO[i], bank->read_only ? BST_CHECKED : BST_UNCHECKED);
				bank->addr = bank->ram ? lpDebuggerCalc->mem_c.ram : lpDebuggerCalc->mem_c.flash;
				bank->addr += bank->page * PAGE_SIZE;
			}
			break;
		}
		case VF_DESELECT_CHILDREN:
			EnumChildWindows(hwnd, EnumDeselectChildren, (LPARAM) hwnd);
			break;
		default:
			break;
		}
		return 0;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
}

LRESULT CALLBACK DBCPUProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND chkHalt, editFreq;
	static double freq;

	switch (Message) {
	case WM_CREATE:
	{

		HWND hwndValue;
		freq = ((double) lpDebuggerCalc->cpu.timer_c->freq) / 1000000.0;
		hwndValue = CreateValueField(hwnd, _T("Freq."), kRegAddr*3/2, &freq, sizeof(double), 5, FLOAT2);
		SetWindowPos(hwndValue, NULL, 0, kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, _T("Bus"), kRegAddr*2 + kRegAddr/4, &lpDebuggerCalc->cpu.bus, 1, 2, HEX2);
		SetWindowPos(hwndValue, NULL, 0, kRegRow*2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		chkHalt =
		CreateWindow(
			_T("BUTTON"),
			_T("Halt"),
			WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
			0, 0, 3*kRegAddr, kRegRow,
			hwnd, (HMENU) 1, g_hInst, NULL);
		SetWindowFont(chkHalt, hfontSegoe, TRUE);

		return 0;
	}
	case WM_COMMAND:
	{
		switch (HIWORD(wParam)) {
			case BN_CLICKED:
				switch (LOWORD(wParam)) {
					case 1:
						lpDebuggerCalc->cpu.halt = !lpDebuggerCalc->cpu.halt;
						break;
				}
				Debug_UpdateWindow(lpDebuggerCalc->hwndDebug);
				break;
			case EN_CHANGE: {
				lpDebuggerCalc->cpu.timer_c->freq = (uint32_t)(freq * 1000000.0);
				break;
			}
		}
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;

		hdc = BeginPaint(hwnd, &ps);

		RECT rc;
		GetClientRect(hwnd, &rc);

		FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_SIZE:
	{
		SetWindowPos(hwnd, NULL, 0, 0, kRegAddr*6, kRegRow * 2 + kRegRow*3, SWP_NOMOVE | SWP_NOZORDER);
		return 0;
	}
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
	{
		return (LRESULT) GetStockObject(WHITE_BRUSH);
	}
	case WM_USER:
		switch (wParam) {
		case DB_UPDATE:
			Button_SetCheck(chkHalt, lpDebuggerCalc->cpu.halt ? BST_CHECKED : BST_UNCHECKED);
			break;
		case VF_DESELECT_CHILDREN:
			EnumChildWindows(hwnd, EnumDeselectChildren, (LPARAM) hwnd);
			break;
		default:
			break;
		}
		return 0;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
}


LRESULT CALLBACK DBInterruptProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND chkIff1, chkIff2;

	switch (Message) {
	case WM_CREATE:
	{
		chkIff1 =
		CreateWindow(
			_T("BUTTON"),
			_T("iff1"),
			WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
			0, 0, 2*kRegAddr, kRegRow,
			hwnd, (HMENU) 1, g_hInst, NULL);
		SetWindowFont(chkIff1, hfontSegoe, TRUE);

		chkIff2 =
		CreateWindowEx(
			WS_EX_TRANSPARENT,
			_T("BUTTON"),
			_T("iff2"),
			WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
			2*kRegAddr, 0, 2*kRegAddr, kRegRow,
			hwnd, (HMENU) 2, g_hInst, NULL);
		SetWindowFont(chkIff2, hfontSegoe, TRUE);

		HWND hwndValue = CreateValueField(hwnd, _T("IM"), kRegAddr, &lpDebuggerCalc->cpu.imode, 1, 2, HEX2);
		SetWindowPos(hwndValue, NULL, 0, kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, _T("i"), kRegAddr, &lpDebuggerCalc->cpu.i, 1, 2, HEX2);
		SetWindowPos(hwndValue, NULL, 2*kRegAddr, kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, _T("Mask"), kRegAddr, &lpDebuggerCalc->cpu.pio.stdint->intactive, 1, 2, HEX2);
		SetWindowPos(hwndValue, NULL, 0, 2*kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, _T("r"), kRegAddr, &lpDebuggerCalc->cpu.r, 1, 2, HEX2);
		SetWindowPos(hwndValue, NULL, 2*kRegAddr, 2*kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);
		return 0;
	}
	case WM_COMMAND:
	{
		switch (HIWORD(wParam)) {
			case BN_CLICKED:
				switch (LOWORD(wParam)) {
					case 1:
						lpDebuggerCalc->cpu.iff1 = !lpDebuggerCalc->cpu.iff1;
						break;
					case 2:
						lpDebuggerCalc->cpu.iff2 = !lpDebuggerCalc->cpu.iff2;
						break;
				}
				Debug_UpdateWindow(lpDebuggerCalc->hwndDebug);
		}
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;

		hdc = BeginPaint(hwnd, &ps);

		RECT rc;
		GetClientRect(hwnd, &rc);

		FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));

		TCHAR szRegVal[16];
		double ntimer;
		SetRect(&rc, 0, kRegRow*3, kRegAddr*3, kRegRow*4);

		ntimer = tc_elapsed(&lpDebuggerCalc->timer_c) - lpDebuggerCalc->cpu.pio.stdint->lastchk1;
		ntimer *= 1000;
		StringCbPrintf(szRegVal, sizeof(szRegVal), _T("%0.4lf ms"), ntimer);
		SelectObject(hdc, hfontSegoe);
		DrawText(hdc, _T("Next Timer1"), -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

		SelectObject(hdc, hfontLucida);
		OffsetRect(&rc, kRegAddr*3, 0);
		DrawText(hdc, szRegVal, -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

		SelectObject(hdc, hfontSegoe);
		OffsetRect(&rc, -kRegAddr*3, kRegRow);
		DrawText(hdc, _T("Timer1 dur."), -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

		ntimer = lpDebuggerCalc->cpu.pio.stdint->timermax1;
		ntimer *= 1000;
		StringCbPrintf(szRegVal, sizeof(szRegVal), _T("%0.4lf ms"), ntimer);

		SelectObject(hdc, hfontLucida);
		OffsetRect(&rc, kRegAddr*3, 0);
		DrawText(hdc, szRegVal, -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);


		SelectObject(hdc, hfontSegoe);
		OffsetRect(&rc, -kRegAddr*3, kRegRow*3/2);
		DrawText(hdc, _T("Next Timer2"), -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

		ntimer = tc_elapsed(&lpDebuggerCalc->timer_c) - lpDebuggerCalc->cpu.pio.stdint->lastchk2;
		ntimer *= 1000;
		StringCbPrintf(szRegVal, sizeof(szRegVal), _T("%0.4lf ms"), ntimer);

		SelectObject(hdc, hfontLucida);
		OffsetRect(&rc, kRegAddr*3, 0);
		DrawText(hdc, szRegVal, -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

		SelectObject(hdc, hfontSegoe);
		OffsetRect(&rc, -kRegAddr*3, kRegRow);
		DrawText(hdc, _T("Timer2 dur."), -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

		ntimer = lpDebuggerCalc->cpu.pio.stdint->timermax2;
		ntimer *= 1000;
		StringCbPrintf(szRegVal, sizeof(szRegVal), _T("%0.4lf ms"), ntimer);

		SelectObject(hdc, hfontLucida);
		OffsetRect(&rc, kRegAddr*3, 0);
		DrawText(hdc, szRegVal, -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_SIZE:
	{
		SetWindowPos(hwnd, NULL, 0, 0, kRegAddr*6, kRegRow*2 + kRegRow*6 + kRegRow/2 + kRegRow, SWP_NOMOVE | SWP_NOZORDER);
		return 0;
	}
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
	{
		return (LRESULT) GetStockObject(WHITE_BRUSH);
	}
	case WM_USER:
		switch (wParam) {
		case DB_UPDATE:
			Button_SetCheck(chkIff1, lpDebuggerCalc->cpu.iff1 ? BST_CHECKED : BST_UNCHECKED);
			Button_SetCheck(chkIff2, lpDebuggerCalc->cpu.iff2 ? BST_CHECKED : BST_UNCHECKED);

			InvalidateRect(hwnd, NULL, FALSE);
			break;
		case VF_DESELECT_CHILDREN:
			EnumChildWindows(hwnd, EnumDeselectChildren, (LPARAM) hwnd);
			break;
		default:
			break;
		}
		return 0;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
}



LRESULT CALLBACK DBLCDProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND chkOn, grpMode, rdoXinc, rdoYinc, rdoXdec, rdoYdec;

	switch (Message) {
	case WM_CREATE:
	{
		/*struct {char *name; void *data; size_t size;} reg[] =
		{
				{"X", &lpDebuggerCalc->cpu.pio.lcd->x, 1},
				{"Y", &lpDebuggerCalc->cpu.pio.lcd->y, 1},
				{"Z", &lpDebuggerCalc->cpu.pio.lcd->z, 1},
				{"Contrast", &lpDebuggerCalc->cpu.pio.lcd->contrast, 1},
				{"Mode", &lpDebuggerCalc->cpu.pio.lcd->mode, 1},
		};*/

		/*
		// Create all of the value fields
		int i;
		for (i = 0; i < NumElm(reg); i++) {
			HWND hwndValue = CreateValueField(hwnd, reg[i].name, reg[i].data, reg[i].size);
			SetWindowPos(hwndValue, NULL, 0, kRegRow * i, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			SendMessage(hwndValue, WM_SIZE, 0, 0);
		}
		*/

		HWND hwndValue;

		hwndValue = CreateValueField(hwnd, _T("X"), kRegAddr, &lpDebuggerCalc->cpu.pio.lcd->x, 4, 2, DEC3);
		SetWindowPos(hwndValue, NULL, 7*kRegAddr/2, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, _T("Y"), kRegAddr, &lpDebuggerCalc->cpu.pio.lcd->y, 4, 2, DEC3);
		SetWindowPos(hwndValue, NULL, 7*kRegAddr/2, kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, _T("Z"), kRegAddr, &lpDebuggerCalc->cpu.pio.lcd->z, 4, 2, DEC3);
		SetWindowPos(hwndValue, NULL, 7*kRegAddr/2, 2*kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, _T("Contrast"), kRegAddr*2, &lpDebuggerCalc->cpu.pio.lcd->contrast, 4, 2, DEC3);
		SetWindowPos(hwndValue, NULL, 0, 5*kRegRow/4, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		chkOn =
		CreateWindow(
			_T("BUTTON"),
			_T("Powered"),
			WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
			0, 0, 3*kRegAddr, kRegRow,
			hwnd, (HMENU) IDC_LCD_ON, g_hInst, NULL);
		SetWindowFont(chkOn, hfontSegoe, TRUE);

		grpMode =
		CreateWindow(
			_T("BUTTON"),
			_T("Cursor Mode"),
			WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
			0, 3*kRegRow, 5*kRegAddr, 7*kRegRow/2,
			hwnd, (HMENU) 34234, g_hInst, NULL);
		SetWindowFont(grpMode, hfontSegoe, TRUE);

		rdoXinc =
		CreateWindow(
			_T("BUTTON"),
			_T("X-Inc."),
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
			kRegAddr/2, 4*kRegRow, 2*kRegAddr, kRegRow,
			hwnd, (HMENU) 84354, g_hInst, NULL);
		SetWindowFont(rdoXinc, hfontSegoe, TRUE);

		rdoYinc =
		CreateWindow(
			_T("BUTTON"),
			_T("Y-Inc."),
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON ,
			2*kRegAddr + kRegAddr*2/3, 4*kRegRow, 2*kRegAddr, kRegRow,
			hwnd, (HMENU) 84355, g_hInst, NULL);
		SetWindowFont(rdoYinc, hfontSegoe, TRUE);

		rdoXdec =
		CreateWindow(
			_T("BUTTON"),
			_T("X-Dec."),
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON ,
			kRegAddr/2, 5*kRegRow, 2*kRegAddr, kRegRow,
			hwnd, (HMENU) 84356, g_hInst, NULL);
		SetWindowFont(rdoXdec, hfontSegoe, TRUE);

		rdoYdec =
		CreateWindow(
			_T("BUTTON"),
			_T("Y-Dec."),
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON ,
			2*kRegAddr + kRegAddr*2/3, 5*kRegRow, 2*kRegAddr, kRegRow,
			hwnd, (HMENU) 84357, g_hInst, NULL);
		SetWindowFont(rdoYdec, hfontSegoe, TRUE);

		return 0;
	}
	case WM_COMMAND:
	{
		switch (HIWORD(wParam)) {
			case BN_CLICKED:
				if ((HWND) lParam == chkOn)
					lpDebuggerCalc->cpu.pio.lcd->active = !lpDebuggerCalc->cpu.pio.lcd->active;
				else if ((HWND) lParam == rdoXinc)
					lpDebuggerCalc->cpu.pio.lcd->cursor_mode = X_DOWN;
				else if ((HWND) lParam == rdoXdec)
					lpDebuggerCalc->cpu.pio.lcd->cursor_mode = X_UP;
				else if ((HWND) lParam == rdoYdec)
					lpDebuggerCalc->cpu.pio.lcd->cursor_mode = Y_DOWN;
				else if ((HWND) lParam == rdoYinc)
					lpDebuggerCalc->cpu.pio.lcd->cursor_mode = Y_UP;
				Debug_UpdateWindow(lpDebuggerCalc->hwndDebug);
		}
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;

		hdc = BeginPaint(hwnd, &ps);

		RECT rc;
		GetClientRect(hwnd, &rc);

		FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_SIZE:
	{
		SetWindowPos(hwnd, NULL, 0, 0, kRegAddr*6, kRegRow * 9, SWP_NOMOVE | SWP_NOZORDER);
		return 0;
	}
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
	{
		return (LRESULT) GetStockObject(WHITE_BRUSH);
	}
	case WM_USER:
		switch (wParam) {
		case DB_UPDATE:
			Button_SetCheck(chkOn, lpDebuggerCalc->cpu.pio.lcd->active ? BST_CHECKED : BST_UNCHECKED);

			Button_SetCheck(rdoXdec, BST_UNCHECKED);
			Button_SetCheck(rdoXinc, BST_UNCHECKED);
			Button_SetCheck(rdoYinc, BST_UNCHECKED);
			Button_SetCheck(rdoYdec, BST_UNCHECKED);

			switch (lpDebuggerCalc->cpu.pio.lcd->cursor_mode)
			{
			case X_UP:		Button_SetCheck(rdoXdec, BST_CHECKED); break;
			case X_DOWN:	Button_SetCheck(rdoXinc, BST_CHECKED); break;
			case Y_UP:		Button_SetCheck(rdoYinc, BST_CHECKED); break;
			case Y_DOWN:	Button_SetCheck(rdoYdec, BST_CHECKED); break;
			default:
				break;
			}

			InvalidateRect(lpDebuggerCalc->hwndLCD, NULL, FALSE);
			break;
		case VF_DESELECT_CHILDREN:
			EnumChildWindows(hwnd, EnumDeselectChildren, (LPARAM) hwnd);
			break;
		default:
			break;
		}
		return 0;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
}

LRESULT CALLBACK DBFlagProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND chk_z, chk_c, chk_s, chk_pv, chk_hc, chk_n; /*,chk_iff1, chk_iff2, chk_halt;*/
	#define FLAGS_TOP 0

	switch (Message) {
		case WM_CREATE: {

			HFONT hfontFlags = hfontSegoe;

			chk_z =
			CreateWindow(
				_T("BUTTON"),
				_T("z"),
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				0, FLAGS_TOP, 3*kRegAddr/2, kRegRow,
				hwnd, (HMENU) REG_CHK_Z, g_hInst, NULL);
			SetWindowFont(chk_z, hfontFlags, TRUE);

			chk_c =
			CreateWindow(
				_T("BUTTON"),
				_T("c"),
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				2*kRegAddr, FLAGS_TOP, 3*kRegAddr/2, kRegRow,
				hwnd, (HMENU) REG_CHK_C, g_hInst, NULL);
			SetWindowFont(chk_c, hfontFlags, TRUE);

			chk_s =
			CreateWindow(
				_T("BUTTON"),
				_T("s"),
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				4*kRegAddr, FLAGS_TOP, 3*kRegAddr/2, kRegRow,
				hwnd, (HMENU) REG_CHK_S, g_hInst, NULL);
			SetWindowFont(chk_s, hfontFlags, TRUE);

			chk_pv =
			CreateWindow(
				_T("BUTTON"),
				_T("p/v"),
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				0, FLAGS_TOP + kRegRow, 3*kRegAddr/2, kRegRow,
				hwnd, (HMENU) REG_CHK_PV, g_hInst, NULL);
			SetWindowFont(chk_pv, hfontFlags, TRUE);

			chk_hc =
			CreateWindow(
				_T("BUTTON"),
				_T("hc"),
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				2*kRegAddr, FLAGS_TOP + kRegRow, 3*kRegAddr/2, kRegRow,
				hwnd, (HMENU) REG_CHK_HC, g_hInst, NULL);
			SetWindowFont(chk_hc, hfontFlags, TRUE);

			chk_n =
			CreateWindow(
				_T("BUTTON"),
				_T("n"),
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				4*kRegAddr, FLAGS_TOP + kRegRow, 3*kRegAddr/2, kRegRow,
				hwnd, (HMENU) REG_CHK_N, g_hInst, NULL);
			SetWindowFont(chk_n, hfontFlags, TRUE);

			Debug_UpdateWindow(hwnd);
			return 0;
		}
		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLORBTN:
		{
			return (LRESULT) GetStockObject(WHITE_BRUSH);
		}

		case WM_MOVE:
			return 0;

		case WM_COMMAND:
		{
			switch (HIWORD(wParam)) {
				case BN_CLICKED:
					switch (LOWORD(wParam)) {
						case REG_CHK_Z:
							lpDebuggerCalc->cpu.f ^= ZERO_MASK;
							break;
						case REG_CHK_C:
							lpDebuggerCalc->cpu.f ^= CARRY_MASK;
							break;
						case REG_CHK_S:
							lpDebuggerCalc->cpu.f ^= SIGN_MASK;
							break;
						case REG_CHK_PV:
							lpDebuggerCalc->cpu.f ^= PV_MASK;
							break;
						case REG_CHK_HC:
							lpDebuggerCalc->cpu.f ^= HC_MASK;
							break;
						case REG_CHK_N:
							lpDebuggerCalc->cpu.f ^= N_MASK;
							break;
					}
					Debug_UpdateWindow(lpDebuggerCalc->hwndDebug);
					return 0;
			}

			return 0;
		}
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);

			RECT rc;
			GetClientRect(hwnd, &rc);

			FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));

			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_SIZE: {
			SetWindowPos(hwnd, NULL, 0, 0, kRegAddr*6, kRegRow*4, SWP_NOMOVE | SWP_NOZORDER);
			return 0;
		}
		case WM_USER: {
			switch (wParam) {
			case DB_UPDATE: {
				Button_SetCheck(chk_z, (lpDebuggerCalc->cpu.f & ZERO_MASK) ? BST_CHECKED : BST_UNCHECKED);
				Button_SetCheck(chk_c, (lpDebuggerCalc->cpu.f & CARRY_MASK) ? BST_CHECKED : BST_UNCHECKED);
				Button_SetCheck(chk_s, (lpDebuggerCalc->cpu.f & SIGN_MASK) ? BST_CHECKED : BST_UNCHECKED);
				Button_SetCheck(chk_pv, (lpDebuggerCalc->cpu.f & PV_MASK) ? BST_CHECKED : BST_UNCHECKED);
				Button_SetCheck(chk_hc, (lpDebuggerCalc->cpu.f & HC_MASK) ? BST_CHECKED : BST_UNCHECKED);
				Button_SetCheck(chk_n, (lpDebuggerCalc->cpu.f & N_MASK) ? BST_CHECKED : BST_UNCHECKED);
				break;
			}
			}
			return 0;
		}
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
}


LRESULT CALLBACK RegProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	/*static HWND chk_z, chk_c, chk_s, chk_pv, chk_hc, chk_n,
								chk_iff1, chk_iff2, chk_halt;
	static HWND hwndExp;*/
	static BOOL FirstRun = TRUE;

	//static WB_IDropTarget *pDropTarget;

	switch (Message) {
		case WM_CREATE:
		{
			HDC hdc = GetDC(hwnd);
			HWND hwndExp;

			SelectObject(hdc, hfontLucida);
			GetTextMetrics(hdc, &tm);
			kRegRow = tm.tmHeight + tm.tmHeight/3;
			kRegAddr = tm.tmAveCharWidth*4;


			//RegisterExpandPaneDropWindow(hwnd, &pDropTarget);

			RECT r;
			GetClientRect(hwnd, &r);

#define REGISTERS_TOP 2*kRegRow
#define VECTORS_TOP (12*kRegRow+kRegRow/2)

			if (FirstRun == TRUE) {
				WNDCLASSEX wcx;
				ZeroMemory(&wcx, sizeof(wcx));

				wcx.cbSize = sizeof(wcx);
				wcx.style = CS_DBLCLKS;
				wcx.lpszClassName = _T("WabbitRegDisp");
				wcx.lpfnWndProc = DBRegProc;
				wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
				RegisterClassEx(&wcx);

				wcx.lpszClassName = _T("WabbitFlagDisp");
				wcx.lpfnWndProc = DBFlagProc;
				RegisterClassEx(&wcx);

				wcx.lpszClassName = _T("WabbitLCDDisp");
				wcx.lpfnWndProc = DBLCDProc;
				RegisterClassEx(&wcx);

				wcx.lpszClassName = _T("WabbitCPUDisp");
				wcx.lpfnWndProc = DBCPUProc;
				RegisterClassEx(&wcx);

				wcx.lpszClassName = _T("WabbitMemDisp");
				wcx.lpfnWndProc = DBMemMapProc;
				RegisterClassEx(&wcx);

				wcx.lpszClassName = _T("WabbitInterruptDisp");
				wcx.lpfnWndProc = DBInterruptProc;
				RegisterClassEx(&wcx);

				FirstRun = FALSE;
			}

			HWND hwndContent;


			hwndContent =
				CreateWindow(
						_T("WabbitRegDisp"),
						_T("Registers"),
						WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						0, 0, 1, 1,
						hwnd,
						(HMENU) 1, g_hInst, NULL);

			hwndExp = CreateExpandPane(hwnd, _T("Registers"), hwndContent);


			hwndContent =
				CreateWindow(
						_T("WabbitFlagDisp"),
						_T("Flags"),
						WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						0, 0, 1, 1,
						hwnd,
						(HMENU) 1, g_hInst, NULL);

			CreateExpandPane(hwnd, _T("Flags"), hwndContent);

			hwndContent =
				CreateWindow(
						_T("WabbitCPUDisp"),
						_T("CPU Status"),
						WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						0, 0, 1, 1,
						hwnd,
						(HMENU) 1, g_hInst, NULL);
			CreateExpandPane(hwnd, _T("CPU Status"), hwndContent);

			hwndContent =
				CreateWindow(
						_T("WabbitMemDisp"),
						_T("Memory Map"),
						WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						0, 0, 1, 1,
						hwnd,
						(HMENU) 1, g_hInst, NULL);
			CreateExpandPane(hwnd, _T("Memory Map"), hwndContent);

			hwndContent =
				CreateWindow(
						_T("WabbitInterruptDisp"),
						_T("Interrupts"),
						WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						0, 0, 1, 1,
						hwnd,
						(HMENU) 1, g_hInst, NULL);
			CreateExpandPane(hwnd, _T("Interrupts"), hwndContent);

			hwndContent =
				CreateWindow(
					_T("WabbitLCDDisp"),
					_T("Display"),
					WS_CHILD  | WS_CLIPSIBLINGS,
					0, 0, 1, 1,
					hwnd,
					(HMENU) 1, g_hInst, NULL);
			CreateExpandPane(hwnd, _T("Display"), hwndContent);

			RECT rc;
			GetClientRect(hwnd, &rc);

			return 0;
		}

		case WM_SIZE:
		{
			RECT rc;
			GetClientRect(hwnd, &rc);
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask  = SIF_ALL;
			GetScrollInfo (hwnd, SB_VERT, &si);
			ArrangeExpandPanes();
			int height = GetExpandPanesHeight() + 7;
			//positive diff we opened a pane, negative diff we closed
			int diff = height - si.nMax;

			si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
			si.nMin = 0;
			si.nMax = height;
			si.nPage = rc.bottom;
			regPanesYScroll = min(0, max(0, si.nPos + diff));
			if (diff < 0)
				si.nPos = max(0, si.nPos + diff);
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

			/*if (si.nPos != 0)
				ScrollWindow(hwnd, 0, -si.nPos, NULL, NULL);*/

			DrawExpandPanes();
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
			// Get all the vertical scroll bar information
			 si.cbSize = sizeof (si);
			 si.fMask  = SIF_ALL;
			 GetScrollInfo (hwnd, SB_VERT, &si);
			 // Save the position for comparison later on
			 int yPos = si.nPos;
			 switch (LOWORD (wParam))
			 {
				 // user clicked the HOME keyboard key
				 case SB_TOP:
					  si.nPos = si.nMin;
					  break;
				 // user clicked the END keyboard key
				 case SB_BOTTOM:
					  si.nPos = si.nMax;
					  break;
				 // user clicked the top arrow
				 case SB_LINEUP:
					  si.nPos -= 10;
					  break;
				 // user clicked the bottom arrow
				 case SB_LINEDOWN:
					  si.nPos += 10;
					  break;
				 // user clicked the scroll bar shaft above the scroll box
				 case SB_PAGEUP:
					  si.nPos -= si.nPage;
					  break;
				 // user clicked the scroll bar shaft below the scroll box
				 case SB_PAGEDOWN:
					  si.nPos += si.nPage;
					  break;
				 // user dragged the scroll box
				 case SB_THUMBTRACK:
					  si.nPos = si.nTrackPos;
					  break;
				 default:
					  break;
			 }
			 // Set the position and then retrieve it.  Due to adjustments
			 //   by Windows it may not be the same as the value set.
			 si.fMask = SIF_POS;
			 SetScrollInfo (hwnd, SB_VERT, &si, TRUE);
			 GetScrollInfo (hwnd, SB_VERT, &si);
			 // If the position has changed, scroll window and update it
			 regPanesYScroll = -si.nPos;
			 if (si.nPos != yPos)
				  ScrollWindow(hwnd, 0, yPos - si.nPos, NULL, NULL);
			 DrawExpandPanes();
			 UpdateWindow(hwnd);
			break;
		}
		case WM_COMMAND:
			switch (HIWORD(wParam)) {
				case EN_CHANGE:
				case 256:
				case EN_UPDATE:
				case EN_MAXTEXT:
					break;

				case EN_KILLFOCUS:
					if (GetFocus() == hwnd) break;
				case EN_SUBMIT:

				default:
				if (vi != -1) {
					if (vi < REG16_ROWS*REG16_COLS) {
						ValueSubmit(hwndVal,
							((TCHAR *) (&lpDebuggerCalc->cpu)) + reg_offset[vi].offset, 2);
					} else {
						ValueSubmit(hwndVal,
							((TCHAR *) (&lpDebuggerCalc->cpu)) + reg_offset[vi].offset, 1);

					}
					Debug_UpdateWindow(GetParent(hwnd));
				}
				hwndVal = NULL;
			}
			return 0;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdcDest;

			hdcDest = BeginPaint(hwnd, &ps);
			RECT r;
			GetClientRect(hwnd, &r);
			//InvalidateRect(hwnd, &r, true);
			FillRect(hdcDest, &r, GetStockBrush(WHITE_BRUSH));
			DrawEdge(hdcDest, &r,EDGE_ETCHED, BF_LEFT);
			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_USER:
			return 0;
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		{
			SetFocus(hwnd);
			return 0;
		}
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}
