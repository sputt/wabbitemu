#ifndef GUIBUTTONS_H
#define GUIBUTTONS_H

#include "calc.h"

#define	DBS_DOWN	0x01
#define DBS_UP		0x00
#define	DBS_LOCK	0x02
#define DBS_PRESS	0x04

void FinalizeButtons(LPCALC lpCalc);
void HandleKeyDown(LPCALC lpCalc, unsigned int);
void HandleKeyUp(LPCALC lpCalc, unsigned int);
void DrawButtonState(LPCALC lpCalc, HDC, HDC, POINT *, UINT);

#endif /* GUIBUTTONS_H */