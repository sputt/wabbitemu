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

			if ((LONG)x < rect->left) {
				rect->left = x;
			} else if ((LONG)x > rect->right) {
				rect->right = x;
			}

			if ((LONG)y > rect->bottom) {
				rect->bottom = y;
			}
		}
	}
}

void LogKeypress(LPMAINWINDOW lpMainWindow, int model, int group, int bit) {
	if (!lpMainWindow->keylogging_enabled) {
		return;
	}

	int i;
	key_string_t *keystrings = model == TI_85 || model == TI_86 ? ti86keystrings : ti83pkeystrings;
	for (i = 0; i < KEY_STRING_SIZE; i++) {
		if (keystrings[i].group == group && keystrings[i].bit == bit) {
			break;
		}
	}

	key_string_t current;
	current.bit = bit;
	current.group = group;
	if (i != KEY_STRING_SIZE) {
		current.text = keystrings[i].text;
	}

	lpMainWindow->keys_pressed->push_back(current);

	if (lpMainWindow->keys_pressed->size() >= MAX_KEYPRESS_HISTORY) {
		lpMainWindow->keys_pressed->pop_front();
	}

	if (lpMainWindow->hwndKeyListDialog) {
		SendMessage(lpMainWindow->hwndKeyListDialog, WM_USER, REFRESH_LISTVIEW, 0);
	}
}	

void DrawButtonStateNoSkin(Bitmap *pBitmapButton, Bitmap *pBitmapSkin, Bitmap *pBitmapKeymap,
	RECT brect, UINT state)
{
	DisplayButtonState dbs = (DisplayButtonState)state;
	LONG width = brect.right - brect.left;
	LONG height = brect.bottom - brect.top;
	Rect buttonRect(0, 0, width, height);
	
	Bitmap bitmapMask(width, height, PixelFormat32bppARGB);
	Graphics graphics(pBitmapButton);
	Graphics mask(&bitmapMask);
	graphics.DrawImage(pBitmapSkin, buttonRect, brect.left, brect.top, width, height, UnitPixel);
	mask.DrawImage(pBitmapKeymap, buttonRect, brect.left, brect.top, width, height, UnitPixel);

	BitmapData *data = new BitmapData;
	bitmapMask.LockBits(&buttonRect, ImageLockModeWrite, PixelFormat32bppARGB, data);

	LPUINT pPixel = (LPUINT) data->Scan0;
	for (; pPixel < (LPUINT) data->Scan0 + (width * height); pPixel++) {
		if (dbs & DBS_COPY) {
			if (*pPixel != 0xFFFFFFFF) {
				*pPixel = 0x00000000;
			}
		} else if (*pPixel == 0xFFFFFFFF) {
			*pPixel = 0x00000000;
		} else if (dbs & DBS_LOCK) {
			*pPixel = 0x80FF0000;
		} else {
			*pPixel = 0x80000000;
		}
	}

	bitmapMask.UnlockBits(data);

	graphics.DrawImage(&bitmapMask, buttonRect);

	delete data;
}


void DrawButtonState(Bitmap *pBitmapSkin, Bitmap *pBitmapKeymap, RECT brect, UINT state) {
	if (IsRectEmpty(&brect)) {
		return;
	}

	int width = brect.right - brect.left;
	int height = brect.bottom - brect.top;
	
	Bitmap buttonBitmap(width, height, PixelFormat32bppARGB);
	DrawButtonStateNoSkin(&buttonBitmap, pBitmapSkin, pBitmapKeymap, brect, state);
	
	Graphics graphics(pBitmapSkin);
	graphics.DrawImage(&buttonBitmap, brect.left, brect.top, width, height);
}

void DrawButtonStatesAll(keypad_t *keypad, HWND hwndFrame, Bitmap *pBitmapSkin,
	Bitmap *pBitmapKeymap, double skinScale)
{
	if (keypad == NULL) {
		return;
	}

	int group, bit;
	RECT brect;
	for(group = 0; group < 7; group++) {
		for(bit = 0; bit < 8; bit++) {
			brect = ButtonRect[bit + (group << 3)];
			if (brect.left != 0) {
				int val = keypad->keys[group][bit];
				if (val & KEY_LOCKPRESS) {
					DrawButtonState(pBitmapSkin, pBitmapKeymap, brect, DBS_LOCK | DBS_DOWN);
				} else if ((val & KEY_MOUSEPRESS) || (val & KEY_KEYBOARDPRESS)) {
					DrawButtonState(pBitmapSkin, pBitmapKeymap, brect, DBS_PRESS | DBS_DOWN);
				}

				RECT scaleRect;
				CopyRect(&scaleRect, &brect);
				scaleRect.left = (LONG)(scaleRect.left * skinScale);
				scaleRect.right = (LONG)(scaleRect.right * skinScale);
				scaleRect.top = (LONG)(scaleRect.top * skinScale);
				scaleRect.bottom = (LONG)(scaleRect.bottom * skinScale);
				InvalidateRect(hwndFrame, &scaleRect, FALSE);
			}
		}
	}
	
	group	= 5;
	bit		= 0;
	brect = ButtonRect[bit + (group << 3)];
	int val = keypad->on_pressed;
	if (val & KEY_LOCKPRESS) {
		DrawButtonState(pBitmapSkin, pBitmapKeymap, brect, DBS_LOCK | DBS_DOWN);
	} else if ((val & KEY_MOUSEPRESS) || (val & KEY_KEYBOARDPRESS)) {
		DrawButtonState(pBitmapSkin, pBitmapKeymap, brect, DBS_PRESS | DBS_DOWN);
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

	HandleTeacherViewKey(lpMainWindow->hwndTeacherView, lpCalc);
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

	keyprog_t *kp = keypad_key_release(&lpCalc->cpu, key);
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

	keypad_t *kp = lpCalc->cpu.pio.keypad;

	if (kp != NULL) {
		for (int group = 0; group < 7; group++) {
			for (int bit = 0; bit < 8; bit++) {
				int val = kp->keys[group][bit];
				if (((val & KEY_STATEDOWN) && !(val & KEY_MOUSEPRESS) && !(val & KEY_KEYBOARDPRESS))) {
					kp->keys[group][bit] &= ~KEY_STATEDOWN;
				}
			}
		}
	}

	if (lpMainWindow->bSkinEnabled) {
		DrawButtonStatesAll(lpCalc->cpu.pio.keypad, lpMainWindow->hwndFrame,
			lpMainWindow->m_lpBitmapRenderedSkin, lpMainWindow->m_lpBitmapKeymap, lpMainWindow->skin_scale);
	}
}
