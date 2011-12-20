#include "stdafx.h"

#include "dbcommon.h"
#include "gui.h"
#include "resource.h"
#include "label.h"

extern HINSTANCE g_hInst;

unsigned int goto_addr;
int find_value;
BOOL search_backwards;

extern HFONT hfontLucida;
HWND hwndPrev;

INT_PTR CALLBACK GotoDialogProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND edtAddr;
	switch (Message) {
		case WM_INITDIALOG:
			edtAddr = GetDlgItem(hwndDlg, IDC_EDTGOTOADDR);
			SetFocus(edtAddr);
			hwndPrev = GetParent(hwndDlg);
			return FALSE;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK: {
					TCHAR result[64];
					GetDlgItemText(hwndDlg, IDC_EDTGOTOADDR, result, 64);
					
					if (result[0] != '$') {
						label_struct *label;
						label = lookup_label(lpDebuggerCalc, result);
						if (label == NULL) _stscanf_s(result, _T("%x"), &goto_addr);
						else goto_addr = label->addr;
					} else {
						_stscanf_s(result+1, _T("%x"), &goto_addr);
					}
					EndDialog(hwndDlg, IDOK);
					return TRUE;
				}
				case IDCANCEL:
					EndDialog(hwndDlg, IDCANCEL);
					break;
			}
			break;
	}
	return FALSE;
}

INT_PTR CALLBACK FindDialogProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND edtAddr, forwardsCheck, backwardsCheck;
	switch (Message) {
		case WM_INITDIALOG:
			edtAddr = GetDlgItem(hwndDlg, IDC_EDT_FIND);
			forwardsCheck = GetDlgItem(hwndDlg, IDC_RADIO_FORWARDS);
			backwardsCheck = GetDlgItem(hwndDlg, IDC_RADIO_BACKWARDS);
			SetFocus(GetDlgItem(hwndDlg, IDC_EDT_FIND));
			hwndPrev = GetParent(hwndDlg);
			Button_SetCheck(forwardsCheck, TRUE);
			return FALSE;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_FIND_NEXT: {
					TCHAR result[32];
					GetDlgItemText(hwndDlg, IDC_EDT_FIND, result, 32);
					_stscanf_s(result, _T("%x"), &find_value);
					SendMessage(hwndPrev, WM_COMMAND, DB_FIND_NEXT, 0);
					return TRUE;
				}
				case IDCANCEL:
					EndDialog(hwndDlg, IDCANCEL);
					break;
			}
			switch(HIWORD(wParam)) {
				case BN_CLICKED:
					search_backwards = LOWORD(wParam) == IDC_RADIO_BACKWARDS;
					break;
			}
			break;
	}
	return FALSE;
}

const TCHAR* byte_to_binary(int x, BOOL isWord) {
	static TCHAR b[17];
	b[0] = '\0';
	int z;
	TCHAR *p = b;
	for (z = 32768; z > 0; z >>= 1) {
		*p++ = x & z ? '1' : '0';
	}
	if (!isWord)
		return b + 8;
	return b;
}

int ValueSubmit(HWND hwndDlg, TCHAR *loc, int size, int max_value) {
	TCHAR result[32];
	int got_line;
	if (hwndDlg == NULL) return 0;
	((WORD*)result)[0] = sizeof(result);	//string size
	got_line = (int) Edit_GetLine(hwndDlg, 0, result, sizeof(result));
	
	VALUE_FORMAT format = (VALUE_FORMAT) GetWindowLongPtr(hwndDlg, GWLP_USERDATA);
	
	char value[8];
	int i;
	/* Parse input here */

	if (got_line > 0) {
		switch (format) {
		case HEX2:
		case HEX4:
			_stscanf_s(result, _T("%x"), (int*) value);
			if (*((int *) value) > max_value)
				*((int *) value) = max_value;
			break;
		case FLOAT2:
		case FLOAT4:
			if (size == sizeof(float))
				_stscanf_s(result, _T("%f"), (float *) value);
			else
				_stscanf_s(result, _T("%lf"), (double *) value);
			if (*((float *) value) > max_value)
				*((float *) value) = (float) max_value;
			break;
		case DEC3:
			_stscanf_s(result, _T("%d"), (int*) value);
			if (*((int *) value) > max_value)
				*((int *) value) = max_value;
			break;
		case BIN16:
		case BIN8: {
			*((int *) value) = 0;
			for (int i = strlen(result) - 1, j = 1; i >= 0; i--, j <<= 1) {
				if (result[i] == '1') {
					*((int *) value) += j;
				}
			}
			if (*((int *) value) > max_value)
				*((int *) value) = max_value;
			break;
		}
		case CHAR1:
			_stscanf_s(result, _T("%c"), (char*) value);
			if (*((char *) value) > max_value)
				*((char *) value) = max_value;
			break;
		}
	} else {
		*((int*)value) = 0;
	}
	for (i = 0; i < size; i++) loc[i] = value[i];
	DestroyWindow(hwndDlg);
	return 0;
}

static WNDPROC wndProcEdit;

LRESULT CALLBACK ValueProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {

	switch (Message) {
		case WM_PAINT:
		{	
			SetWindowFont(hwnd, hfontLucida, FALSE);
			break;
		}
		case WM_KEYDOWN:
			switch (wParam) {
				case VK_RETURN:
					SendMessage(GetParent(hwnd), WM_COMMAND, 
						MAKEWPARAM(0, EN_SUBMIT), (LPARAM) hwnd);
					return 0;
				case VK_ESCAPE:
					SendMessage(GetParent(hwnd), WM_COMMAND, 
						MAKEWPARAM(0, EN_CANCEL), (LPARAM) hwnd);
					DestroyWindow(hwnd);
					return 0;
			}
	}
	return CallWindowProc(wndProcEdit, hwnd, Message, wParam, lParam);
}

void SubclassEdit(HWND hwndEdt, int edit_width, VALUE_FORMAT format) {
	if (hwndEdt) {
		wndProcEdit = (WNDPROC) SetWindowLongPtr(hwndEdt, GWLP_WNDPROC, (LONG_PTR) ValueProc);	
		SetWindowLongPtr(hwndEdt, GWLP_USERDATA, (DWORD) format);
		Edit_LimitText(hwndEdt, edit_width);
		Edit_SetSel(hwndEdt, 0, edit_width);
		SetFocus(hwndEdt);
	}
}

// Converts a hexadecimal string to integer
int xtoi(const TCHAR* xs) {
	int val;
	int error = sscanf(xs, _T("%X"), &val);
	if (error == EOF)
		return INT_MAX;
	return val;
}