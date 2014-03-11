#include "stdafx.h"

#include "dbcommon.h"
#include "gui.h"
#include "guidebug.h"
#include "resource.h"
#include "label.h"

extern HINSTANCE g_hInst;
static WNDPROC wpOrigEditProc;

static LRESULT APIENTRY EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_KEYUP:
		if (wParam == VK_RETURN) {
			SendMessage(GetParent(hwnd), WM_COMMAND, IDOK, 0);
			return FALSE;
		} else if (wParam == VK_ESCAPE) {
			SendMessage(GetParent(hwnd), WM_COMMAND, IDCANCEL, 0);
			return FALSE;
		}
	case WM_KEYDOWN:
		return FALSE;
	case WM_CHAR:
		if (wParam == VK_RETURN || wParam == VK_ESCAPE) {
			return FALSE;
		}
		break;
	}

	return CallWindowProc(wpOrigEditProc, hwnd, uMsg,
		wParam, lParam);
}

void position_goto_dialog(HWND hGotoDialog) {
	if (hGotoDialog == NULL) {
		return;
	}

	RECT client_rect, parent_rect;
	GetClientRect(GetParent(hGotoDialog), &parent_rect);
	GetClientRect(hGotoDialog, &client_rect);
	int client_height = client_rect.bottom - client_rect.top;
	SetWindowPos(hGotoDialog, NULL, 0, parent_rect.bottom - client_height - 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}


INT_PTR CALLBACK GotoDialogProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
	case WM_INITDIALOG: {
		HWND hEditAddr = GetDlgItem(hwndDlg, IDC_EDTGOTOADDR);
		wpOrigEditProc = (WNDPROC)SetWindowLongPtr(hEditAddr, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);
		LPDEBUGWINDOWINFO lpDebugInfo = (LPDEBUGWINDOWINFO)lParam;
		SetWindowLongPtr(hwndDlg, GWLP_USERDATA, (LONG_PTR)lpDebugInfo);
		return TRUE;
	}
	case WM_ACTIVATE: {
		if (0 == wParam) {
			hwndCurrentDlg = NULL;
		} else {
			hwndCurrentDlg = hwndDlg;
		}

		return FALSE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			TCHAR result[64];
			GetDlgItemText(hwndDlg, IDC_EDTGOTOADDR, result, 64);
			LPDEBUGWINDOWINFO lpDebugInfo = (LPDEBUGWINDOWINFO)GetWindowLongPtr(hwndDlg, GWLP_USERDATA);
			if (lpDebugInfo == NULL) {
				break;
			}

			LPCALC lpCalc = lpDebugInfo->lpCalc;
			int goto_addr;
			BOOL success = FALSE;

			if (*result != '\0') {
				if (result[0] != '$') {
					label_struct *label;
					label = lookup_label(lpCalc, result);
					if (label == NULL) {
						success = _stscanf(result, _T("%x"), &goto_addr);
					} else {
						goto_addr = label->addr;
					}
				} else {
					success = _stscanf(result + 1, _T("%x"), &goto_addr);
				}
			}

			if (!success) {
				goto_addr = lpCalc->cpu.pc;
			}

			SendMessage(GetParent(hwndDlg), WM_USER, DB_GOTO_RESULT, goto_addr);
			EndDialog(hwndDlg, IDOK);
			DestroyWindow(hwndDlg);
			return TRUE;
		}
		case IDCANCEL:
			SetWindowLongPtr(GetDlgItem(hwndDlg, IDC_EDTGOTOADDR), GWLP_WNDPROC,
				(LONG)wpOrigEditProc);

			SendMessage(GetParent(hwndDlg), WM_USER, DB_GOTO_RESULT, -1);
			EndDialog(hwndDlg, IDCANCEL);
			DestroyWindow(hwndDlg);
			return TRUE;
		}
		switch (HIWORD(wParam)) {
		case BN_CLICKED: {
			switch (LOWORD(wParam)) {
			case IDC_GOTOCLOSE:
				SendMessage(GetParent(hwndDlg), WM_USER, DB_GOTO_RESULT, -1);
				EndDialog(hwndDlg, IDCANCEL);
				DestroyWindow(hwndDlg);
				return TRUE;
			}
			break;
		}
		break;
		}
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

