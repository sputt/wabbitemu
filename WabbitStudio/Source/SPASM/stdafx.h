#pragma once

#ifdef WIN32
#define _WIN32_WINNT 0x0501

#include <windows.h>
#include <windowsx.h>
#include <WinCrypt.h>
#include <wincon.h>
#include <fcntl.h>
#include <sys/timeb.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#include <tchar.h>
#include <strsafe.h>
#include <assert.h>

#ifdef _ATL
#define _ATL_STATIC_REGISTRY
#include <atlbase.h>
#include <atlcom.h>
#include <atlrc.h>
#include <atlconv.h>
#endif

#ifdef USE_GMP
#include <gmp/gmp.h>
#endif
