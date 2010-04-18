#ifndef GUIBUTTONS_H
#define GUIBUTTONS_H

#include <windows.h>

#define	DBS_DOWN	0x01
#define DBS_UP		0x00
#define	DBS_LOCK	0x02
#define DBS_PRESS	0x04

void DrawButtonState(HDC hdcSkin, HDC hdcKeymap, POINT *pt, UINT state);

#endif /* GUIBUTTONS_H */
