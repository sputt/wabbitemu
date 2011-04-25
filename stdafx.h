#ifndef _STDAFX_H
#define _STDAFX_H

#if defined(_WINDOWS)
#pragma once

#define _WIN32_LEAN_AND_MEAN

#ifndef STRICT
#define STRICT
#endif

#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <winsock2.h>
#include <Windows.h>
#include <WindowsX.h>
#include <tchar.h>
#include <strsafe.h>
#include <CommCtrl.h>
#include <shlobj.h>
#include <math.h>
#include <Uxtheme.h>
#include <dwmapi.h>
#include <Ole2.h>
#include <OleIdl.h>
#include <ShObjIdl.h>
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <mmsystem.h>
#include <stddef.h>
#include <time.h>

#ifdef __cplusplus
#ifdef USE_GDIPLUS
#include <gdiplus.h>
using namespace Gdiplus;
#endif
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
using namespace ATL;
#include <map>
#include <list>
#include <vector>
#include <string>

namespace std
{
#ifdef UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif
}
#endif

#elif defined(_LINUX)
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stddef.h>
#include <time.h>
#include <ctype.h>

typedef char TCHAR;
typedef void *LPVOID;
typedef const char *LPCTSTR;
#define MAX_PATH 256
#define _T(z) z
#define _tprintf_s printf
#define ARRAYSIZE(z) (sizeof(z)/sizeof((z)[0]))
#define _strnicmp strncasecmp
#define _tcsicmp strcasecmp
#define _putts puts
#define _tcsrchr strrchr
#define _tcscpy_s strcpy
#define _tcslen strlen
#define _tcscmp strcmp

#elif defined(_MACVER)


#endif

#endif