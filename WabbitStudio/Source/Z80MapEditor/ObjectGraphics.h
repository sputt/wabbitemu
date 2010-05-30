#pragma once

#include <windows.h>

#define MAX_OBJECT_IMAGES 512

typedef struct {
	TCHAR szName[64], szID[8];
	HDC hdc, hdcMask;
	int cxImage;
	int cyImage;
	int cxMargin, cyMargin;
} OBJECTIMAGE, *LPOBJECTIMAGE;

LPOBJECTIMAGE GetObjectImage(int Index);
HRESULT ReadObjectGraphics(const TCHAR *szFilename);
void PopulateComboBoxWithGraphics(HWND hwndCombo);
