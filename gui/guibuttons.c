#include "stdafx.h"

#include "guibuttons.h"
#include "guiskin.h"
#include "guioptions.h"
#include "gui.h"
#include "guikeylist.h"
#include "resource.h"

RECT ButtonRect[64];

extern HINSTANCE g_hInst;

void FindButtonsRect(BitmapData *bitmapData) {
	BOOL hasSeen[64];
	ZeroMemory(hasSeen, sizeof(hasSeen));
	ZeroMemory(ButtonRect, sizeof(ButtonRect));

	UINT *pixels = (UINT *)bitmapData->Scan0;
	LPUINT pPixel = pixels;
	UINT pixel = 0;
	int bit, group, index;
	for (UINT y = 0; y < bitmapData->Height; y++) {
		for (UINT x = 0; x < bitmapData->Width; x++) {
			pixel = *pPixel++ & 0xFFFFFF;
			// loading an int this way means GetRValue and GetBValues are flipped
			if (pixel == RGB(0xFF, 0xFF, 0xFF) || GetBValue(pixel) != 0) {
				continue;
			}

			bit =  GetRValue(pixel) >> 4;
			group = GetGValue(pixel) >> 4;
			index = bit + (group << 3);
			RECT *rect = &ButtonRect[index];
			if (!hasSeen[index]) {
				rect->left = x;
				rect->right = x;
				rect->top = y;
				rect->bottom = y;
				hasSeen[index] = TRUE;
				continue;
			}

			if (x < rect->left) {
				rect->left = x;
			} else if (x > rect->right) {
				rect->right = x;
			}

			if (y > rect->bottom) {
				rect->bottom = y;
			}
		}
	}
}

extern key_string ti83pkeystrings[KEY_STRING_SIZE];
extern key_string ti86keystrings[KEY_STRING_SIZE];
void LogKeypress(LPMAINWINDOW lpMainWindow, int model, int group, int bit) {
	int i;
	key_string *keystrings = model == TI_85 || model == TI_86 ? ti86keystrings : ti83pkeystrings;
	for (i = 0; i < KEY_STRING_SIZE; i++) {
		if (keystrings[i].group == group && keystrings[i].bit == bit) {
			break;
		}
	}
	if (i == KEY_STRING_SIZE) {
		return;
	}
	key_string *current = (key_string *) malloc(sizeof(key_string));
	current->bit = bit;
	current->group = group;
	current->next = lpMainWindow->last_keypress_head;
	current->text = (TCHAR *) malloc(_tcslen(keystrings[i].text) + 1);
	StringCbCopy(current->text, _tcslen(keystrings[i].text) + 1, keystrings[i].text);
	lpMainWindow->last_keypress_head = current;
	lpMainWindow->num_keypresses++;

	if (lpMainWindow->num_keypresses > MAX_KEYPRESS_HISTORY) {
		key_string *current = lpMainWindow->last_keypress_head;
		key_string *last = current;
		while (current) {
			if (current->next != NULL) {
				last = current;
				current = current->next;
			} else {
				break;
			}
		}
		free(current->text);
		free(current);
		last->next = NULL;
		if (last == lpMainWindow->last_keypress_head) {
			lpMainWindow->last_keypress_head = NULL;
		}

		lpMainWindow->num_keypresses--;
	}

	if (lpMainWindow->hwndKeyListDialog) {
		SendMessage(lpMainWindow->hwndKeyListDialog, WM_USER, REFRESH_LISTVIEW, 0);
	}
}	

