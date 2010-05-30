#pragma once

#include <Windows.h>
#include "Layers.h"

int SetStartLocationBackupLayer(LAYER Layer);
int SetStartLocation(int x, int y);
void RestoreStartLocationBackupLayer();
void DrawStartLocation(HDC hdc, BOOL fPostScale);
