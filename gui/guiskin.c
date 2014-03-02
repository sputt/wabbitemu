#include "stdafx.h"

#include "guiskin.h"

#include "gui.h"
#include "guibuttons.h"
#include "guicutout.h"
#include "guisize.h"
#include "CGdiPlusBitmap.h"

extern BOOL silent_mode;
extern HINSTANCE g_hInst;
#define LCD_MARKER_COLOR 0xFFFF0000

BOOL FindLCDRect(BitmapData *bitmapData, RECT *rectLCD) {
	int foundX = -1, foundY = -1;
	// find the top left corner of the screen (marked as a red rectangle)
	UINT *pixels = (UINT *)bitmapData->Scan0; 
	UINT pixel = 0;
	for (UINT y = 0; y < bitmapData->Height && foundX == -1; y++) {
		for (UINT x = 0; x < bitmapData->Width && foundY == -1; x++) {
			pixel = pixels[y * bitmapData->Width + x];
			if (pixel == LCD_MARKER_COLOR)	{
				//81 92
				foundX = x;
				foundY = y;
			}
		}
	}
	if (foundX == -1 || foundY == -1) {
		return FALSE;
	}
	rectLCD->left = foundX;
	rectLCD->top = foundY;
	//find right edge
	do {
		foundX++;
		pixel = pixels[foundY * bitmapData->Width + foundX];
	} while (pixel == LCD_MARKER_COLOR);
	rectLCD->right = foundX--;
	//find left edge
	do { 
		foundY++;
		pixel = pixels[foundY * bitmapData->Width + foundX];
	} while (pixel == LCD_MARKER_COLOR);
	rectLCD->bottom = foundY;
	return TRUE;
}

void UpdateWabbitemuMainWindow(LPMAINWINDOW lpMainWindow) {
	RECT rc;
	HMENU hMenu = GetMenu(lpMainWindow->hwndFrame);
	BOOL bChecked;
	if (lpMainWindow->hwndStatusBar != NULL) {
		DestroyWindow(lpMainWindow->hwndStatusBar);
		lpMainWindow->hwndStatusBar = NULL;
	}

	if (lpMainWindow->bSkinEnabled) {
		bChecked = MF_CHECKED;
		CopyRect(&rc, &lpMainWindow->rectSkin);
		LONG screenHeight = (LONG) GetSystemMetrics(SM_CYVIRTUALSCREEN);
		LONG clientHeight = rc.bottom - rc.top;
		// not checking if you screenWidth < rectWidth
		// please save yourself if you have a screen < 300px
		if (screenHeight < clientHeight * lpMainWindow->default_skin_scale) {
			// you have a tiny computer screen, scale the skin down to fit
			LONG clientWidth = rc.right - rc.left;
			LONG newWidth = SKIN_WIDTH * clientHeight / SKIN_HEIGHT;

			LONG heightDiff = clientHeight - screenHeight;
			LONG widthDiff = clientWidth - newWidth;

			lpMainWindow->skin_scale = GetSkinScale(clientWidth, clientHeight,
				&widthDiff, &heightDiff, lpMainWindow->default_skin_scale);
		}

		rc.left = (LONG)(rc.left * lpMainWindow->skin_scale);
		rc.top = (LONG)(rc.top * lpMainWindow->skin_scale);
		rc.right = (LONG)(rc.right * lpMainWindow->skin_scale);
		rc.bottom = (LONG)(rc.bottom * lpMainWindow->skin_scale);
		AdjustWindowRect(&rc, WS_CAPTION | WS_TILEDWINDOW , FALSE);
		rc.bottom += GetSystemMetrics(SM_CYMENU);
	} else {
		LPCALC lpCalc = lpMainWindow->lpCalc;
		bChecked = MF_UNCHECKED;
		// Create status bar
		// fix scale so that it is valid for this calc
		lpMainWindow->scale = max(GetDefaultKeymapScale(lpCalc->model), lpMainWindow->scale);
		SetRect(&rc, 0, 0, lpCalc->cpu.pio.lcd->width * lpMainWindow->scale, lpCalc->cpu.pio.lcd->height * lpMainWindow->scale);
		int iStatusWidths[] = { 100, -1 };
		lpMainWindow->hwndStatusBar = CreateWindow(STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0, lpMainWindow->hwndFrame, NULL, g_hInst, NULL);
		// set the text. Text on the left will be the FPS (set in LCD code)
		// text on the right will be the model currently being emulated
		SendMessage(lpMainWindow->hwndStatusBar, SB_SETPARTS, 2, (LPARAM)&iStatusWidths);
		SendMessage(lpMainWindow->hwndStatusBar, SB_SETTEXT, 1, (LPARAM)CalcModelTxt[lpCalc->model]);
		// get the height of the status bar and factor that into our total app window height
		RECT src;
		GetWindowRect(lpMainWindow->hwndStatusBar, &src);
		AdjustWindowRect(&rc, (WS_TILEDWINDOW | WS_CLIPCHILDREN) & ~WS_MAXIMIZEBOX, hMenu != NULL);
		rc.bottom += src.bottom - src.top;
	}

	if (hMenu) {
		CheckMenuItem(hMenu, IDM_VIEW_SKIN, MF_BYCOMMAND | bChecked);
	}

	UINT flags = SWP_NOMOVE;
	if (!lpMainWindow->bAlwaysOnTop) {
		flags |= SWP_NOZORDER;
	}

	if (silent_mode == TRUE) {
		flags |= SWP_HIDEWINDOW;
	} else {
		flags |= SWP_SHOWWINDOW;
	}

	SetWindowPos(lpMainWindow->hwndFrame, HWND_TOPMOST, 0, 0, rc.right - rc.left, rc.bottom - rc.top, flags);
}

