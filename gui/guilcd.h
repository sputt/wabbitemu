#ifndef GUILCD_H
#define GUILCD_H

#include "gui.h"
#include "core.h"
#include "lcd.h"

#define MAX_SHADES 255
#define RAM_COLOR		(RGB(0x60, 0xC0, 0x40))
#define ARCHIVE_COLOR 	(RGB(0xC0, 0x40, 0x60))

#define SEND_COLOR 		(RGB(0xFF, 0xFF, 0xFF))

#define LCD_LOW			(RGB(0x9E, 0xAB, 0x88))

#define LCD_X	63
#define LCD_Y	74

LRESULT CALLBACK LCDProc(HWND, UINT, WPARAM, LPARAM);
void PaintLCD(HWND, HDC);
void SaveStateDialog(HWND);



#endif
