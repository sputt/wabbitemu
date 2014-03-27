#pragma once

#if defined(_WINDOWS)


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
#include <crtdbg.h>

#include <stdio.h>
#include <sys/types.h>
#include <mmsystem.h>
#include <stddef.h>
#include <time.h>

#include <stdint.h>

#include <io.h>
#include <fcntl.h>
#include <direct.h>
#include <WinInet.h>

#ifdef __cplusplus
#include <gdiplus.h>
using namespace Gdiplus;

//#define _ATL_DEBUG_INTERFACES
#define _ATL_APARTMENT_THREADED
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlcore.h>
#include <atlcoll.h>
#include <atlsafe.h>
using namespace ATL;

#include <algorithm>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <comdef.h>

#include <DbgHelp.h>

#include "Wabbitemu_h.h"
#include "resource.h"
#include <strsafe.h>

namespace std
{
#ifdef UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif
}
#endif

#elif defined(_LINUX) || defined(_ANDROID)
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <stddef.h>
#include <time.h>
#include <ctype.h>
#ifdef __cplusplus
#include <map>
#endif

typedef void *LPVOID;
typedef const char *LPCTSTR;
typedef int errno_t;
#define MAX_PATH 256
#define _T(z) z
#define _tprintf_s printf
#define ARRAYSIZE(z) (sizeof(z)/sizeof((z)[0]))
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
#define _strnicmp strncasecmp
#define _tcsicmp strcasecmp
#define _putts puts
#define _tcsrchr strrchr
#define _tcscpy_s(dest, len, src) strcpy(dest, src)
#define _tcslen strlen
#define _tcscmp strcmp
#define _tcsncmp strncmp
#define _tcsnicmp _strnicmp
#define _stscanf_s sscanf_s
#define sscanf_s sscanf
#define vsprintf_s vsprintf
#define ZeroMemory(dest, size) memset(dest, 0, size)
#define memcpy_s(dest, num, source, size) memcpy(dest, source, size)
#define StringCbCopy(dest, size, source) strcpy(dest, source)
#define StringCchCopy(dest, size, source) strcpy(dest, source)
#define StringCbPrintf(dest, size, format, args...) sprintf(dest, format, args)
#define fopen_s(pFile,filename,mode) (*(pFile)) = fopen((filename),(mode))
#define tmpfile_s(pFile) (*(pFile)) = tmpfile()
#define _tfopen_s fopen_s

#elif defined(_MACVER)
#include <assert.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <stddef.h>
#include <time.h>
#include <ctype.h>
#ifdef __cplusplus
#include <map>
#endif

typedef int errno_t;

#define MAX_PATH 256
#define _T(z) z
#define _tprintf_s printf
#define ARRAYSIZE(z) (sizeof(z)/sizeof((z)[0]))
#define _strnicmp strncasecmp
#define _tcsnicmp strncasecmp
#define _tcsicmp strcasecmp
#define _putts puts
#define _tcsrchr strrchr
#define _tcscpy_s strcpy
#define _tcslen strlen
#define _tcscmp strcmp

#endif