enum DRAWSKINERROR {
	ERROR_FACEPLATE = 1,
	ERROR_SKIN,
	ERROR_KEYMAP,
};

DRAWSKINERROR DrawSkin(HDC hdc, LPMAINWINDOW lpMainWindow, Bitmap *m_pBitmapSkin, Bitmap *m_pBitmapKeymap) {
	if (!m_pBitmapSkin) {
		return ERROR_SKIN;
	}

	if (!m_pBitmapKeymap) {
		return ERROR_KEYMAP;
	}
	
	LPCALC lpCalc = lpMainWindow->lpCalc;

	HBITMAP hbmSkinOld, hbmKeymapOld;
	// translate to regular GDI compatibility to simplify coding :/
	m_pBitmapKeymap->GetHBITMAP(Color::White, &hbmKeymapOld);
	SelectObject(lpMainWindow->hdcKeymap, hbmKeymapOld);
	// get the HBITMAP for the skin DONT change the first value, it is necessary for transparency to work
	m_pBitmapSkin->GetHBITMAP(Color::AlphaMask, &hbmSkinOld);
	HDC hdcOverlay = CreateCompatibleDC(lpMainWindow->hdcSkin);
	HBITMAP blankBitmap = CreateCompatibleBitmap(hdc, m_pBitmapSkin->GetWidth(), m_pBitmapSkin->GetHeight());
	SelectObject(lpMainWindow->hdcSkin, blankBitmap);
	if (!lpMainWindow->bCutout || !lpMainWindow->bSkinEnabled) {
		FillRect(lpMainWindow->hdcSkin, &lpMainWindow->rectSkin, GetStockBrush(GRAY_BRUSH));
	}

	LONG skinWidth = lpMainWindow->rectSkin.right;
	LONG skinHeight = lpMainWindow->rectSkin.bottom;
	BOOL drawFaceplate = lpCalc->model == TI_84PSE || lpCalc->model == TI_84PCSE && !lpMainWindow->bCustomSkin;
	if (drawFaceplate) {
		if (DrawFaceplateRegion(lpMainWindow->hdcSkin, lpMainWindow->default_skin_scale, lpMainWindow->FaceplateColor)) {
			return ERROR_FACEPLATE;
		}
	}

	// this needs to be done so we can alpha blend the screen
	SelectObject(hdcOverlay, hbmSkinOld);
	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;
	
	AlphaBlend(lpMainWindow->hdcSkin, 0, 0, skinWidth, skinHeight, hdcOverlay,
		lpMainWindow->rectSkin.left, lpMainWindow->rectSkin.top, skinWidth, skinHeight, bf);
	BitBlt(lpMainWindow->hdcButtons, 0, 0, skinWidth, skinHeight, lpMainWindow->hdcSkin, 0, 0, SRCCOPY);
	FinalizeButtons(lpMainWindow);

	if (drawFaceplate) {
		BITMAPINFOHEADER bih;
		ZeroMemory(&bih, sizeof(BITMAPINFOHEADER));
		bih.biSize = sizeof(BITMAPINFOHEADER);
		bih.biWidth = skinWidth;
		bih.biHeight = skinHeight;
		bih.biPlanes = 1;
		bih.biBitCount = 32;
		bih.biCompression = BI_RGB;
		BITMAPINFO bi;
		bi.bmiHeader = bih;
		bi.bmiColors[0].rgbBlue = 0;
		bi.bmiColors[0].rgbGreen = 0;
		bi.bmiColors[0].rgbRed = 0;
		bi.bmiColors[0].rgbReserved = 0;
		// Gets the "bits" from the bitmap and copies them into a buffer
		// which is pointed to by lpBitmap.

		DWORD dwBmpSize = ((skinWidth * bi.bmiHeader.biBitCount + 31) / 32) * 4 * skinHeight;
		LPUINT bitmap = (LPUINT) malloc(dwBmpSize);		

		GetDIBits(lpMainWindow->hdcSkin, (HBITMAP)GetCurrentObject(lpMainWindow->hdcButtons, OBJ_BITMAP),
			0, skinHeight,
			bitmap,
			&bi, DIB_RGB_COLORS);

		// this really sucked to figure out, but basically you can't touch
		// the alpha channel in a bitmap unless you use GetDIBits to get it
		// in an array, change the highest byte, then reset with SetDIBits
		// This colors the faceplate that way
		LPUINT pPixel = bitmap;
		HRGN rgn = GetFaceplateRegion(lpMainWindow->default_skin_scale);
		unsigned int x, y;
		for (y = 0; y < skinHeight; y++) {
			for (x = 0; x < skinWidth; x++) {
				if (PtInRegion(rgn, x, skinHeight - y)) {
					*pPixel |= 0xFF000000;
				}
				pPixel++;
			}
		}

		SetDIBitsToDevice(lpMainWindow->hdcButtons, 0, 0, skinWidth, skinHeight, 0, 0, 0,
			skinHeight,
			bitmap,
			&bi, DIB_RGB_COLORS);
		DeleteObject(rgn);
		free(bitmap);
	}

	DeleteObject(hbmKeymapOld);
	DeleteObject(hbmSkinOld);
	DeleteObject(blankBitmap);
	DeleteDC(hdcOverlay);

	return (DRAWSKINERROR) ERROR_SUCCESS;
}

