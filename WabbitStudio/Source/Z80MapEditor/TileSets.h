#pragma once

#include <windows.h>

BOOL AddTileset(const TCHAR *szName, const TCHAR *szFilename);
HDC GetTileset(const TCHAR *szName);
void GetTilesetFilename(const TCHAR *szName, TCHAR *szFilename, int cbFilename);

typedef BOOL (* TILESETENUM)(TCHAR *, LPVOID);
BOOL EnumTilesets(TILESETENUM TilesetEnum, LPVOID lpParam);