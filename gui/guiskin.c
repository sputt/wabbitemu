#include "stdafx.h"

#include "guiskin.h"

#include "gui.h"
#include "guibuttons.h"
#include "guicutout.h"
#include "guisize.h"
#include "CGdiPlusBitmap.h"

extern HINSTANCE g_hInst;
#define LCD_MARKER_COLOR 0xFFFF0000

BOOL FindLCDRect(BitmapData *bitmapData, Rect *rectLCD) {
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
	rectLCD->X = foundX;
	rectLCD->Y = foundY;
	//find right edge
	do {
		foundX++;
		pixel = pixels[foundY * bitmapData->Width + foundX];
	} while (pixel == LCD_MARKER_COLOR);
	rectLCD->Width = foundX-- - rectLCD->X;
	//find left edge
	do { 
		foundY++;
		pixel = pixels[foundY * bitmapData->Width + foundX];
	} while (pixel == LCD_MARKER_COLOR);
	rectLCD->Height = foundY - rectLCD->Y;
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
		rc.left = lpMainWindow->m_RectSkin.X;
		rc.top = lpMainWindow->m_RectSkin.Y;
		rc.right = lpMainWindow->m_RectSkin.GetRight();
		rc.bottom = lpMainWindow->m_RectSkin.GetBottom();
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
		SendMessage(lpMainWindow->hwndStatusBar, SB_SETTEXT, 1, (LPARAM)calc_get_model_string(lpCalc->model));
		// get the height of the status bar and factor that into our total app window height
		RECT src;
		GetWindowRect(lpMainWindow->hwndStatusBar, &src);
		rc.bottom += src.bottom - src.top;
	}

	if (hMenu) {
		CheckMenuItem(hMenu, IDM_VIEW_SKIN, MF_BYCOMMAND | bChecked);
	}

	UINT flags = SWP_NOMOVE;
	if (lpMainWindow->bAlwaysOnTop == FALSE) {
		flags |= SWP_NOZORDER;
	}

	if (lpMainWindow->silent_mode == TRUE) {
		flags |= SWP_HIDEWINDOW;
	} else {
		flags |= SWP_SHOWWINDOW;
	}

	LONG clientWidth = rc.right - rc.left;
	LONG clientHeight = rc.bottom - rc.top;
	AdjustWindowRect(&rc, WS_TILEDWINDOW | WS_CLIPCHILDREN, hMenu != NULL);
	LONG windowWidth = rc.right - rc.left;
	LONG windowHeight = rc.bottom - rc.top;

	SetWindowPos(lpMainWindow->hwndFrame, HWND_TOPMOST, 0, 0, windowWidth, windowHeight, flags);
	SendMessage(lpMainWindow->hwndFrame, WM_SIZE, SIZE_RESTORED, MAKELPARAM(clientWidth, clientHeight));
}

Bitmap *DrawSkin(Bitmap *pBitmapSkin, Bitmap *pBitmapKeymap, Color faceplateColor,
	BOOL isCutout, double default_skin_scale)
{
	if (pBitmapSkin == NULL) {
		return NULL;
	}

	if (pBitmapKeymap == NULL) {
		return NULL;
	}

	Rect skinRect(0, 0, pBitmapSkin->GetWidth(), pBitmapSkin->GetHeight());
	Bitmap *renderedSkin = new Bitmap(skinRect.Width, skinRect.Height, PixelFormat32bppARGB);
	Graphics g(renderedSkin);

	if (!isCutout) {
		SolidBrush grayBrush((Gdiplus::ARGB)Color::Gray);
		g.FillRectangle(&grayBrush, skinRect);
	}

	if (faceplateColor.GetValue() != 0) {
		SolidBrush faceplateBrush(faceplateColor);
		Region faceplateRegion(GetFaceplateRegion(default_skin_scale));
		g.FillRegion(&faceplateBrush, &faceplateRegion);
	}

	g.DrawImage(pBitmapSkin, skinRect);

	return renderedSkin;
}

