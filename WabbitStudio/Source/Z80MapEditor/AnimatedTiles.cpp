#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

#include "AnimatedTiles.h"
#include "spasm.h"
#include "MapView.h"
#include "MapHierarchy.h"
#include "Wabbitemu.h"

extern MAPSETSETTINGS g_MapSet;

static int g_AnimateKeyCount = 1;
static ANIMATEKEY g_AnimateKeys[16];

int g_AnimateTypeCount = 0;
ANIMATETYPE g_AnimateTypes[16];

static ANIMATETYPE CustomAnimateType;

typedef enum {
	AKRS_STARTLABEL,
	AKRS_KEYNAME,
	AKRS_KEYTIME,
	AKRS_KEYOFFSET,
	AKRS_KEYLOOP,
} ANIMATEKEYREADSTATE;

BOOL InitAnimateKeys(const LPTSTR lpszFilename) {
	FILE *file = _tfopen(lpszFilename, _T("r"));
	if (file == NULL)
		return FALSE;

	TCHAR szBuffer[256];
	ANIMATEKEYREADSTATE State = AKRS_STARTLABEL;

	while (_fgetts(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), file)) {
		switch (State)
		{
		case AKRS_STARTLABEL:
			{
				const TCHAR *szLabel = _T(";animate_keys:");
				if (_tcsnicmp(szBuffer, szLabel, lstrlen(szLabel)) == 0) {
					State = AKRS_KEYNAME;
				}
				break;
			}
		case AKRS_KEYNAME:
			{
				if (_istalpha(szBuffer[0])) {
					TCHAR *psz = _tcstok(szBuffer, _T(": \r\n"));
					if (psz != NULL) {
						StringCbCopy(g_AnimateKeys[g_AnimateKeyCount].szName, sizeof(g_AnimateKeys[0].szName), psz);
						g_AnimateKeys[g_AnimateKeyCount].FrameCount = 0;
						State = AKRS_KEYTIME;
					}
				}
				break;
			}
		case AKRS_KEYTIME:
			{
				if (_tcsstr(szBuffer, _T(".db")) != NULL) {
					TCHAR *psz = _tcstok(szBuffer, _T("\t .db\r\n"));
					if (psz != NULL) {
						BYTE Frames = (BYTE) _ttoi(psz);
						if (Frames > 0) {
							g_AnimateKeys[g_AnimateKeyCount].Frames[g_AnimateKeys[g_AnimateKeyCount].FrameCount].Time = Frames;
							State = AKRS_KEYOFFSET;
						} else {
							State = AKRS_KEYLOOP;
						}
					}
				}

				break;
			}

		case AKRS_KEYOFFSET:
			{
				TCHAR *psz = NULL;
				if ((psz = _tcsstr(szBuffer, _T(".dw"))) != NULL) {
					WORD Offset;
					char szAssembleBuffer[256];
					WideCharToMultiByte(CP_ACP, 0, psz, -1, szAssembleBuffer, sizeof(szAssembleBuffer), NULL, NULL);
					RunAssemblyWithArguments(szAssembleBuffer, (LPBYTE) &Offset, sizeof(Offset));
					g_AnimateKeys[g_AnimateKeyCount].Frames[g_AnimateKeys[g_AnimateKeyCount].FrameCount].Offset = Offset;
					g_AnimateKeys[g_AnimateKeyCount].FrameCount++;
					State = AKRS_KEYTIME;
				}
				break;
			}

		case AKRS_KEYLOOP:
			{
				TCHAR *psz = NULL;
				if ((psz = _tcsstr(szBuffer, _T(".dw"))) != NULL) {

					char szLine[256];
					char szLabel[256];
					WideCharToMultiByte(CP_ACP, 0, g_AnimateKeys[g_AnimateKeyCount].szName, -1, szLabel, sizeof(szLabel), NULL, NULL);
					StringCbPrintfA(szLine, sizeof(szLine), "#define %s %d", szLabel, g_AnimateKeyCount);
					RunAssemblyWithArguments(szLine, NULL, 0);

					State = AKRS_KEYNAME;
					g_AnimateKeyCount++;
				}
				break;
			}

		default:
			break;
		}
	}

	fclose(file);
	return TRUE;
}


