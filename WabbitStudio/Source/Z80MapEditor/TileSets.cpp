#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

#include "TileSets.h"

extern HINSTANCE g_hInstance;

static struct {
	TCHAR szName[64];
	TCHAR szPath[MAX_PATH];
	HDC hdc;
} g_TileSets[8];

static int g_TileSetCount = 0;

static HDC DoBitmapLoad(const TCHAR* szFileName) {
    BITMAP tmpBMP;
    HBITMAP bmpTiles;
    bmpTiles = (HBITMAP) LoadImage(g_hInstance, szFileName, IMAGE_BITMAP,
            0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_MONOCHROME);
    if (!bmpTiles) {
        MessageBox(NULL,_T("Error loading bitmap."),_T("Error"),MB_OK | MB_ICONERROR);
        return FALSE;
    }
    GetObject(bmpTiles,sizeof(BITMAP),&tmpBMP);
    if (tmpBMP.bmWidth>16 || tmpBMP.bmHeight<16 || tmpBMP.bmHeight>128*16) {
        MessageBox(NULL,_T("Not a valid tilemap"),NULL,MB_OK | MB_ICONERROR);
        return FALSE;
    }

	HDC hdc = CreateCompatibleDC(GetDC(NULL));
    SelectObject(hdc, bmpTiles);
    return hdc;
}

BOOL AddTileset(const TCHAR *szName, const TCHAR *szFilename) {
	int i;
	for (i = 0; i < g_TileSetCount; i++) {
		if (lstrcmp(g_TileSets[i].szName, szName) == 0)
			return TRUE;
	}

	StringCbCopy(g_TileSets[i].szName, sizeof(g_TileSets[i].szName), szName);
	StringCbCopy(g_TileSets[i].szPath, sizeof(g_TileSets[i].szPath), szFilename);
	g_TileSets[i].hdc = DoBitmapLoad(szFilename);
	g_TileSetCount++;
	return TRUE;
}

void RemoveTileset(const TCHAR *szName) {

}

HDC GetTileset(const TCHAR *szName) {
	int i;
	for (i = 0; i < g_TileSetCount; i++) {
		if (lstrcmp(g_TileSets[i].szName, szName) == 0)
			return g_TileSets[i].hdc;
	}
	return NULL;
}

void GetTilesetFilename(const TCHAR *szName, TCHAR *szFilename, int cbFilename) {
	int i;
	for (i = 0; i < g_TileSetCount; i++) {
		if (lstrcmp(g_TileSets[i].szName, szName) == 0) {
			StringCbCopy(szFilename, cbFilename, g_TileSets[i].szPath);
			return;
		}
	}
}

BOOL EnumTilesets(TILESETENUM TilesetEnum, LPVOID lpParam) {
	for (int i = 0; i < g_TileSetCount; i++) {
		if (TilesetEnum(g_TileSets[i].szName, lpParam) == FALSE)
			return FALSE;
	}
	return TRUE;
}