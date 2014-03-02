#ifndef GUISKIN_H
#define GUISKIN_H

#include "gui.h"
#include "calc.h"

int DrawFaceplateRegion(HDC, double, COLORREF);
HRGN GetFaceplateRegion(double);
int gui_frame_update(LPMAINWINDOW);

#define DEFAULT_84PCSE_SKIN_SCALE 1
#define DEFAULT_SKIN_SCALE 2
#define DEFAULT_84PCSE_KEYMAP_SCALE 1
#define DEFAULT_KEYMAP_SCALE 2

#endif //GUISKIN_H