void DrawButtonShadow(HDC hdc, HDC hdcKeymap, RECT brect, UINT keymap_scale)
{
	int width = brect.right - brect.left;
	int height = brect.bottom - brect.top;
	COLORREF colormatch = GetPixel(hdcKeymap, brect.left + width / 2, brect.top + height / 2);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			COLORREF colortest = GetPixel(hdcKeymap, x + brect.left, y + brect.top);
			if (colortest == colormatch) {
				COLORREF colortestdown = GetPixel(hdcKeymap, x + brect.left, y + brect.top + 1);
				COLORREF colortestup = GetPixel(hdcKeymap, x + brect.left, y + brect.top - 1);
				COLORREF colortestright = GetPixel(hdcKeymap, x + brect.left + 1, y + brect.top);
				COLORREF colortestleft = GetPixel(hdcKeymap, x + brect.left - 1, y + brect.top);
#define SHADOW_OFFSET 100
#define SHADOW_CONSTANT 40
#define SHADOW_SIZE 3
				if (GetRValue(colortestdown) == 0xFF) {
					for (int i = 1; i < SHADOW_SIZE; i++) {
						SetPixel(hdc, x, y + i,  RGB(SHADOW_CONSTANT*i+SHADOW_OFFSET, SHADOW_CONSTANT*i+SHADOW_OFFSET, SHADOW_CONSTANT*i+SHADOW_OFFSET));
					}
				}
				if (GetRValue(colortestup) == 0xFF) {
					for (int i = 1; i < SHADOW_SIZE; i++) {
						SetPixel(hdc, x, y - i,  RGB(SHADOW_CONSTANT*i+SHADOW_OFFSET, SHADOW_CONSTANT*i+SHADOW_OFFSET, SHADOW_CONSTANT*i+SHADOW_OFFSET));
					}
				}
				if (GetRValue(colortestright) == 0xFF) {
					for (int i = 1; i < SHADOW_SIZE; i++) {
						SetPixel(hdc, x + i, y,  RGB(SHADOW_CONSTANT*i+SHADOW_OFFSET, SHADOW_CONSTANT*i+SHADOW_OFFSET, SHADOW_CONSTANT*i+SHADOW_OFFSET));
					}
				}
				if (GetRValue(colortestleft) == 0xFF) {
					for (int i = 1; i < SHADOW_SIZE; i++) {
						SetPixel(hdc, x - i, y,  RGB(SHADOW_CONSTANT*i+SHADOW_OFFSET, SHADOW_CONSTANT*i+SHADOW_OFFSET, SHADOW_CONSTANT*i+SHADOW_OFFSET));
					}
				}
			}
		}
	}
}

void DrawButtonStateNoSkin(HDC hdc, HDC hdcSkin, HDC hdcKeymap, RECT brect, UINT state, UINT keymap_scale) {
	DisplayButtonState dbs = (DisplayButtonState)state;
	LONG width = brect.right - brect.left;
	LONG height = brect.bottom - brect.top;
	HDC hdcMask = CreateCompatibleDC(hdc);
	HBITMAP hbm = CreateCompatibleBitmap(hdc, width, height);
	SelectObject(hdcMask, hbm);	
	BitBlt(hdc, 0, 0, width, height, hdcSkin, brect.left, brect.top, SRCCOPY);
	BitBlt(hdcMask, 0, 0, width, height, hdcKeymap, brect.left, brect.top, SRCCOPY);

	BITMAPINFOHEADER bih;
	ZeroMemory(&bih, sizeof(BITMAPINFOHEADER));
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = width;
	bih.biHeight = height;
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	bih.biCompression = BI_RGB;
	BITMAPINFO bi;
	bi.bmiHeader = bih;
	bi.bmiColors[0].rgbBlue = 0;
	bi.bmiColors[0].rgbGreen = 0;
	bi.bmiColors[0].rgbRed = 0;
	bi.bmiColors[0].rgbReserved = 0;

	DWORD dwBmpSize = ((width * bi.bmiHeader.biBitCount + 31) / 32) * 4 * height;
	LPUINT bitmap = (LPUINT)malloc(dwBmpSize);
	GetDIBits(hdcMask, hbm,
		0, height,
		bitmap,
		&bi, DIB_RGB_COLORS);

	LPUINT pPixel = bitmap;
	for (; pPixel < bitmap + (width * height); pPixel++) {
		if (*pPixel == 0xFFFFFFFF) {
			*pPixel = 0x00000000;
		} else if (dbs & DBS_COPY) {
			*pPixel = 0x00000000;
		} else if (dbs & DBS_LOCK) {
			*pPixel = 0x80FF0000;
		} else {
			*pPixel = 0x80000000;
		}
	}

	SetDIBitsToDevice(hdcMask, 0, 0, width, height, 0, 0, 0,
		height,
		bitmap,
		&bi, DIB_RGB_COLORS);

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;

	AlphaBlend(hdc, 0, 0, width, height, hdcMask, 0, 0, width, height, bf);

	free(bitmap);
	DeleteDC(hdcMask);
	return;
}


void DrawButtonState(HDC hdcSkin, HDC hdcKeymap, RECT brect, UINT state, UINT keymap_scale) {
	if (IsRectEmpty(&brect)) {
		return;
	}

	int width = brect.right - brect.left;
	int height = brect.bottom - brect.top;
	
	HDC hdc = CreateCompatibleDC(hdcSkin);
	HBITMAP hbmButton = CreateCompatibleBitmap(hdcSkin, width, height);
	SelectObject(hdc, hbmButton);

	DrawButtonStateNoSkin(hdc, hdcSkin, hdcKeymap, brect, state, keymap_scale);
	
	BitBlt(hdcSkin, brect.left, brect.top, width, height,
				hdc, 0, 0, SRCCOPY);
				
	DeleteObject(hbmButton);
	DeleteObject(hdc);
}

