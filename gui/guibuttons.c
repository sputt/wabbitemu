#include "stdafx.h"

#include "guibuttons.h"
#include "calc.h"

POINT ButtonCenter[64] = {
	{0x0EA,0x15C},{0x0C6,0x141},{0x10E,0x141},{0x0EA,0x125},
	{0xFFF,0xFFF},{0xFFF,0xFFF},{0xFFF,0xFFF},{0xFFF,0xFFF},
	{0x100,0x264},{0x100,0x23B},{0x101,0x216},{0x100,0x1F1},
	{0x101,0x1CD},{0x101,0x1A7},{0x101,0x181},{0xFFF,0xFFF},
	{0x0CD,0x25F},{0x0CD,0x23A},{0x0CD,0x215},{0x0CD,0x1F1},
	{0x0CD,0x1CC},{0x0CD,0x1A7},{0x0CD,0x181},{0xFFF,0xFFF},
	{0x09A,0x25F},{0x09A,0x23A},{0x09A,0x215},{0x099,0x1F1},
	{0x09A,0x1CC},{0x09A,0x1A7},{0x099,0x181},{0x09A,0x15D},
	{0x066,0x260},{0x066,0x23A},{0x066,0x215},{0x066,0x1F1},
	{0x066,0x1CC},{0x066,0x1A6},{0x066,0x182},{0x066,0x15D},
	{0x033,0x25F},{0x033,0x239},{0x033,0x215},{0x033,0x1F0},
	{0x033,0x1CC},{0x033,0x1A6},{0x033,0x182},{0x033,0x15D},
	{0x100,0x0FF},{0x0CD,0x0FF},{0x099,0x0FF},{0x066,0x0FF},
	{0x033,0x0FF},{0x033,0x139},{0x066,0x138},{0x09A,0x138}
};

extern HINSTANCE g_hInst;

//Buttons are assumed to be mostly convex
//creates a rect around the button
static RECT FindButtonRect(HDC hdcKeymap, POINT *pt) {
	RECT brect = {0,0,0,0};
	int mult = 1;
	int width,height, x, y;
	int bit, group;
	COLORREF colormatch, colortest;
	

	colormatch = GetPixel(hdcKeymap, pt->x, pt->y);
	
	if (GetRValue(colormatch) != 0 ) return brect;
		
		
	bit		= GetBValue(colormatch)>>4;
	group	= GetGValue(colormatch)>>4;

	
	brect.right		= ButtonCenter[bit+(group<<3)].x*mult;
	brect.left		= ButtonCenter[bit+(group<<3)].x*mult;
	brect.top		= ButtonCenter[bit+(group<<3)].y*mult;
	brect.bottom	= ButtonCenter[bit+(group<<3)].y*mult;
	
	width	= SKIN_WIDTH*mult;
	height	= SKIN_HEIGHT*mult;
	
	//Find the vertical center
	y = ( brect.top + brect.bottom )/2;
	
	//Search for left edge
	colortest = colormatch;
	for(x = brect.left; (colortest == colormatch) && ( x >= 0 ) ; x--) {
		colortest = GetPixel(hdcKeymap, x, y);
	}
	brect.left = x+1;
	
	//Search for right edge
	colortest = colormatch;
	for(x = brect.right; (colortest == colormatch) && ( x < width ) ; x++) {
		colortest = GetPixel(hdcKeymap, x, y);
	}
	brect.right = x-1;
	
	
	//Find the Horizontal center
	x = ( brect.right + brect.left )/2;
	
	//Search for top edge
	colortest = colormatch;
	for(y = brect.top; (colortest == colormatch) && ( y >= 0 ) ; y--) {
		colortest = GetPixel(hdcKeymap, x, y);
	}
	brect.top = y+1;
	
	//Search for bottom edge
	colortest = colormatch;
	for(y = brect.bottom; (colortest == colormatch) && ( y < height ) ; y++) {
		colortest = GetPixel(hdcKeymap, x, y);
	}
	brect.bottom = y-1;
	return brect;
}
	

void DrawButtonState(HDC hdcSkin, HDC hdcKeymap, POINT *pt, UINT state) {
	RECT brect;
	COLORREF colormatch;
	int x, y, width, height;

	colormatch = GetPixel(hdcKeymap, pt->x, pt->y);
	if (GetRValue(colormatch) !=0) return;
	
	brect = FindButtonRect(hdcKeymap, pt);
	if (IsRectEmpty(&brect)) return;

	width = brect.right-brect.left;
	height = brect.bottom-brect.top;
	
	HDC hdc = CreateCompatibleDC(hdcSkin);
	HBITMAP hbmButton = CreateCompatibleBitmap(hdcSkin,width,height);
	SelectObject(hdc, hbmButton);


	for(y=0;y<height;y++) {
		for(x=0;x<width;x++) {
			COLORREF skincolor = GetPixel(hdcSkin,x+brect.left, y+brect.top);
			COLORREF colortest = GetPixel(hdcKeymap, x+brect.left, y+brect.top);
			if (colormatch == colortest) {
				unsigned char red, blue, green;
				red = GetRValue(skincolor);
				blue = GetBValue(skincolor);
				green = GetGValue(skincolor);
				
				if (state & DBS_DOWN) {
					// button is down
					if (state & DBS_LOCK) {
						SetPixel(hdc, x, y, RGB( (red/2)+128 , (blue/2) , (green/2)) );
					} else
					
					if (state & DBS_PRESS) {
						SetPixel(hdc, x, y, RGB( (red/2) , (blue/2) , (green/2)) );
					}
				} else {
					if (state & DBS_LOCK) {
						SetPixel(hdc, x, y, RGB( (red-128)*2 , blue*2, green*2 ));
					} else
					
					if (state & DBS_PRESS) {
						SetPixel(hdc, x, y, RGB( red*2 , blue*2, green*2 ));
					}
				}
				
			} else {
				SetPixel(hdc, x, y, skincolor );
			}
		}
	}
	
	StretchBlt(
		hdcSkin, brect.left, brect.top, width, height,
		hdc, 0, 0, width, height, SRCCOPY);
				
	DeleteObject(hbmButton);
	DeleteObject(hdc);
}


void DrawButtonLockAll(HDC hdcSkin, HDC hdcKeymap) {
	keypad_t *keypad = calcs[gslot].cpu.pio.keypad;
	int group,bit;
	int mult;
	//RECT cr;
	POINT pt;
	//GetClientRect(hwnd, &cr);
	//if ((cr.bottom-cr.top)>=SKIN_HEIGHT-10) 
	mult = 1;
	//else mult = 0.75f;
	for(group=0;group<7;group++) {
		for(bit=0;bit<8;bit++) {
			if ( ButtonCenter[bit+(group<<3)].x != 0xFFF ) {
				pt.x	= ButtonCenter[bit+(group<<3)].x*mult;
				pt.y	= ButtonCenter[bit+(group<<3)].y*mult;
				if ( keypad->keys[group][bit] & KEY_LOCKPRESS ) {
					DrawButtonState(hdcSkin,hdcKeymap, &pt, DBS_LOCK | DBS_DOWN);
				}
			}
		}
	}
	
	group	= 5;
	bit		= 0;
	pt.x	= ButtonCenter[bit+(group<<3)].x*mult;
	pt.y	= ButtonCenter[bit+(group<<3)].y*mult;
	if ( calcs[gslot].cpu.pio.keypad->on_pressed & KEY_LOCKPRESS ) {
		DrawButtonState(hdcSkin,hdcKeymap, &pt, DBS_LOCK | DBS_DOWN);
	}

}
