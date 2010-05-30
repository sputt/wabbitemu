#include <windows.h>
#include <WindowsX.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <strsafe.h>
#include <assert.h>

#include "MapView.h"
#include "MapHierarchy.h"
#include "MapObjects.h"
#include "ObjectGraphics.h"
#include "ObjectSelection.h"
#include "Layers.h"

#include "utils.h"
#include "spasm.h"

extern MAPSETSETTINGS g_MapSet;
extern HINSTANCE g_hInstance;
extern LAYER g_Layer;

OBJECTTYPE ObjectTypes[256];
DWORD ObjectTypeCount = 0;

OBJECTTYPE EnemyTypes[256];
DWORD EnemyTypeCount = 0;

extern int g_AnimateTypeCount;
extern ANIMATETYPE g_AnimateTypes[16];

#define MASK_COLOR RGB(168, 230, 29)

TCHAR read_expr (TCHAR** ptr, TCHAR word[256], const TCHAR *delims);

/*
 * Given a zelda object, set all of the editor object fields you can
 */
HRESULT SetObjectFromReal(LPOBJECT lpo, const ZOBJECT *pzobj) {
	if (lpo == NULL || pzobj == NULL)
		return E_FAIL;

	lpo->x = pzobj->x;
	lpo->w = pzobj->w;
	lpo->y = pzobj->y;
	lpo->h = pzobj->h;
	lpo->z = pzobj->z;
	lpo->d = pzobj->d;

	lpo->lpImage = GetObjectImage(pzobj->anim_ptr);
	return S_OK;
}

HRESULT SetEnemyFromReal(LPOBJECT lpo, const ZENEMY *pzenemy) {
	if (lpo == NULL || pzenemy == NULL)
		return E_FAIL;

	lpo->x = pzenemy->x;
	lpo->w = pzenemy->w;
	lpo->y = pzenemy->y;
	lpo->h = pzenemy->h;
	lpo->z = pzenemy->z;
	lpo->d = pzenemy->d;

	lpo->lpImage = GetObjectImage(pzenemy->anim_ptr);
	return S_OK;
}

static void GetObjectRect(const LPOBJECT lpo, RECT *pr) {
	int new_x = lpo->x, new_y = lpo->y;

	if (lpo->lpImage == NULL)
		return;
	
	new_x -= (lpo->lpImage->cxImage - (int) lpo->w) / 2;
	new_y -= (lpo->lpImage->cyImage - (int) lpo->h);
	new_y -= lpo->z;

	pr->left = new_x;
	pr->top = new_y;
	pr->right = pr->left + lpo->lpImage->cxImage;
	pr->bottom = pr->top + lpo->lpImage->cyImage;
}

static void GetObjectRealRect(const LPOBJECT lpo, RECT *pr) {
	assert(lpo != NULL);

	GetObjectRect(lpo, pr);

	pr->left *= g_MapSet.Scale;
	pr->top *= g_MapSet.Scale;
	pr->right *= g_MapSet.Scale;
	pr->bottom *= g_MapSet.Scale;
}

