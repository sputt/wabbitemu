#include "stdafx.h"

#include "guibuttons.h"
#include "calc.h"
#include "resource.h"

POINT ButtonCenter83[64] = {
	{0x0EA+18,0x15C+18},{0x0C6+18,0x141+18},{0x10E+18,0x141+18},{0x0EA+18,0x125+18},
	{0xFFF+18,0xFFF+18},{0xFFF+18,0xFFF+18},{0xFFF+18,0xFFF+18},{0xFFF+18,0xFFF+18},
	{0x100+18,0x264+18},{0x100+18,0x23B+18},{0x101+18,0x216+18},{0x100+18,0x1F1+18},
	{0x101+18,0x1CD+18},{0x101+18,0x1A7+18},{0x101+18,0x181+18},{0xFFF+18,0xFFF+18},
	{0x0CD+18,0x25F+18},{0x0CD+18,0x23A+18},{0x0CD+18,0x215+18},{0x0CD+18,0x1F1+18},
	{0x0CD+18,0x1CC+18},{0x0CD+18,0x1A7+18},{0x0CD+18,0x181+18},{0xFFF+18,0xFFF+18},
	{0x09A+18,0x25F+18},{0x09A+18,0x23A+18},{0x09A+18,0x215+18},{0x099+18,0x1F1+18},
	{0x09A+18,0x1CC+18},{0x09A+18,0x1A7+18},{0x099+18,0x181+18},{0x09A+18,0x15D+18},
	{0x066+18,0x260+18},{0x066+18,0x23A+18},{0x066+18,0x215+18},{0x066+18,0x1F1+18},
	{0x066+18,0x1CC+18},{0x066+18,0x1A6+18},{0x066+18,0x182+18},{0x066+18,0x15D+18},
	{0x033+18,0x25F+18},{0x033+18,0x239+18},{0x033+18,0x215+18},{0x033+18,0x1F0+18},
	{0x033+18,0x1CC+18},{0x033+18,0x1A6+18},{0x033+18,0x182+18},{0x033+18,0x15D+18},
	{0x100+18,0x0FF+18},{0x0CD+18,0x0FF+18},{0x099+18,0x0FF+18},{0x066+18,0x0FF+18},
	{0x033+18,0x0FF+18},{0x033+18,0x139+18},{0x066+18,0x138+18},{0x09A+18,0x138+18}
};

POINT ButtonCenter84[64] = {
	{236+18,330+18},{206+18,308+18},{265+18,308+18},{236+18,282+18},
	{0xFFF+18,0xFFF+18},{0xFFF+18,0xFFF+18},{0xFFF+18,0xFFF+18},{0xFFF+18,0xFFF+18},
	{250+18,562+18},{250+18,525+18},{250+18,490+18},{250+18,456+18},{250+18,422+18},{250+18,390+18},{250+18,358+18},{0xFFF+18,0xFFF+18},
	{207+18,598+18},{207+18,553+18},{207+18,508+18},{207+18,465+18},{207+18,434+18},{207+18,396+18},{207+18,364+18},{0xFFF+18,0xFFF+18},
	{160+18,602+18},{160+18,556+18},{160+18,511+18},{160+18,468+18},{160+18,431+18},{160+18,398+18},{160+18,365+18},{160+18,332+18},
	{110+18,599+18},{110+18,554+18},{110+18,510+18},{110+18,467+18},{110+18,431+18},{110+18,396+18},{110+18,364+18},{110+18,332+18},
	{66+18,572+18},{66+18,532+18},{66+18,496+18},{66+18,460+18},{66+18,428+18},{66+18,392+18},{66+18,362+18},{66+18,326+18},
	{258+18,236+18},{208+18,240+18},{158+18,242+18},{112+18,240+18},{66+18,236+18},{66+18,296+18},{112+18,299+18},{160+18,304+18}
};
POINT *ButtonCenter[64];

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

	if (calcs[gslot].model == TI_84P || calcs[gslot].model == TI_84PSE)
		*ButtonCenter = ButtonCenter84;
	else
		*ButtonCenter = ButtonCenter83;
	
	brect.right		= (*ButtonCenter)[bit+(group<<3)].x*mult;
	brect.left		= (*ButtonCenter)[bit+(group<<3)].x*mult;
	brect.top		= (*ButtonCenter)[bit+(group<<3)].y*mult;
	brect.bottom	= (*ButtonCenter)[bit+(group<<3)].y*mult;
	
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
	if (GetRValue(colormatch) != 0) return;
	
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
			if ( (*ButtonCenter)[bit+(group<<3)].x != 0xFFF ) {
				pt.x	= (*ButtonCenter)[bit+(group<<3)].x*mult;
				pt.y	= (*ButtonCenter)[bit+(group<<3)].y*mult;
				if ( keypad->keys[group][bit] & KEY_LOCKPRESS ) {
					DrawButtonState(hdcSkin,hdcKeymap, &pt, DBS_LOCK | DBS_DOWN);
				}
			}
		}
	}
	
	group	= 5;
	bit		= 0;
	pt.x	= (*ButtonCenter)[bit+(group<<3)].x*mult;
	pt.y	= (*ButtonCenter)[bit+(group<<3)].y*mult;
	if ( calcs[gslot].cpu.pio.keypad->on_pressed & KEY_LOCKPRESS ) {
		DrawButtonState(hdcSkin,hdcKeymap, &pt, DBS_LOCK | DBS_DOWN);
	}
}

