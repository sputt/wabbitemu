#pragma once

#include <Windows.h>
#include "MapView.h"

void SaveUndoStep(HWND hwndMap);
void RestoreUndoStep(void);
