#ifndef GUIBUTTONS_H
#define GUIBUTTONS_H

#define	DBS_DOWN	0x01
#define DBS_UP		0x00
#define	DBS_LOCK	0x02
#define DBS_PRESS	0x04

void FinalizeButtons();
void HandleKeyDown(unsigned int);
void HandleKeyUp(unsigned int);
void DrawButtonState(HDC, HDC, POINT *, UINT);

#endif /* GUIBUTTONS_H */
