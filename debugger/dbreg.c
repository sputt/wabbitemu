#include "stdafx.h"

#include "dbreg.h"
#include "ti_stdint.h"
#include "alu.h"
#include "guidebug.h"
#include "print.h"
#include "expandpane.h"
#include "dbvalue.h"


extern HINSTANCE g_hInst;
extern HFONT hfontSegoe, hfontLucida, hfontLucidaBold;

#define DBREG_ORGX	12
#define DBREG_ORGY	0
SCROLLINFO si;

struct db_reg {
	unsigned int offset;
	char name[8];
};

static const struct db_reg reg_offset[] = {
	coff(af,"af"), coff(afp,"af'"),
	coff(bc,"bc"), coff(bcp,"bc'"),
	coff(de,"de"), coff(dep,"de'"),
	coff(hl,"hl"), coff(hlp,"hl'"),
	coff(ix,"ix"), coff(sp,"sp"),
	coff(iy,"iy"), coff(pc,"pc"),
	coff(i,"i"), coff(imode,"im"), coff(r,"r")};
	//coff(halt,"hlt"), coff(iff1,"iff1"),coff(iff2,"iff2")};

static RECT val_locs[NumElm(reg_offset)];
static int kRegRow, kRegAddr;

void ValueDraw(HDC hdc, RECT *dr, int i) {
	char szRegVal[16];

	dr->right = dr->left + kRegAddr;
	sprintf(szRegVal, "%s", reg_offset[i].name);
	SelectObject(hdc, hfontLucida);
	SetTextColor(hdc, DBCOLOR_HILIGHT);
	DrawText(hdc, szRegVal, -1, dr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	SetTextColor(hdc, RGB(0,0,0));
	dr->left = dr->right;
	dr->right += kRegAddr;
	SelectObject(hdc, hfontLucida);

	if (i < REG16_ROWS * REG16_COLS) {
		sprintf(szRegVal, "%04X", reg16(reg_offset[i].offset));
		DrawText(hdc, szRegVal, -1, dr, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	} else {
		sprintf(szRegVal, "%02X", reg8(reg_offset[i].offset));
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
				ValueSubmit(hwndVal, ((char*) (&calcs[gslot].cpu)) + reg_offset[vi].offset, 2);
			} else {
				ValueSubmit(hwndVal, ((char*) (&calcs[gslot].cpu)) + reg_offset[vi].offset, 1);

			}
			SendMessage(GetParent(hwnd), WM_USER, DB_UPDATE, 0);
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
	char rval[8];
	int edit_width = 4;
	if (vi < REG16_ROWS*REG16_COLS) {
		sprintf(rval, "%04X", reg16(reg_offset[vi].offset));
		edit_width = 4;
	} else {
		sprintf(rval, "%02X", reg8(reg_offset[vi].offset));
		edit_width = 2;
	}


	RECT rrc;
	GetWindowRect(hwnd, &rrc);

	hwndVal =
	CreateWindow("EDIT", rval,
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
		struct {char *name; void *data; size_t size;} reg[] =
		{
				{"af", &calcs[gslot].cpu.af, 2}, {"af'", &calcs[gslot].cpu.afp, 2},
				{"bc", &calcs[gslot].cpu.bc, 2}, {"bc'", &calcs[gslot].cpu.bcp, 2},
				{"de", &calcs[gslot].cpu.de, 2}, {"de'", &calcs[gslot].cpu.dep, 2},
				{"hl", &calcs[gslot].cpu.hl, 2}, {"hl'", &calcs[gslot].cpu.hlp, 2},
				{"ix", &calcs[gslot].cpu.ix, 2}, {"sp",  &calcs[gslot].cpu.sp, 2},
				{"iy", &calcs[gslot].cpu.iy, 2}, {"pc",  &calcs[gslot].cpu.pc, 2},
		};

		// Create all of the value fields
		int i;
		for (i = 0; i < NumElm(reg); i++) {
			HWND hwndValue = CreateValueField(hwnd, reg[i].name, kRegAddr, reg[i].data, reg[i].size, 4, HEX4);
			SetWindowPos(hwndValue, NULL, (i % 2) * kRegAddr*3, kRegRow * (i/2), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			SendMessage(hwndValue, WM_SIZE, 0, 0);
		}
		return 0;
	}
	case WM_COMMAND:
	{
		SendMessage(calcs[gslot].hwndDebug, WM_USER, DB_UPDATE, 0);
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
		SetWindowPos(hwnd, NULL, 0, 0, kRegAddr*6, kRegRow*8, SWP_NOMOVE | SWP_NOZORDER);
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
		for (i = 0; i < 4; i++)
		{
			int row_y = kRegRow/4 + kRegRow*(i+1);

			hwndValue = CreateValueField(hwnd, "", 0, &calcs[gslot].cpu.mem_c->banks[i].page, 1, 3, DEC);
			SetWindowPos(hwndValue, NULL, kRegAddr*4, row_y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			SendMessage(hwndValue, WM_SIZE, 0, 0);

			rdoType[2*i] =
				CreateWindow(
					"BUTTON",
					"",
					WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
					kRegAddr*3/2+kRegAddr/8, row_y, kRegAddr/2, kRegRow,
					hwnd, (HMENU) (20+2*i), g_hInst, NULL);
			rdoType[2*i+1] =
				CreateWindow(
					"BUTTON",
					"",
					WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
					kRegAddr*5/2-kRegAddr/8, row_y, kRegAddr/2, kRegRow,
					hwnd, (HMENU) (20+2*i+1), g_hInst, NULL);
			// not quite the right thing, but close enough
			DWORD check_width = GetSystemMetrics(SM_CYMENUCHECK);
			chkRO[i] =
			CreateWindow(
				"BUTTON",
				"",
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				kRegAddr*3+((kRegAddr-check_width)/2), row_y, kRegAddr/2, kRegRow,
				hwnd, (HMENU) (30+i), g_hInst, NULL);
			SendMessage(chkRO[i], WM_SETFONT, (WPARAM) hfontSegoe, (LPARAM) TRUE);
		}

		return 0;
	}
	case WM_COMMAND:
	{
		switch (HIWORD(wParam)) {
		case BN_CLICKED:
			if ((LOWORD(wParam) >= 20) && (LOWORD(wParam) < 28)) {
				SendMessage(rdoType[((LOWORD(wParam)-20)/2) * 2 + 0],
						BM_SETCHECK, BST_UNCHECKED, 0);
				SendMessage(rdoType[((LOWORD(wParam)-20)/2) * 2 + 1],
						BM_SETCHECK, BST_UNCHECKED, 0);
				bank_state_t *bank = &calcs[gslot].mem_c.banks[(LOWORD(wParam)-20)/2];
				bank->ram = !(LOWORD(wParam) % 2);
			} else if (LOWORD(wParam) >= 30 && LOWORD(wParam) < 34) {
				calcs[gslot].mem_c.banks[LOWORD(wParam)-30].read_only =
					!calcs[gslot].mem_c.banks[LOWORD(wParam)-30].read_only;
			}
			break;
		}
		SendMessage(hwnd, WM_USER, DB_UPDATE, 0);
		SendMessage(calcs[gslot].hwndDebug, WM_USER, DB_UPDATE, 0);
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
		SetRect(&rc, kRegAddr*3/2+kRegAddr/8, 0, kRegAddr*3/2+kRegAddr/8 + kRegAddr/2, kRegRow);
		DrawTextA(hdc, "R", -1, &rc, DT_CENTER);

		SetRect(&rc, kRegAddr*3/2+kRegAddr/8, 0, kRegAddr*5/2-kRegAddr/8+ kRegAddr/2, kRegRow);
		DrawTextA(hdc, "-", -1, &rc, DT_CENTER);

		SetRect(&rc, kRegAddr*5/2-kRegAddr/8, 0, kRegAddr*5/2-kRegAddr/8+ kRegAddr/2, kRegRow);
		DrawTextA(hdc, "F", -1, &rc, DT_CENTER);

		SetRect(&rc, kRegAddr*3, 0, kRegAddr*4, kRegRow);
		DrawTextA(hdc, "RO", -1, &rc, DT_CENTER);

		OffsetRect(&rc, kRegAddr, 0);
		DrawTextA(hdc, "Page", -1, &rc, DT_CENTER);

		SetRect(&rc, 0, kRegRow/4 + kRegRow, kRegAddr*3/2, kRegRow/4 + kRegRow*2);
		int i;
		for (i = 0; i < 4; i++)
		{
			char bank[16];
			sprintf(bank, "Bank %d", i);
			DrawTextA(hdc, bank, -1, &rc, DT_LEFT);
			OffsetRect(&rc, 0, kRegRow);
		}

		SelectObject(hdc, GetStockObject(DC_PEN));
		SetDCPenColor(hdc, GetSysColor(COLOR_BTNFACE));

		GetClientRect(hwnd, &rc);
		MoveToEx(hdc, kRegAddr*3/2, 0, NULL);
		LineTo(hdc,  kRegAddr*3/2, rc.bottom - kRegRow*2);
		MoveToEx(hdc, 3*kRegAddr, 0, NULL);
		LineTo(hdc,  3*kRegAddr, rc.bottom - kRegRow*2);
		MoveToEx(hdc, 4*kRegAddr-kRegRow/8, 0, NULL);
		LineTo(hdc,  4*kRegAddr-kRegRow/8, rc.bottom - kRegRow*2);
		MoveToEx(hdc, 5*kRegAddr, 0, NULL);
		LineTo(hdc,  5*kRegAddr, rc.bottom - kRegRow*2);
		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_SIZE:
	{
		SetWindowPos(hwnd, NULL, 0, 0, kRegAddr*6, kRegRow/4 + kRegRow * 2 + kRegRow*5, SWP_NOMOVE | SWP_NOZORDER);
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
				bank_state_t *bank = &calcs[gslot].mem_c.banks[i];
				SendMessage(rdoType[2*i + (bank->ram ? 0 : 1)], BM_SETCHECK, BST_CHECKED, 0);
				SendMessage(chkRO[i], BM_SETCHECK, bank->read_only ? BST_CHECKED : BST_UNCHECKED, 0);
				bank->addr = bank->ram ? calcs[gslot].mem_c.ram : calcs[gslot].mem_c.flash;
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
	static HWND chkHalt;
	static double freq;

	switch (Message) {
	case WM_CREATE:
	{

		HWND hwndValue;
		freq = ((double) calcs[gslot].cpu.timer_c->freq) / 1000000.0;
		hwndValue = CreateValueField(hwnd, "Freq.", kRegAddr*3/2, &freq, sizeof(double), 5, FLOAT2);
		SetWindowPos(hwndValue, NULL, 0, kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, "Bus", kRegAddr*2 + kRegAddr/4, &calcs[gslot].cpu.bus, 1, 2, HEX2);
		SetWindowPos(hwndValue, NULL, 0, kRegRow*2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		chkHalt =
		CreateWindow(
			"BUTTON",
			"Halt",
			WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
			0, 0, 3*kRegAddr, kRegRow,
			hwnd, (HMENU) 1, g_hInst, NULL);
		SendMessage(chkHalt, WM_SETFONT, (WPARAM) hfontSegoe, (LPARAM) TRUE);

		return 0;
	}
	case WM_COMMAND:
	{
		switch (HIWORD(wParam)) {
			case BN_CLICKED:
				switch (LOWORD(wParam)) {
					case 1:
						calcs[gslot].cpu.halt = !calcs[gslot].cpu.halt;
						break;
				}
				SendMessage(calcs[gslot].hwndDebug, WM_USER, DB_UPDATE, 0);
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
			SendMessage(chkHalt, BM_SETCHECK,
			calcs[gslot].cpu.halt ? BST_CHECKED : BST_UNCHECKED, 0);
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
			"BUTTON",
			"iff1",
			WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
			0, 0, 2*kRegAddr, kRegRow,
			hwnd, (HMENU) 1, g_hInst, NULL);
		SendMessage(chkIff1, WM_SETFONT, (WPARAM) hfontSegoe, (LPARAM) TRUE);

		chkIff2 =
		CreateWindow(
			"BUTTON",
			"iff2",
			WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
			2*kRegAddr, 0, 2*kRegAddr, kRegRow,
			hwnd, (HMENU) 2, g_hInst, NULL);
		SendMessage(chkIff2, WM_SETFONT, (WPARAM) hfontSegoe, (LPARAM) TRUE);
/*
		hwndValue = CreateValueField(hwnd, "Mask", kRegAddr, &calcs[gslot].cpu.pio.stdint->intactive, 1, 2, HEX2);
		SetWindowPos(hwndValue, NULL, 0, 1*kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, "i", kRegAddr, &calcs[gslot].cpu.i, 1, 2, HEX2);
		SetWindowPos(hwndValue, NULL, 0, 2*kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, "r", kRegAddr, &calcs[gslot].cpu.r, 1, 2, HEX2);
		SetWindowPos(hwndValue, NULL, 0, 3*kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);
		*/
		return 0;
	}
	case WM_COMMAND:
	{
		switch (HIWORD(wParam)) {
			case BN_CLICKED:
				switch (LOWORD(wParam)) {
					case 1:
						calcs[gslot].cpu.iff1 = !calcs[gslot].cpu.iff1;
						break;
					case 2:
						calcs[gslot].cpu.iff2 = !calcs[gslot].cpu.iff2;
						break;
				}
				SendMessage(calcs[gslot].hwndDebug, WM_USER, DB_UPDATE, 0);
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

		char szRegVal[16];
		double ntimer;
		SetRect(&rc, 0, kRegRow*2, kRegAddr*3, kRegRow*3);

		ntimer = tc_elapsed(&calcs[gslot].timer_c) - calcs[gslot].cpu.pio.stdint->lastchk1;
		ntimer *= 1000;
		sprintf(szRegVal, "%0.4lf ms", ntimer);
		SelectObject(hdc, hfontSegoe);
		DrawTextA(hdc, "Next Timer1", -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

		SelectObject(hdc, hfontLucida);
		OffsetRect(&rc, kRegAddr*3, 0);
		DrawTextA(hdc, szRegVal, -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

		SelectObject(hdc, hfontSegoe);
		OffsetRect(&rc, -kRegAddr*3, kRegRow);
		DrawTextA(hdc, "Timer1 dur.", -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

		ntimer = calcs[gslot].cpu.pio.stdint->timermax1;
		ntimer *= 1000;
		sprintf(szRegVal, "%0.4lf ms", ntimer);

		SelectObject(hdc, hfontLucida);
		OffsetRect(&rc, kRegAddr*3, 0);
		DrawTextA(hdc, szRegVal, -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);


		SelectObject(hdc, hfontSegoe);
		OffsetRect(&rc, -kRegAddr*3, kRegRow*3/2);
		DrawTextA(hdc, "Next Timer2", -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

		ntimer = tc_elapsed(&calcs[gslot].timer_c) - calcs[gslot].cpu.pio.stdint->lastchk2;
		ntimer *= 1000;
		sprintf(szRegVal, "%0.4lf ms", ntimer);

		SelectObject(hdc, hfontLucida);
		OffsetRect(&rc, kRegAddr*3, 0);
		DrawTextA(hdc, szRegVal, -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);



		SelectObject(hdc, hfontSegoe);
		OffsetRect(&rc, -kRegAddr*3, kRegRow);
		DrawTextA(hdc, "Timer2 dur.", -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

		ntimer = calcs[gslot].cpu.pio.stdint->timermax2;
		ntimer *= 1000;
		sprintf(szRegVal, "%0.4lf ms", ntimer);

		SelectObject(hdc, hfontLucida);
		OffsetRect(&rc, kRegAddr*3, 0);
		DrawTextA(hdc, szRegVal, -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_SIZE:
	{
		SetWindowPos(hwnd, NULL, 0, 0, kRegAddr*6, kRegRow * 2 + kRegRow*6 + kRegRow/2, SWP_NOMOVE | SWP_NOZORDER);
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
			SendMessage(chkIff1, BM_SETCHECK,
			calcs[gslot].cpu.iff1 ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(chkIff2, BM_SETCHECK,
			calcs[gslot].cpu.iff2 ? BST_CHECKED : BST_UNCHECKED, 0);

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
				{"X", &calcs[gslot].cpu.pio.lcd->x, 1},
				{"Y", &calcs[gslot].cpu.pio.lcd->y, 1},
				{"Z", &calcs[gslot].cpu.pio.lcd->z, 1},
				{"Contrast", &calcs[gslot].cpu.pio.lcd->contrast, 1},
				{"Mode", &calcs[gslot].cpu.pio.lcd->mode, 1},
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

		hwndValue = CreateValueField(hwnd, "X", kRegAddr, &calcs[gslot].cpu.pio.lcd->x, 4, 2, DEC);
		SetWindowPos(hwndValue, NULL, 7*kRegAddr/2, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, "Y", kRegAddr, &calcs[gslot].cpu.pio.lcd->y, 4, 2, DEC);
		SetWindowPos(hwndValue, NULL, 7*kRegAddr/2, kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, "Z", kRegAddr, &calcs[gslot].cpu.pio.lcd->z, 4, 2, DEC);
		SetWindowPos(hwndValue, NULL, 7*kRegAddr/2, 2*kRegRow, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		hwndValue = CreateValueField(hwnd, "Contrast", kRegAddr*2, &calcs[gslot].cpu.pio.lcd->contrast, 4, 2, DEC);
		SetWindowPos(hwndValue, NULL, 0, 5*kRegRow/4, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SendMessage(hwndValue, WM_SIZE, 0, 0);

		chkOn =
		CreateWindow(
			"BUTTON",
			"Powered",
			WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
			0, 0, 3*kRegAddr, kRegRow,
			hwnd, (HMENU) IDC_LCD_ON, g_hInst, NULL);
		SendMessage(chkOn, WM_SETFONT, (WPARAM) hfontSegoe, (LPARAM) TRUE);

		grpMode =
		CreateWindow(
			"BUTTON",
			"Cursor Mode",
			WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
			0, 3*kRegRow, 5*kRegAddr, 7*kRegRow/2,
			hwnd, (HMENU) 34234, g_hInst, NULL);

		SendMessage(grpMode, WM_SETFONT, (WPARAM) hfontSegoe, (LPARAM) TRUE);

		rdoXinc =
		CreateWindow(
			"BUTTON",
			"X-Inc.",
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
			kRegAddr/2, 4*kRegRow, 2*kRegAddr, kRegRow,
			hwnd, (HMENU) 84354, g_hInst, NULL);
		SendMessage(rdoXinc, WM_SETFONT, (WPARAM) hfontSegoe, (LPARAM) TRUE);

		rdoYinc =
		CreateWindow(
			"BUTTON",
			"Y-Inc.",
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON ,
			2*kRegAddr + kRegAddr*2/3, 4*kRegRow, 2*kRegAddr, kRegRow,
			hwnd, (HMENU) 84355, g_hInst, NULL);
		SendMessage(rdoYinc, WM_SETFONT, (WPARAM) hfontSegoe, (LPARAM) TRUE);

		rdoXdec =
		CreateWindow(
			"BUTTON",
			"X-Dec.",
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON ,
			kRegAddr/2, 5*kRegRow, 2*kRegAddr, kRegRow,
			hwnd, (HMENU) 84356, g_hInst, NULL);
		SendMessage(rdoXdec, WM_SETFONT, (WPARAM) hfontSegoe, (LPARAM) TRUE);

		rdoYdec =
		CreateWindow(
			"BUTTON",
			"Y-Dec.",
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON ,
			2*kRegAddr + kRegAddr*2/3, 5*kRegRow, 2*kRegAddr, kRegRow,
			hwnd, (HMENU) 84357, g_hInst, NULL);
		SendMessage(rdoYdec, WM_SETFONT, (WPARAM) hfontSegoe, (LPARAM) TRUE);

		return 0;
	}
	case WM_COMMAND:
	{
		switch (HIWORD(wParam)) {
			case BN_CLICKED:
				if ((HWND) lParam == chkOn)
					calcs[gslot].cpu.pio.lcd->active = !calcs[gslot].cpu.pio.lcd->active;
				else if ((HWND) lParam == rdoXinc)
					calcs[gslot].cpu.pio.lcd->cursor_mode = X_DOWN;
				else if ((HWND) lParam == rdoXdec)
					calcs[gslot].cpu.pio.lcd->cursor_mode = X_UP;
				else if ((HWND) lParam == rdoYdec)
					calcs[gslot].cpu.pio.lcd->cursor_mode = Y_DOWN;
				else if ((HWND) lParam == rdoYinc)
					calcs[gslot].cpu.pio.lcd->cursor_mode = Y_UP;
				SendMessage(calcs[gslot].hwndDebug, WM_USER, DB_UPDATE, 0);
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
			SendMessage(chkOn, BM_SETCHECK,
			calcs[gslot].cpu.pio.lcd->active ? BST_CHECKED : BST_UNCHECKED, 0);

			SendMessage(rdoXdec, BM_SETCHECK, BST_UNCHECKED, 0);
			SendMessage(rdoXinc, BM_SETCHECK, BST_UNCHECKED, 0);
			SendMessage(rdoYinc, BM_SETCHECK, BST_UNCHECKED, 0);
			SendMessage(rdoYdec, BM_SETCHECK, BST_UNCHECKED, 0);

			switch (calcs[gslot].cpu.pio.lcd->cursor_mode)
			{
			case X_UP:		SendMessage(rdoXdec, BM_SETCHECK, BST_CHECKED, 0); break;
			case X_DOWN:	SendMessage(rdoXinc, BM_SETCHECK, BST_CHECKED, 0); break;
			case Y_UP:		SendMessage(rdoYinc, BM_SETCHECK, BST_CHECKED, 0); break;
			case Y_DOWN:	SendMessage(rdoYdec, BM_SETCHECK, BST_CHECKED, 0); break;
			default:
				break;
			}

			InvalidateRect(calcs[gslot].hwndLCD, NULL, FALSE);
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
				"BUTTON",
				"z",
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				0, FLAGS_TOP, 3*kRegAddr/2, kRegRow,
				hwnd, (HMENU) REG_CHK_Z, g_hInst, NULL);
			SendMessage(chk_z, WM_SETFONT, (WPARAM) hfontFlags, (LPARAM) TRUE);

			chk_c =
			CreateWindow(
				"BUTTON",
				"c",
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				2*kRegAddr, FLAGS_TOP, 3*kRegAddr/2, kRegRow,
				hwnd, (HMENU) REG_CHK_C, g_hInst, NULL);
			SendMessage(chk_c, WM_SETFONT, (WPARAM) hfontFlags, (LPARAM) TRUE);

			chk_s =
			CreateWindow(
				"BUTTON",
				"s",
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				4*kRegAddr, FLAGS_TOP, 3*kRegAddr/2, kRegRow,
				hwnd, (HMENU) REG_CHK_S, g_hInst, NULL);
			SendMessage(chk_s, WM_SETFONT, (WPARAM) hfontFlags, (LPARAM) TRUE);

			chk_pv =
			CreateWindow(
				"BUTTON",
				"p/v",
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				0, FLAGS_TOP + kRegRow, 3*kRegAddr/2, kRegRow,
				hwnd, (HMENU) REG_CHK_PV, g_hInst, NULL);
			SendMessage(chk_pv, WM_SETFONT, (WPARAM) hfontFlags, (LPARAM) TRUE);

			chk_hc =
			CreateWindow(
				"BUTTON",
				"hc",
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				2*kRegAddr, FLAGS_TOP + kRegRow, 3*kRegAddr/2, kRegRow,
				hwnd, (HMENU) REG_CHK_HC, g_hInst, NULL);
			SendMessage(chk_hc, WM_SETFONT, (WPARAM) hfontFlags, (LPARAM) TRUE);

			chk_n =
			CreateWindow(
				"BUTTON",
				"n",
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX, // | BS_LEFTTEXT,
				4*kRegAddr, FLAGS_TOP + kRegRow, 3*kRegAddr/2, kRegRow,
				hwnd, (HMENU) REG_CHK_N, g_hInst, NULL);
			SendMessage(chk_n, WM_SETFONT, (WPARAM) hfontFlags, (LPARAM) TRUE);

			SendMessage(hwnd, WM_USER, DB_UPDATE, 0);
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
							calcs[gslot].cpu.f ^= ZERO_MASK;
							break;
						case REG_CHK_C:
							calcs[gslot].cpu.f ^= CARRY_MASK;
							break;
						case REG_CHK_S:
							calcs[gslot].cpu.f ^= SIGN_MASK;
							break;
						case REG_CHK_PV:
							calcs[gslot].cpu.f ^= PV_MASK;
							break;
						case REG_CHK_HC:
							calcs[gslot].cpu.f ^= HC_MASK;
							break;
						case REG_CHK_N:
							calcs[gslot].cpu.f ^= N_MASK;
							break;
					}
					SendMessage(calcs[gslot].hwndDebug, WM_USER, DB_UPDATE, 0);
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
				SendMessage(chk_z, BM_SETCHECK,
				(calcs[gslot].cpu.f & ZERO_MASK) ? BST_CHECKED : BST_UNCHECKED, 0);
				SendMessage(chk_c, BM_SETCHECK,
				(calcs[gslot].cpu.f & CARRY_MASK) ? BST_CHECKED : BST_UNCHECKED, 0);
				SendMessage(chk_s, BM_SETCHECK,
				(calcs[gslot].cpu.f & SIGN_MASK) ? BST_CHECKED : BST_UNCHECKED, 0);
				SendMessage(chk_pv, BM_SETCHECK,
				(calcs[gslot].cpu.f & PV_MASK) ? BST_CHECKED : BST_UNCHECKED, 0);
				SendMessage(chk_hc, BM_SETCHECK,
				(calcs[gslot].cpu.f & HC_MASK) ? BST_CHECKED : BST_UNCHECKED, 0);
				SendMessage(chk_n, BM_SETCHECK,
				(calcs[gslot].cpu.f & N_MASK) ? BST_CHECKED : BST_UNCHECKED, 0);
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
				wcx.lpszClassName = "WabbitRegDisp";
				wcx.lpfnWndProc = DBRegProc;
				wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
				RegisterClassEx(&wcx);

				wcx.lpszClassName = "WabbitFlagDisp";
				wcx.lpfnWndProc = DBFlagProc;
				RegisterClassEx(&wcx);

				wcx.lpszClassName = "WabbitLCDDisp";
				wcx.lpfnWndProc = DBLCDProc;
				RegisterClassEx(&wcx);

				wcx.lpszClassName = "WabbitCPUDisp";
				wcx.lpfnWndProc = DBCPUProc;
				RegisterClassEx(&wcx);

				wcx.lpszClassName = "WabbitMemDisp";
				wcx.lpfnWndProc = DBMemMapProc;
				RegisterClassEx(&wcx);

				wcx.lpszClassName = "WabbitInterruptDisp";
				wcx.lpfnWndProc = DBInterruptProc;
				RegisterClassEx(&wcx);

				FirstRun = FALSE;
			}

			HWND hwndContent;


			hwndContent =
				CreateWindow(
						"WabbitRegDisp",
						"Registers",
						WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						0, 0, 1, 1,
						hwnd,
						(HMENU) 1, g_hInst, NULL);

			hwndExp = CreateExpandPane(hwnd, "Registers", hwndContent);


			hwndContent =
				CreateWindow(
						"WabbitFlagDisp",
						"Flags",
						WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						0, 0, 1, 1,
						hwnd,
						(HMENU) 1, g_hInst, NULL);

			CreateExpandPane(hwnd, "Flags", hwndContent);

			hwndContent =
				CreateWindow(
						"WabbitCPUDisp",
						"CPU Status",
						WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						0, 0, 1, 1,
						hwnd,
						(HMENU) 1, g_hInst, NULL);
			CreateExpandPane(hwnd, "CPU Status", hwndContent);

			hwndContent =
				CreateWindow(
						"WabbitMemDisp",
						"Memory Map",
						WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						0, 0, 1, 1,
						hwnd,
						(HMENU) 1, g_hInst, NULL);
			CreateExpandPane(hwnd, "Memory Map", hwndContent);

			hwndContent =
				CreateWindow(
						"WabbitInterruptDisp",
						"Interrupts",
						WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
						0, 0, 1, 1,
						hwnd,
						(HMENU) 1, g_hInst, NULL);
			CreateExpandPane(hwnd, "Interrupts", hwndContent);

			hwndContent =
				CreateWindow(
					"WabbitLCDDisp",
					"Display",
					WS_CHILD  | WS_CLIPSIBLINGS,
					0, 0, 1, 1,
					hwnd,
					(HMENU) 1, g_hInst, NULL);
			CreateExpandPane(hwnd, "Display", hwndContent);

			RECT rc;
			GetClientRect(hwnd, &rc);

			return 0;
		}

		case WM_SIZE:
		{
			ArrangeExpandPanes();
			int height = GetExpandPanesHeight() + 50;
			RECT rc;
			GetClientRect(hwnd, &rc);
			si.cbSize = sizeof (si);
			si.fMask  = SIF_ALL;
			GetScrollInfo (hwnd, SB_VERT, &si);
			float percent;
			if (si.nMax != 0)
				percent = (float)si.nPos / (float)si.nMax;
			else
				percent = 0;

			si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
			if (height < rc.bottom) {
				//si.fMask = SIF_RANGE;
				si.nPage = rc.bottom;
				height = 0;
			}
			else
				height -= rc.bottom;
			si.cbSize = sizeof(SCROLLINFO);
			si.nMin = 0;
			si.nMax = height;
			si.nPos = percent * height;
			if (height != 0)
				si.nPage = 40 - rc.bottom  / height;
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
			if (percent != 0){
				ScrollWindow(hwnd, 0, -si.nPos, NULL, NULL);
			}

			InvalidateRect(hwnd, NULL, FALSE);
			UpdateWindow(hwnd);
			return 0;
		}
		case WM_VSCROLL: {
			// Get all the vertial scroll bar information
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
			 if (si.nPos != yPos)
			 {
				  ScrollWindow(hwnd, 0, yPos - si.nPos, NULL, NULL);
				  UpdateWindow (hwnd);
			 }
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
							((char*) (&calcs[gslot].cpu)) + reg_offset[vi].offset, 2);
					} else {
						ValueSubmit(hwndVal,
							((char*) (&calcs[gslot].cpu)) + reg_offset[vi].offset, 1);

					}
					SendMessage(GetParent(hwnd), WM_USER, DB_UPDATE, 0);
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
			return 0;
		}
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}
