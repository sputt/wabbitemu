#pragma once

#ifdef WIN32
#include <windows.h>
#include <windowsx.h>
#include <WinCrypt.h>
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

#ifdef USE_GMP
#include <gmp.h>
#endif
