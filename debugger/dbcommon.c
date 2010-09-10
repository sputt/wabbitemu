#include "stdafx.h"

#include "dbcommon.h"
#include "gui.h"
#include "resource.h"
#include "label.h"

extern HINSTANCE g_hInst;

unsigned short goto_addr;

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
					char result[32];
					GetDlgItemText(hwndDlg, IDC_EDTGOTOADDR, result, 32);
					
					if (result[0] != '$') {
						label_struct *label;
						label = lookup_label(result);
#ifdef WINVER
						if (label == NULL) sscanf_s(result, "%x", &goto_addr);
#else
						if (label == NULL) sscanf(result, "%x", &goto_addr);
#endif
						else goto_addr = label->addr;
					} else {
#ifdef WINVER
						sscanf_s(result+1, "%x", &goto_addr);
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


int ValueSubmit(HWND hwndDlg, char *loc, int size) {
	char result[32];
	int got_line;
	if (hwndDlg == NULL) return 0;
	((WORD*)result)[0] = sizeof(result);	//string size
	got_line = (int) SendMessage(hwndDlg, EM_GETLINE, (WPARAM) 0, (LPARAM) result);
	
	VALUE_FORMAT format = (VALUE_FORMAT) GetWindowLongPtr(hwndDlg, GWLP_USERDATA);
	
	char value[8];
	int i;
	/* Parse input here */

	if (got_line > 0) {
		switch (format) {
		case HEX2:
		case HEX4:
#ifdef WINVER
			sscanf_s(result, "%x", (int*) value);
#else
			sscanf(result, "%x", (int*) value);
#endif
			break;
		case FLOAT2:
		case FLOAT4:
			if (size == sizeof(float))
#ifdef WINVER
				sscanf_s(result, "%f", (float *) value);
			else
				sscanf_s(result, "%lf", (double *) value);
#else
				sscanf(result, "%f", (float *) value);
			else
				sscanf(result, "%lf", (double *) value);
#endif
			break;
		case DEC:
#ifdef WINVER
			sscanf_s(result, "%d", (int*) value);
#else
			sscanf(result, "%d", (int*) value);
#endif
			break;
		case CHAR1:
#ifdef WINVER
			sscanf_s(result, "%c", (char*) value);
#else
			sscanf(result, "%c", (char*) value);
#endif
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
