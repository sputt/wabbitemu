#pragma once

#include "gui.h"

DWORD CALLBACK ReplayKeypressThread(LPVOID lpParam);
int LoadKeyFile(TCHAR* filename, LPMAINWINDOW lpMainWindow);
LRESULT CALLBACK KeysListProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#define REFRESH_LISTVIEW 1
#define MAX_KEYPRESS_HISTORY 200