void HandleKeyDown(unsigned int key) {
	/* make this an accel*/
	if (key == VK_F8) {
		if (calcs[gslot].speed == 100)
			SendMessage(calcs[gslot].hwndFrame, WM_COMMAND, IDM_SPEED_QUADRUPLE, 0);
		else
			SendMessage(calcs[gslot].hwndFrame, WM_COMMAND, IDM_SPEED_NORMAL, 0);
	}

	if (key == VK_SHIFT) {
		if (GetKeyState(VK_LSHIFT) & 0xFF00) {
			key = VK_LSHIFT;
		} else {
			key = VK_RSHIFT;
		}
	}

	keyprog_t *kp = keypad_key_press(&calcs[gslot].cpu, key);
	if (kp) {
		extern POINT ButtonCenter83[64];
		extern POINT ButtonCenter84[64];
		if ((calcs[gslot].cpu.pio.keypad->keys[kp->group][kp->bit] & KEY_STATEDOWN) == 0) {
			/*if (calcs[gslot].model == TI_84P || calcs[gslot].model == TI_84PSE) {
				DrawButtonState(calcs[gslot].hdcSkin, calcs[gslot].hdcKeymap, &ButtonCenter84[kp->bit+(kp->group<<3)], DBS_DOWN | DBS_PRESS);
			} else {
				DrawButtonState(calcs[gslot].hdcSkin, calcs[gslot].hdcKeymap, &ButtonCenter83[kp->bit+(kp->group<<3)], DBS_DOWN | DBS_PRESS);
			}*/
			calcs[gslot].cpu.pio.keypad->keys[kp->group][kp->bit] |= KEY_STATEDOWN;
			SendMessage(calcs[gslot].hwndFrame, WM_SIZE, 0, 0);
			FinalizeButtons();
		}
	}
}

void HandleKeyUp(unsigned int key) {
	if (key == VK_SHIFT) {
		keypad_key_release(&calcs[gslot].cpu, VK_LSHIFT);
		keypad_key_release(&calcs[gslot].cpu, VK_RSHIFT);
	} else {
		keypad_key_release(&calcs[gslot].cpu, key);
	}
	FinalizeButtons();
}

void FinalizeButtons() {
	int group, bit;
	keypad_t *kp = calcs[gslot].cpu.pio.keypad;
	for(group=0;group<7;group++) {
		for(bit=0;bit<8;bit++) {
			if ((kp->keys[group][bit] & KEY_STATEDOWN) &&
				((kp->keys[group][bit] & KEY_MOUSEPRESS) == 0) &&
				((kp->keys[group][bit] & KEY_KEYBOARDPRESS) == 0)) {
				/*if (calcs[gslot].model == TI_84P || calcs[gslot].model == TI_84PSE) {
					DrawButtonState(calcs[gslot].hdcSkin, calcs[gslot].hdcKeymap, &ButtonCenter84[bit+(group<<3)], DBS_UP | DBS_PRESS);
					} else {
					DrawButtonState(calcs[gslot].hdcSkin, calcs[gslot].hdcKeymap, &ButtonCenter83[bit+(group<<3)], DBS_UP | DBS_PRESS);
				}*/	
					kp->keys[group][bit] &= (~KEY_STATEDOWN);
					//SendMessage(hwnd, WM_SIZE, 0, 0);
			}
		}
	}
}
