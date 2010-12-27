#include "stdafx.h"

#include "dbcommon.h"
#include "gui.h"
#include "resource.h"
#include "label.h"

extern HINSTANCE g_hInst;

unsigned short goto_addr;
int find_value;
BOOL search_backwards;

extern HFONT hfontLucida;
HWND hwndPrev;

INT_PTR CALLBACK GotoDialogProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND edtAddr;
	switch (Message) {
		case WM_INITDIALOG:
			edtAddr = GetDlgItem(hwndDlg, IDC_EDTGOTOADDR);
			SetFocus(GetDlgItem(hwndDlg, IDC_EDTGOTOADDR));
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
#ifdef WINVER
						if (label == NULL) _stscanf_s(result, _T("%x"), &goto_addr);
#else
						if (label == NULL) sscanf(result, "%x", &goto_addr);
#endif
						else goto_addr = label->addr;
					} else {
#ifdef WINVER
						_stscanf_s(result+1, _T("%x"), &goto_addr);
#else
						sscanf(result+1, "%x", &goto_addr);
#endif
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


int ValueSubmit(HWND hwndDlg, TCHAR *loc, int size, int max_value) {
	TCHAR result[32];
	int got_line;
	if (hwndDlg == NULL) return 0;
	((WORD*)result)[0] = sizeof(result);	//string size
	got_line = (int) Edit_GetLine(hwndDlg, 0, result, sizeof(result));//SendMessage(hwndDlg, EM_GETLINE, (WPARAM) 0, (LPARAM) result);
	
	VALUE_FORMAT format = (VALUE_FORMAT) GetWindowLongPtr(hwndDlg, GWLP_USERDATA);
	
	char value[8];
	int i;
	/* Parse input here */

	if (got_line > 0) {
		switch (format) {
		case HEX2:
		case HEX4:
#ifdef WINVER
			_stscanf_s(result, _T("%x"), (int*) value);
#else
			sscanf(result, "%x", (int*) value);
#endif
			if (*((int *) value) > max_value)
				*((int *) value) = max_value;
			break;
		case FLOAT2:
		case FLOAT4:
			if (size == sizeof(float))
#ifdef WINVER
				_stscanf_s(result, _T("%f"), (float *) value);
			else
				_stscanf_s(result, _T("%lf"), (double *) value);
#else
				sscanf(result, "%f", (float *) value);
			else
				sscanf(result, "%lf", (double *) value);
#endif
			if (*((float *) value) > max_value)
				*((float *) value) = (float) max_value;
			break;
		case DEC:
#ifdef WINVER
			_stscanf_s(result, _T("%d"), (int*) value);
#else
			sscanf(result, "%d", (int*) value);
#endif
			if (*((int *) value) > max_value)
				*((int *) value) = max_value;
			break;
		case CHAR1:
#ifdef WINVER
			_stscanf_s(result, _T("%c"), (char*) value);
#else
			sscanf(result, "%c", (char*) value);
#endif
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
			SendMessage(hwnd, WM_SETFONT, 
				(WPARAM) hfontLucida, (LPARAM) FALSE);
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
		SetWindowLong(hwndEdt, GWLP_USERDATA, (DWORD) format);
		SendMessage(hwndEdt, EM_SETLIMITTEXT, edit_width, 0);
		SendMessage(hwndEdt, EM_SETSEL, 0, edit_width);
		SetFocus(hwndEdt);
	}
}