void DrawButtonStatesAll(LPMAINWINDOW lpMainWindow, HDC hdcSkin, HDC hdcKeymap) {
	if (lpMainWindow == NULL) {
		return;
	}

	LPCALC lpCalc = lpMainWindow->lpCalc;
	if (lpCalc == NULL) {
		return;
	}

	keypad_t *keypad = lpCalc->cpu.pio.keypad;
	if (keypad == NULL) {
		return;
	}

	int group, bit;
	POINT pt;
	RECT brect;
	UINT keymap_scale = (UINT)(1.0 / lpMainWindow->default_skin_scale);
	for(group = 0; group < 7; group++) {
		for(bit = 0; bit < 8; bit++) {
			brect = ButtonRect[bit + (group << 3)];
			if (brect.left != 0) {
				int val = keypad->keys[group][bit];
				if (val & KEY_LOCKPRESS) {
					DrawButtonState(hdcSkin, hdcKeymap, brect, DBS_LOCK | DBS_DOWN, keymap_scale);
				} else if ((val & KEY_MOUSEPRESS) || (val & KEY_KEYBOARDPRESS)) {
					DrawButtonState(hdcSkin, hdcKeymap, brect, DBS_PRESS | DBS_DOWN, keymap_scale);
				}
			}
		}
	}
	
	group	= 5;
	bit		= 0;
	brect = ButtonRect[bit + (group << 3)];
	int val = lpCalc->cpu.pio.keypad->on_pressed;
	if (val & KEY_LOCKPRESS) {
		DrawButtonState(hdcSkin, hdcKeymap, brect, DBS_LOCK | DBS_DOWN, keymap_scale);
	} else if ((val & KEY_MOUSEPRESS) || (val & KEY_KEYBOARDPRESS)) {
		DrawButtonState(hdcSkin, hdcKeymap, brect, DBS_PRESS | DBS_DOWN, keymap_scale);
	}
}

static int last_shift;
void HandleKeyDown(LPMAINWINDOW lpMainWindow, WPARAM key) {
	if (lpMainWindow == NULL) {
		return;
	}

	LPCALC lpCalc = lpMainWindow->lpCalc;
	if (lpCalc == NULL) {
		return;
	}

	/* make this an accel*/
	if (key == VK_F8) {
		if (lpCalc->speed == 100) {
			SendMessage(lpMainWindow->hwndFrame, WM_COMMAND, IDM_SPEED_QUADRUPLE, 0);
		} else {
			SendMessage(lpMainWindow->hwndFrame, WM_COMMAND, IDM_SPEED_NORMAL, 0);
		}
	}

	if (key == VK_SHIFT) {
		if (GetKeyState(VK_LSHIFT) & 0xFF00) {
			key = VK_LSHIFT;
		} else {
			key = VK_RSHIFT;
		}
		last_shift = key;
	}
	BOOL changed;
	keyprog_t *kp = keypad_key_press(&lpCalc->cpu, key, &changed);
	if (!kp) {
		return;
	}

	LogKeypress(lpMainWindow, lpCalc->model, kp->group, kp->bit);
	if (changed) {
		FinalizeButtons(lpMainWindow);
	}
}

void HandleKeyUp(LPMAINWINDOW lpMainWindow, WPARAM key) {
	if (lpMainWindow == NULL) {
		return;
	}

	LPCALC lpCalc = lpMainWindow->lpCalc;
	if (lpCalc == NULL) {
		return;
	}

	if (key == VK_SHIFT) {
		key = last_shift;
	}

	keypad_key_release(&lpCalc->cpu, key);
	FinalizeButtons(lpMainWindow);
}

void FinalizeButtons(LPMAINWINDOW lpMainWindow) {
	if (lpMainWindow == NULL) {
		return;
	}

	LPCALC lpCalc = lpMainWindow->lpCalc;
	if (lpCalc == NULL) {
		return;
	}

	int group, bit;
	keypad_t *kp = lpCalc->cpu.pio.keypad;

	if (kp) {
		for (group = 0; group < 7; group++) {
			for (bit = 0; bit < 8; bit++) {
				int val = kp->keys[group][bit];
				if (((val & KEY_STATEDOWN) && !(val & KEY_MOUSEPRESS) && !(val & KEY_KEYBOARDPRESS))) {
					kp->keys[group][bit] &= ~KEY_STATEDOWN;
				}
			}
		}
	}

	if (lpMainWindow->bSkinEnabled && !lpMainWindow->bCutout) {
		DrawButtonStatesAll(lpMainWindow, lpMainWindow->hdcButtons, lpMainWindow->hdcKeymap);
		InvalidateRect(lpMainWindow->hwndFrame, NULL, FALSE);
	}
}
