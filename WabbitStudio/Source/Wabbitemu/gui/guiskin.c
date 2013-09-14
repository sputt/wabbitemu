#include "stdafx.h"

#include "guiskin.h"

#include "gui.h"
#include "guibuttons.h"
#include "guicutout.h"
#include "CGdiPlusBitmap.h"

extern BOOL silent_mode;
extern HINSTANCE g_hInst;

BOOL FindLCDRect(Bitmap *m_pBitmapKeymap, u_int skinWidth, u_int skinHeight, RECT *rectLCD) {
	u_int foundX = -1, foundY = -1;
	Color pixel;
	//find the top left corner
	for (u_int y = 0; y < skinHeight && foundX == -1; y++) {
		for (u_int x = 0; x < skinWidth && foundY == -1; x++) {
			m_pBitmapKeymap->GetPixel(x, y, &pixel);
			if (pixel.GetValue() == 0xFFFF0000)	{
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
	} while (pixel.GetValue() == 0xFFFF0000);
	rectLCD->right = foundX--;
	//find left edge
	do { 
		foundY++;
		m_pBitmapKeymap->GetPixel(foundX, foundY, &pixel);
	} while (pixel.GetValue() == 0xFFFF0000);
	rectLCD->bottom = foundY;
	return TRUE;
}

void UpdateWabbitemuMainWindow(LPCALC lpCalc) {
	RECT rc;
	HMENU hMenu = GetMenu(lpCalc->hwndFrame);
	BOOL bChecked;
	if (lpCalc->hwndStatusBar != NULL) {
		DestroyWindow(lpCalc->hwndStatusBar);
		CloseWindow(lpCalc->hwndStatusBar);
		lpCalc->hwndStatusBar = NULL;
	}

	if (lpCalc->SkinEnabled) {
		bChecked = MF_CHECKED;
		CopyRect(&rc, &lpCalc->rectSkin);
		AdjustWindowRect(&rc, WS_CAPTION | WS_TILEDWINDOW , FALSE);
		rc.bottom += GetSystemMetrics(SM_CYMENU);
		
	} else {
		bChecked = MF_UNCHECKED;
		// Create status bar
		SetRect(&rc, 0, 0, 128 * lpCalc->scale, 64 * lpCalc->scale);
		int iStatusWidths[] = { 100, -1 };
		lpCalc->hwndStatusBar = CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE,
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

	SetWindowPos(lpCalc->hwndFrame, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
			SWP_NOMOVE | SWP_NOZORDER);
}

enum DRAWSKINERROR {
	ERROR_FACEPLATE = 1,
	ERROR_CUTOUT,
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
	//translate to regular gdi compatibility to simplify coding :/
	m_pBitmapKeymap->GetHBITMAP(Color::White, &hbmKeymapOld);
	SelectObject(lpCalc->hdcKeymap, hbmKeymapOld);
	//get the HBITMAP for the skin DONT change the first value, it is necessary for transparency to work
	m_pBitmapSkin->GetHBITMAP(Color::AlphaMask, &hbmSkinOld);
	//84+SE has custom faceplates :D, draw it to the background
	//thanks MSDN your documentation rules :))))
	HDC hdcOverlay = CreateCompatibleDC(lpCalc->hdcSkin);
	HBITMAP blankBitmap = CreateCompatibleBitmap(hdc, m_pBitmapSkin->GetWidth(), m_pBitmapSkin->GetHeight());
	SelectObject(lpCalc->hdcSkin, blankBitmap);
	if (!lpCalc->bCutout || !lpCalc->SkinEnabled) {
		FillRect(lpCalc->hdcSkin, &lpCalc->rectSkin, GetStockBrush(GRAY_BRUSH));
	}

	if (lpCalc->model == TI_84PSE) {
		if (DrawFaceplateRegion(lpCalc->hdcSkin, lpCalc->FaceplateColor)) {
			return ERROR_FACEPLATE;
		}
	}

	//this needs to be done so we can alpha blend the screen
	HBITMAP oldSkin = (HBITMAP) SelectObject(hdcOverlay, hbmSkinOld);
	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;
	
	AlphaBlend(lpCalc->hdcSkin, 0, 0, lpCalc->rectSkin.right, lpCalc->rectSkin.bottom, hdcOverlay,
		lpCalc->rectSkin.left, lpCalc->rectSkin.top, lpCalc->rectSkin.right, lpCalc->rectSkin.bottom, bf);
	BitBlt(lpCalc->hdcButtons, 0, 0, lpCalc->rectSkin.right, lpCalc->rectSkin.bottom, lpCalc->hdcSkin, 0, 0, SRCCOPY);
	FinalizeButtons(lpCalc);
	if (lpCalc->bCutout && lpCalc->SkinEnabled)	{
		if (EnableCutout(lpCalc) != 0) {
			return ERROR_CUTOUT;
		}
		//TODO: figure out why this needs to be called again
		EnableCutout(lpCalc);
	} else {
		DisableCutout(lpCalc->hwndFrame);
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
			//your skin failed to load, lets disable it and load the normal skin
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
	if ((skinWidth != keymapWidth) || (skinHeight != keymapHeight) || skinHeight <= 0 || skinWidth <= 0) {
		lpCalc->SkinEnabled = false;
		MessageBox(lpCalc->hwndFrame, _T("Skin and Keymap are not the same size"), _T("Error"),  MB_OK);
	} else {
		lpCalc->rectSkin.right = skinWidth;
		lpCalc->rectSkin.bottom = skinHeight;
		foundScreen = FindLCDRect(m_pBitmapKeymap, skinWidth, skinHeight, &lpCalc->rectLCD);
	}
	if (!foundScreen) {
		MessageBox(lpCalc->hwndFrame, _T("Unable to find the screen box"), _T("Error"), MB_OK);
		lpCalc->SkinEnabled = false;
	}
	if (lpCalc->hwndFrame == NULL) {
		return 0;
	}

	//set the size of the HDC
	HBITMAP hbmTemp = CreateCompatibleBitmap(hdc, lpCalc->rectSkin.right, lpCalc->rectSkin.bottom);
	SelectObject(lpCalc->hdcButtons, hbmTemp);
	DeleteObject(hbmTemp);

	UpdateWabbitemuMainWindow(lpCalc);
	
	switch (DrawSkin(hdc, lpCalc, m_pBitmapSkin, m_pBitmapKeymap)) {
		case ERROR_FACEPLATE:
			MessageBox(lpCalc->hwndFrame, _T("Unable to draw faceplate"), _T("Error"), MB_OK);
			break;
		case ERROR_CUTOUT:
			MessageBox(lpCalc->hwndFrame, _T("Couldn't cutout window"), _T("Error"),  MB_OK);
			break;
		case ERROR_SKIN:
			MessageBox(lpCalc->hwndFrame, _T("Unable to load skin resource"), _T("Error"), MB_OK);
			break;
		case ERROR_KEYMAP:
			MessageBox(lpCalc->hwndFrame, _T("Unable to load keymap resource"), _T("Error"), MB_OK);
			break;
	}
	if (lpCalc->bCustomSkin) {
		if (m_pBitmapKeymap) {
			delete m_pBitmapKeymap;
		}
		if (m_pBitmapSkin) {
			delete m_pBitmapSkin;
		}
	}
	ReleaseDC(lpCalc->hwndFrame, hdc);

	if (lpCalc->hwndStatusBar != NULL) {
		SendMessage(lpCalc->hwndStatusBar, SB_SETTEXT, 1, (LPARAM) CalcModelTxt[lpCalc->model]);
	}

	SendMessage(lpCalc->hwndFrame, WM_SIZE, 0, 0);

	return 0;
}

static POINT ptRgnEdge[] = {{75,675},
							{95,683},
							{262,682},
							{279,675},
							{316,535},
							{316,273},
							{37,273},
							{37,568}};

HRGN GetFaceplateRegion()
{
	unsigned int nPoints = (sizeof(ptRgnEdge) / sizeof(POINT)) * 2;
	POINT ptRgn[(sizeof(ptRgnEdge) / sizeof(POINT)) * 2];

	// Copy points and their reverses to the new array
	memcpy(ptRgn, ptRgnEdge, (nPoints / 2) * sizeof(POINT));

	u_int i;
	for (i = nPoints/2; i < nPoints; i++) {
		ptRgn[i].x = SKIN_WIDTH - ptRgnEdge[nPoints - i - 1].x;
		ptRgn[i].y = ptRgnEdge[nPoints - i - 1].y;
	}

	HRGN hrgn = CreatePolygonRgn(ptRgn, nPoints, WINDING);
	return hrgn;

}

int DrawFaceplateRegion(HDC hdc, COLORREF ref) {
	HRGN hrgn = GetFaceplateRegion();
	if (hrgn == NULL)
		return 1;
	HBRUSH hFaceplateColor = CreateSolidBrush(ref);
	FillRgn(hdc, hrgn, hFaceplateColor);
	DeleteObject(hFaceplateColor);
	DeleteObject(hrgn);
	return 0;
}