#ifndef DBCOMMON_H
#define DBCOMMON_H

#include "gui.h"
#include <windows.h>
#include "calc.h"

#include "dbreg.h"

int get_value(HWND hwndParent);
INT_PTR CALLBACK GotoDialogProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam);
int ValueSubmit(HWND hwndDlg, char *loc, int size);
LRESULT CALLBACK ValueProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

typedef enum {
	HEX2,
	HEX4,
	FLOAT2,
	FLOAT4,
	DEC,
	CHAR1,
} VALUE_FORMAT;

void SubclassEdit(HWND hwndEdt, int edit_width, VALUE_FORMAT format);

#define EN_CANCEL 0x9999

#endif /* DBCOMMON_H */
