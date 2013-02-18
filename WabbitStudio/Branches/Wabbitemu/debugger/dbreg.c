#include "stdafx.h"

#include "dbreg.h"
#include "ti_stdint.h"
#include "alu.h"
#include "guidebug.h"
#include "print.h"
#include "expandpane.h"
#include "dbvalue.h"
#include "dbdisasm.h"
#include "resource.h"

extern HINSTANCE g_hInst;

#define DBREG_ORGX	12
#define DBREG_ORGY	0

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

void ValueDraw(LPCALC lpCalc, LPDEBUGWINDOWINFO lpDebugInfo, HDC hdc, RECT *dr, int i) {
	TCHAR szRegVal[16];

	dr->right = dr->left + lpDebugInfo->kRegAddr;
	StringCbPrintf(szRegVal, sizeof(szRegVal), _T("%s"), reg_offset[i].name);
	SelectObject(hdc, lpDebugInfo->hfontLucida);
	SetTextColor(hdc, DBCOLOR_HILIGHT);
	DrawText(hdc, szRegVal, -1, dr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	SetTextColor(hdc, RGB(0,0,0));
	dr->left = dr->right;
	dr->right += lpDebugInfo->kRegAddr;
	SelectObject(hdc, lpDebugInfo->hfontLucida);

	if (i < REG16_ROWS * REG16_COLS) {
		StringCbPrintf(szRegVal, sizeof(szRegVal), _T("%04X"), reg16(reg_offset[i].offset));
		DrawText(hdc, szRegVal, -1, dr, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
		StringCbPrintf(szRegVal, sizeof(szRegVal), _T("%02X"), reg8(reg_offset[i].offset));
		DrawText(hdc, szRegVal, -1, dr, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}
	lpDebugInfo->val_locs[i] = *dr;

}

static TEXTMETRIC tm;
static HWND hwndVal = NULL;
static int vi;

//void HandleEditMessages(HWND hwnd, WPARAM wParam, LPARAM lParam) {
//	switch (HIWORD(wParam)) {
//		case EN_CHANGE:
//		case 256:
//		case EN_UPDATE:
//		case EN_MAXTEXT:
//			break;
//
//		case EN_KILLFOCUS:
//			if (GetFocus() == hwnd) break;
//		case EN_SUBMIT:
//
//		default:
//		if (vi != -1) {
//			if (vi < REG16_ROWS*REG16_COLS) {
//				ValueSubmit(hwndVal, ((TCHAR *) (&lpCalc->cpu)) + reg_offset[vi].offset, 2);
//			} else {
//				ValueSubmit(hwndVal, ((TCHAR *) (&lpCalc->cpu)) + reg_offset[vi].offset, 1);
//
//			}
//			Debug_UpdateWindow(GetParent(hwnd));
//		}
//		hwndVal = NULL;
//		return;
//	}
//	return;
//}

void ClearEditField(HWND hwnd) {

	if (hwndVal) {
		SendMessage(hwnd, WM_COMMAND, EN_SUBMIT<<16, (LPARAM) hwndVal);
	}

}

void CreateEditField(LPCALC lpCalc, HWND hwnd, LPDEBUGWINDOWINFO lpDebugInfo, POINT p) {

	if (hwndVal) {
		SendMessage(hwnd, WM_COMMAND, EN_SUBMIT<<16, (LPARAM) hwndVal);
	}

	unsigned int i;
	for (i = 0; i < NumElm(reg_offset); i++) {
		if (PtInRect(&lpDebugInfo->val_locs[i], p)) {
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
		lpDebugInfo->val_locs[vi].left - 2,
		lpDebugInfo->val_locs[vi].top,
		(edit_width * lpDebugInfo->kRegAddr / 4) + 4,
		lpDebugInfo->kRegRow,
		hwnd,
		0, g_hInst, NULL);

	SubclassEdit(hwndVal, lpDebugInfo->hfontLucida, edit_width, HEX4);
}

static BOOL CALLBACK EnumDeselectChildren(HWND hwndChild, LPARAM lParam) {
	SendMessage(hwndChild, WM_USER, VF_DESELECT, 0);
	return TRUE;
}

LRESULT CALLBACK DBRegProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static LPCALC lpCalc;
	static LPDEBUGWINDOWINFO lpDebugInfo;
	switch (Message) {
	case WM_CREATE:
	{
		lpCalc = (LPCALC) ((LPCREATESTRUCT) lParam)->lpCreateParams;
		struct {TCHAR *name; void *data; size_t size;} reg[] =
		{
				{_T("af"), &lpCalc->cpu.af, 2}, {_T("af'"), &lpCalc->cpu.afp, 2},
				{_T("bc"), &lpCalc->cpu.bc, 2}, {_T("bc'"), &lpCalc->cpu.bcp, 2},
				{_T("de"), &lpCalc->cpu.de, 2}, {_T("de'"), &lpCalc->cpu.dep, 2},
				{_T("hl"), &lpCalc->cpu.hl, 2}, {_T("hl'"), &lpCalc->cpu.hlp, 2},
				{_T("ix"), &lpCalc->cpu.ix, 2}, {_T("sp"),  &lpCalc->cpu.sp, 2},
				{_T("iy"), &lpCalc->cpu.iy, 2}, {_T("pc"),  &lpCalc->cpu.pc, 2},
		};

		lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);
		// Create all of the value fields
		int i;
		for (i = 0; i < NumElm(reg); i++) {
			HWND hwndValue = CreateValueField(hwnd, lpDebugInfo, reg[i].name, lpDebugInfo->kRegAddr, reg[i].data, reg[i].size, 4, HEX4);
			SetWindowPos(hwndValue, NULL, (i % 2) * lpDebugInfo->kRegAddr*3, lpDebugInfo->kRegRow * (i / 2), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			SendMessage(hwndValue, WM_SIZE, 0, 0);
		}
		return 0;
	}
	case WM_COMMAND:
	{
		value_field_settings *vfs = (value_field_settings *) GetWindowLongPtr((HWND) lParam, GWLP_USERDATA);
		if (!_tcscmp(vfs->szName, _T("pc")))
			SendMessage(GetParent(lpDebugInfo->hdisasm), WM_COMMAND, DB_CYCLEPCS, 0);
		Debug_UpdateWindow(lpCalc->hwndDebug);
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
		SetWindowPos(hwnd, NULL, 0, 0, lpDebugInfo->kRegAddr * 6, lpDebugInfo->kRegRow * 8, SWP_NOMOVE | SWP_NOZORDER);
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
	static LPCALC lpCalc;
	static LPDEBUGWINDOWINFO lpDebugInfo;
	static HWND hwndPageMode0[4], hwndPageMode1[4];

	switch (Message) {
	case WM_CREATE:
	{
		lpCalc = (LPCALC) ((LPCREATESTRUCT) lParam)->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) lpCalc);
		lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);
		int i;
		for (i = 0; i < 4; i++) {
			int row_y = lpDebugInfo->kRegRow / 4 + lpDebugInfo->kRegRow * (i + 1);
			lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);

			hwndPageMode0[i] = CreateValueField(hwnd, lpDebugInfo, _T(""), 0, &lpCalc->cpu.mem_c->normal_banks[i].page, 1, 3, HEX2, lpCalc->mem_c.flash_pages - 1);
			hwndPageMode1[i] = CreateValueField(hwnd, lpDebugInfo, _T(""), 0, &lpCalc->cpu.mem_c->bootmap_banks[i].page, 1, 3, HEX2, lpCalc->mem_c.flash_pages - 1);
			SetWindowPos(hwndPageMode0[i], NULL, lpDebugInfo->kRegAddr * 4, row_y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			SetWindowPos(hwndPageMode1[i], NULL, lpDebugInfo->kRegAddr * 4, row_y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			SendMessage(hwndPageMode0[i], WM_SIZE, 0, 0);
			SendMessage(hwndPageMode1[i], WM_SIZE, 0, 0);
			if (lpCalc->mem_c.boot_mapped) {
				ShowWindow(hwndPageMode0[i], FALSE);
				ShowWindow(hwndPageMode1[i], TRUE);
			} else {
				ShowWindow(hwndPageMode0[i], FALSE);
				ShowWindow(hwndPageMode1[i], TRUE);
			}

			rdoType[2 * i] =
				CreateWindow(
					_T("BUTTON"),
					_T(""),
					WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
					lpDebugInfo->kRegAddr*3/2+lpDebugInfo->kRegAddr/8, row_y, lpDebugInfo->kRegAddr/2, lpDebugInfo->kRegRow,
					hwnd, (HMENU) (20+2*i), g_hInst, NULL);
			rdoType[2*i+1] =
				CreateWindow(
					_T("BUTTON"),
					_T(""),
					WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
					lpDebugInfo->kRegAddr * 5 / 2 - lpDebugInfo->kRegAddr / 8, row_y, lpDebugInfo->kRegAddr / 2, lpDebugInfo->kRegRow,
					hwnd, (HMENU) (20 + 2 * i + 1), g_hInst, NULL);
			// not quite the right thing, but close enough
			DWORD check_width = GetSystemMetrics(SM_CYMENUCHECK);
			chkRO[i] =
			CreateWindow(
				_T("BUTTON"),
				_T(""),
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				lpDebugInfo->kRegAddr * 3 + ((lpDebugInfo->kRegAddr - check_width) / 2), row_y, lpDebugInfo->kRegAddr / 2, lpDebugInfo->kRegRow,
				hwnd, (HMENU) (30 + i), g_hInst, NULL);
			SetWindowFont(chkRO[i], lpDebugInfo->hfontSegoe, TRUE);
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
				bank_state_t *bank = &lpCalc->mem_c.banks[(LOWORD(wParam) - 20) / 2];
				bank->ram = !(LOWORD(wParam) % 2);
			} else if (LOWORD(wParam) >= 30 && LOWORD(wParam) < 34) {
				lpCalc->mem_c.banks[LOWORD(wParam) - 30].read_only = !lpCalc->mem_c.banks[LOWORD(wParam) - 30].read_only;
			}
			break;
		}
		Debug_UpdateWindow(hwnd);
		Debug_UpdateWindow(lpCalc->hwndDebug);
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

		rc.bottom = rc.top + lpDebugInfo->kRegRow;
		rc.left = lpDebugInfo->kRegAddr * 2;
		rc.right = lpDebugInfo->kRegAddr * 3;
		SelectObject(hdc, lpDebugInfo->hfontSegoe);
		SetRect(&rc, lpDebugInfo->kRegAddr * 3 / 2 + lpDebugInfo->kRegAddr / 8, 0, lpDebugInfo->kRegAddr * 3 / 2 + lpDebugInfo->kRegAddr / 8 + lpDebugInfo->kRegAddr / 2, lpDebugInfo->kRegRow);
		DrawTextA(hdc, "R", -1, &rc, DT_CENTER);

		SetRect(&rc, lpDebugInfo->kRegAddr * 3 / 2 + lpDebugInfo->kRegAddr / 8, 0, lpDebugInfo->kRegAddr * 5 / 2 - lpDebugInfo->kRegAddr / 8 + lpDebugInfo->kRegAddr / 2, lpDebugInfo->kRegRow);
		DrawTextA(hdc, "-", -1, &rc, DT_CENTER);

		SetRect(&rc, lpDebugInfo->kRegAddr * 5 / 2 - lpDebugInfo->kRegAddr / 8, 0, lpDebugInfo->kRegAddr * 5 / 2 - lpDebugInfo->kRegAddr / 8 + lpDebugInfo->kRegAddr / 2, lpDebugInfo->kRegRow);
		DrawTextA(hdc, "F", -1, &rc, DT_CENTER);

		SetRect(&rc, lpDebugInfo->kRegAddr * 3, 0, lpDebugInfo->kRegAddr * 4, lpDebugInfo->kRegRow);
		DrawTextA(hdc, "RO", -1, &rc, DT_CENTER);

		OffsetRect(&rc, lpDebugInfo->kRegAddr, 0);
		DrawTextA(hdc, "Page", -1, &rc, DT_CENTER);

		SetRect(&rc, 0, lpDebugInfo->kRegRow / 4 + lpDebugInfo->kRegRow, lpDebugInfo->kRegAddr * 3 / 2, lpDebugInfo->kRegRow / 4 + lpDebugInfo->kRegRow * 2);
		int i;
		for (i = 0; i < 4; i++)
		{
			char bank[16];
			sprintf_s(bank, "Bank %d", i);
			DrawTextA(hdc, bank, -1, &rc, DT_LEFT);
			OffsetRect(&rc, 0, lpDebugInfo->kRegRow);
		}

		SelectObject(hdc, GetStockObject(DC_PEN));
		SetDCPenColor(hdc, GetSysColor(COLOR_BTNFACE));

		GetClientRect(hwnd, &rc);
		MoveToEx(hdc, lpDebugInfo->kRegAddr * 3 / 2, 0, NULL);
		LineTo(hdc,  lpDebugInfo->kRegAddr * 3 / 2, rc.bottom - lpDebugInfo->kRegRow * 2);
		MoveToEx(hdc, 3 * lpDebugInfo->kRegAddr, 0, NULL);
		LineTo(hdc,  3 * lpDebugInfo->kRegAddr, rc.bottom - lpDebugInfo->kRegRow*2);
		MoveToEx(hdc, 4 * lpDebugInfo->kRegAddr - lpDebugInfo->kRegRow / 8, 0, NULL);
		LineTo(hdc,  4 * lpDebugInfo->kRegAddr - lpDebugInfo->kRegRow / 8, rc.bottom - lpDebugInfo->kRegRow * 2);
		MoveToEx(hdc, 5 * lpDebugInfo->kRegAddr, 0, NULL);
		LineTo(hdc,  5 * lpDebugInfo->kRegAddr, rc.bottom - lpDebugInfo->kRegRow * 2);
		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_SIZE:
	{
		SetWindowPos(hwnd, NULL, 0, 0, lpDebugInfo->kRegAddr * 6, lpDebugInfo->kRegRow / 4 + lpDebugInfo->kRegRow * 2 + lpDebugInfo->kRegRow * 5, SWP_NOMOVE | SWP_NOZORDER);
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
				bank_state_t *bank = &lpCalc->mem_c.banks[i];
				Button_SetCheck(rdoType[2 * i + (bank->ram ? 0 : 1)], BST_CHECKED);
				Button_SetCheck(rdoType[2 * i + (bank->ram ? 1 : 0)], BST_UNCHECKED);
				Button_SetCheck(chkRO[i], bank->read_only ? BST_CHECKED : BST_UNCHECKED);
				bank->addr = bank->ram ? lpCalc->mem_c.ram : lpCalc->mem_c.flash;
				bank->addr += bank->page * PAGE_SIZE;

				if (lpCalc->mem_c.boot_mapped) {
					ShowWindow(hwndPageMode0[i], FALSE);
					ShowWindow(hwndPageMode1[i], TRUE);
				} else {
					ShowWindow(hwndPageMode0[i], TRUE);
					ShowWindow(hwndPageMode1[i], FALSE);
				}
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
	static LPCALC lpCalc;
	static LPDEBUGWINDOWINFO lpDebugInfo;

	switch (Message) {
	case WM_CREATE:
	{
		lpCalc = (LPCALC) ((LPCREATESTRUCT) lParam)->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) lpCalc);
		lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);

		HWND hwndValue;
		freq = ((double) lpCalc->cpu.timer_c->freq) / 1000000.0;
		hwndValue = CreateValueField(hwnd, lpDebugInfo, _T("Freq."), lpDebugInfo->kRegAddr*3/2, &freq, sizeof(double), 5, FLOAT2);
		SetWindowPos(hwndValue, NULL, 0, lpDebugInfo->kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, lpDebugInfo, _T("Bus"), lpDebugInfo->kRegAddr*2 + lpDebugInfo->kRegAddr/4, &lpCalc->cpu.bus, 1, 2, HEX2);
		SetWindowPos(hwndValue, NULL, 0, lpDebugInfo->kRegRow*2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		chkHalt =
		CreateWindow(
			_T("BUTTON"),
			_T("Halt"),
			WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
			0, 0, 3*lpDebugInfo->kRegAddr, lpDebugInfo->kRegRow,
			hwnd, (HMENU) 1, g_hInst, NULL);
		SetWindowFont(chkHalt, lpDebugInfo->hfontSegoe, TRUE);

		return 0;
	}
	case WM_COMMAND:
	{
		lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		switch (HIWORD(wParam)) {
			case BN_CLICKED:
				switch (LOWORD(wParam)) {
					case 1:
						lpCalc->cpu.halt = !lpCalc->cpu.halt;
						break;
				}
				Debug_UpdateWindow(lpCalc->hwndDebug);
				break;
			case EN_CHANGE: {
				lpCalc->cpu.timer_c->freq = (uint32_t)(freq * 1000000.0);
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
		lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);
		SetWindowPos(hwnd, NULL, 0, 0, lpDebugInfo->kRegAddr*6, lpDebugInfo->kRegRow * 2 + lpDebugInfo->kRegRow*3, SWP_NOMOVE | SWP_NOZORDER);
		return 0;
	}
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
	{
		return (LRESULT) GetStockObject(WHITE_BRUSH);
	}
	case WM_USER: {
		lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		switch (wParam) {
		case DB_UPDATE:
			Button_SetCheck(chkHalt, lpCalc->cpu.halt ? BST_CHECKED : BST_UNCHECKED);
			break;
		case VF_DESELECT_CHILDREN:
			EnumChildWindows(hwnd, EnumDeselectChildren, (LPARAM) hwnd);
			break;
		default:
			break;
		}
		return 0;
	}
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
}

LRESULT CALLBACK DBKeyboardProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND chkOn;
	static char value[8];
	static LPCALC lpCalc;
	static LPDEBUGWINDOWINFO lpDebugInfo;

	switch (Message) {
	case WM_CREATE:
	{
		TCHAR buf[32];
		HWND hwndValue;
		lpCalc = (LPCALC) ((LPCREATESTRUCT) lParam)->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) lpCalc);
		lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);

		keypad_t *kp = lpCalc->cpu.pio.keypad;
		for (int i = 0; i < 7; i++) {
			value[i] = 0;
			for (int j = 0; j < 8; j++) {
				if (kp->keys[i][j]) {
					value[i] += 1 << j;
				}
			}
			StringCbPrintf(buf, sizeof(buf), _T("Group %02X"), 0xFF & ~(1 << i));
			hwndValue = CreateValueField(hwnd, lpDebugInfo, buf, lpDebugInfo->kRegAddr * 3, &value[i], sizeof(char), 8, BIN8);
			SetWindowPos(hwndValue, NULL, 0, lpDebugInfo->kRegRow * i, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			SendMessage(hwndValue, WM_SIZE, 0, 0);
		}

		chkOn =
		CreateWindow(
			_T("BUTTON"),
			_T("On Key Pressed"),
			WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
			0, lpDebugInfo->kRegRow * 7 + 3, lpDebugInfo->kRegAddr * 3, lpDebugInfo->kRegRow,
			hwnd, (HMENU) 1, g_hInst, NULL);
		SetWindowFont(chkOn, lpDebugInfo->hfontSegoe, TRUE);
		SendMessage(hwnd, WM_SIZE, 0, 0);

		return 0;
	}
	case WM_COMMAND:
	{
		lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		switch (HIWORD(wParam)) {
			case BN_CLICKED:
				switch (LOWORD(wParam)) {
					case 1:
						lpCalc->cpu.pio.keypad->on_pressed = !lpCalc->cpu.pio.keypad->on_pressed;
						break;
				}
				Debug_UpdateWindow(lpCalc->hwndDebug);
				break;
			case EN_CHANGE: {
				keypad_t *kp = lpCalc->cpu.pio.keypad;
				for (int i = 0; i < 7; i++) {
					char val = value[i];
					for (int j = 0; j < 8; j++, val >>= 1) {
						kp->keys[i][j] = val & 0x01;
					}
				}
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
		lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);
		SetWindowPos(hwnd, NULL, 0, 0, lpDebugInfo->kRegAddr * 6, lpDebugInfo->kRegRow * 9 + 10, SWP_NOMOVE | SWP_NOZORDER);
		return 0;
	}
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
	{
		return (LRESULT) GetStockObject(WHITE_BRUSH);
	}
	case WM_USER:
		lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		switch (wParam) {
			case DB_UPDATE: {
				keypad_t *kp = lpCalc->cpu.pio.keypad;
				Button_SetCheck(chkOn, kp->on_pressed);
				for (int i = 0; i < 7; i++) {
					value[i] = 0;
					for (int j = 0; j < 8; j++) {
						if (kp->keys[i][j]) {
							value[i] += 1 << j;
						}
					}
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


LRESULT CALLBACK DBInterruptProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND chkIff1, chkIff2;
	static LPCALC lpCalc;
	static LPDEBUGWINDOWINFO lpDebugInfo;

	switch (Message) {
	case WM_CREATE:
	{
		lpCalc = (LPCALC) ((LPCREATESTRUCT) lParam)->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) lpCalc);
		lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);

		chkIff1 =
		CreateWindow(
			_T("BUTTON"),
			_T("iff1"),
			WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
			0, 0, 2*lpDebugInfo->kRegAddr, lpDebugInfo->kRegRow,
			hwnd, (HMENU) 1, g_hInst, NULL);
		SetWindowFont(chkIff1, lpDebugInfo->hfontSegoe, TRUE);

		chkIff2 =
		CreateWindow(
			_T("BUTTON"),
			_T("iff2"),
			WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
			2*lpDebugInfo->kRegAddr, 0, 2*lpDebugInfo->kRegAddr, lpDebugInfo->kRegRow,
			hwnd, (HMENU) 2, g_hInst, NULL);
		SetWindowFont(chkIff2, lpDebugInfo->hfontSegoe, TRUE);

		HWND hwndValue = CreateValueField(hwnd, lpDebugInfo, _T("IM"), lpDebugInfo->kRegAddr, &lpCalc->cpu.imode, 1, 2, HEX2);
		SetWindowPos(hwndValue, NULL, 0, lpDebugInfo->kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, lpDebugInfo, _T("i"), lpDebugInfo->kRegAddr, &lpCalc->cpu.i, 1, 2, HEX2);
		SetWindowPos(hwndValue, NULL, 2*lpDebugInfo->kRegAddr, lpDebugInfo->kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, lpDebugInfo, _T("Mask"), lpDebugInfo->kRegAddr, &lpCalc->cpu.pio.stdint->intactive, 1, 2, HEX2);
		SetWindowPos(hwndValue, NULL, 0, 2*lpDebugInfo->kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, lpDebugInfo, _T("r"), lpDebugInfo->kRegAddr, &lpCalc->cpu.r, 1, 2, HEX2);
		SetWindowPos(hwndValue, NULL, 2*lpDebugInfo->kRegAddr, 2*lpDebugInfo->kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);
		return 0;
	}
	case WM_COMMAND:
	{
		lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		switch (HIWORD(wParam)) {
			case BN_CLICKED:
				switch (LOWORD(wParam)) {
					case 1:
						lpCalc->cpu.iff1 = !lpCalc->cpu.iff1;
						break;
					case 2:
						lpCalc->cpu.iff2 = !lpCalc->cpu.iff2;
						break;
				}
				Debug_UpdateWindow(lpCalc->hwndDebug);
		}
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;

		hdc = BeginPaint(hwnd, &ps);

		lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);

		RECT rc;
		GetClientRect(hwnd, &rc);

		FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));

		TCHAR szRegVal[16];
		double ntimer;
		SetRect(&rc, 0, lpDebugInfo->kRegRow*3, lpDebugInfo->kRegAddr*3, lpDebugInfo->kRegRow*4);

		ntimer = tc_elapsed(&lpCalc->timer_c) - lpCalc->cpu.pio.stdint->lastchk1;
		ntimer *= 1000;
		StringCbPrintf(szRegVal, sizeof(szRegVal), _T("%0.4lf ms"), ntimer);
		SelectObject(hdc, lpDebugInfo->hfontSegoe);
		DrawText(hdc, _T("Next Timer1"), -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

		SelectObject(hdc, lpDebugInfo->hfontLucida);
		OffsetRect(&rc, lpDebugInfo->kRegAddr*3, 0);
		DrawText(hdc, szRegVal, -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

		SelectObject(hdc, lpDebugInfo->hfontSegoe);
		OffsetRect(&rc, -lpDebugInfo->kRegAddr*3, lpDebugInfo->kRegRow);
		DrawText(hdc, _T("Timer1 dur."), -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

		ntimer = lpCalc->cpu.pio.stdint->timermax1;
		ntimer *= 1000;
		StringCbPrintf(szRegVal, sizeof(szRegVal), _T("%0.4lf ms"), ntimer);

		SelectObject(hdc, lpDebugInfo->hfontLucida);
		OffsetRect(&rc, lpDebugInfo->kRegAddr*3, 0);
		DrawText(hdc, szRegVal, -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

		if (lpCalc->cpu.pio.model != TI_85 && lpCalc->cpu.pio.model != TI_86) {
			SelectObject(hdc, lpDebugInfo->hfontSegoe);
			OffsetRect(&rc, -lpDebugInfo->kRegAddr*3, lpDebugInfo->kRegRow*3/2);
			DrawText(hdc, _T("Next Timer2"), -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

			ntimer = tc_elapsed(&lpCalc->timer_c) - lpCalc->cpu.pio.stdint->lastchk2;
			ntimer *= 1000;
			StringCbPrintf(szRegVal, sizeof(szRegVal), _T("%0.4lf ms"), ntimer);

			SelectObject(hdc, lpDebugInfo->hfontLucida);
			OffsetRect(&rc, lpDebugInfo->kRegAddr*3, 0);
			DrawText(hdc, szRegVal, -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

			SelectObject(hdc, lpDebugInfo->hfontSegoe);
			OffsetRect(&rc, -lpDebugInfo->kRegAddr*3, lpDebugInfo->kRegRow);
			DrawText(hdc, _T("Timer2 dur."), -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

			ntimer = lpCalc->cpu.pio.stdint->timermax2;
			ntimer *= 1000;
			StringCbPrintf(szRegVal, sizeof(szRegVal), _T("%0.4lf ms"), ntimer);

			SelectObject(hdc, lpDebugInfo->hfontLucida);
			OffsetRect(&rc, lpDebugInfo->kRegAddr*3, 0);
			DrawText(hdc, szRegVal, -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
		}

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_SIZE:
	{
		lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);
		int height;
		if (lpCalc->cpu.pio.model == TI_85 || lpCalc->cpu.pio.model == TI_86) {
			height = lpDebugInfo->kRegRow*6 + lpDebugInfo->kRegRow/2 + lpDebugInfo->kRegRow;
		} else {
			height = lpDebugInfo->kRegRow*2 + lpDebugInfo->kRegRow*6 + lpDebugInfo->kRegRow/2 + lpDebugInfo->kRegRow;
		}
		SetWindowPos(hwnd, NULL, 0, 0, lpDebugInfo->kRegAddr*6, height, SWP_NOMOVE | SWP_NOZORDER);
		return 0;
	}
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
	{
		return (LRESULT) GetStockObject(WHITE_BRUSH);
	}
	case WM_USER:
		lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		switch (wParam) {
		case DB_UPDATE:
			Button_SetCheck(chkIff1, lpCalc->cpu.iff1 ? BST_CHECKED : BST_UNCHECKED);
			Button_SetCheck(chkIff2, lpCalc->cpu.iff2 ? BST_CHECKED : BST_UNCHECKED);

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
	static LPCALC lpCalc;
	static LPDEBUGWINDOWINFO lpDebugInfo;

	switch (Message) {
	case WM_CREATE:
	{
		lpCalc = (LPCALC) ((LPCREATESTRUCT) lParam)->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) lpCalc);
		lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);

		HWND hwndValue;

		hwndValue = CreateValueField(hwnd, lpDebugInfo, _T("X"), lpDebugInfo->kRegAddr, &lpCalc->cpu.pio.lcd->x, 4, 2, DEC3);
		SetWindowPos(hwndValue, NULL, 7*lpDebugInfo->kRegAddr/2, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, lpDebugInfo, _T("Y"), lpDebugInfo->kRegAddr, &lpCalc->cpu.pio.lcd->y, 4, 2, DEC3);
		SetWindowPos(hwndValue, NULL, 7*lpDebugInfo->kRegAddr/2, lpDebugInfo->kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, lpDebugInfo, _T("Z"), lpDebugInfo->kRegAddr, &lpCalc->cpu.pio.lcd->z, 4, 2, DEC3);
		SetWindowPos(hwndValue, NULL, 7*lpDebugInfo->kRegAddr/2, 2*lpDebugInfo->kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, lpDebugInfo, _T("Contrast"), lpDebugInfo->kRegAddr*2, &lpCalc->cpu.pio.lcd->contrast, 4, 2, DEC3);
		SetWindowPos(hwndValue, NULL, 0, 5*lpDebugInfo->kRegRow/4, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		chkOn =
		CreateWindow(
			_T("BUTTON"),
			_T("Powered"),
			WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
			0, 0, 3*lpDebugInfo->kRegAddr, lpDebugInfo->kRegRow,
			hwnd, (HMENU) IDC_LCD_ON, g_hInst, NULL);
		SetWindowFont(chkOn, lpDebugInfo->hfontSegoe, TRUE);

		grpMode =
		CreateWindow(
			_T("BUTTON"),
			_T("Cursor Mode"),
			WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
			0, 3*lpDebugInfo->kRegRow, 5*lpDebugInfo->kRegAddr, 7*lpDebugInfo->kRegRow/2,
			hwnd, (HMENU) 34234, g_hInst, NULL);
		SetWindowFont(grpMode, lpDebugInfo->hfontSegoe, TRUE);

		rdoXinc =
		CreateWindow(
			_T("BUTTON"),
			_T("X-Inc."),
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
			lpDebugInfo->kRegAddr/2, 4*lpDebugInfo->kRegRow, 2*lpDebugInfo->kRegAddr, lpDebugInfo->kRegRow,
			hwnd, (HMENU) 84354, g_hInst, NULL);
		SetWindowFont(rdoXinc, lpDebugInfo->hfontSegoe, TRUE);

		rdoYinc =
		CreateWindow(
			_T("BUTTON"),
			_T("Y-Inc."),
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON ,
			2*lpDebugInfo->kRegAddr + lpDebugInfo->kRegAddr*2/3, 4*lpDebugInfo->kRegRow, 2*lpDebugInfo->kRegAddr, lpDebugInfo->kRegRow,
			hwnd, (HMENU) 84355, g_hInst, NULL);
		SetWindowFont(rdoYinc, lpDebugInfo->hfontSegoe, TRUE);

		rdoXdec =
		CreateWindow(
			_T("BUTTON"),
			_T("X-Dec."),
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON ,
			lpDebugInfo->kRegAddr/2, 5*lpDebugInfo->kRegRow, 2*lpDebugInfo->kRegAddr, lpDebugInfo->kRegRow,
			hwnd, (HMENU) 84356, g_hInst, NULL);
		SetWindowFont(rdoXdec, lpDebugInfo->hfontSegoe, TRUE);

		rdoYdec =
		CreateWindow(
			_T("BUTTON"),
			_T("Y-Dec."),
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON ,
			2*lpDebugInfo->kRegAddr + lpDebugInfo->kRegAddr*2/3, 5*lpDebugInfo->kRegRow, 2*lpDebugInfo->kRegAddr, lpDebugInfo->kRegRow,
			hwnd, (HMENU) 84357, g_hInst, NULL);
		SetWindowFont(rdoYdec, lpDebugInfo->hfontSegoe, TRUE);

		return 0;
	}
	case WM_COMMAND:
	{
		lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		switch (HIWORD(wParam)) {
			case BN_CLICKED:
				if ((HWND) lParam == chkOn)
					lpCalc->cpu.pio.lcd->active = !lpCalc->cpu.pio.lcd->active;
				else if ((HWND) lParam == rdoXinc)
					lpCalc->cpu.pio.lcd->cursor_mode = X_DOWN;
				else if ((HWND) lParam == rdoXdec)
					lpCalc->cpu.pio.lcd->cursor_mode = X_UP;
				else if ((HWND) lParam == rdoYdec)
					lpCalc->cpu.pio.lcd->cursor_mode = Y_DOWN;
				else if ((HWND) lParam == rdoYinc)
					lpCalc->cpu.pio.lcd->cursor_mode = Y_UP;
				Debug_UpdateWindow(lpCalc->hwndDebug);
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
		lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);
		SetWindowPos(hwnd, NULL, 0, 0, lpDebugInfo->kRegAddr*6, lpDebugInfo->kRegRow * 9, SWP_NOMOVE | SWP_NOZORDER);
		return 0;
	}
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
	{
		return (LRESULT) GetStockObject(WHITE_BRUSH);
	}
	case WM_USER:
		lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		switch (wParam) {
		case DB_UPDATE:
			Button_SetCheck(chkOn, lpCalc->cpu.pio.lcd->active ? BST_CHECKED : BST_UNCHECKED);

			Button_SetCheck(rdoXdec, BST_UNCHECKED);
			Button_SetCheck(rdoXinc, BST_UNCHECKED);
			Button_SetCheck(rdoYinc, BST_UNCHECKED);
			Button_SetCheck(rdoYdec, BST_UNCHECKED);

			if (lpCalc->model != TI_84PCSE) {
				switch (lpCalc->cpu.pio.lcd->cursor_mode)
				{
				case X_UP:		Button_SetCheck(rdoXdec, BST_CHECKED); break;
				case X_DOWN:	Button_SetCheck(rdoXinc, BST_CHECKED); break;
				case Y_UP:		Button_SetCheck(rdoYinc, BST_CHECKED); break;
				case Y_DOWN:	Button_SetCheck(rdoYdec, BST_CHECKED); break;
				default:
					break;
				}
			}

			InvalidateRect(lpCalc->hwndLCD, NULL, FALSE);
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
	static LPCALC lpCalc;
	static LPDEBUGWINDOWINFO lpDebugInfo;

	switch (Message) {
		case WM_CREATE: {
			lpCalc = (LPCALC) ((LPCREATESTRUCT) lParam)->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) lpCalc);
			lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);
			HFONT hfontFlags = lpDebugInfo->hfontSegoe;

			chk_z =
			CreateWindow(
				_T("BUTTON"),
				_T("z"),
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				0, FLAGS_TOP, 3*lpDebugInfo->kRegAddr/2, lpDebugInfo->kRegRow,
				hwnd, (HMENU) REG_CHK_Z, g_hInst, NULL);
			SetWindowFont(chk_z, hfontFlags, TRUE);

			chk_c =
			CreateWindow(
				_T("BUTTON"),
				_T("c"),
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				2*lpDebugInfo->kRegAddr, FLAGS_TOP, 3*lpDebugInfo->kRegAddr/2, lpDebugInfo->kRegRow,
				hwnd, (HMENU) REG_CHK_C, g_hInst, NULL);
			SetWindowFont(chk_c, hfontFlags, TRUE);

			chk_s =
			CreateWindow(
				_T("BUTTON"),
				_T("s"),
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				4*lpDebugInfo->kRegAddr, FLAGS_TOP, 3*lpDebugInfo->kRegAddr/2, lpDebugInfo->kRegRow,
				hwnd, (HMENU) REG_CHK_S, g_hInst, NULL);
			SetWindowFont(chk_s, hfontFlags, TRUE);

			chk_pv =
			CreateWindow(
				_T("BUTTON"),
				_T("p/v"),
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				0, FLAGS_TOP + lpDebugInfo->kRegRow, 3*lpDebugInfo->kRegAddr/2, lpDebugInfo->kRegRow,
				hwnd, (HMENU) REG_CHK_PV, g_hInst, NULL);
			SetWindowFont(chk_pv, hfontFlags, TRUE);

			chk_hc =
			CreateWindow(
				_T("BUTTON"),
				_T("hc"),
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				2*lpDebugInfo->kRegAddr, FLAGS_TOP + lpDebugInfo->kRegRow, 3*lpDebugInfo->kRegAddr/2, lpDebugInfo->kRegRow,
				hwnd, (HMENU) REG_CHK_HC, g_hInst, NULL);
			SetWindowFont(chk_hc, hfontFlags, TRUE);

			chk_n =
			CreateWindow(
				_T("BUTTON"),
				_T("n"),
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				4*lpDebugInfo->kRegAddr, FLAGS_TOP + lpDebugInfo->kRegRow, 3*lpDebugInfo->kRegAddr/2, lpDebugInfo->kRegRow,
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
			lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			switch (HIWORD(wParam)) {
				case BN_CLICKED:
					switch (LOWORD(wParam)) {
						case REG_CHK_Z:
							lpCalc->cpu.f ^= ZERO_MASK;
							break;
						case REG_CHK_C:
							lpCalc->cpu.f ^= CARRY_MASK;
							break;
						case REG_CHK_S:
							lpCalc->cpu.f ^= SIGN_MASK;
							break;
						case REG_CHK_PV:
							lpCalc->cpu.f ^= PV_MASK;
							break;
						case REG_CHK_HC:
							lpCalc->cpu.f ^= HC_MASK;
							break;
						case REG_CHK_N:
							lpCalc->cpu.f ^= N_MASK;
							break;
					}
					Debug_UpdateWindow(lpCalc->hwndDebug);
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
		case WM_SIZE:
		{
			lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);
			SetWindowPos(hwnd, NULL, 0, 0, lpDebugInfo->kRegAddr*6, lpDebugInfo->kRegRow*4, SWP_NOMOVE | SWP_NOZORDER);
			return 0;
		}
		case WM_USER: {
			lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			switch (wParam) {
			case DB_UPDATE: {
				Button_SetCheck(chk_z, (lpCalc->cpu.f & ZERO_MASK) ? BST_CHECKED : BST_UNCHECKED);
				Button_SetCheck(chk_c, (lpCalc->cpu.f & CARRY_MASK) ? BST_CHECKED : BST_UNCHECKED);
				Button_SetCheck(chk_s, (lpCalc->cpu.f & SIGN_MASK) ? BST_CHECKED : BST_UNCHECKED);
				Button_SetCheck(chk_pv, (lpCalc->cpu.f & PV_MASK) ? BST_CHECKED : BST_UNCHECKED);
				Button_SetCheck(chk_hc, (lpCalc->cpu.f & HC_MASK) ? BST_CHECKED : BST_UNCHECKED);
				Button_SetCheck(chk_n, (lpCalc->cpu.f & N_MASK) ? BST_CHECKED : BST_UNCHECKED);
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
	static LPCALC lpCalc;
	static SCROLLINFO si;
	static LPDEBUGWINDOWINFO lpDebugInfo;

	//static WB_IDropTarget *pDropTarget;

	switch (Message) {
		case WM_CREATE:
		{
			lpCalc = (LPCALC) ((LPCREATESTRUCT) lParam)->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) lpCalc);
			HDC hdc = GetDC(hwnd);
			HWND hwndExp;
			lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);

			SelectObject(hdc, lpDebugInfo->hfontLucida);
			GetTextMetrics(hdc, &tm);
			lpDebugInfo->kRegRow = tm.tmHeight + tm.tmHeight/3;
			lpDebugInfo->kRegAddr = tm.tmAveCharWidth*4;


			//RegisterExpandPaneDropWindow(hwnd, &pDropTarget);

			RECT r;
			GetClientRect(hwnd, &r);

#define REGISTERS_TOP 2*lpDebugInfo->kRegRow
#define VECTORS_TOP (12*lpDebugInfo->kRegRow+lpDebugInfo->kRegRow/2)

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

				wcx.lpszClassName = _T("WabbitKeyDisp");
				wcx.lpfnWndProc = DBKeyboardProc;
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
						(HMENU) 1, g_hInst, lpCalc);

			hwndExp = CreateExpandPane(hwnd, lpDebugInfo, _T("Registers"), hwndContent);


			hwndContent =
				CreateWindow(
						_T("WabbitFlagDisp"),
						_T("Flags"),
						WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						0, 0, 1, 1,
						hwnd,
						(HMENU) 1, g_hInst, lpCalc);

			CreateExpandPane(hwnd, lpDebugInfo, _T("Flags"), hwndContent);

			hwndContent =
				CreateWindow(
						_T("WabbitCPUDisp"),
						_T("CPU Status"),
						WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						0, 0, 1, 1,
						hwnd,
						(HMENU) 1, g_hInst, lpCalc);
			CreateExpandPane(hwnd, lpDebugInfo, _T("CPU Status"), hwndContent);

			hwndContent =
				CreateWindow(
						_T("WabbitMemDisp"),
						_T("Memory Map"),
						WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						0, 0, 1, 1,
						hwnd,
						(HMENU) 1, g_hInst, lpCalc);
			CreateExpandPane(hwnd, lpDebugInfo, _T("Memory Map"), hwndContent);

			hwndContent =
				CreateWindow(
						_T("WabbitKeyDisp"),
						_T("Keyboard"),
						WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						0, 0, 1, 1,
						hwnd,
						(HMENU) 1, g_hInst, lpCalc);
			CreateExpandPane(hwnd, lpDebugInfo, _T("Keyboard"), hwndContent);

			hwndContent =
				CreateWindow(
						_T("WabbitInterruptDisp"),
						_T("Interrupts"),
						WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						0, 0, 1, 1,
						hwnd,
						(HMENU) 1, g_hInst, lpCalc);
			CreateExpandPane(hwnd, lpDebugInfo, _T("Interrupts"), hwndContent);

			hwndContent =
				CreateWindow(
					_T("WabbitLCDDisp"),
					_T("Display"),
					WS_CHILD  | WS_CLIPSIBLINGS,
					0, 0, 1, 1,
					hwnd,
					(HMENU) 1, g_hInst, lpCalc);
			CreateExpandPane(hwnd, lpDebugInfo, _T("Display"), hwndContent);

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

			lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);

			ArrangeExpandPanes(lpDebugInfo);
			int height = GetExpandPanesHeight(lpDebugInfo) + 7;
			//positive diff we opened a pane, negative diff we closed
			int diff = height - si.nMax;

			si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
			si.nMin = 0;
			si.nMax = height;
			si.nPage = rc.bottom;
			lpDebugInfo->regPanesYScroll = min(0, max(0, si.nPos + diff));
			if (diff < 0)
				si.nPos = max(0, si.nPos + diff);
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

			/*if (si.nPos != 0)
				ScrollWindow(hwnd, 0, -si.nPos, NULL, NULL);*/

			DrawExpandPanes(lpDebugInfo);
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
			lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);
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
			lpDebugInfo->regPanesYScroll = -si.nPos;
			if (si.nPos != yPos) {
				ScrollWindow(hwnd, 0, yPos - si.nPos, NULL, NULL);
			}
			DrawExpandPanes(lpDebugInfo);
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
					lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					if (vi != -1) {
						if (vi < REG16_ROWS*REG16_COLS) {
							ValueSubmit(hwndVal,
								((TCHAR *) (&lpCalc->cpu)) + reg_offset[vi].offset, 2);
						} else {
							ValueSubmit(hwndVal,
								((TCHAR *) (&lpCalc->cpu)) + reg_offset[vi].offset, 1);

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
