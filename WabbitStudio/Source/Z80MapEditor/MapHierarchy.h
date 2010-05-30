#pragma once

#include <windows.h>

#include "MapView.h"

typedef enum {
	MP_ABOVE, MP_BELOW, MP_LEFT, MP_RIGHT, MP_ROOT, MP_NONE,
} MP_ORIENTATION;

typedef struct tagMAPSETSETTINGS {
	TCHAR szName[64];
	BOOL fLoaded, fDirty, fTesting, fPendingAdd;
	HWND hwndTest;
	TCHAR szFilename[MAX_PATH];
	int cx, cy;
	int cxTile, cyTile;
	int cxMargin;
	double Scale;
	HDWP hdwp;
	HWND hwndRoot;

	HWND hwndHot;
	MP_ORIENTATION HotOrientation;
	HWND hwndSelected;
	MP_ORIENTATION SelectedOrientation;

	HWND *MapHierarchy;
	int cxMapHierarchy, cyMapHierarchy;

	BYTE StartMapOffset;
	BYTE StartX, StartY;
} MAPSETSETTINGS, *LPMAPSETSETTINGS;

HWND CreateMapHierarchy(HWND hwndParent);
BOOL AddMap(MAPVIEWSETTINGS *PrevMap, MAPVIEWSETTINGS *NewMap, MP_ORIENTATION mpOrient);
BOOL IsMapSlot(HWND hwndHierarchy, int x, int y, HWND *phwnd, MP_ORIENTATION *pOrientation);
void FlattenMapTree(HWND hwndHierarchy, LPMAPVIEWSETTINGS **lpMvsArray, LPINT lpWidth, LPINT lpHeight);
void UpdateAllMaps(void);
