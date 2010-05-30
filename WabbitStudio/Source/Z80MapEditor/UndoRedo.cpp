#include <Windows.h>
#include "MapHierarchy.h"
#include "MapView.h"

#define MAX_UNDO_STEPS	32

extern MAPSETSETTINGS g_MapSet;

static MAPVIEWSETTINGS mvsUndoSteps[MAX_UNDO_STEPS];
static int nUndoStep = -1;
static int nTotalSteps = 0;

void SaveUndoStep(HWND hwndMap) {
	if (nUndoStep == MAX_UNDO_STEPS - 1) {
		memcpy(&mvsUndoSteps[0], &mvsUndoSteps[1], sizeof(MAPVIEWSETTINGS) * (MAX_UNDO_STEPS - 1));
	} else {
		nUndoStep++;
	}

	LPBYTE lpMapData = (LPBYTE) malloc(g_MapSet.cx * g_MapSet.cy);
	GetMapViewSettings(hwndMap, &mvsUndoSteps[nUndoStep]);
	memcpy(lpMapData, mvsUndoSteps[nUndoStep].pMapData, g_MapSet.cx * g_MapSet.cy);
	mvsUndoSteps[nUndoStep].pMapData = lpMapData;
}


void RestoreUndoStep(void) {
	if (nUndoStep >= 0) {
		SetMapViewSettings(mvsUndoSteps[nUndoStep].hwndMap, &mvsUndoSteps[nUndoStep]);
		InvalidateRect(mvsUndoSteps[nUndoStep].hwndMap, NULL, FALSE);
		UpdateWindow(mvsUndoSteps[nUndoStep].hwndMap);
		nUndoStep--;
	}
}



