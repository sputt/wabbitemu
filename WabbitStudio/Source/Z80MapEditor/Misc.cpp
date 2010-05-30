#include <windows.h>
#include <WindowsX.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <strsafe.h>
#include <assert.h>

#include "MapView.h"
#include "MapHierarchy.h"
#include "Misc.h"
#include "ObjectSelection.h"

#define MISC_COLOR RGB(100, 200, 100)
#define IDC_PROPERTY_BASE	(IDC_EDITX)


extern MAPSETSETTINGS g_MapSet;
extern HINSTANCE g_hInstance;

MISCTYPE MiscTypes[256];
DWORD MiscTypeCount = 0;

void AddMisc(LPMAPVIEWSETTINGS lpmvs, int x, int y) {
	MISC *lpmisc = &lpmvs->MiscArray[lpmvs->MiscCount];
	lpmisc->MiscID = 0;
	RECT target = {x, y, x+16, y+16};
	RECT limits = {0, 0, 255, 255};
	RECT result;
	IntersectRect(&result, &target, &limits);

	lpmisc->MagicNum = MISC_MAGIC_NUM;
	lpmisc->x = result.left;
	lpmisc->y = result.top;
	lpmisc->w = result.right - result.left;
	lpmisc->h = result.bottom - result.top;
	lpmisc->lpmvs = lpmvs;

	lpmvs->MiscCount++;
}

static void GetMiscRealRect(const LPMISC lpm, RECT *pr) {
	assert(lpm != NULL);

	pr->left = lpm->x * g_MapSet.Scale;
	pr->top = lpm->y * g_MapSet.Scale;
	pr->right = (lpm->x + lpm->w) * g_MapSet.Scale;
	pr->bottom = (lpm->y + lpm->h) * g_MapSet.Scale;
}


void DrawMisc(HDC hdc, LPMISC lpm) {
	SelectObject(hdc, GetStockObject(DC_PEN));
	SetDCPenColor(hdc, MISC_COLOR);
	SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));

	RECT r = {lpm->x, lpm->y, (lpm->x + lpm->w), (lpm->y + lpm->h)};
	Rectangle(hdc, r.left, r.top, r.right, r.bottom);
	if ((r.right - r.left) > 1 && (r.bottom - r.top) > 1) {
		Rectangle(hdc, r.left + 1, r.top + 1, r.right - 1, r.bottom - 1);
	}
	
	if (IsObjectSelected(lpm) == TRUE) {
		InflateRect(&r, 2, 2);
		SelectObject(hdc, GetStockObject(DC_PEN));
		SetDCPenColor(hdc, GetSysColor(COLOR_MENUHILIGHT));
		Rectangle(hdc, r.left, r.top, r.right, r.bottom);
		InflateRect(&r, -1, -1);
		Rectangle(hdc, r.left, r.top, r.right, r.bottom);
	}
}


int GetMiscEdge(LPMISC lpm, int x, int y) {
	RECT r;
	RECT or;
	RECT cr;

	POINT p = {x, y};
	GetMiscRealRect(lpm, &r);

	CopyRect(&or, &r);

	if (PtInRect(&r, p) == FALSE)
		return -1;

	int CornerAbs = 4 * g_MapSet.Scale;

	cr.left = or.left;
	cr.top = or.top;
	cr.bottom = or.top + CornerAbs;
	cr.right = or.left + CornerAbs;
	if (PtInRect(&cr, p) == TRUE) {
		return WMSZ_TOPLEFT;
	}

	cr.bottom = or.bottom;
	cr.top = or.bottom - CornerAbs;
	if (PtInRect(&cr, p) == TRUE) {
		return WMSZ_BOTTOMLEFT;
	}

	cr.left = or.right - CornerAbs;
	cr.right = or.right;
	if (PtInRect(&cr, p) == TRUE) {
		return WMSZ_BOTTOMRIGHT;
	}

	cr.top = or.top;
	cr.bottom = or.top + CornerAbs;
	if (PtInRect(&cr, p) == TRUE) {
		return WMSZ_TOPRIGHT;
	}

	InsetRect(&r, 2 * g_MapSet.Scale, 2 * g_MapSet.Scale);
	if (PtInRect(&r, p) == TRUE)
		return 0;

	if (p.x < r.left) {
		return WMSZ_LEFT;
	}
	if (p.x > r.right) {
		return WMSZ_RIGHT;
	}
	if (p.y < r.top) {
		return WMSZ_TOP;
	}
	if (p.y > r.bottom) {
		return WMSZ_BOTTOM;
	}

	return -1;
}


