#ifndef GUISKIN_H
#define GUISKIN_H

#include "calc.h"

int DrawFaceplateRegion(HDC, COLORREF);
HRGN GetFaceplateRegion();
int gui_frame_update(LPCALC lpCalc);

#define DEFAULT_84PCSE_SKIN_SCALE 1
#define DEFAULT_SKIN_SCALE 2
#define DEFAULT_84PCSE_KEYMAP_SCALE 1
#define DEFAULT_KEYMAP_SCALE 2

#endif //GUISKIN_H