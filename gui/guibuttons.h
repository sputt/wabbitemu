#ifndef GUIBUTTONS_H
#define GUIBUTTONS_H

#include "calc.h"

#define	DBS_DOWN	0x01
#define DBS_UP		0x00
#define	DBS_LOCK	0x02
#define DBS_PRESS	0x04
#define DBS_COPY	0x08

void FinalizeButtons(LPCALC lpCalc);
void HandleKeyDown(LPCALC lpCalc, unsigned int);
void HandleKeyUp(LPCALC lpCalc, unsigned int);
void DrawButtonState(HDC, HDC, POINT *, UINT state, UINT keymap_scale);
void DrawButtonStateNoSkin(HDC hdcDest, HDC hdcSkin, HDC hdcKeymap, POINT *pt, UINT state, UINT keymap_scale);
void DrawButtonStatesAll(LPCALC lpCalc, HDC hdcSkin, HDC hdcKeymap);
void LogKeypress(LPCALC lpCalc, int group, int bit);
HBITMAP DrawButtonAndMask(LPCALC lpCalc, POINT pt, HBITMAP *hbmButton, HBITMAP *hbmMask);
void DrawButtonShadow(HDC hdc, HDC hdcKeymap, POINT *pt, UINT keymap_scale);

#endif /* GUIBUTTONS_H */
