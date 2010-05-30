#pragma once

HWND CreateObjectToolbar(HWND hwndParent);
void SizeObjectToolbar(HWND hwndToolbar);

#define WM_CREATEOBJECT (WM_USER+30)
#define WM_CREATEENEMY  (WM_USER+31)