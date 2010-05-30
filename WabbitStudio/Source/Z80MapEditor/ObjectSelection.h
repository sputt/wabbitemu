#pragma once

#include <windows.h>
#include "MapObjects.h"

void ClearObjectSelection(void);
void AddObjectToSelection(LPVOID lpo);
void RemoveObjectFromSelection(LPVOID lpo);
BOOL IsObjectSelected(const LPVOID lpo);
DWORD GetSelectedObjectCount();
LPVOID GetSelectedObject(int Index);