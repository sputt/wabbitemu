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

BOOL FindLCDRect(Bitmap *m_pBitmapKeymap, u_int skinWidth, u_int skinHeight, RECT *rectLCD) {
	int foundX = -1, foundY = -1;
	Color pixel;
	//find the top left corner
	for (u_int y = 0; y < skinHeight && foundX == -1; y++) {
		for (u_int x = 0; x < skinWidth && foundY == -1; x++) {
			m_pBitmapKeymap->GetPixel(x, y, &pixel);
			if (pixel.GetValue() == LCD_MARKER_COLOR)	{
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
		m_pBitmapKeymap->GetPixel(foundX, foundY, &pixel);
	} while (pixel.GetValue() == LCD_MARKER_COLOR);
	rectLCD->right = foundX--;
	//find left edge
	do { 
		foundY++;
		m_pBitmapKeymap->GetPixel(foundX, foundY, &pixel);
	} while (pixel.GetValue() == LCD_MARKER_COLOR);
	rectLCD->bottom = foundY;
	return TRUE;
}

void UpdateWabbitemuMainWindow(LPCALC lpCalc) {
	RECT rc;
	HMENU hMenu = GetMenu(lpCalc->hwndFrame);
	BOOL bChecked;
	if (lpCalc->hwndStatusBar != NULL) {
		DestroyWindow(lpCalc->hwndStatusBar);
		lpCalc->hwndStatusBar = NULL;
	}

	if (lpCalc->bSkinEnabled) {
		bChecked = MF_CHECKED;
		CopyRect(&rc, &lpCalc->rectSkin);
		int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
		// not checking if you screenWidth < rectWidth
		// please save yourself if you have a screen < 300px
		if (screenHeight < (rc.bottom - rc.top) * lpCalc->default_skin_scale) {
			// you have a tiny computer screen, scale the skin down to fit
			int clientWidth = rc.right - rc.left;
			int clientHeight = rc.bottom - rc.top;

			int heightDiff = clientHeight - screenHeight;
			int newWidth = SKIN_WIDTH * clientHeight / SKIN_HEIGHT;
			int widthDiff = clientWidth - newWidth;

			double width_scale = (double)newWidth / SKIN_WIDTH;
			double height_scale = (double)screenHeight / SKIN_HEIGHT;
			lpCalc->skin_scale = min(width_scale, height_scale) * lpCalc->default_skin_scale;
		}

		rc.left = (LONG)(rc.left * lpCalc->skin_scale);
		rc.top = (LONG)(rc.top * lpCalc->skin_scale);
		rc.right = (LONG)(rc.right * lpCalc->skin_scale);
		rc.bottom = (LONG)(rc.bottom * lpCalc->skin_scale);
		AdjustWindowRect(&rc, WS_CAPTION | WS_TILEDWINDOW , FALSE);
		rc.bottom += GetSystemMetrics(SM_CYMENU);
	} else {
		bChecked = MF_UNCHECKED;
		// Create status bar
		// fix scale so that it is valid for this calc
		lpCalc->scale = max(GetDefaultKeymapScale(lpCalc->model), lpCalc->scale);
		SetRect(&rc, 0, 0, lpCalc->cpu.pio.lcd->width * lpCalc->scale, lpCalc->cpu.pio.lcd->height * lpCalc->scale);
		int iStatusWidths[] = { 100, -1 };
		lpCalc->hwndStatusBar = CreateWindow(STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0, lpCalc->hwndFrame, NULL, g_hInst, NULL);
		// set the text. Text on the left will be the FPS (set in LCD code)
		// text on the right will be the model currently being emulated
		SendMessage(lpCalc->hwndStatusBar, SB_SETPARTS, 2, (LPARAM) &iStatusWidths);
		SendMessage(lpCalc->hwndStatusBar, SB_SETTEXT, 1, (LPARAM) CalcModelTxt[lpCalc->model]);
		// get the height of the status bar and factor that into our total app window height
		RECT src;
		GetWindowRect(lpCalc->hwndStatusBar, &src);
		AdjustWindowRect(&rc, (WS_TILEDWINDOW | WS_CLIPCHILDREN) & ~WS_MAXIMIZEBOX, hMenu != NULL);
		rc.bottom += src.bottom - src.top;
	}

	if (hMenu) {
		CheckMenuItem(hMenu, IDM_VIEW_SKIN, MF_BYCOMMAND | bChecked);
	}

	UINT flags = SWP_NOMOVE;
	if (!lpCalc->bAlwaysOnTop) {
		flags |= SWP_NOZORDER;
	}

	SetWindowPos(lpCalc->hwndFrame, HWND_TOPMOST, 0, 0, rc.right - rc.left, rc.bottom - rc.top, flags);
}

enum DRAWSKINERROR {
	ERROR_FACEPLATE = 1,
	ERROR_SKIN,
	ERROR_KEYMAP,
};

DRAWSKINERROR DrawSkin(HDC hdc, LPCALC lpCalc, Bitmap *m_pBitmapSkin, Bitmap *m_pBitmapKeymap) {
	if (!m_pBitmapSkin) {
		return ERROR_SKIN;
	}

	if (!m_pBitmapKeymap) {
		return ERROR_KEYMAP;
	}
	
	HBITMAP hbmSkinOld, hbmKeymapOld;
	// translate to regular GDI compatibility to simplify coding :/
	m_pBitmapKeymap->GetHBITMAP(Color::White, &hbmKeymapOld);
	SelectObject(lpCalc->hdcKeymap, hbmKeymapOld);
	// get the HBITMAP for the skin DONT change the first value, it is necessary for transparency to work
	m_pBitmapSkin->GetHBITMAP(Color::AlphaMask, &hbmSkinOld);
	HDC hdcOverlay = CreateCompatibleDC(lpCalc->hdcSkin);
	HBITMAP blankBitmap = CreateCompatibleBitmap(hdc, m_pBitmapSkin->GetWidth(), m_pBitmapSkin->GetHeight());
	SelectObject(lpCalc->hdcSkin, blankBitmap);
	if (!lpCalc->bCutout || !lpCalc->bSkinEnabled) {
		FillRect(lpCalc->hdcSkin, &lpCalc->rectSkin, GetStockBrush(GRAY_BRUSH));
	}

	unsigned int skinWidth = lpCalc->rectSkin.right;
	unsigned int skinHeight = lpCalc->rectSkin.bottom;
	BOOL drawFaceplate = lpCalc->model == TI_84PSE || lpCalc->model == TI_84PCSE && !lpCalc->bCustomSkin;
	if (drawFaceplate) {
		if (DrawFaceplateRegion(lpCalc, lpCalc->hdcSkin, lpCalc->FaceplateColor)) {
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
	
	AlphaBlend(lpCalc->hdcSkin, 0, 0, skinWidth, skinHeight, hdcOverlay,
		lpCalc->rectSkin.left, lpCalc->rectSkin.top, lpCalc->rectSkin.right, lpCalc->rectSkin.bottom, bf);
	BitBlt(lpCalc->hdcButtons, 0, 0, skinWidth, skinHeight, lpCalc->hdcSkin, 0, 0, SRCCOPY);
	FinalizeButtons(lpCalc);

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
		LPBYTE bitmap = (LPBYTE) malloc(dwBmpSize);		

		GetDIBits(lpCalc->hdcSkin, (HBITMAP)GetCurrentObject(lpCalc->hdcButtons, OBJ_BITMAP),
			0, skinHeight,
			bitmap,
			&bi, DIB_RGB_COLORS);

		// this really sucked to figure out, but basically you can't touch
		// the alpha channel in a bitmap unless you use GetDIBits to get it
		// in an array, change the highest byte, then reset with SetDIBits
		// This colors the faceplate that way
		BYTE* pPixel = bitmap;
		HRGN rgn = GetFaceplateRegion(lpCalc);
		unsigned int x, y;
		for (y = 0; y < skinHeight; y++) {
			for (x = 0; x < skinWidth; x++) {
				if (PtInRegion(rgn, x, skinHeight - y)) {
					pPixel[3] = 0xFF;
				}
				pPixel += 4;
			}
		}

		SetDIBitsToDevice(lpCalc->hdcButtons, 0, 0, skinWidth, skinHeight, 0, 0, 0,
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

int gui_frame_update(LPCALC lpCalc) {
	int skinWidth = 0, skinHeight = 0, keymapWidth = -1, keymapHeight = -1;
	HDC hdc = GetDC(lpCalc->hwndFrame);
	if (lpCalc->hdcKeymap) {
		DeleteDC(lpCalc->hdcKeymap);
	}

	if (lpCalc->hdcSkin) {
		DeleteDC(lpCalc->hdcSkin);
	}

	if (lpCalc->hdcButtons) {
		DeleteDC(lpCalc->hdcButtons);
	}

	lpCalc->hdcKeymap = CreateCompatibleDC(hdc);
	lpCalc->hdcSkin = CreateCompatibleDC(hdc);
	lpCalc->hdcButtons = CreateCompatibleDC(hdc);
	//load skin and keymap
	CGdiPlusBitmapResource hbmSkin, hbmKeymap;
	Bitmap *m_pBitmapSkin = NULL, *m_pBitmapKeymap = NULL;
	if (lpCalc->bCustomSkin) {
#ifdef _UNICODE
		m_pBitmapSkin = new Bitmap(lpCalc->skin_path);
		m_pBitmapKeymap = new Bitmap(lpCalc->keymap_path);
#else
		wchar_t widePath[MAX_PATH];
		size_t converted;
		mbstowcs_s(&converted, widePath, lpCalc->skin_path, (size_t) ARRAYSIZE(widePath));
		m_pBitmapSkin = new Bitmap(widePath);
		mbstowcs_s(&converted, widePath, lpCalc->keymap_path, (size_t) ARRAYSIZE(widePath));
		m_pBitmapKeymap = new Bitmap(widePath);
#endif
	}

	if (!m_pBitmapSkin || m_pBitmapSkin->GetWidth() == 0 || m_pBitmapKeymap->GetWidth() == 0) {
		if (lpCalc->bCustomSkin) {
			MessageBox(lpCalc->hwndFrame, _T("Custom skin failed to load."), _T("Error"),  MB_OK);
			delete m_pBitmapKeymap;
			delete m_pBitmapSkin;
			m_pBitmapKeymap = NULL;
			m_pBitmapSkin = NULL;
			// your skin failed to load, lets disable it and load the normal skin
			lpCalc->bCustomSkin = FALSE;
		}
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

	BOOL foundScreen = FALSE;
	if ((skinWidth % SKIN_WIDTH) || (skinHeight % SKIN_HEIGHT) || skinHeight <= 0 || skinWidth <= 0) {
		lpCalc->bSkinEnabled = false;
		MessageBox(lpCalc->hwndFrame, _T("Invalid skin size."), _T("Error"), MB_OK | MB_ICONERROR);
	} else if ((skinWidth != keymapWidth) || (skinHeight != keymapHeight)) {
		lpCalc->bSkinEnabled = false;
		MessageBox(lpCalc->hwndFrame, _T("Skin and Keymap are not the same size"), _T("Error"), MB_OK | MB_ICONERROR);
	} else {
		if (lpCalc->default_skin_scale) {
			lpCalc->skin_scale = lpCalc->skin_scale / lpCalc->default_skin_scale;
		}

		lpCalc->default_skin_scale = (double)SKIN_WIDTH / skinWidth;
		lpCalc->skin_scale = lpCalc->skin_scale * lpCalc->default_skin_scale;
		lpCalc->rectSkin.right = skinWidth;
		lpCalc->rectSkin.bottom = skinHeight;
		foundScreen = FindLCDRect(m_pBitmapKeymap, skinWidth, skinHeight, &lpCalc->rectLCD);
	}
	if (!foundScreen) {
		MessageBox(lpCalc->hwndFrame, _T("Unable to find the screen box"), _T("Error"), MB_OK);
		lpCalc->bSkinEnabled = false;
	}
	if (lpCalc->hwndFrame == NULL) {
		return 0;
	}

	// set the size of the HDC
	HBITMAP hbmTemp = CreateCompatibleBitmap(hdc, lpCalc->rectSkin.right, lpCalc->rectSkin.bottom);
	SelectObject(lpCalc->hdcButtons, hbmTemp);
	
	switch (DrawSkin(hdc, lpCalc, m_pBitmapSkin, m_pBitmapKeymap)) {
		case ERROR_FACEPLATE:
			MessageBox(lpCalc->hwndFrame, _T("Unable to draw faceplate"), _T("Error"), MB_OK);
			break;
		case ERROR_SKIN:
			MessageBox(lpCalc->hwndFrame, _T("Unable to load skin resource"), _T("Error"), MB_OK);
			break;
		case ERROR_KEYMAP:
			MessageBox(lpCalc->hwndFrame, _T("Unable to load keymap resource"), _T("Error"), MB_OK);
			break;
	}

	if (lpCalc->bCutout && lpCalc->bSkinEnabled)	{
		if (EnableCutout(lpCalc) != 0) {
			MessageBox(lpCalc->hwndFrame, _T("Couldn't cutout window"), _T("Error"),  MB_OK);
		}
	} else {
		DisableCutout(lpCalc);
	}

	UpdateWabbitemuMainWindow(lpCalc);

	if (lpCalc->bCustomSkin) {
		if (m_pBitmapKeymap) {
			delete m_pBitmapKeymap;
		}
		if (m_pBitmapSkin) {
			delete m_pBitmapSkin;
		}
	}
	DeleteObject(hbmTemp);
	ReleaseDC(lpCalc->hwndFrame, hdc);

	SendMessage(lpCalc->hwndFrame, WM_SIZE, SIZE_RESTORED, 0);

	return 0;
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

HRGN GetFaceplateRegion(LPCALC lpCalc) {
	unsigned int nPoints = sizeof(ptRgnEdge) / sizeof(POINT);
	POINT *points = (POINT *) malloc(sizeof(ptRgnEdge));
	UINT scale = (UINT) 1.0 / lpCalc->default_skin_scale;
	for (int i = 0; i < nPoints; i++) {
		points[i].x = ptRgnEdge[i].x * scale;
		points[i].y = ptRgnEdge[i].y * scale;
	}

	HRGN hrgn = CreatePolygonRgn(points, nPoints, WINDING);
	free(points);
	return hrgn;

}

int DrawFaceplateRegion(LPCALC lpCalc, HDC hdc, COLORREF ref) {
	HRGN hrgn = GetFaceplateRegion(lpCalc);
	if (hrgn == NULL) {
		return 1;
	}

	HBRUSH hFaceplateColor = CreateSolidBrush(ref);
	FillRgn(hdc, hrgn, hFaceplateColor);
	DeleteObject(hFaceplateColor);
	DeleteObject(hrgn);
	return 0;
}