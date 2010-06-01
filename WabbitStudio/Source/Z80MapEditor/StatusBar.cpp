#include "stdafx.h"

extern HWND hwndStatus;

void StatusBarPrintf(LPCTSTR lpszFormat, ...)
{
	TCHAR szStatus[256];

	va_list argList;
	va_start(argList, lpszFormat);
	StringCbVPrintf(szStatus, sizeof(szStatus), lpszFormat, argList);
	va_end(argList);

	SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM) szStatus);
}

// Pass -1 for z if you don't have a coordinate

void SetStatusBarCoordinates(int x, int y, int z)
{
	TCHAR szStatus[256];

	if (z != -1)
	{
		StringCbPrintf(szStatus, sizeof(szStatus), _T("\t%d, %d, %d"), x, y, z);
	}
	else
	{
		StringCbPrintf(szStatus, sizeof(szStatus), _T("\t%d, %d"), x, y);
	}
	SendMessage(hwndStatus, SB_SETTEXT, 1, (LPARAM) szStatus);
}
