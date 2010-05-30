#include <windows.h>
#include <CommCtrl.h>

#include "MapHierarchy.h"
#include "Layers.h"
#include "resource.h"

LAYER g_Layer = NO_LAYER;

extern MAPSETSETTINGS g_MapSet;
extern HWND hwndMain, hwndHierarchy;
extern HWND hwndToolbar;

void SetEditorLayer(LAYER layer) {
	if (g_Layer == layer)
		return;

	HMENU hmenu = GetMenu(hwndMain);
	CheckMenuItem(hmenu, ID_LAYER_MAPOVERVIEW, MF_UNCHECKED);
	CheckMenuItem(hmenu, ID_LAYER_MAP, MF_UNCHECKED);
	CheckMenuItem(hmenu, ID_LAYER_OBJECT, MF_UNCHECKED);
	CheckMenuItem(hmenu, ID_LAYER_ENEMY, MF_UNCHECKED);
	CheckMenuItem(hmenu, ID_LAYER_MISC, MF_UNCHECKED);
	CheckMenuItem(hmenu, ID_LAYER_STARTLOCATIONS, MF_UNCHECKED);

	CheckMenuItem(hmenu, layer, MF_CHECKED);
	g_Layer = layer;

	SendMessage(hwndToolbar, TB_CHECKBUTTON, g_Layer, (LPARAM) TRUE);

	InvalidateRect(hwndHierarchy, NULL, FALSE);
	if (g_MapSet.hwndHot != NULL && g_MapSet.HotOrientation == MP_NONE) {
		InvalidateRect(g_MapSet.hwndHot, NULL, FALSE);
	}
	g_MapSet.hwndHot = NULL;
	g_MapSet.HotOrientation = MP_NONE;

	HWND hwndMap;
	MP_ORIENTATION Orientation;
	POINT pt;
	GetCursorPos(&pt);
	MapWindowPoints(NULL, hwndHierarchy, &pt, 1);

	if (IsMapSlot(hwndHierarchy, pt.x, pt.y, &hwndMap, NULL)) {
		MAPVIEWSETTINGS mvs;
		GetMapViewSettings(hwndMap, &mvs);
		mvs.iHot = -1;
		SetMapViewSettings(hwndMap, &mvs);
		LRESULT SendMouseMove(HWND hwnd);
		SendMouseMove(hwndMap);
		InvalidateRect(hwndMap, NULL, FALSE);
		UpdateWindow(hwndMap);
	}
	UpdateWindow(hwndHierarchy);
}