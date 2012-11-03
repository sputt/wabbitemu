#ifndef GUISKIN_H
#define GUISKIN_H

#include "calc.h"

int DrawFaceplateRegion(HDC, COLORREF);
HRGN GetFaceplateRegion();
int gui_frame_update(LPCALC lpCalc);

#endif //GUISKIN_H