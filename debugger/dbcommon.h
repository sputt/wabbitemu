#ifndef DBCOMMON_H
#define DBCOMMON_H

#include "gui.h"
#include "calc.h"

#include "dbreg.h"

int get_value(HWND hwndParent);
INT_PTR CALLBACK GotoDialogProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK FindDialogProc(HWND, UINT, WPARAM, LPARAM);
int ValueSubmit(HWND hwndDlg, TCHAR *loc, int size, int max_value = INT_MAX);
LRESULT CALLBACK ValueProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
void DrawItemSelection(HDC hdc, RECT *r, BOOL active, COLORREF breakpoint, int opacity);
const TCHAR * byte_to_binary(int x, BOOL isWord = FALSE);
int xtoi(const TCHAR *xs, int* result);

typedef enum {
	HEX2,
	HEX4,
	FLOAT2,
	FLOAT4,
	DEC3,
	DEC5,
	BIN8,
	BIN16,
	CHAR1,
} VALUE_FORMAT;

typedef enum {
	HEX,
	DEC,
	BIN,
} DISPLAY_BASE;

static const TCHAR* DisplayTypeString = _T("Disp_Type");

void SubclassEdit(HWND hwndEdt, int edit_width, VALUE_FORMAT format);

#define EN_CANCEL 0x9999

#endif /* DBCOMMON_H */