// TRUE if success, FALSE if failure
int gui_frame_update(LPMAINWINDOW lpMainWindow) {
	if (lpMainWindow == NULL) {
		return FALSE;
	}

	int skinWidth = 0, skinHeight = 0, keymapWidth = -1, keymapHeight = -1;
	HDC hdc = GetDC(lpMainWindow->hwndFrame);
	if (lpMainWindow->hdcKeymap) {
		DeleteDC(lpMainWindow->hdcKeymap);
	}

	if (lpMainWindow->hdcSkin) {
		DeleteDC(lpMainWindow->hdcSkin);
	}

	if (lpMainWindow->hdcButtons) {
		DeleteDC(lpMainWindow->hdcButtons);
	}

	lpMainWindow->hdcKeymap = CreateCompatibleDC(hdc);
	lpMainWindow->hdcSkin = CreateCompatibleDC(hdc);
	lpMainWindow->hdcButtons = CreateCompatibleDC(hdc);
	//load skin and keymap
	CGdiPlusBitmapResource hbmSkin, hbmKeymap;
	Bitmap *m_pBitmapSkin = NULL, *m_pBitmapKeymap = NULL;
	if (lpMainWindow->bCustomSkin) {
#ifdef _UNICODE
		m_pBitmapSkin = new Bitmap(lpCalc->skin_path);
		m_pBitmapKeymap = new Bitmap(lpCalc->keymap_path);
#else
		wchar_t widePath[MAX_PATH];
		size_t converted;
		mbstowcs_s(&converted, widePath, lpMainWindow->skin_path, (size_t) ARRAYSIZE(widePath));
		m_pBitmapSkin = new Bitmap(widePath);
		mbstowcs_s(&converted, widePath, lpMainWindow->keymap_path, (size_t)ARRAYSIZE(widePath));
		m_pBitmapKeymap = new Bitmap(widePath);
#endif
	}

	if (!m_pBitmapSkin || m_pBitmapSkin->GetWidth() == 0 || m_pBitmapKeymap->GetWidth() == 0) {
		if (lpMainWindow->bCustomSkin) {
			MessageBox(lpMainWindow->hwndFrame, _T("Custom skin failed to load."), _T("Error"), MB_OK);
			delete m_pBitmapKeymap;
			delete m_pBitmapSkin;
			m_pBitmapKeymap = NULL;
			m_pBitmapSkin = NULL;
			// your skin failed to load, lets disable it and load the normal skin
			lpMainWindow->bCustomSkin = FALSE;
		}

		LPCALC lpCalc = lpMainWindow->lpCalc;
		hbmSkin.Load(CalcModelTxt[lpCalc->model], _T("PNG"), g_hInst);
		switch(lpCalc->model) {
			case TI_81:
				hbmKeymap.Load(_T("TI-81Keymap"), _T("PNG"), g_hInst);
				break;
			case TI_82:
				hbmKeymap.Load(_T("TI-82Keymap"), _T("PNG"), g_hInst);
				break;
			case TI_83:
				hbmKeymap.Load(_T("TI-83Keymap"), _T("PNG"), g_hInst);
				break;
			case TI_84P:
			case TI_84PSE:
				hbmKeymap.Load(_T("TI-84+SEKeymap"), _T("PNG"), g_hInst);
				break;
			case TI_84PCSE:
				hbmKeymap.Load(_T("TI-84+CSEKeymap"), _T("PNG"), g_hInst);
				break;
			case TI_85:
				hbmKeymap.Load(_T("TI-85Keymap"), _T("PNG"), g_hInst);
				break;
			case TI_86:
				hbmKeymap.Load(_T("TI-86Keymap"), _T("PNG"), g_hInst);
				break;
			case TI_73:
			case TI_83P:
			case TI_83PSE:
			default:
				hbmKeymap.Load(_T("TI-83+Keymap"), _T("PNG"), g_hInst);
				break;
		}
		m_pBitmapSkin = hbmSkin.m_pBitmap;
		m_pBitmapKeymap = hbmKeymap.m_pBitmap;
	}

	if (m_pBitmapSkin) {
		skinWidth = m_pBitmapSkin->GetWidth();
		skinHeight = m_pBitmapSkin->GetHeight();
	}

	if (m_pBitmapKeymap) {
		keymapWidth = m_pBitmapKeymap->GetWidth();
		keymapHeight = m_pBitmapKeymap->GetHeight();
	}

	if ((skinWidth % SKIN_WIDTH) || (skinHeight % SKIN_HEIGHT) || skinHeight <= 0 || skinWidth <= 0) {
		lpMainWindow->bSkinEnabled = false;
		MessageBox(lpMainWindow->hwndFrame, _T("Invalid skin size."), _T("Error"), MB_OK | MB_ICONERROR);
		return 0;
	} else if ((skinWidth != keymapWidth) || (skinHeight != keymapHeight)) {
		lpMainWindow->bSkinEnabled = false;
		MessageBox(lpMainWindow->hwndFrame, _T("Skin and Keymap are not the same size"), _T("Error"), MB_OK | MB_ICONERROR);
		return 0;
	} else {
		if (lpMainWindow->skin_scale == 0.0) {
			lpMainWindow->skin_scale = 1.0;
		} else if (lpMainWindow->default_skin_scale) {
			lpMainWindow->skin_scale = lpMainWindow->skin_scale / lpMainWindow->default_skin_scale;
		}

		lpMainWindow->default_skin_scale = (double)SKIN_WIDTH / skinWidth;
		lpMainWindow->skin_scale = lpMainWindow->skin_scale * lpMainWindow->default_skin_scale;
		
		lpMainWindow->rectSkin.right = skinWidth;
		lpMainWindow->rectSkin.bottom = skinHeight;
		BitmapData *data = new BitmapData;
		Rect keymapRect(0, 0, keymapWidth, keymapHeight);
		m_pBitmapKeymap->LockBits(&keymapRect, ImageLockModeRead, PixelFormat32bppARGB, data);
		BOOL foundScreen = FindLCDRect(data, &lpMainWindow->rectLCD);

		if (!foundScreen) {
			free(data);
			MessageBox(lpMainWindow->hwndFrame, _T("Unable to find the screen box"), _T("Error"), MB_OK);
			lpMainWindow->bSkinEnabled = false;
			return FALSE;
		}

		FindButtonsRect(data);

		free(data);

	}

	if (lpMainWindow->hwndFrame == NULL) {
		return FALSE;
	}

	// set the size of the HDC
	HBITMAP hbmTemp = CreateCompatibleBitmap(hdc, lpMainWindow->rectSkin.right, lpMainWindow->rectSkin.bottom);
	SelectObject(lpMainWindow->hdcButtons, hbmTemp);
	
	switch (DrawSkin(hdc, lpMainWindow, m_pBitmapSkin, m_pBitmapKeymap)) {
		case ERROR_FACEPLATE:
			MessageBox(lpMainWindow->hwndFrame, _T("Unable to draw faceplate"), _T("Error"), MB_OK);
			break;
		case ERROR_SKIN:
			MessageBox(lpMainWindow->hwndFrame, _T("Unable to load skin resource"), _T("Error"), MB_OK);
			break;
		case ERROR_KEYMAP:
			MessageBox(lpMainWindow->hwndFrame, _T("Unable to load keymap resource"), _T("Error"), MB_OK);
			break;
	}

	if (lpMainWindow->bCutout && lpMainWindow->bSkinEnabled) {
		if (EnableCutout(lpMainWindow) != 0) {
			MessageBox(lpMainWindow->hwndFrame, _T("Couldn't cutout window"), _T("Error"), MB_OK);
		}
	} else {
		DisableCutout(lpMainWindow);
	}

	UpdateWabbitemuMainWindow(lpMainWindow);

	if (lpMainWindow->bCustomSkin) {
		if (m_pBitmapKeymap) {
			delete m_pBitmapKeymap;
		}
		if (m_pBitmapSkin) {
			delete m_pBitmapSkin;
		}
	}
	DeleteObject(hbmTemp);
	ReleaseDC(lpMainWindow->hwndFrame, hdc);

	SendMessage(lpMainWindow->hwndFrame, WM_SIZE, SIZE_RESTORED, 0);

	return TRUE;
}

