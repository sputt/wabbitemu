#ifndef GUILCD_H
#define GUILCD_H

#include "gui.h"
#include "core.h"
#include "lcd.h"

#include <windows.h>

#define MAX_SHADES 255
#define RAM_COLOR		(RGB(0x60, 0xC0, 0x40))
#define ARCHIVE_COLOR 	(RGB(0xC0, 0x40, 0x60))

#define SEND_COLOR 		(RGB(0xFF, 0xFF, 0xFF))

#define LCD_LOW			(RGB(0x9E, 0xAB, 0x88))

/*#define LCD_X	63+16+1
#define LCD_Y	74+16+2*/

LRESULT CALLBACK LCDProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
void PaintLCD(HWND hwnd, HDC hdcDest);



#endif
