#define _IN_MAPVIEW_H

#include <windows.h>

#ifndef _HAS_MAPVIEWSETTINGS

#include "MapObjects.h"
#include "Misc.h"
#include "AnimatedTiles.h"

typedef struct tagMAPVIEWSETTINGS {
	int Index;
	int iHot;
	RECT rSelection;
	LPBYTE pMapData;

	TCHAR szName[64];
	TCHAR szFilename[MAX_PATH];
	TCHAR szDefaultsFilename[MAX_PATH];
	TCHAR szTilesFilename[MAX_PATH];
	HWND hwndMap;
	BOOL fAdjusted;

	DWORD MiscCount;
	struct tagMISC MiscArray[MAX_MISC];

	DWORD ObjectCount;
	struct tagOBJECT ObjectArray[MAX_OBJECT];

	DWORD EnemyCount;
	struct tagOBJECT EnemyArray[MAX_ENEMY];

	DWORD AnimateCount;
	struct tagANIMATE AnimateArray[MAX_ANIMATE];

	HWND Left, Right, Above, Below;
	double DragStartX, DragStartY;

	HDC hdcTiles;
	int cxMapHierarchy;

} MAPVIEWSETTINGS, *LPMAPVIEWSETTINGS;
#define _HAS_MAPVIEWSETTINGS
#endif

#ifndef _MAPVIEW_H
#define _MAPVIEW_H
#define WM_UPDATE (WM_USER + 4)

HWND CreateMapView(HWND);
BOOL SetMapViewSettings(HWND hwndMap, LPMAPVIEWSETTINGS);
BOOL GetMapViewSettings(HWND hwndMap, LPMAPVIEWSETTINGS);
#endif

#undef _IN_MAPVIEW_H