#include "stdafx.h"
#include "dbprofile.h"
#include "calc.h"
#include "resource.h"

LRESULT CALLBACK ProfileDialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch(Message) {
		case WM_INITDIALOG:
			return TRUE;
		case WM_COMMAND: {
			switch(LOWORD(wParam)) {
				case IDC_PROFILE_OK: {
					char string[9];
					int output;
					HWND hWndEdit;
					hWndEdit = GetDlgItem(hwnd, IDC_LOW_EDT);
					SendMessage(hWndEdit, WM_GETTEXT, 8, (LPARAM)&string);
					xtoi((const TCHAR *)&string, &output);
					lpDebuggerCalc->profiler.lowAddress = output;
					hWndEdit = GetDlgItem(hwnd, IDC_HIGH_EDT);
					SendMessage(hWndEdit, WM_GETTEXT, 8, (LPARAM)&string);
					xtoi((const TCHAR *)&string, &output);
					lpDebuggerCalc->profiler.highAddress = output;
					hWndEdit = GetDlgItem(hwnd, IDC_BLOCK_EDT);
					SendMessage(hWndEdit, WM_GETTEXT, 8, (LPARAM)&string);
					output = _tstoi((const TCHAR *) &string);
					lpDebuggerCalc->profiler.blockSize = output;
					EndDialog(hwnd, IDOK);
					break;
				}
				case IDC_PROFILE_CANCEL:
					EndDialog(hwnd, IDCANCEL);
					break;
			}
			break;
		}
		case WM_CLOSE:
			EndDialog(hwnd, IDCANCEL);
			return FALSE;
	}
	return FALSE;//DefWindowProc(hwnd, Message, wParam, lParam);
}

// Converts a hexadecimal string to integer
int xtoi(const TCHAR* xs, int* result) {
	int i, szlen = (int) _tcslen(xs);
	int xv, fact;
	if (szlen <= 0)
		// Nothing to convert
		return 1;
	// Converting more than 32bit hexadecimal value?
	if (szlen>8) return 2; // exit
	// Begin conversion here
	*result = 0;
	fact = 1;
	// Run until no more character to convert
	for(i = szlen-1; i >= 0; i--) {
		if (isxdigit(*(xs + i))) {
			if (*(xs+i)>=97) {
				xv = ( *(xs+i) - 97) + 10;
			}
			else if ( *(xs+i) >= 65) {
				xv = (*(xs+i) - 65) + 10;
			} else {
				xv = *(xs+i) - 48;
			}
			*result += (xv * fact);
			fact *= 16;
		} else {
		// Conversion was abnormally terminated
		// by non hexadecimal digit, hence
		// returning only the converted with
		// an error value 4 (illegal hex character)
			return 4;
		}
	}
	return 0;
}