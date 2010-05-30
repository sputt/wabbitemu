#pragma once

#include <windows.h>

HWND CreateTilesToolbar(HWND);
void SizeTilesToolbar(HWND hwndToolbar);
int GetSelectedTile(HWND hwndToolbar);
void SetSelectedTile(HWND hwndToolbar, int Index);

#include "AnimatedTiles.h"
LPANIMATETYPE GetSelectedAnimate(HWND hwndToolbar);