typedef enum {
	ATRS_STARTLABEL,
	ATRS_DESCRIPTION,
	ATRS_MACRO,
	ATRS_ENDMACRO,
} ANIMATETYPESREADSTATE;

BOOL LoadAnimateTypes(const LPTSTR lpszFilename) {
	FILE *file = _tfopen(lpszFilename, _T("r"));
	if (file == NULL)
		return FALSE;

	char szLine[256] = "#define tile_table 0";
	RunAssemblyWithArguments(szLine, NULL, 0);

	TCHAR szBuffer[256];
	ANIMATETYPESREADSTATE State = ATRS_STARTLABEL;

	while (_fgetts(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), file)) {
		switch (State)
		{
		case ATRS_STARTLABEL:
			{
				const TCHAR *szLabel = _T(";animate_types:");
				if (_tcsnicmp(szBuffer, szLabel, lstrlen(szLabel)) == 0) {
					State = ATRS_DESCRIPTION;
				}
				break;
			}
		case ATRS_DESCRIPTION:
			{
				if (szBuffer[0] != _T(';'))
					break;

				TCHAR *szDesc = _tcschr(szBuffer, _T('-'));
				if (szDesc == NULL)
					continue;
				*(szDesc - 1) = _T('\0');

				// Fill in some defaults
				StringCbCopy(g_AnimateTypes[g_AnimateTypeCount].szName, sizeof(g_AnimateTypes[0].szName), szBuffer + 1);
				StringCbCopy(g_AnimateTypes[g_AnimateTypeCount].szDesc, sizeof(g_AnimateTypes[0].szDesc), szDesc + 2);
				State = ATRS_MACRO;
				break;
			}
		case ATRS_MACRO:
			{
				if (_tcsnccmp(szBuffer, _T("#macro"), 5) != 0)
					break;

				TCHAR *szArgs = _tcschr(szBuffer, _T('('));
				if (szArgs == NULL)
					continue;
				*szArgs = _T('\0');
				
				WideCharToMultiByte(CP_ACP, 0, szBuffer + 7, -1, g_AnimateTypes[g_AnimateTypeCount].szMacro,
					sizeof(g_AnimateTypes[0].szMacro), NULL, NULL);
				State = ATRS_ENDMACRO;
				break;
			}
		case ATRS_ENDMACRO:
			{
				if (_tcsstr(szBuffer, _T("#endmacro")) == NULL)
					break;

				char szLine[256];
				char szMacro[256];

				StringCbPrintfA(szLine, sizeof(szLine), "%s(0,0)\n", g_AnimateTypes[g_AnimateTypeCount].szMacro);

				OutputDebugStringA(szLine);

				ZANIMATE zanim = {0};
				RunAssemblyWithArguments(szLine, (LPBYTE) &zanim, sizeof(zanim));

				g_AnimateTypes[g_AnimateTypeCount].TileIndex = zanim.anim_ptr / 32;
				g_AnimateTypes[g_AnimateTypeCount].lpKey = &g_AnimateKeys[zanim.key_ptr - 1];

				State = ATRS_DESCRIPTION;
				g_AnimateTypeCount++;
				break;
			}
		default:
			break;
		}
	}

	fclose(file);
	return TRUE;
}

static void GetAnimateRect(const LPANIMATE lpa, RECT *pr) {
	int new_x = lpa->x, new_y = lpa->y;
	
	pr->left = new_x;
	pr->top = new_y;
	pr->right = pr->left + lpa->w;
	pr->bottom = pr->top + lpa->h;
}

static void GetObjectRealRect(const LPANIMATE lpa, RECT *pr) {
	GetAnimateRect(lpa, pr);

	pr->left *= g_MapSet.Scale;
	pr->top *= g_MapSet.Scale;
	pr->right *= g_MapSet.Scale;
	pr->bottom *= g_MapSet.Scale;
}