int ValueSubmit(HWND hwndDlg, void *loc, int size, int max_value) {
	TCHAR result[32] = {0};
	int got_line;
	if (hwndDlg == NULL) {
		return 0;
	}

	((WORD *)result)[0] = sizeof(result);	//string size
	got_line = (int) Edit_GetLine(hwndDlg, 0, result, sizeof(result));
	
	VALUE_FORMAT format = (VALUE_FORMAT) GetWindowLongPtr(hwndDlg, GWLP_USERDATA);
	
	char value[8];
	int i;

	/* Parse input here */
	if (got_line > 0) {
		switch (format) {
		case HEX2:
		case HEX4:
			_stscanf(result, _T("%x"), (int *) value);
			if (*((int *) value) > max_value)
				*((int *) value) = max_value;
			break;
		case FLOAT2:
		case FLOAT4:
			if (size == sizeof(float)) {
				_stscanf(result, _T("%f"), (float *) value);
			} else {
				_stscanf(result, _T("%lf"), (double *) value);
			}

			if (*((float *) value) > max_value) {
				*((float *) value) = (float) max_value;
			}
			break;
		case DEC3:
			_stscanf(result, _T("%d"), (int*) value);
			if (*((int *) value) > max_value) {
				*((int *) value) = max_value;
			}
			break;
		case BIN16:
		case BIN8: {
			*((int *) value) = 0;
			for (int i = _tcslen(result) - 1, j = 1; i >= 0; i--, j <<= 1) {
				if (result[i] == '1') {
					*((int *) value) += j;
				}
			}
			if (*((int *) value) > max_value) {
				*((int *) value) = max_value;
			}
			break;
		}
		case CHAR1:
			_stscanf(result, _T("%c"), (char *) value);
			if (*((char *)value) > (char)max_value) {
				*((char *) value) = (char)max_value;
			}
			break;
		}
	} else {
		*((int *)value) = 0;
	}

	for (i = 0; i < size; i++) {
		((u_char *) loc)[i] = value[i];
	}

	DestroyWindow(hwndDlg);
	return 0;
}

static WNDPROC wndProcEdit;

static LRESULT CALLBACK ValueProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {

	switch (Message) {
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

void SubclassEdit(HWND hwndEdt, HFONT hfontLucida, int edit_width, VALUE_FORMAT format) {
	if (hwndEdt) {
		wndProcEdit = (WNDPROC) SetWindowLongPtr(hwndEdt, GWLP_WNDPROC, (LONG_PTR) ValueProc);
		SetWindowLongPtr(hwndEdt, GWLP_USERDATA, (DWORD) format);
		SetWindowFont(hwndEdt, hfontLucida, FALSE);
		Edit_LimitText(hwndEdt, edit_width);
		Edit_SetSel(hwndEdt, 0, edit_width);
		SetFocus(hwndEdt);
	}
}

// Converts a hexadecimal string to integer
int xtoi(const TCHAR *xs) {
	int val;
	int error = _stscanf(xs, _T("%X"), &val);
	if (error == EOF)
		return INT_MAX;
	return val;
}

int StringToValue(TCHAR *str) {
	int value = 0;
	int len = _tcslen(str) - 1;
	BOOL is_bin = FALSE, is_hex = FALSE;
	if (!_tcsicmp(str, _T("True"))) {
		return TRUE;
	} else if (!_tcsicmp(str, _T("False"))) {
		return FALSE;
	} 
	
	if (*str == '$') {
		is_hex = TRUE;
		str++;
	} else if (*str == '%') {
		str++;
		is_bin = TRUE;
	} else if (str[len] == 'b') {
		str[len] = '\0';
		is_bin = TRUE;
	} else if (str[len] == 'h') {
		str[len] = '\0';
		is_hex = TRUE;
	}

	if (is_hex) {
		value = xtoi(str);
	} else if (is_bin) {
		for (int i = 0; i < len; i++) {
			value <<= 1;
			if (str[i] == '1') {
				value += 1;
			}
			else if (str[i] != '0') {
				// error parsing assume 0
				value = INT_MAX;
				break;
			}
		}
	} else {
		value = _ttoi(str);
	}

	return value;
}

void DrawItemSelection(HDC hdc, RECT *r, BOOL active, COLORREF breakpoint, BYTE opacity) {

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

	gRect.UpperLeft = 0;
	gRect.LowerRight = 1;


	if (active == TRUE) {
		rgbSel = RGB(153, 222, 253);
		//rgbSel = RGB(24, 153, 255);
	}
	else {
		rgbSel = RGB(190, 190, 190);
	}

	if (breakpoint)
		rgbSel = breakpoint;

	vert[0].x = 2;
	vert[0].y = 1;
	vert[0].Red = 0xff00;
	vert[0].Green = 0xff00;
	vert[0].Blue = 0xff00;

	vert[1].x = r->right - r->left - 2;
	vert[1].y = (r->bottom - r->top - 2) * 2;
	vert[1].Red = GetRValue(rgbSel) << 8;
	vert[1].Green = GetGValue(rgbSel) << 8;
	vert[1].Blue = GetBValue(rgbSel) << 8;

	GradientFill(hdcSel, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V);


	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = opacity;
	bf.AlphaFormat = 0;

	SelectObject(hdcSel, GetStockObject(NULL_BRUSH));
	SetDCPenColor(hdcSel, rgbSel);

	RoundRect(hdcSel, 0, 0, r->right - r->left, r->bottom - r->top, 5, 5);

	SetDCPenColor(hdcSel, GetPixel(hdcSel, 3, (r->bottom - r->top) / 3));
	RoundRect(hdcSel, 1, 1, r->right - r->left - 1, r->bottom - r->top - 1, 5, 5);


	tl = GetPixel(hdc, r->left, r->top);
	tr = GetPixel(hdc, r->right - 1, r->top);
	br = GetPixel(hdc, r->right - 1, r->bottom - 1);
	bl = GetPixel(hdc, r->left, r->bottom - 1);

	AlphaBlend(hdc, r->left, r->top, r->right - r->left, r->bottom - r->top,
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