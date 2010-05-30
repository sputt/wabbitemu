#include <Windows.h>

#include "MapHierarchy.h"
#include "Layers.h"
#include "resource.h"

// Fuck it, let's make it global
extern HWND hwndHierarchy;
extern MAPSETSETTINGS g_MapSet;

// Back up the previously selected layer
static LAYER g_LayerBackup;


int SetStartLocationBackupLayer(LAYER Layer) {
	return g_LayerBackup = Layer;
}

void RestoreStartLocationBackupLayer() {
	SetEditorLayer(g_LayerBackup);
}

void DrawStartLocation(HDC hdc, BOOL fPostScale) {
	HPEN hpen = NULL;
	int PenWidth = 2;
	if (fPostScale == TRUE) {
		PenWidth *= g_MapSet.Scale;
	}
	
	hpen = CreatePen(PS_SOLID, PenWidth, RGB(255, 0, 0));

	SelectObject(hdc, hpen);
	SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
	if (fPostScale == TRUE) {
		Ellipse(hdc, g_MapSet.StartX * g_MapSet.Scale, g_MapSet.StartY * g_MapSet.Scale, 
			(g_MapSet.StartX + 12) * g_MapSet.Scale, (g_MapSet.StartY + 9) * g_MapSet.Scale);
	} else {
		Ellipse(hdc, g_MapSet.StartX, g_MapSet.StartY, (g_MapSet.StartX + 12), (g_MapSet.StartY + 9));
	}

	DeleteObject(hpen);
}

int SetStartLocation(int x, int y) {
	HWND hwndMap;
	MP_ORIENTATION Orientation;

	BOOL fResult = IsMapSlot(hwndHierarchy, x, y, &hwndMap, &Orientation);
	if (fResult == TRUE && Orientation == MP_NONE) {
		MAPVIEWSETTINGS mvs;
		GetMapViewSettings(hwndMap, &mvs);

		RECT wr;
		GetWindowRect(hwndMap, &wr);
		MapWindowPoints(NULL, hwndHierarchy, (LPPOINT) &wr, 2);

		int LocalX = (x - wr.left) / g_MapSet.Scale;
		int LocalY = (y - wr.top) / g_MapSet.Scale;

		g_MapSet.StartX = LocalX - 6;
		g_MapSet.StartY = LocalY - 5;

		return mvs.Index;
	} else {
		return -1;
	}
}