// TRUE if success, FALSE if failure
int gui_frame_update(LPMAINWINDOW lpMainWindow) {
	if (lpMainWindow == NULL) {
		return FALSE;
	}

	int skinWidth = 0, skinHeight = 0, keymapWidth = -1, keymapHeight = -1;

	if (lpMainWindow->m_lpBitmapSkin) {
		delete lpMainWindow->m_lpBitmapSkin;
	}

	if (lpMainWindow->m_lpBitmapKeymap) {
		delete lpMainWindow->m_lpBitmapKeymap;
	}

	if (lpMainWindow->m_lpBitmapRenderedSkin) {
		delete lpMainWindow->m_lpBitmapRenderedSkin;
	}

	int model = lpMainWindow->lpCalc->model;
	//load skin and keymap
	CGdiPlusBitmapResource hbmSkin, hbmKeymap;
	Bitmap *pBitmapSkin = NULL;
	Bitmap *pBitmapKeymap = NULL;
	if (lpMainWindow->bCustomSkin) {
#ifdef _UNICODE
		pBitmapSkin = new Bitmap(lpMainWindow->skin_path);
		pBitmapKeymap = new Bitmap(lpMainWindow->keymap_path);
#else
		wchar_t widePath[MAX_PATH];
		size_t converted;
		mbstowcs_s(&converted, widePath, lpMainWindow->skin_path, (size_t) ARRAYSIZE(widePath));
		pBitmapSkin = new Bitmap(widePath);
		mbstowcs_s(&converted, widePath, lpMainWindow->keymap_path, (size_t)ARRAYSIZE(widePath));
		pBitmapKeymap = new Bitmap(widePath);
#endif
	}

	if (!pBitmapSkin || pBitmapSkin->GetWidth() == 0 || pBitmapKeymap->GetWidth() == 0) {
		if (lpMainWindow->bCustomSkin) {
			MessageBox(lpMainWindow->hwndFrame, _T("Custom skin failed to load."), _T("Error"), MB_OK);
			delete pBitmapKeymap;
			delete pBitmapSkin;
			pBitmapKeymap = NULL;
			pBitmapSkin = NULL;
			// your skin failed to load, lets disable it and load the normal skin
			lpMainWindow->bCustomSkin = FALSE;
		}

		hbmSkin.Load(calc_get_model_string(model), _T("PNG"), g_hInst);
		switch(model) {
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

		pBitmapSkin = hbmSkin.m_pBitmap;
		pBitmapKeymap = hbmKeymap.m_pBitmap->Clone(0, 0, hbmSkin.m_pBitmap->GetWidth(), hbmSkin.m_pBitmap->GetHeight(), PixelFormat24bppRGB);
	}

	if (pBitmapSkin) {
		skinWidth = pBitmapSkin->GetWidth();
		skinHeight = pBitmapSkin->GetHeight();
	}

	if (pBitmapKeymap) {
		keymapWidth = pBitmapKeymap->GetWidth();
		keymapHeight = pBitmapKeymap->GetHeight();
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
		
		lpMainWindow->m_RectSkin.Width = skinWidth;
		lpMainWindow->m_RectSkin.Height = skinHeight;
		BitmapData *data = new BitmapData;
		Rect keymapRect(0, 0, keymapWidth, keymapHeight);
		pBitmapKeymap->LockBits(&keymapRect, ImageLockModeRead, PixelFormat32bppARGB, data);
		BOOL foundScreen = FindLCDRect(data, &lpMainWindow->m_RectLCD);

		if (!foundScreen) {
			free(data);
			MessageBox(lpMainWindow->hwndFrame, _T("Unable to find the screen box"), _T("Error"), MB_OK);
			lpMainWindow->bSkinEnabled = false;
			return FALSE;
		}

		FindButtonsRect(data);

		pBitmapKeymap->UnlockBits(data);
		delete data;
	}

	if (lpMainWindow->hwndFrame == NULL) {
		return FALSE;
	}

	lpMainWindow->m_lpBitmapKeymap = pBitmapKeymap;
	
	Color faceplateColor = model == TI_84PSE || model == TI_84PCSE && !lpMainWindow->bCustomSkin ? 
		Color(0xFF, GetRValue(lpMainWindow->m_FaceplateColor), GetGValue(lpMainWindow->m_FaceplateColor),
		GetBValue(lpMainWindow->m_FaceplateColor)) : 0;
	Bitmap *renderedSkin = DrawSkin(pBitmapSkin, pBitmapKeymap, faceplateColor,
		lpMainWindow->bCutout, lpMainWindow->default_skin_scale);
	if (renderedSkin == NULL) {
		MessageBox(lpMainWindow->hwndFrame, _T("Unable to draw skin"), _T("Error"), MB_OK);
		lpMainWindow->bSkinEnabled = FALSE;
	} else {
		lpMainWindow->m_lpBitmapSkin = renderedSkin;
		lpMainWindow->m_lpBitmapRenderedSkin = renderedSkin->Clone(lpMainWindow->m_RectSkin, PixelFormat32bppARGB);
	}

	if (lpMainWindow->bCutout && lpMainWindow->bSkinEnabled) {
		if (EnableCutout(lpMainWindow) != 0) {
			MessageBox(lpMainWindow->hwndFrame, _T("Couldn't cutout window"), _T("Error"), MB_OK);
		}
	} else {
		DisableCutout(lpMainWindow);
	}

	UpdateWabbitemuMainWindow(lpMainWindow);

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
	UINT nPoints = sizeof(ptRgnEdge) / sizeof(POINT);
	POINT *points = (POINT *) malloc(sizeof(ptRgnEdge));
	UINT scale = (UINT)(1.0 / default_skin_scale);
	for (UINT i = 0; i < nPoints; i++) {
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