void DrawObject(HDC hdc, LPOBJECT lpo, COLORREF cr) {
	static HDC hdcBuffer = NULL;
	static HBITMAP hbmBuffer = NULL;

	if (hdcBuffer == NULL) {
		hdcBuffer = CreateCompatibleDC(hdc);
		hbmBuffer = CreateCompatibleBitmap(hdc, 64 * 8, 64 * 8);
		SelectObject(hdcBuffer, hbmBuffer);
	}

	RECT r;
	GetObjectRect(lpo, &r);

	if (lpo->lpImage == NULL)
		return;

	

	if (g_MapSet.fTesting == TRUE) {
		SelectObject(hdcBuffer, GetStockObject(BLACK_BRUSH));
	} else {
		SelectObject(hdcBuffer, GetStockObject(DC_BRUSH));
		SetDCBrushColor(hdcBuffer, cr);	
	}
	if (g_Layer == MAP_LAYER) {
		BLENDFUNCTION bf = {0};
		bf.BlendOp = AC_SRC_OVER;
		bf.SourceConstantAlpha = 160;
		bf.AlphaFormat = 0;

		BitBlt(hdcBuffer, 0, 0, r.right - r.left, r.bottom - r.top, hdc, r.left, r.top, SRCCOPY);
		AlphaBlend(hdcBuffer, 0, 0, r.right - r.left, r.bottom - r.top, 
			lpo->lpImage->hdc, lpo->lpImage->cxMargin, lpo->lpImage->cyMargin, lpo->lpImage->cxImage, lpo->lpImage->cyImage, bf);

		StretchBlt(hdc, r.left, r.top, r.right - r.left, r.bottom - r.top,
			lpo->lpImage->hdcMask, lpo->lpImage->cxMargin, lpo->lpImage->cyMargin, lpo->lpImage->cxImage, lpo->lpImage->cyImage, SRCPAINT);

		BitBlt(hdc, r.left, r.top, r.right - r.left, r.bottom - r.top,
			hdcBuffer, 0, 0, SRCAND);

	} else {
		Rectangle(hdcBuffer, -1, -1, r.right - r.left + 2, r.bottom - r.top + 2);
		StretchBlt(hdcBuffer, 0, 0, r.right - r.left, r.bottom - r.top,
			lpo->lpImage->hdc, lpo->lpImage->cxMargin, lpo->lpImage->cyMargin, lpo->lpImage->cxImage, lpo->lpImage->cyImage, SRCPAINT);

		StretchBlt(hdc, r.left, r.top, r.right - r.left, r.bottom - r.top,
			lpo->lpImage->hdcMask, lpo->lpImage->cxMargin, lpo->lpImage->cyMargin, lpo->lpImage->cxImage, lpo->lpImage->cyImage, SRCPAINT);

		BitBlt(hdc, r.left, r.top, r.right - r.left, r.bottom - r.top, hdcBuffer, 0, 0, SRCAND);
	}

	SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
	SetDCPenColor(hdc, GetSysColor(COLOR_MENUHILIGHT));
	SelectObject(hdc, GetStockObject(DC_PEN));

	if (IsObjectSelected(lpo) == TRUE) {
		Rectangle(hdc, r.left, r.top, r.right, r.bottom);
		InflateRect(&r, -1, -1);
		Rectangle(hdc, r.left, r.top, r.right, r.bottom);
	}
}

LPOBJECT CheckObjectMouse(LPMAPVIEWSETTINGS lpmvs, int x, int y) {
	if (g_Layer == ENEMY_LAYER)
	{
		for (DWORD i = 0; i < lpmvs->EnemyCount; i++) {
			RECT r;
			POINT p = {x, y};
			GetObjectRealRect(&lpmvs->EnemyArray[i], &r);
			if (PtInRect(&r, p) == TRUE) {
				return &lpmvs->EnemyArray[i];
			}
		}
	} else if (g_Layer == OBJECT_LAYER) {
		for (DWORD i = 0; i < lpmvs->ObjectCount; i++) {
			RECT r;
			POINT p = {x, y};
			GetObjectRealRect(&lpmvs->ObjectArray[i], &r);
			if (PtInRect(&r, p) == TRUE) {
				return &lpmvs->ObjectArray[i];
			}
		}
	}
	return NULL;
}


void DrawMapEnemies(HDC hdc, LPMAPVIEWSETTINGS lpmvs) {
	for (DWORD i = 0; i < lpmvs->EnemyCount; i++) {
		DrawObject(hdc, &lpmvs->EnemyArray[i], RGB(250, 120, 40));
	}
}

