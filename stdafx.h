#pragma once

#define _WIN32_LEAN_AND_MEAN

#ifndef STRICT
#define STRICT
#endif

#define _ATL_APARTMENT_THREADED
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
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <mmsystem.h>
#include <stddef.h>
#include <time.h>

#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
using namespace ATL;