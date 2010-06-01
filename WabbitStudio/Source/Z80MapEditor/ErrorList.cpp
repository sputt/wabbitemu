#include "stdafx.h"

extern HINSTANCE g_hInstance;

static HWND hwndList = NULL;

static void SetupColumns(HWND hwndList)
{
	LVCOLUMN lvc = {0};

	lvc.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_FMT | LVCF_WIDTH;
	lvc.iSubItem = 0;
	lvc.fmt = LVCFMT_LEFT;
	lvc.pszText = _T("File");
	lvc.cx = 100;
	ListView_InsertColumn(hwndList, lvc.iSubItem, &lvc);

	lvc.iSubItem = 1;
	lvc.fmt = LVCFMT_RIGHT;
	lvc.pszText = _T("Line");
	lvc.cx = 50;
	ListView_InsertColumn(hwndList, lvc.iSubItem, &lvc);

	lvc.iSubItem = 2;
	lvc.fmt = LVCFMT_LEFT;
	lvc.pszText = _T("Error text");
	lvc.cx = 250;
	ListView_InsertColumn(hwndList, lvc.iSubItem, &lvc);
}

// Given SPASM's output string, returns error count
int ProcessErrorsForErrorList(LPSTR lpszOutput)
{
	int nErrors = 0;
	LPSTR szLine = strtok(lpszOutput, "\r\n");
	while (szLine != NULL)
	{
		if (strstr(szLine, "error: ") != NULL)
		{
			LVITEMA lvi = {0};
			lvi.mask = LVIF_TEXT;
			lvi.iItem = 0;
			lvi.iSubItem = 2;
			lvi.pszText = szLine;
			ListView_InsertItem(hwndList, &lvi);
			nErrors++;
		}
		szLine = strtok(NULL, "\r\n");
	}

	return nErrors;
}


HWND CreateErrorList(HWND hwnd)
{
	hwndList = CreateWindowEx(
		0,
		WC_LISTVIEW,
		_T("Error list"),
		WS_OVERLAPPEDWINDOW | LVS_REPORT | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		400,
		300,
		hwnd,
		NULL,
		g_hInstance,
		NULL);

	SetupColumns(hwndList);
	return hwndList;
}

