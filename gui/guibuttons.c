#include "stdafx.h"

#include "guibuttons.h"
#include "guicutout.h"
#include "gui.h"
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
static RECT FindButtonRect(LPCALC lpCalc, HDC hdcKeymap, POINT *pt) {
	RECT brect = {0,0,0,0};
	int x, y;
	int bit, group;
	COLORREF colormatch, colortest;

	colormatch = GetPixel(hdcKeymap, pt->x, pt->y);
	
	if (GetRValue(colormatch) != 0 ) {
	return brect;
  }
		
	bit		= GetBValue(colormatch) >> 4;
	group	= GetGValue(colormatch) >> 4;
	
	brect.right		= (*ButtonCenter)[bit+(group<<3)].x;
	brect.left		= (*ButtonCenter)[bit+(group<<3)].x;
	brect.top		= (*ButtonCenter)[bit+(group<<3)].y;
	brect.bottom	= (*ButtonCenter)[bit+(group<<3)].y;
	
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
	for(x = brect.right; (colortest == colormatch) && ( x < SKIN_WIDTH ) ; x++) {
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
	for(y = brect.bottom; (colortest == colormatch) && ( y < SKIN_HEIGHT ) ; y++) {
		colortest = GetPixel(hdcKeymap, x, y);
	}
	brect.bottom = y-1;
	return brect;
}
	

void DrawButtonState(LPCALC lpCalc, HDC hdcSkin, HDC hdcKeymap, POINT *pt, UINT state) {
	RECT brect;
	COLORREF colormatch;
	int x, y, width, height;

	colormatch = GetPixel(hdcKeymap, pt->x, pt->y);
	if (GetRValue(colormatch) != 0) return;
	
	brect = FindButtonRect(lpCalc, hdcKeymap, pt);
	if (IsRectEmpty(&brect)) return;

	width = brect.right - brect.left;
	height = brect.bottom - brect.top;
	
	HDC hdc = CreateCompatibleDC(hdcSkin);
	HBITMAP hbmButton = CreateCompatibleBitmap(hdcSkin,width,height);
	SelectObject(hdc, hbmButton);


	for(y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {
			COLORREF skincolor = GetPixel(hdcSkin,x + brect.left, y + brect.top);
			COLORREF colortest = GetPixel(hdcKeymap, x + brect.left, y + brect.top);
			if (colormatch == colortest) {
				unsigned char red, blue, green;
				red = GetRValue(skincolor);
				blue = GetBValue(skincolor);
				green = GetGValue(skincolor);
				
				if (state & DBS_DOWN) {
					// button is down
					if (state & DBS_LOCK)
						SetPixel(hdc, x, y, RGB((red / 2) + 128 , blue / 2, green / 2));
					else if (state & DBS_PRESS)
						SetPixel(hdc, x, y, RGB(red / 2, blue / 2, green / 2));
				} else {
					if (state & DBS_LOCK)
						SetPixel(hdc, x, y, RGB((red - 128) * 2 , blue * 2, green * 2));
					else if (state & DBS_PRESS)
						SetPixel(hdc, x, y, RGB(red * 2 , blue * 2, green * 2));
				}
				
			} else {
				SetPixel(hdc, x, y, skincolor);
			}
		}
	}
	
	StretchBlt(hdcSkin, brect.left, brect.top, width, height,
				hdc, 0, 0, width, height, SRCCOPY);
				
	DeleteObject(hbmButton);
	DeleteObject(hdc);
}


void DrawButtonStatesAll(LPCALC lpCalc, HDC hdcSkin, HDC hdcKeymap) {
	keypad_t *keypad = lpCalc->cpu.pio.keypad;
	int group, bit;
	POINT pt;
	for(group = 0; group < 7; group++) {
		for(bit = 0; bit < 8; bit++) {
			if ((*ButtonCenter)[bit + (group << 3)].x != 0xFFF) {
				pt.x	= (*ButtonCenter)[bit + (group << 3)].x;
				pt.y	= (*ButtonCenter)[bit + (group << 3)].y;
				int val = keypad->keys[group][bit];
				if (val & KEY_LOCKPRESS) {
					DrawButtonState(lpCalc, hdcSkin, hdcKeymap, &pt, DBS_LOCK | DBS_DOWN);
				} else if ((val & KEY_MOUSEPRESS) || (val & KEY_KEYBOARDPRESS)) {
					DrawButtonState(lpCalc, hdcSkin, hdcKeymap, &pt, DBS_PRESS | DBS_DOWN);
				}
			}
		}
	}
	
	group	= 5;
	bit		= 0;
	pt.x	= (*ButtonCenter)[bit + (group << 3)].x;
	pt.y	= (*ButtonCenter)[bit + (group << 3)].y;
  int val = lpCalc->cpu.pio.keypad->on_pressed;
	if (val & KEY_LOCKPRESS) {
	DrawButtonState(lpCalc, hdcSkin, hdcKeymap, &pt, DBS_LOCK | DBS_DOWN);
  } else if ((val & KEY_MOUSEPRESS) || (val & KEY_KEYBOARDPRESS)) {
	DrawButtonState(lpCalc, hdcSkin, hdcKeymap, &pt, DBS_PRESS | DBS_DOWN);
  }
}

static int last_shift;
void HandleKeyDown(LPCALC lpCalc, unsigned int key) {
	/* make this an accel*/
	if (key == VK_F8) {
		if (lpCalc->speed == 100)
			SendMessage(lpCalc->hwndFrame, WM_COMMAND, IDM_SPEED_QUADRUPLE, 0);
		else
			SendMessage(lpCalc->hwndFrame, WM_COMMAND, IDM_SPEED_NORMAL, 0);
	}

	if (key == VK_SHIFT) {
		if (GetKeyState(VK_LSHIFT) & 0xFF00) {
			key = VK_LSHIFT;
		} else {
			key = VK_RSHIFT;
		}
		last_shift = key;
	}
	keyprog_t *kp = keypad_key_press(&lpCalc->cpu, key);
	if (!kp)
		return;
	LogKeypress(kp->group, kp->bit, key, TRUE, lpCalc->cpu.pio.model);
	FinalizeButtons(lpCalc);
}

void HandleKeyUp(LPCALC lpCalc, unsigned int key) {
	if (key == VK_SHIFT)
		key = last_shift;
	keyprog_t *kp = keypad_key_release(&lpCalc->cpu, key);
	if (kp)
		LogKeypress(kp->group, kp->bit, key, FALSE, lpCalc->cpu.pio.model);
	FinalizeButtons(lpCalc);
}

void FinalizeButtons(LPCALC lpCalc) {
	int group, bit;
	keypad_t *kp = lpCalc->cpu.pio.keypad;
	if (lpCalc->model >= TI_84P)
		*ButtonCenter = ButtonCenter84;
	else
		*ButtonCenter = ButtonCenter83;

	for(group = 0; group < 7; group++) {
		for(bit = 0; bit < 8; bit++) {
			int val = kp->keys[group][bit];
			if (((val & KEY_STATEDOWN) && (val & KEY_MOUSEPRESS) == 0 || (val & KEY_KEYBOARDPRESS) == 0)) {
				kp->keys[group][bit] &= ~KEY_STATEDOWN;
			}
		}
	}
	if (lpCalc->bCutout && lpCalc->SkinEnabled) {
		EnableCutout(lpCalc);
	} else {
		DrawButtonStatesAll(lpCalc, lpCalc->hdcButtons, lpCalc->hdcKeymap);
		InvalidateRect(lpCalc->hwndFrame, NULL, TRUE);
		UpdateWindow(lpCalc->hwndFrame);
	}
}
