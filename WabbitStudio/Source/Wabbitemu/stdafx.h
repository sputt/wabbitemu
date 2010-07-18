#pragma once

#define _WIN32_LEAN_AND_MEAN

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
#include <stdint.h>
#include <sys/types.h>
#include <mmsystem.h>
#include <stddef.h>
#include <time.h>
#ifdef USE_GDIPLUS
#include <gdiplus.h>
using namespace Gdiplus;
#endif
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
