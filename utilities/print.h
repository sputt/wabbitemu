#ifndef PRINT_H
#define PRINT_H

#include "disassemble.h"

void MyDrawText(HDC, RECT *, Z80_info_t *, const TCHAR *, ...);
TCHAR* mysprintf(Z80_info_t *, const TCHAR *, ...);

#define DBCOLOR_BASE (RGB(4, 72, 117))
//#define DBCOLOR_HILIGHT (RGB(112, 169, 168))
#define DBCOLOR_HILIGHT (RGB(108, 173, 101))
#define DBCOLOR_CONDITION (RGB(40, 160, 180))

#endif