static POINT ptRgnEdge[] = {{75,675},
							{95,683},
							{262,682},
							{279,675},
							{316,535},
							{316,273},
							{37,273},
							{37,568},
// the skin is symmetrical so we can use that to generate the rest of the points
							{SKIN_WIDTH-37,568},
							{SKIN_WIDTH-37,273},
							{SKIN_WIDTH-316,273},
							{SKIN_WIDTH-316,535},
							{SKIN_WIDTH-279,675},
							{SKIN_WIDTH-262,682},
							{SKIN_WIDTH-95,683},
							{SKIN_WIDTH-75,675}};

HRGN GetFaceplateRegion(double default_skin_scale) {
	unsigned int nPoints = sizeof(ptRgnEdge) / sizeof(POINT);
	POINT *points = (POINT *) malloc(sizeof(ptRgnEdge));
	UINT scale = (UINT)(1.0 / default_skin_scale);
	for (int i = 0; i < nPoints; i++) {
		points[i].x = ptRgnEdge[i].x * scale;
		points[i].y = ptRgnEdge[i].y * scale;
	}

	HRGN hrgn = CreatePolygonRgn(points, nPoints, WINDING);
	free(points);
	return hrgn;

}

int DrawFaceplateRegion(HDC hdc, double default_skin_scale, COLORREF ref) {
	HRGN hrgn = GetFaceplateRegion(default_skin_scale);
	if (hrgn == NULL) {
		return 1;
	}

	HBRUSH hFaceplateColor = CreateSolidBrush(ref);
	FillRgn(hdc, hrgn, hFaceplateColor);
	DeleteObject(hFaceplateColor);
	DeleteObject(hrgn);
	return 0;
}