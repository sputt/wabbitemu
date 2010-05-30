#pragma once

#include <windows.h>
BOOL LoadMapSet(HWND hwndParent, LPCTSTR szFilename);
BOOL SaveMapSet(TCHAR *szFilename, LPMAPSETSETTINGS lpmss);
LPMAPVIEWSETTINGS LoadMap(HWND hwndParent, TCHAR *szMapFilename, TCHAR *szDefaultsFilename, TCHAR *szName);