void DrawAnimate(HDC hdc, LPANIMATE lpa, LPMAPVIEWSETTINGS lpmvs, double Scale) {
	if (lpa == NULL || lpmvs == NULL)
		return;

	RECT r;
	GetAnimateRect(lpa, &r);

	r.left *= Scale;
	r.top *= Scale;
	r.right *= Scale;
	r.bottom *= Scale;

	if (g_MapSet.fTesting == TRUE) {
		SetDCBrushColor(hdc, RGB(0, 0, 0));
	} else {
		SetDCBrushColor(hdc, ANIMATE_COLOR);
	}
	FillRect(hdc, &r, (HBRUSH) GetStockObject(DC_BRUSH));

	StretchBlt(hdc, r.left, r.top, r.right - r.left, r.bottom - r.top,
		lpmvs->hdcTiles, 0, (16 * (lpa->lpType->TileIndex % 128)) + lpa->lpType->lpKey->Frames[lpa->Frame].Offset / 2, 16, 16, SRCPAINT);
}

#include "hash.h"
extern hash_t *ZeldaImageHashTable;
// Reverse lookup of zelda labels
typedef struct {
	char *szValue;
	int Index;
} ZELDALABEL, *LPZELDALABEL;

HRESULT SetAnimateFromReal(LPANIMATE lpa, const ZANIMATE *pzanim) {
	if (lpa == NULL || pzanim == NULL)
		return E_FAIL;

	lpa->x = pzanim->x;
	lpa->w = pzanim->w;
	lpa->y = pzanim->y;
	lpa->h = pzanim->h;

	if (g_MapSet.fTesting == TRUE) {
		int frame = 0;
		unsigned short addr = 0;
		int result = FindAnimationInfo(0, pzanim->key_ptr, &frame, &addr);
		if (result == 0) {
			lpa->lpType = &g_AnimateTypes[0];
			lpa->Frame = 0;
			lpa->TimeRemaining = 10;
		} else {
			char key_name[256];
			StringCbPrintfA(key_name, sizeof(key_name), "%04X", addr);
			LPZELDALABEL lpzl = (LPZELDALABEL) hash_lookup(ZeldaImageHashTable, key_name);
			if (lpzl != NULL) {
				CustomAnimateType.lpKey = &g_AnimateKeys[lpzl->Index];
				CustomAnimateType.TileIndex = (pzanim->anim_ptr - 0xA66C) / 32;

				lpa->lpType = &CustomAnimateType;
				lpa->Frame = frame;
				lpa->TimeRemaining = pzanim->anim_ctr;
			} else {
				return E_FAIL;
			}
		}
	}
	return S_OK;
}

HRESULT CreateAnimate(LPANIMATETYPE lpat, int x, int y, LPANIMATE lpa) {
	ZeroMemory(lpa, sizeof(ANIMATE));

	lpa->x = x;
	lpa->y = y;
	lpa->lpType = lpat;
	char szLine[256];
	StringCbPrintfA(szLine, sizeof(szLine), "%s(%d,%d)", lpat->szMacro, x, y);

	ZANIMATE zanim = {0};
	RunAssemblyWithArguments(szLine, (BYTE *) &zanim, sizeof(zanim));
	SetAnimateFromReal(lpa, &zanim);

	lpa->TimeRemaining = 1;
	lpa->Frame = 0;
	return S_OK;
}

int AddAnimate(LPMAPVIEWSETTINGS lpmvs, LPANIMATETYPE lpat, int x, int y) {
	if (lpmvs->AnimateCount == MAX_ANIMATE)
		return -1;

	ANIMATE anim = {0};
	CreateAnimate(lpat, x - 8, y - 8, &anim);
	
	lpmvs->AnimateArray[lpmvs->AnimateCount++] = anim;	
	InvalidateRect(lpmvs->hwndMap, NULL, FALSE);
	return lpmvs->AnimateCount - 1;
}

void DrawMapAnimate(HDC hdc, LPMAPVIEWSETTINGS lpmvs, double Scale, BOOL fTesting) {

	ZANIMATE *zanim;
	if (fTesting == TRUE) {
		zanim = GetZeldaAnimateArray();
	}
	for (DWORD i = 0; i < MAX_ANIMATE; i++) {
		if (fTesting == TRUE) {
			if (zanim[i].alive != 0) {
				ANIMATE anim;
				if (SUCCEEDED(SetAnimateFromReal(&anim, &zanim[i]))) {
					DrawAnimate(hdc, &anim, lpmvs, Scale);
				}
			}
		} else if (i < lpmvs->AnimateCount) {
			DrawAnimate(hdc, &lpmvs->AnimateArray[i], lpmvs, Scale);
		}
	}
}