void DrawMapObjects(HDC hdc, LPMAPVIEWSETTINGS lpmvs) {
	for (DWORD i = 0; i < lpmvs->ObjectCount; i++) {
		DrawObject(hdc, &lpmvs->ObjectArray[i], RGB(0, 0, 255));
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

LPOBJECTTYPE LookupObjectType(TCHAR *szName) {
	for (int i = 0; i < ObjectTypeCount; i++) {
		if (_tcscmp(szName, ObjectTypes[i].szMacro) == 0)
			return &ObjectTypes[i];
	}
	return NULL;
}

LPOBJECTTYPE LookupEnemyType(LPCTSTR lpszName) {
	for (int i = 0; i < EnemyTypeCount; i++) {
		if (_tcscmp(lpszName, EnemyTypes[i].szMacro) == 0)
			return &EnemyTypes[i];
	}
	return NULL;
}



HRESULT CreateObject(LPOBJECTTYPE lpot, int x, int y, LPOBJECT lpo) {
	ZeroMemory(lpo, sizeof(OBJECT));

	lpo->MagicNum = OBJECT_MAGIC_NUM;
	lpo->lpType = lpot;
	char szLine[256];
	CreateObjectOutput(lpo, szLine, sizeof(szLine), FALSE);

	ZOBJECT zobj = {0};
	RunAssemblyWithArguments(szLine, (BYTE *) &zobj, sizeof(zobj));
	SetObjectFromReal(lpo, &zobj);

	lpo->x = x;
	lpo->y = y;
	return S_OK;
}

HRESULT CreateEnemy(LPOBJECTTYPE lpot, int x, int y, LPOBJECT lpenemy) {
	ZeroMemory(lpenemy, sizeof(OBJECT));

	lpenemy->MagicNum = ENEMY_MAGIC_NUM;
	lpenemy->x = x;
	lpenemy->y = y;
	lpenemy->lpType = lpot;

	char szLine[256];
	CreateObjectOutput(lpenemy, szLine, sizeof(szLine), FALSE);

	ZENEMY zenemy = {0};
	RunAssemblyWithArguments(szLine, (LPBYTE) &zenemy, sizeof(zenemy));
	SetEnemyFromReal(lpenemy, &zenemy);
	return S_OK;
}

int AddObject(LPMAPVIEWSETTINGS lpmvs, LPOBJECTTYPE lpot, int x, int y) {
	if (lpmvs->ObjectCount == MAX_OBJECT)
		return -1;

	OBJECT obj = {0};
//	CreateObject(lpot, x - (lpot->cxObject / 2), y - (lpot->cyObject / 2), &obj);
	CreateObject(lpot, x, y, &obj);

	obj.lpmvs = lpmvs;
	lpmvs->ObjectArray[lpmvs->ObjectCount] = obj;
	
	lpmvs->ObjectCount++;
	InvalidateRect(lpmvs->hwndMap, NULL, FALSE);
	return lpmvs->ObjectCount - 1;
}

int AddEnemy(LPMAPVIEWSETTINGS lpmvs, LPOBJECTTYPE lpot, int x, int y) {
	if (lpmvs->EnemyCount == MAX_ENEMY)
		return -1;

	OBJECT obj = {0};
//	CreateObject(lpot, x - (lpot->cxObject / 2), y - (lpot->cyObject / 2), &obj);
	CreateEnemy(lpot, x, y, &obj);

	obj.lpmvs = lpmvs;
	lpmvs->EnemyArray[lpmvs->EnemyCount] = obj;
	
	lpmvs->EnemyCount++;
	InvalidateRect(lpmvs->hwndMap, NULL, FALSE);
	return lpmvs->EnemyCount - 1;
}

HRESULT LoadMapDefaults(TCHAR *szFilename, LPMAPVIEWSETTINGS lpmvs) {
	if (szFilename == NULL)
		return E_FAIL;

	FILE *file = _tfopen(szFilename, _T("r"));
	TCHAR szBuffer[256];

	if (file == NULL)
		return E_FAIL;

	StringCbCopy(lpmvs->szDefaultsFilename, sizeof(lpmvs->szDefaultsFilename), szFilename);

	TCHAR szLabel[256];
	StringCbCopy(szLabel, sizeof(szLabel), lpmvs->szName);
	StringCbCat(szLabel, sizeof(szLabel), _T("_defaults:\n"));
	SeekFileLabel(file, szLabel);

	while (_fgetts(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), file)) {
		if (_tcsncmp(szBuffer, _T("animate_section"), 15) == 0) {
			break;
		}
	}

	if (feof(file))
		return E_FAIL;

	lpmvs->AnimateCount = 0;

	while (_fgetts(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), file) && (szBuffer[0] == _T('\n') || szBuffer[0] == _T('\t'))) {
		TCHAR szWord[256];

		if (szBuffer[1] == _T('.') || lstrlen(szBuffer) < 4)
			continue;

		ZANIMATE zanim = {0};
		
#ifdef _UNICODE
		char macro_line[256];
		WideCharToMultiByte(CP_ACP, 0, szBuffer, -1, macro_line, sizeof(macro_line), NULL, NULL);
#else
		char *macro_line = szBuffer;
#endif
		RunAssemblyWithArguments(macro_line, (BYTE *) &zanim, sizeof(zanim));

		TCHAR *szName = _tcstok(szBuffer, _T("\t ("));
		if (szName == NULL)
			continue;

		LPANIMATE lpa = &lpmvs->AnimateArray[lpmvs->AnimateCount++];
		SetAnimateFromReal(lpa, &zanim);

		for (int i = 0; i < g_AnimateTypeCount; i++) {
			char name[64];
			WideCharToMultiByte(CP_ACP, 0, szName, -1, name, sizeof(name), NULL, NULL);
			if (_stricmp(g_AnimateTypes[i].szMacro, name) == 0) {
				lpa->lpType = &g_AnimateTypes[i];
				break;
			}
		}
	}

	do {
		if (_tcsncmp(szBuffer, _T("object_section"), 14) == 0) {
			break;
		}
	} while (_fgetts(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), file));

	if (feof(file))
		return E_FAIL;

	lpmvs->ObjectCount = 0;

	while (_fgetts(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), file) && (szBuffer[0] == _T('\n') || szBuffer[0] == _T('\t'))) {
		TCHAR szWord[256];

		if (szBuffer[1] == _T('.'))
			continue;

		ZOBJECT zobj = {0};
		
		char macro_line[256];
		WideCharToMultiByte(CP_ACP, 0, szBuffer, -1, macro_line, sizeof(macro_line), NULL, NULL);
		RunAssemblyWithArguments(macro_line, (BYTE *) &zobj, sizeof(zobj));

		TCHAR *szArgs = _tcschr(szBuffer, _T('('));
		if (szArgs == NULL)
			continue;

		LPOBJECT lpo = &lpmvs->ObjectArray[lpmvs->ObjectCount++];
		lpo->MagicNum = OBJECT_MAGIC_NUM;
		lpo->lpmvs = lpmvs;
		SetObjectFromReal(lpo, &zobj);

		*szArgs = _T('\0');
		LPOBJECTTYPE lpot = LookupObjectType(szBuffer + 1);

		szArgs++;
		int i = 0;
		while (read_expr(&szArgs, lpo->szArgs[i++], _T(","))) ;

		lpo->lpType = lpot;
	}

	// ENEMY SECTION
	do {
		if (_tcsncmp(szBuffer, _T("enemy_section"), 13) == 0) {
			break;
		}
	} while (_fgetts(szBuffer, ARRAYSIZE(szBuffer), file));

	if (feof(file))
		return E_FAIL;

	lpmvs->EnemyCount = 0;

	while (_fgetts(szBuffer, ARRAYSIZE(szBuffer), file) && (szBuffer[0] == _T('\n') || szBuffer[0] == _T('\t'))) {
		TCHAR szWord[256];

		if (szBuffer[1] == _T('.'))
			continue;

		ZENEMY zenemy = {0};
		
		char macro_line[256];
		WideCharToMultiByte(CP_ACP, 0, szBuffer, -1, macro_line, sizeof(macro_line), NULL, NULL);
		RunAssemblyWithArguments(macro_line, (BYTE *) &zenemy, sizeof(zenemy));

		TCHAR *szArgs = _tcschr(szBuffer, _T('('));
		if (szArgs == NULL)
			continue;

		LPOBJECT lpo = &lpmvs->EnemyArray[lpmvs->EnemyCount++];
		lpo->MagicNum = ENEMY_MAGIC_NUM;
		lpo->lpmvs = lpmvs;
		SetEnemyFromReal(lpo, &zenemy);

		*szArgs = _T('\0');
		LPOBJECTTYPE lpot = LookupEnemyType(szBuffer + 1);

		szArgs++;
		int i = 0;
		while (read_expr(&szArgs, lpo->szArgs[i++], _T(","))) ;

		lpo->lpType = lpot;
	}

	// MISC SECTION
	do {
		if (_tcsncmp(szBuffer, _T("misc_section"), 12) == 0) {
			break;
		}
	} while (_fgetts(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), file));

	if (feof(file))
		return E_FAIL;

	lpmvs->MiscCount = 0;

	while (_fgetts(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), file) && (szBuffer[0] == _T('\n') || szBuffer[0] == _T('\t'))) {
		TCHAR szArgCopy[ARRAYSIZE(szBuffer)];
		StringCbCopy(szArgCopy, sizeof(szArgCopy), szBuffer);

		if (szBuffer[1] == _T('.'))
			continue;

		TCHAR *szArgs = _tcschr(szBuffer, _T('('));
		if (szArgs == NULL)
			continue;

		LPMISC lpm = &lpmvs->MiscArray[lpmvs->MiscCount++];
		szArgs[0] = _T('\0');
		_stscanf(szArgs + 1, _T("%d,%d,%d,%d"), &lpm->x, &lpm->y, &lpm->w, &lpm->h);

		_tcstok(szBuffer, _T("\t "));

		lpm->lpmvs = lpmvs;

		extern MISCTYPE MiscTypes[256];
		extern DWORD MiscTypeCount;

		TCHAR *lpszName = _tcstok(szBuffer, _T("\t\n "));
		for (int i = 0; i < MiscTypeCount; i++) {
			if (_tcsicmp(lpszName, MiscTypes[i].szMacro) == 0) {
				lpm->MiscID = i;
				break;
			}
		}
		lpm->MagicNum = MISC_MAGIC_NUM;

		szArgs = _tcschr(szArgCopy, _T('('));
		if (szArgs == NULL)
			continue;

		szArgs++;
		int i = 0;
		while (read_expr(&szArgs, lpm->szArgs[i++], _T(","))) ;
	}

	fclose(file);
}



