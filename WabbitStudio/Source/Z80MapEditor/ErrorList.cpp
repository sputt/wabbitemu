#include "stdafx.h"

static void SetupColumns(HWND hwndList)
{
	LVCOLUMN lvc = {0};

	lvc.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_FMT;
	lvc.iSubItem = 0;
	lvc.fmt = LVCFMT_LEFT;
	lvc.pszText = _T("Error text");
}

HWND CreateErrorList(HWND hwnd)
{

}