LPMISC CheckMiscMouse(LPMAPVIEWSETTINGS lpmvs, int x, int y) {
	for (DWORD i = 0; i < lpmvs->MiscCount; i++) {
		RECT r;
		POINT p = {x, y};
		GetMiscRealRect(&lpmvs->MiscArray[i], &r);
		if (PtInRect(&r, p) == TRUE) {
			return &lpmvs->MiscArray[i];
		}
	}
	return NULL;
}

void DrawMapMisc(HDC hdc, LPMAPVIEWSETTINGS lpmvs) {
	for (DWORD i = 0; i < lpmvs->MiscCount; i++) {
		DrawMisc(hdc, &lpmvs->MiscArray[i]);
	}
}


typedef enum {
	PTS_NAME,
	PTS_ARGS,
	PTS_PROPERTIES,
	PTS_MACRO,
	PTS_ENDMACRO,
	PTS_DONE,
} PARSETYPESSTATE;


BOOL LoadMiscTypes(LPCTSTR szFileName) {
	FILE *file = _tfopen(szFileName, _T("r"));
	if (file == NULL)
		return E_FAIL;

	PARSETYPESSTATE pts = PTS_NAME;
	TCHAR szBuffer[256];
	LPMISCTYPE lpmt = &MiscTypes[0];

	lpmt->ArgCount = 0;
	MiscTypeCount = 0;

	while (pts != PTS_DONE && _fgetts(szBuffer, ARRAYSIZE(szBuffer), file)) {
RedoSwitch:
		switch (pts)
		{
		case PTS_NAME:
			{
				if (lstrlen(szBuffer) == 0 || szBuffer[0] != _T(';'))
					continue;
				TCHAR *szDesc = _tcschr(szBuffer, _T('-'));
				if (szDesc == NULL)
					continue;
				*(szDesc - 1) = _T('\0');

				// Fill in some defaults
				StringCbCopy(lpmt->szName, sizeof(lpmt->szName), szBuffer + 1);
				StringCbCopy(lpmt->szDesc, sizeof(lpmt->szDesc), szDesc + 2);
				pts = PTS_ARGS;
				lpmt->ArgCount = 0;
				break;
			}
		case PTS_ARGS:
			{
				if (_tcsnccmp(szBuffer, _T("#macro"), 5) == 0) {
					pts = PTS_MACRO;
					goto RedoSwitch;
				}
				if (_tcsnccmp(szBuffer, _T(";Properties"), 11) == 0) {
					pts = PTS_PROPERTIES;
					goto RedoSwitch;
				}
				if (lstrlen(szBuffer) == 0 || szBuffer[0] != _T(';'))
					continue;
				TCHAR *szDesc = _tcschr(szBuffer, _T('-'));
				szDesc = _tcstok(szDesc, _T("\r\n"));
				if (szDesc == NULL)
					continue;
				*(szDesc - 1) = _T('\0');

				StringCbCopy(lpmt->szArgs[lpmt->ArgCount][0], 
					sizeof(lpmt->szArgs[lpmt->ArgCount][0]), szBuffer + 2);
				StringCbCopy(lpmt->szArgs[lpmt->ArgCount][1], 
					sizeof(lpmt->szArgs[lpmt->ArgCount][1]), szDesc + 2);
				lpmt->ArgCount++;
				break;
			}
		case PTS_PROPERTIES:
			{
				if (_tcsnccmp(szBuffer, _T("#macro"), 5) == 0) {
					pts = PTS_MACRO;
					goto RedoSwitch;
				}
				if (lstrlen(szBuffer) == 0 || szBuffer[0] != _T(';'))
					continue;
				TCHAR *szDesc = _tcschr(szBuffer, _T('='));
				TCHAR *szProp = szBuffer + 2;
				if (szDesc == NULL)
					continue;
				*(szDesc - 1) = _T('\0');
				szDesc += 2;
				break;
			}
		case PTS_MACRO:
			{
				TCHAR *szArgs = _tcschr(szBuffer, _T('('));
				if (szArgs == NULL)
					continue;
				*szArgs = _T('\0');
				StringCbCopy(lpmt->szMacro, sizeof(lpmt->szMacro), szBuffer + 7);
				pts = PTS_ENDMACRO;
				break;
			}
		case PTS_ENDMACRO:
			{
				char szLine[256];
				char szMacro[256];

				if (_tcsnccmp(szBuffer, _T("#endmacro"), 8) == 0) {
					lpmt++;
					MiscTypeCount++;
					pts = PTS_NAME;
				}

				break;
			}
		}
	}
	
	fclose(file);
	return TRUE;
}