HRESULT CreateObjectOutput(LPOBJECT lpo, char *szOutput, int cbOutput, BOOL fIncludeOptional) {

	char szArgs[256];

	if (lpo->lpType == NULL)
		return E_FAIL;

	StringCbCopyA(szArgs, sizeof(szArgs), "");

	for (int i = 0; i < lpo->lpType->ArgCount; i++) {
		TCHAR *lpszArgName = lpo->lpType->szArgs[i][0];
		char szValue[32];
		BOOL fSkip = FALSE;
		BOOL fIsOptional = FALSE;

		if (_tcsstr(lpo->lpType->szArgs[i][1], _T("[optional]")) != NULL) {
			fIsOptional = TRUE;
		}

		if (fIsOptional == TRUE && fIncludeOptional == FALSE) {
			fSkip = TRUE;
		} else {
			if (_tcscmp(lpszArgName, _T("x")) == 0) {
				StringCbPrintfA(szValue, sizeof(szValue), "%1.lf", lpo->x);
			} else if (_tcscmp(lpszArgName, _T("w")) == 0) {
				StringCbPrintfA(szValue, sizeof(szValue), "%1.lf", lpo->w);
			} else if (_tcscmp(lpszArgName, _T("y")) == 0) {
				StringCbPrintfA(szValue, sizeof(szValue), "%1.lf", lpo->y);
			} else if (_tcscmp(lpszArgName, _T("h")) == 0) {
				StringCbPrintfA(szValue, sizeof(szValue), "%1.lf", lpo->h);
			} else if (_tcscmp(lpszArgName, _T("z")) == 0) {
				StringCbPrintfA(szValue, sizeof(szValue), "%1.lf", lpo->z);
			} else if (_tcscmp(lpszArgName, _T("d")) == 0) {
				StringCbPrintfA(szValue, sizeof(szValue), "%1.lf", lpo->d);
			} else {
				if (lstrlen(lpo->szArgs[i]) > 0) {
					WideCharToMultiByte(CP_ACP, 0, lpo->szArgs[i], -1, szValue, sizeof(szValue), NULL, NULL);
				} else {
					StringCbCopyA(szValue, sizeof(szValue), "0");
				}
			}
		}

		if (fSkip == FALSE) {
			StringCbCatA(szArgs, sizeof(szArgs), szValue);
			StringCbCatA(szArgs, sizeof(szArgs), ",");
		}
	}
	szArgs[strlen(szArgs) - 1] = _T('\0');

	char szMacro[256];
	WideCharToMultiByte(CP_ACP, 0, lpo->lpType->szMacro, -1, szMacro, sizeof(szMacro), NULL, NULL);
	StringCbPrintfA(szOutput, cbOutput, "%s(%s)", szMacro, szArgs);
	return S_OK;
}


HRESULT LoadObjectTypes(TCHAR *szFilename, LPOBJECTTYPE lpTypes, LPDWORD lpdwCount) {
	FILE *file = _tfopen(szFilename, _T("r"));
	if (file == NULL)
		return E_FAIL;

	PARSETYPESSTATE pts = PTS_NAME;
	TCHAR szBuffer[256];
	OBJECTTYPE *pObjectType = lpTypes;

	pObjectType->ArgCount = 0;
	*lpdwCount = 0;

	while (pts != PTS_DONE && _fgetts(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), file)) {
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
				StringCbCopy(pObjectType->szName, sizeof(pObjectType->szName), szBuffer + 1);
				StringCbCopy(pObjectType->szDesc, sizeof(pObjectType->szDesc), szDesc + 2);
				pts = PTS_ARGS;
				pObjectType->ArgCount = 0;
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

				StringCbCopy(pObjectType->szArgs[pObjectType->ArgCount][0], 
					sizeof(pObjectType->szArgs[pObjectType->ArgCount][0]), szBuffer + 2);
				StringCbCopy(pObjectType->szArgs[pObjectType->ArgCount][1], 
					sizeof(pObjectType->szArgs[pObjectType->ArgCount][1]), szDesc + 2);
				pObjectType->ArgCount++;
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
				StringCbCopy(pObjectType->szMacro, sizeof(pObjectType->szMacro), szBuffer + 7);
				pts = PTS_ENDMACRO;
				break;
			}
		case PTS_ENDMACRO:
			{
				/*
				char szLine[256];
				char szMacro[256];

				WideCharToMultiByte(CP_ACP, 0, pObjectType->szMacro, -1, szMacro, sizeof(szMacro), NULL, NULL);
				StringCbPrintfA(szLine, sizeof(szLine), "%s(0,0)\n", szMacro);

				OutputDebugStringA(szLine);

				ZOBJECT zobj = {0};
				RunAssemblyWithArguments(szLine, (LPBYTE) &zobj, sizeof(zobj));

				pObjectType->cxObject = zobj.w;
				pObjectType->cyObject = zobj.h;
				*/

				if (_tcsnccmp(szBuffer, _T("#endmacro"), 8) == 0) {
					pObjectType++;
					(*lpdwCount)++;
					pts = PTS_NAME;
				}

				break;
			}
		}
	}
	
	fclose(file);
	return S_OK;
}