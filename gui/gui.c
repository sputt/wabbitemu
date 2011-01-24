#include "stdafx.h"

#include "gui.h"
#include "resource.h"
#include "uxtheme.h"

#include "core.h"
#include "calc.h"
#include "label.h"

#include "gifhandle.h"
#include "gif.h"

#include "var.h"
#include "link.h"
#include "keys.h"
#include "fileutilities.h"

#include "dbmem.h"
#include "dbreg.h"
#include "dbtoolbar.h"
#include "dbtrack.h"
#include "dbdisasm.h"

#include "guibuttons.h"
#include "guicontext.h"
#include "guicutout.h"
#include "guidebug.h"
#include "guifaceplate.h"
#include "guiglow.h"
#include "guikeylist.h"
#include "guilcd.h"
#include "guiopenfile.h"
#include "guioptions.h"
#include "guisavestate.h"
#include "guispeed.h"
#include "guivartree.h"
#include "guiwizard.h"

#include "DropTarget.h"
#include "expandpane.h"
#include "registry.h"
#include "sendfiles.h"
#include "state.h"
#ifdef USE_COM
#include "wbded.h"
#endif
#ifdef USE_GDIPLUS
#include "CGdiPlusBitmap.h"
#endif


#ifdef _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#define MENU_FILE 0
#define MENU_EDIT 1
#define MENU_CALC 2
#define MENU_HELP 3

TCHAR ExeDir[512];

INT_PTR CALLBACK DlgVarlist(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
HINSTANCE g_hInst;
HACCEL hacceldebug;
HACCEL haccelmain;
POINT drop_pt;
BOOL gif_anim_advance;
BOOL silent_mode = FALSE;

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ToolProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);


void gui_draw(calc_t *lpCalc)
{
	if (lpCalc->hwndLCD != NULL) {
		InvalidateRect(lpCalc->hwndLCD, NULL, FALSE);
	}

	if (lpCalc->gif_disp_state != GDS_IDLE) {
		static int skip = 0;
		if (skip == 0) {
			gif_anim_advance = TRUE;
			if (lpCalc->hwndFrame != NULL) {
				InvalidateRect(lpCalc->hwndFrame, NULL, FALSE);
			}
		}
		
		skip = (skip + 1) % 4;
	}
}

VOID CALLBACK TimerProc(HWND hwnd, UINT Message, UINT_PTR idEvent, DWORD dwTimer) {
	static long difference;
	static DWORD prevTimer;

	// How different the timer is from where it should be
	// guard from erroneous timer calls with an upper bound
	// that's the limit of time it will take before the
	// calc gives up and claims it lost time
	difference += ((dwTimer - prevTimer) & 0x003F) - TPF;
	prevTimer = dwTimer;


	/* 12/15 BuckeyeDude:
	I have no clue what this code does apart from make it impossible to have >2 cores open at once
	and cause calculators to runs slower as more are opened. Brilliant work :|
	int i;
	for (i = 0; i < MAX_CALCS; i++) {
		if (calcs[i].active) {
			static int frameskip = 0;
			frameskip = (frameskip + 1) % 3;

			if (frameskip == 0) {
				difference = 0;
				return;
			}
		}
	}*/

	// Are we greater than Ticks Per Frame that would call for
	// a frame skip?
	if (difference > -TPF) {
		calc_run_all();
		while (difference >= TPF) {
			calc_run_all();
			difference -= TPF;
		}

		int i;
		for (i = 0; i < MAX_CALCS; i++) {
			if (calcs[i].active) {
				gui_draw(&calcs[i]);
			}
		}
	// Frame skip if we're too far ahead.
	} else difference += TPF;
}


extern WINDOWPLACEMENT db_placement;

int gui_debug(LPCALC lpCalc) {
	if (lpCalc->audio != NULL)
	pausesound(lpCalc->audio);
	HWND hdebug;
	BOOL set_place = TRUE;
	int flags = 0;
	RECT pos = {CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT+600, CW_USEDEFAULT+400};
	if (!db_placement.length) {
		db_placement.flags = SW_SHOWNORMAL;
		db_placement.length = sizeof(WINDOWPLACEMENT);
		CopyRect(&db_placement.rcNormalPosition, &pos);
		set_place = FALSE;
		flags = WS_VISIBLE;
	}

	pos.right -= pos.left;
	pos.bottom -= pos.top;

	if ((hdebug = FindWindow(g_szDebugName, _T("Debugger")))) {
		SwitchToThisWindow(hdebug, TRUE);
		SendMessage(hdebug, WM_USER, DB_RESUME, 0);
		return -1;
	}
	lpCalc->running = FALSE;
	hdebug = CreateWindowEx(
		WS_EX_APPWINDOW,
		g_szDebugName,
        _T("Debugger"),
		flags | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        pos.left, pos.top, pos.right, pos.bottom,
        0, 0, g_hInst, (LPVOID) lpCalc);
	if (set_place)
		SetWindowPlacement(hdebug, &db_placement);

	lpCalc->hwndDebug = hdebug;
	SendMessage(hdebug, WM_SIZE, 0, 0);
	return 0;
}

void SkinCutout(HWND hwnd);

int gui_frame(LPCALC lpCalc) {
 	RECT r;

 	if (!lpCalc->Scale)
		lpCalc->Scale = 2;
	if (lpCalc->SkinEnabled) {
 		SetRect(&r, 0, 0, lpCalc->rectSkin.right, lpCalc->rectSkin.bottom);
 	} else {
 		SetRect(&r, 0, 0, 128 * lpCalc->Scale, 64 * lpCalc->Scale);
 	}
	AdjustWindowRect(&r, WS_CAPTION | WS_TILEDWINDOW, FALSE);
	r.bottom += GetSystemMetrics(SM_CYMENU);

	lpCalc->hwndFrame = CreateWindowEx(
		0, //WS_EX_APPWINDOW,
		g_szAppName,
        _T("Z80"),
		(WS_TILEDWINDOW |  (silent_mode ? 0 : WS_VISIBLE) | WS_CLIPCHILDREN) & ~(WS_MAXIMIZEBOX /* | WS_SIZEBOX */),
        startX, startY, r.right - r.left, r.bottom - r.top,
        NULL, 0, g_hInst, (LPVOID) lpCalc);

	HDC hdc = GetDC(lpCalc->hwndFrame);
	//HBITMAP hbmSkin = LoadBitmap(g_hInst, lpCalc->model);
	lpCalc->hdcSkin = CreateCompatibleDC(hdc);
	//SelectObject(lpCalc->hdcSkin, hbmSkin);

	//this is now (intuitively) created in guicutout.c (Enable/Disable cutout function)
	/*lpCalc->hwndLCD = CreateWindowEx(
		0,
		g_szLCDName,
		"LCD",
		WS_VISIBLE |  WS_CHILD,
		0, 0, lpCalc->cpu.pio.lcd->width*lpCalc->Scale, 64*lpCalc->Scale,
		lpCalc->hwndFrame, (HMENU) 99, g_hInst,  NULL);*/

	if (lpCalc->hwndFrame == NULL /*|| lpCalc->hwndLCD == NULL*/) return -1;

	GetClientRect(lpCalc->hwndFrame, &r);
	lpCalc->running = TRUE;
	lpCalc->speed = 100;
	HMENU hmenu = GetMenu(lpCalc->hwndFrame);
	CheckMenuRadioItem(GetSubMenu(GetSubMenu(hmenu, 2),4), IDM_SPEED_QUARTER, IDM_SPEED_MAX, IDM_SPEED_NORMAL, MF_BYCOMMAND);
	gui_frame_update(lpCalc);
	ReleaseDC(lpCalc->hwndFrame, hdc);
	return 0;
}

#ifdef USE_GDIPLUS
int gui_frame_update(LPCALC lpCalc) {
	int skinWidth = 0, skinHeight = 0, keymapWidth = -1, keymapHeight = -1;
	HDC hdc = GetDC(lpCalc->hwndFrame);
	if (lpCalc->hdcKeymap)
		DeleteDC(lpCalc->hdcKeymap);
	if (lpCalc->hdcSkin)
		DeleteDC(lpCalc->hdcSkin);
	if (lpCalc->hdcButtons)
		DeleteDC(lpCalc->hdcButtons);
	lpCalc->hdcKeymap = CreateCompatibleDC(hdc);
	lpCalc->hdcSkin = CreateCompatibleDC(hdc);
	lpCalc->hdcButtons = CreateCompatibleDC(hdc);
	//load skin and keymap
	CGdiPlusBitmapResource hbmSkin, hbmKeymap;
	Bitmap *m_pBitmapSkin = NULL, *m_pBitmapKeymap = NULL;
	if (lpCalc->bCustomSkin) {
		m_pBitmapSkin = new Bitmap(lpCalc->skin_path);
		m_pBitmapKeymap = new Bitmap(lpCalc->keymap_path);
	}
	if (!m_pBitmapSkin || m_pBitmapSkin->GetWidth() == 0 || m_pBitmapKeymap->GetWidth() == 0) {
		if (lpCalc->bCustomSkin) {
			MessageBox(NULL, _T("Custom skin failed to load."), _T("Error"),  MB_OK);
			delete m_pBitmapKeymap;
			delete m_pBitmapSkin;
		}
		hbmSkin.Load(CalcModelTxt[lpCalc->model], _T("PNG"), g_hInst);
		switch(lpCalc->model) {
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
	int x, y, foundX = 0, foundY = 0;
	bool foundScreen = FALSE;
	if (((skinWidth != keymapWidth) || (skinHeight != keymapHeight)) && skinHeight > 0 && skinWidth > 0) {
		lpCalc->SkinEnabled = false;
		MessageBox(NULL, _T("Skin and Keymap are not the same size"), _T("Error"),  MB_OK);
	} else {
		lpCalc->rectSkin.right = skinWidth;
		lpCalc->rectSkin.bottom = skinHeight;		//find the screen size
		Color pixel;
		for(y = 0; y < skinHeight && foundScreen == false; y++) {
			for (x = 0; x < skinWidth && foundScreen == false; x++) {
				m_pBitmapKeymap->GetPixel(x, y, &pixel);
				if (pixel.GetBlue() == 0 && pixel.GetRed() == 255 && pixel.GetGreen() == 0)	{
					//81 92
					foundX = x;
					foundY = y;
					foundScreen = true;
				}
			}
		}
		lpCalc->rectLCD.left = foundX;
		lpCalc->rectLCD.top = foundY;
		do {
			foundX++;
			m_pBitmapKeymap->GetPixel(foundX, foundY, &pixel);
		} while (pixel.GetBlue() == 0 && pixel.GetRed() == 255 && pixel.GetGreen() == 0);
		lpCalc->rectLCD.right = foundX--;
		do { 
			foundY++;
			m_pBitmapKeymap->GetPixel(foundX, foundY, &pixel);
		} while (pixel.GetBlue() == 0 && pixel.GetRed() == 255 && pixel.GetGreen() == 0);
		lpCalc->rectLCD.bottom = foundY;
	}
	if (!foundScreen) {
		MessageBox(NULL, _T("Unable to find the screen box"), _T("Error"), MB_OK);
		lpCalc->SkinEnabled = false;
	}
	if (!lpCalc->hwndFrame)
		return 0;

	HBITMAP hbmTemp = CreateCompatibleBitmap(hdc, lpCalc->rectSkin.right, lpCalc->rectSkin.bottom);
	SelectObject(lpCalc->hdcButtons, hbmTemp);
	DeleteObject(hbmTemp);
	//this is moved here so going from cutout->skinless creates the lcd now
	if (lpCalc->bCutout && !lpCalc->SkinEnabled) {
		DisableCutout(lpCalc->hwndFrame);
		lpCalc->bCutout = TRUE;
	} else DisableCutout(lpCalc->hwndFrame);
	HMENU hmenu = GetMenu(lpCalc->hwndFrame);	
	if (hmenu != NULL) {
		if (!lpCalc->SkinEnabled) {
			RECT rc;
			CheckMenuItem(GetSubMenu(hmenu, 2), IDM_CALC_SKIN, MF_BYCOMMAND | MF_UNCHECKED);
			// Create status bar
			if (lpCalc->hwndStatusBar != NULL) {
				SendMessage(lpCalc->hwndStatusBar, WM_DESTROY, 0, 0);
				SendMessage(lpCalc->hwndStatusBar, WM_CLOSE, 0, 0);
			}
			SetRect(&rc, 0, 0, 128*lpCalc->Scale, 64*lpCalc->Scale);
			int iStatusWidths[] = {100, -1};
			lpCalc->hwndStatusBar = CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, lpCalc->hwndFrame, (HMENU)99, g_hInst, NULL);
			SendMessage(lpCalc->hwndStatusBar, SB_SETPARTS, 2, (LPARAM) &iStatusWidths);
			SendMessage(lpCalc->hwndStatusBar, SB_SETTEXT, 1, (LPARAM) CalcModelTxt[lpCalc->model]);
			RECT src;
			GetWindowRect(lpCalc->hwndStatusBar, &src);
			AdjustWindowRect(&rc, (WS_TILEDWINDOW | WS_CLIPCHILDREN) & ~WS_MAXIMIZEBOX, FALSE);
			rc.bottom += src.bottom - src.top;
			if (hmenu)
				rc.bottom += GetSystemMetrics(SM_CYMENU);
			SetWindowPos(lpCalc->hwndFrame, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
			GetClientRect(lpCalc->hwndFrame, &rc);
			SendMessage(lpCalc->hwndStatusBar, WM_SIZE, 0, 0);
			SendMessage(lpCalc->hwndStatusBar, SB_SETTEXT, 1, (LPARAM) CalcModelTxt[lpCalc->model]);
		} else {
			CheckMenuItem(GetSubMenu(hmenu, 2), IDM_CALC_SKIN, MF_BYCOMMAND | MF_CHECKED);
			SendMessage(lpCalc->hwndStatusBar, WM_DESTROY, 0, 0);
			SendMessage(lpCalc->hwndStatusBar, WM_CLOSE, 0, 0);
			lpCalc->hwndStatusBar = NULL;
			RECT rc;
			CopyRect(&rc, &lpCalc->rectSkin);
			AdjustWindowRect(&rc, WS_CAPTION | WS_TILEDWINDOW , FALSE);
			rc.bottom += GetSystemMetrics(SM_CYMENU);
			SetWindowPos(lpCalc->hwndFrame, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_DRAWFRAME);
		}
	}
	/*lpCalc->skinGraphics->Clear(Color::Gray);
	if (lpCalc->model == TI_84PSE) {
		if (DrawFaceplateRegion(lpCalc->skinGraphics))
			MessageBox(NULL, "Unable to draw faceplate", "error", MB_OK);
	}
	lpCalc->skinGraphics->DrawImage(hbmSkin, 0, 0, skinWidth, skinHeight);
	lpCalc->skinGraphics->Flush();
	
	RECT rc;
	GetClientRect(lpCalc->hwndFrame, &rc);
	HBITMAP skin;
	if (lpCalc->bCutout && lpCalc->SkinEnabled)	{
		hbmSkin.m_pBitmap->GetHBITMAP(Color::AlphaMask, &skin);
		if (EnableCutout(lpCalc->hwndFrame, skin) != 0) {
			MessageBox(NULL, "Couldn't cutout window", "error",  MB_OK);
		}
	}
	if (lpCalc->hwndStatusBar != NULL)
		SendMessage(lpCalc->hwndStatusBar, SB_SETTEXT, 1, (LPARAM) CalcModelTxt[lpCalc->model]);

	ReleaseDC(lpCalc->hwndFrame, hdc);
	DeleteObject(skin);
	SendMessage(lpCalc->hwndFrame, WM_SIZE, 0, 0);
	return 0;*/
	HBITMAP hbmSkinOld, hbmKeymapOld;
	//translate to regular gdi compatibility to simplify coding :/
	m_pBitmapKeymap->GetHBITMAP(Color::White, &hbmKeymapOld);
	SelectObject(lpCalc->hdcKeymap, hbmKeymapOld);
	//get the HBITMAP for the skin DONT change the first value, it is necessary for transparency to work
	m_pBitmapSkin->GetHBITMAP(Color::AlphaMask, &hbmSkinOld);
	//84+SE has custom faceplates :D, draw it to the background
	//thanks MSDN your documentation rules :))))
	HDC hdcOverlay = CreateCompatibleDC(lpCalc->hdcSkin);
	HBITMAP blankBitmap = CreateCompatibleBitmap(hdc, skinWidth, skinHeight);
	SelectObject(lpCalc->hdcSkin, blankBitmap);
	if (!lpCalc->bCutout || !lpCalc->SkinEnabled)
		FillRect(lpCalc->hdcSkin, &lpCalc->rectSkin, GetStockBrush(GRAY_BRUSH));
	if (lpCalc->model == TI_84PSE) {
		if (DrawFaceplateRegion(lpCalc->hdcSkin, lpCalc->FaceplateColor))
			MessageBox(NULL, _T("Unable to draw faceplate"), _T("Error"), MB_OK);
	}

	//this needs to be done so we can alphablend the screen
	HBITMAP oldSkin = (HBITMAP) SelectObject(hdcOverlay, hbmSkinOld);
	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;
	AlphaBlend(lpCalc->hdcSkin, 0, 0, lpCalc->rectSkin.right, lpCalc->rectSkin.bottom, hdcOverlay,
		lpCalc->rectSkin.left, lpCalc->rectSkin.top, lpCalc->rectSkin.right, lpCalc->rectSkin.bottom, bf);
	BitBlt(lpCalc->hdcButtons, 0, 0, lpCalc->rectSkin.right, lpCalc->rectSkin.bottom, lpCalc->hdcSkin, 0, 0, SRCCOPY);
	if (lpCalc->bCutout && lpCalc->SkinEnabled)	{
		if (EnableCutout(lpCalc, hbmSkinOld) != 0)
			MessageBox(NULL, _T("Couldn't cutout window"), _T("Error"),  MB_OK);
	}
	if (lpCalc->hwndStatusBar != NULL)
		SendMessage(lpCalc->hwndStatusBar, SB_SETTEXT, 1, (LPARAM) CalcModelTxt[lpCalc->model]);
	SendMessage(lpCalc->hwndFrame, WM_SIZE, 0, 0);

	if (lpCalc->bAlwaysOnTop)
		SetWindowPos(lpCalc->hwndFrame, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	else
		SetWindowPos(lpCalc->hwndFrame, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	if (lpCalc->bCustomSkin) {
		delete m_pBitmapKeymap;
		delete m_pBitmapSkin;
	}
	DeleteObject(hbmKeymapOld);
	DeleteObject(hbmSkinOld);
	DeleteObject(blankBitmap);
	DeleteDC(hdcOverlay);
	ReleaseDC(lpCalc->hwndFrame, hdc);
	return 0;
}
#else
int gui_frame_update(LPCALC lpCalc) {
	BITMAP skinSize, keymapSize;
	HBITMAP hbmOldKeymap, hbmKeymap, hbmSkin, hbmOldSkin;
	HDC hdc = GetDC(lpCalc->hwndFrame);
	if (lpCalc->hdcKeymap)
		ReleaseDC(lpCalc->hwndFrame, lpCalc->hdcKeymap);
	if (lpCalc->hdcSkin)
		ReleaseDC(lpCalc->hwndFrame, lpCalc->hdcSkin);

	lpCalc->hdcKeymap = CreateCompatibleDC(hdc);
	lpCalc->hdcSkin = CreateCompatibleDC(hdc);
	hbmSkin = LoadBitmap(g_hInst, CalcModelTxt[lpCalc->model]);
	if (hbmSkin) {
		hbmOldSkin = (HBITMAP) SelectObject(lpCalc->hdcSkin, hbmSkin);
		GetObject(hbmSkin, sizeof(BITMAP), &skinSize);
	}
	TCHAR *name = (TCHAR *) malloc(strlen(CalcModelTxt[lpCalc->model]) + 7);
	strcpy(name, CalcModelTxt[lpCalc->model]);
	strcat(name, "Keymap");
	hbmKeymap = LoadBitmap(g_hInst, name);
	if (hbmKeymap) {
		hbmOldKeymap = (HBITMAP) SelectObject(lpCalc->hdcKeymap, hbmKeymap);
		GetObject(hbmKeymap, sizeof(BITMAP), &keymapSize);	//skin and keymap must match
	}
	free(name);
	int x, y, foundX = 0, foundY = 0;
	bool foundScreen = FALSE;
	if (hbmSkin == NULL || skinSize.bmWidth != keymapSize.bmWidth || skinSize.bmHeight != keymapSize.bmHeight) {
		lpCalc->SkinEnabled = false;
		MessageBox(NULL, "Skin and Keymap are not the same size", "error",  MB_OK);
		//load default data
		lpCalc->rectLCD.left = 0;
		lpCalc->rectLCD.top = 0;
		lpCalc->rectLCD.right = lpCalc->cpu.pio.lcd->width*2;
		lpCalc->rectLCD.bottom = 128;
	} else {
		lpCalc->rectSkin.right = skinSize.bmWidth;
		lpCalc->rectSkin.bottom = skinSize.bmHeight;		//find the screen size
		for(y = 0; y < skinSize.bmHeight && foundScreen == false; y++) {
			for (x = 0; x < skinSize.bmWidth && foundScreen == false; x++) {
				COLORREF pixel = GetPixel(lpCalc->hdcKeymap, x, y);
				if (pixel == RGB(255, 0, 0) && foundScreen != true)	{
					foundX = x;
					foundY = y;	
					foundScreen = true;	
				}
			}
		}
		lpCalc->rectLCD.left = foundX;
		lpCalc->rectLCD.top = foundY;
		COLORREF pixel;
		do {
			foundX++;
			pixel = GetPixel(lpCalc->hdcKeymap, foundX, foundY);
		} while (pixel == RGB(255, 0, 0));
		lpCalc->rectLCD.right = foundX--;
		do { 
			foundY++;
			pixel = GetPixel(lpCalc->hdcKeymap, foundX, foundY);
		} while (pixel == RGB(255, 0, 0));
		lpCalc->rectLCD.bottom = foundY;
	}
	if (!lpCalc->hwndFrame)
		return 0;
	HMENU hmenu = GetMenu(lpCalc->hwndFrame);
	if (hmenu != NULL) {
		if (!lpCalc->SkinEnabled) {
			RECT rc;
			CheckMenuItem(GetSubMenu(hmenu, 2), IDM_CALC_SKIN, MF_BYCOMMAND | MF_UNCHECKED);
			// Create status bar
			if (!lpCalc->hwndStatusBar) {
				SendMessage(lpCalc->hwndStatusBar, WM_DESTROY, 0, 0);
				SendMessage(lpCalc->hwndStatusBar, WM_CLOSE, 0, 0);
			}
			SetRect(&rc, 0, 0, 128*lpCalc->Scale, 64*lpCalc->Scale);
			int iStatusWidths[] = {100, -1};
			lpCalc->hwndStatusBar = CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, lpCalc->hwndFrame, (HMENU)99, g_hInst, NULL);
			SendMessage(lpCalc->hwndStatusBar, SB_SETPARTS, 2, (LPARAM) &iStatusWidths);
			SendMessage(lpCalc->hwndStatusBar, SB_SETTEXT, 1, (LPARAM) CalcModelTxt[lpCalc->model]);
			RECT src;
			GetWindowRect(lpCalc->hwndStatusBar, &src);
			AdjustWindowRect(&rc, (WS_TILEDWINDOW | WS_CLIPCHILDREN) & ~WS_MAXIMIZEBOX, FALSE);
			rc.bottom += src.bottom - src.top;
			if (hmenu)
				rc.bottom += GetSystemMetrics(SM_CYMENU);
			SetWindowPos(lpCalc->hwndFrame, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
			GetClientRect(lpCalc->hwndFrame, &rc);
			SendMessage(lpCalc->hwndStatusBar, WM_SIZE, 0, 0);
			SendMessage(lpCalc->hwndStatusBar, SB_SETTEXT, 1, (LPARAM) CalcModelTxt[lpCalc->model]);
			//InvalidateRect(lpCalc->hwndFrame, NULL, FALSE);
		} else {
			CheckMenuItem(GetSubMenu(hmenu, 2), IDM_CALC_SKIN, MF_BYCOMMAND | MF_CHECKED);
			SendMessage(lpCalc->hwndStatusBar, WM_DESTROY, 0, 0);
			SendMessage(lpCalc->hwndStatusBar, WM_CLOSE, 0, 0);
			lpCalc->hwndStatusBar = NULL;
			//SetRect(&lpCalc->rectSkin, 0, 0, 350, 725);
			RECT rc;
			CopyRect(&rc, &lpCalc->rectSkin);
			AdjustWindowRect(&rc, WS_CAPTION | WS_TILEDWINDOW , FALSE);
			rc.bottom += GetSystemMetrics(SM_CYMENU);
			SetWindowPos(lpCalc->hwndFrame, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_DRAWFRAME);
			//InvalidateRect(hwnd, NULL, TRUE);
		}
	}
	RECT rc;
	GetClientRect(lpCalc->hwndFrame, &rc);
	if (!lpCalc->SkinEnabled || !lpCalc->bCutout)
		FillRect(lpCalc->hdcSkin, &rc, GetStockBrush(GRAY_BRUSH));
	if (lpCalc->model == TI_84PSE) {
		if (DrawFaceplateRegion(lpCalc->hdcSkin, lpCalc->FaceplateColor))
			MessageBox(NULL, "Unable to draw faceplate", "error", MB_OK);
	}
	//this needs to be done so we can alphablend the screen
	HDC hdcOverlay = CreateCompatibleDC(lpCalc->hdcSkin);
	HBITMAP bmpGray = LoadBitmap(g_hInst, CalcModelTxt[lpCalc->model]);
	SelectObject(hdcOverlay, bmpGray);
	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;
	AlphaBlend(lpCalc->hdcSkin, 0, 0, lpCalc->rectSkin.right, lpCalc->rectSkin.bottom, hdcOverlay,
		lpCalc->rectSkin.left, lpCalc->rectSkin.top, lpCalc->rectSkin.right, lpCalc->rectSkin.bottom, bf);
	if (lpCalc->bCutout && lpCalc->SkinEnabled)	{
		if (EnableCutout(lpCalc->hwndFrame, hbmSkin) != 0) {
			MessageBox(NULL, "Couldn't cutout window", "error",  MB_OK);
		}
	} else if (lpCalc->bCutout && !lpCalc->SkinEnabled) {
		DisableCutout(lpCalc->hwndFrame);
		lpCalc->bCutout = TRUE;
	} else {
		DisableCutout(lpCalc->hwndFrame);
	}
	if (lpCalc->hwndStatusBar != NULL)
		SendMessage(lpCalc->hwndStatusBar, SB_SETTEXT, 1, (LPARAM) CalcModelTxt[lpCalc->model]);

	ReleaseDC(lpCalc->hwndFrame, hdc);
	DeleteObject(hbmOldKeymap);
	DeleteObject(hbmOldSkin);
	SendMessage(lpCalc->hwndFrame, WM_SIZE, 0, 0);
	return 0;
}
#endif

/*
 * Checks based on the existence of the main window and the LCD window whether we need
 * to spawn a new process
 */
bool check_no_new_process(HWND Findhwnd, HWND *FindChildhwnd) {
	if (Findhwnd == NULL) {
		return true;
	} else {
		*FindChildhwnd = FindWindowEx(Findhwnd, NULL, g_szLCDName, NULL);
		if (*FindChildhwnd == NULL)
			*FindChildhwnd = FindWindowEx(NULL, NULL, g_szLCDName, NULL);
		return *FindChildhwnd == NULL;
	}
}

extern HWND hwndProp;
extern RECT PropRect;
extern int PropPageLast;

void RegisterWindowClasses(void)
{
	WNDCLASSEX wc;

	wc.cbSize = sizeof(wc);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hInst;
	wc.hIcon = LoadIcon(g_hInst, _T("W"));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MAIN_MENU);
	wc.lpszClassName = g_szAppName;
	wc.hIconSm = LoadIcon(g_hInst, _T("W"));

	RegisterClassEx(&wc);

	// LCD
	wc.lpszClassName = g_szLCDName;
	wc.lpfnWndProc = LCDProc;
	wc.lpszMenuName = NULL;
	RegisterClassEx(&wc);

	// Toolbar
	wc.lpszClassName = g_szToolbar;
	wc.lpfnWndProc = ToolBarProc;
	wc.lpszMenuName = NULL;
	wc.style = 0;
	RegisterClassEx(&wc);

	wc.lpfnWndProc = DebugProc;
	wc.style = CS_DBLCLKS;
	wc.lpszClassName = g_szDebugName;
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_DEBUG_MENU);
	wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE+1);
	RegisterClassEx(&wc);

	wc.lpszMenuName = NULL;
	wc.style = 0;
	wc.lpfnWndProc = DisasmProc;
	wc.lpszClassName = g_szDisasmName;
	wc.hbrBackground = (HBRUSH) NULL;
	RegisterClassEx(&wc);

	// Registers
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = RegProc;
	wc.lpszClassName = g_szRegName;
	wc.hbrBackground = NULL;
	RegisterClassEx(&wc);

	wc.style = 0;
	wc.lpfnWndProc = ExpandPaneProc;
	wc.lpszClassName = g_szExpandPane;
	RegisterClassEx(&wc);

	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = MemProc;
	wc.lpszClassName = g_szMemName;
	wc.hbrBackground = NULL;
	RegisterClassEx(&wc);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
   LPSTR lpszCmdParam, int nCmdShow) {

	MSG Msg;
	
	LPWSTR *argv;
	int argc;
	TCHAR tmpstring[512];
	bool loadfiles = false;
	int length,i;

	length = GetModuleFileName(NULL, ExeDir, 512);
	//LCD_X = 63+16+1;	//LCD_Y = 74+16+2;
	if (length) {
		for(i=length;i>0 && (ExeDir[i]!='\\') ;i--);
		if (i) ExeDir[i+1] = 0;
	}

	//this is here so we get our load_files_first setting
	load_files_first = QueryWabbitKey(_T("load_files_first"));

	argv = CommandLineToArgvW(GetCommandLineW(),&argc);

	HWND Findhwnd = NULL, FindChildhwnd = NULL;
	Findhwnd = FindWindow(g_szAppName, NULL);
	loadfiles = check_no_new_process(Findhwnd, &FindChildhwnd);
	// If there is a setting to load files into a new calc each time and there is a calc already running
	// ask it to create a new core to load into
	if (load_files_first && Findhwnd)
		SendMessage(Findhwnd, WM_COMMAND, IDM_FILE_NEW, 0);

	if (argv && argc>1) {
#ifdef _UNICODE
		_tcscpy(tmpstring, argv[1]);
#else
#ifdef WINVER
		size_t numConv;
		wcstombs_s(&numConv, tmpstring, argv[1], 512);
#else
		wcstombs(tmpstring, argv[1], 512);
#endif
#endif
		if ( (tmpstring[0] == '-') && (tmpstring[1] == 'n') ) loadfiles = TRUE;
		else {
			if (!loadfiles) {
				COPYDATASTRUCT cds;
				TCHAR *FileNames = NULL;
				cds.dwData = SEND_CUR;
				for(i=1; i < argc; i++) {
					memset(tmpstring, 0, 512);
#ifdef _UNICODE
					_tcscpy(tmpstring, argv[i]);
#else
#ifdef WINVER
					wcstombs_s(&numConv, tmpstring, argv[i], 512);
#else
					wcstombs(tmpstring, argv[i], 512);
#endif
#endif
					if (tmpstring[0] != '-') {
						size_t strLen;
						cds.lpData = tmpstring;
						StringCbLength(tmpstring, 512, &strLen);
						cds.cbData = strLen;
						SendMessage(FindChildhwnd, WM_COPYDATA, (WPARAM) NULL, (LPARAM) &cds);
					} else {
						if (toupper(tmpstring[1]) == 'F')
							SwitchToThisWindow(FindChildhwnd, TRUE);
					}
				}
			}
		}
	}
	
	if (Findhwnd && loadfiles)
		SendMessage(Findhwnd, WM_COMMAND, IDM_FILE_NEW, 0);

	if (!loadfiles) {
		SwitchToThisWindow(FindChildhwnd, TRUE);
		exit(0);
	}

    g_hInst = hInstance;

	RegisterWindowClasses();

	// initialize com events
	OleInitialize(NULL);

#ifdef USE_GDIPLUS
	// Initialize GDI+.
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
#endif


	if (argv && argc>1) {
		for (i = 1; i < argc; i++) {
			memset(tmpstring, 0, 512);
#ifdef _UNICODE
			_tcscpy(tmpstring, argv[i]);
#else
			size_t numConv;
			wcstombs_s(&numConv, tmpstring, argv[i], 512);
#endif
			if (tmpstring[0] == '/' || tmpstring[0] == '-') {
				if (toupper(tmpstring[1]) == 'S') {
					silent_mode = TRUE;
				}
			}
		}
	}

	LPCALC lpCalc = calc_slot_new();
	LoadRegistrySettings(lpCalc);

	if (rom_load(lpCalc, lpCalc->rom_path) == TRUE) {
		gui_frame(lpCalc);
	} else {
		calc_slot_free(lpCalc);

		if (show_wizard) {
			BOOL wizardError = DoWizardSheet(NULL);
			//save wizard show
			SaveWabbitKey(_T("show_wizard"), REG_DWORD, &show_wizard);
			SaveWabbitKey(_T("rom_path"), REG_SZ, &lpCalc->rom_path);
			if (wizardError)
				return EXIT_FAILURE;
			LoadRegistrySettings(lpCalc);
		} else {
			const TCHAR lpstrFilter[] 	= _T("Known types ( *.sav; *.rom) \0*.sav;*.rom\0\
												Save States  (*.sav)\0*.sav\0\
												ROMs  (*.rom)\0*.rom\0\
												All Files (*.*)\0*.*\0\0");
			const TCHAR lpstrTitle[] = _T("Wabbitemu: Please select a ROM or save state");
			const TCHAR lpstrDefExt[] = _T("rom");
			TCHAR* FileName = (TCHAR *) malloc(MAX_PATH);
			ZeroMemory(FileName, MAX_PATH);
			if (!BrowseFile(FileName, lpstrFilter, lpstrTitle, lpstrDefExt, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST)) {
				lpCalc = calc_slot_new();
				if (rom_load(lpCalc, FileName) == TRUE)
					gui_frame(lpCalc);
				else return EXIT_FAILURE;
			} else return EXIT_FAILURE;
		}
	}

	StringCbCopy(lpCalc->labelfn, sizeof(lpCalc->labelfn), _T("labels.lab"));

	state_build_applist(&lpCalc->cpu, &lpCalc->applist);
	VoidLabels(lpCalc);

	if (loadfiles) {
		if (argv && argc > 1) {
			TCHAR* FileNames = NULL;
			for(i = 1; i < argc; i++) {
				memset(tmpstring, 0, 512);
#ifdef _UNICODE
				_tcscpy(tmpstring, argv[i]);
#else
				size_t numConv;
				wcstombs_s(&numConv, tmpstring, argv[i], 512);
#endif
				if (tmpstring[0] != '-') {
					SendFileToCalc(lpCalc, tmpstring, TRUE);
				}
			}
		}
	}
	loadfiles = FALSE;
	//initialize linking hub
	memset(link_hub, 0, sizeof(link_hub));
	link_t *hub_link = (link_t *) malloc(sizeof(link_t)); 
	if (!hub_link) {
		printf("Couldn't allocate memory for link\n");
	}
	hub_link->host		= 0;			//neither lines set
	hub_link->client	= &hub_link->host;	//nothing plugged in.
	link_hub[MAX_CALCS] = hub_link;

	InitCommonControls();
#ifdef USE_DIRECTX
	IDirect3D9 *pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferCount = 1;

	if (pD3D->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			lpCalc->hwndLCD,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING|D3DCREATE_FPU_PRESERVE ,
			&d3dpp,
			&pd3dDevice) != D3D_OK) {
		printf("failed to create device\n");
		exit(1);
	}
#endif

	// Set the one global timer for all calcs
	SetTimer(NULL, 0, TPF, TimerProc);

	hacceldebug = LoadAccelerators(g_hInst, _T("DisasmAccel"));
	if (!haccelmain)
		haccelmain = LoadAccelerators(g_hInst, _T("Z80Accel"));

    while (GetMessage(&Msg, NULL, 0, 0)) {
		HACCEL haccel = haccelmain;
		HWND hwndtop = GetForegroundWindow();
		if (hwndtop) {
			if (hwndtop == FindWindow(g_szDebugName, NULL) ) {
				haccel = hacceldebug;
			} else if (hwndtop == FindWindow(g_szAppName, NULL) ) {
				haccel = haccelmain;
				if (lpCalc->bCutout && lpCalc->SkinEnabled)
					hwndtop = FindWindow(g_szLCDName, NULL);
				else
					hwndtop = FindWindowEx(hwndtop, NULL, g_szLCDName, NULL);
				SetForegroundWindow(hwndtop);
			}
		}

		if (hwndProp != NULL) {
			if (PropSheet_GetCurrentPageHwnd(hwndProp) == NULL) {
				GetWindowRect(hwndProp, &PropRect);
				DestroyWindow(hwndProp);
				hwndProp = NULL;
			}
		}

		if (hwndProp == NULL || PropSheet_IsDialogMessage(hwndProp, &Msg) == FALSE) {
			if (!TranslateAccelerator(hwndtop, haccel, &Msg)) {
				TranslateMessage(&Msg);
	        	DispatchMessage(&Msg);
			}
		} else {
			// Get the current tab
			HWND hwndPropTabCtrl = PropSheet_GetTabControl(hwndProp);
			PropPageLast = TabCtrl_GetCurSel(hwndPropTabCtrl);
		}
    }

    // Make sure the GIF has terminated
	if (gif_write_state == GIF_FRAME) {
		gif_write_state = GIF_END;
		handle_screenshot();
	}

	//free the link we setup to act as our hub
	free(hub_link);
	
#ifdef USE_GDIPLUS
	// Shutdown GDI+
	GdiplusShutdown(gdiplusToken);
#endif

	// Shutdown COM
	OleUninitialize();
#if _DEBUG
	_CrtDumpMemoryLeaks();
#endif

    return (int) Msg.wParam;
}

static HWND hListDialog;
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	//static HDC hdcKeymap;
	static POINT ctxtPt;
	calc_t *lpCalc = (calc_t *) GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (Message) {
		case WM_CREATE:
		{
			calc_t *lpCalc = (calc_t *) ((LPCREATESTRUCT) lParam)->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) lpCalc);

			IDropTarget *pDropTarget = NULL;
			RegisterDropWindow(hwnd, &pDropTarget);

			// Force the current skin setting to be enacted
			lpCalc->SkinEnabled = !lpCalc->SkinEnabled;
			SendMessage(hwnd, WM_COMMAND, IDM_CALC_SKIN, 0);

			SetWindowText(hwnd, _T("Wabbitemu"));
			return 0;
		}
		case WM_USER:
			gui_frame_update(lpCalc);
			break;
		case WM_PAINT:
		{
#define GIFGRAD_PEAK 15
#define GIFGRAD_TROUGH 10

			static int GIFGRADWIDTH = 1;
			static int GIFADD = 1;

			if (gif_anim_advance) {
				switch (lpCalc->gif_disp_state) {
					case GDS_STARTING:
						if (GIFGRADWIDTH > 15) {
							lpCalc->gif_disp_state = GDS_RECORDING;
							GIFADD = -1;
						} else {
							GIFGRADWIDTH ++;
						}
						break;
					case GDS_RECORDING:
						GIFGRADWIDTH += GIFADD;
						if (GIFGRADWIDTH > GIFGRAD_PEAK) GIFADD = -1;
						else if (GIFGRADWIDTH < GIFGRAD_TROUGH) GIFADD = 1;
						break;
					case GDS_ENDING:
						if (GIFGRADWIDTH) GIFGRADWIDTH--;
						else {
							lpCalc->gif_disp_state = GDS_IDLE;							
							gui_frame_update(lpCalc);						
						}						
						break;
					case GDS_IDLE:
						break;
				}
				gif_anim_advance = FALSE;
			}

			if (lpCalc->gif_disp_state != GDS_IDLE) {
				RECT screen, rc;
				GetWindowRect(lpCalc->hwndLCD, &screen);
				GetWindowRect(lpCalc->hwndFrame, &rc);
				int orig_w = screen.right - screen.left;
				int orig_h = screen.bottom - screen.top;

				AdjustWindowRect(&screen, WS_CAPTION, FALSE);
				screen.top -= GetSystemMetrics(SM_CYMENU);

				//printf("screen: %d\n", screen.left - rc.left);

				SetRect(&screen, screen.left - rc.left -5, screen.top - rc.top - 5,
						screen.left - rc.left + orig_w - 5,
						screen.top - rc.top + orig_h - 5);

				int grayred = (int) (((double) GIFGRADWIDTH / GIFGRAD_PEAK) * 50);
				HDC hWindow = GetDC(hwnd);
				DrawGlow(lpCalc->hdcSkin, hWindow, &screen, RGB(127 - grayred, 127 - grayred, 127 + grayred),
							GIFGRADWIDTH, lpCalc->SkinEnabled);				
				ReleaseDC(hwnd, hWindow);
				InflateRect(&screen, GIFGRADWIDTH, GIFGRADWIDTH);
				ValidateRect(hwnd, &screen);
			}

			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);
			if (lpCalc->SkinEnabled) {
				BitBlt(hdc, 0, 0, lpCalc->rectSkin.right, lpCalc->rectSkin.bottom, lpCalc->hdcButtons, 0, 0, SRCCOPY);
				BitBlt(lpCalc->hdcButtons, 0, 0, lpCalc->rectSkin.right, lpCalc->rectSkin.bottom, lpCalc->hdcSkin, 0, 0, SRCCOPY);
			} else {
				RECT rc;
				GetClientRect(lpCalc->hwndFrame, &rc);
				FillRect(hdc, &rc, GetStockBrush(GRAY_BRUSH));
			}
			ReleaseDC(hwnd, hdc);
			EndPaint(hwnd, &ps);

			return 0;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDM_FILE_NEW: {
						LPCALC lpCalcNew = calc_slot_new();
						if (rom_load(lpCalcNew, lpCalc->rom_path) == TRUE) {
							lpCalcNew->SkinEnabled = lpCalc->SkinEnabled;
							lpCalcNew->bCutout = lpCalc->bCutout;
							lpCalcNew->Scale = lpCalc->Scale;
							lpCalcNew->FaceplateColor = lpCalc->FaceplateColor;
							lpCalcNew->bAlphaBlendLCD = lpCalc->bAlphaBlendLCD;
							calc_turn_on(lpCalcNew);
							gui_frame(lpCalcNew);
						} else {
							calc_slot_free(lpCalcNew);
						}
						break;
				}
				case IDM_FILE_OPEN: {
					GetOpenSendFileName(hwnd);
					break;
				}
				case IDM_FILE_SAVE: {
					TCHAR FileName[MAX_PATH];
					const TCHAR lpstrFilter[] 	= _T("Known File types ( *.sav; *.rom; *.bin) \0*.sav;*.rom;*.bin\0\
														Save States  (*.sav)\0*.sav\0\
														ROMS  (*.rom; .bin)\0*.rom;*.bin\0\
														All Files (*.*)\0*.*\0\0");
					ZeroMemory(FileName, MAX_PATH);
					SaveFile(FileName, (TCHAR *) lpstrFilter, _T("Wabbitemu Save State"), _T("sav"), OFN_PATHMUSTEXIST);
					SAVESTATE_t *save = SaveSlot(lpCalc);
					gui_savestate(hwnd, save, FileName, lpCalc);
					break;
				}
				case IDM_FILE_GIF: {
					HMENU hmenu = GetMenu(hwnd);
					if (gif_write_state == GIF_IDLE) {
						BOOL start_screenshot = get_gif_filename();
						if (!start_screenshot)
							break;
						gif_write_state = GIF_START;
						for (int i = 0; i < MAX_CALCS; i++)
							if (calcs[i].active)
								calcs[i].gif_disp_state = GDS_STARTING;
						CheckMenuItem(GetSubMenu(hmenu, MENU_FILE), IDM_FILE_GIF, MF_BYCOMMAND | MF_CHECKED);
					} else {
						gif_write_state = GIF_END;
						for (int i = 0; i < MAX_CALCS; i++)
							if (calcs[i].active)
								calcs[i].gif_disp_state = GDS_ENDING;
						CheckMenuItem(GetSubMenu(hmenu, MENU_FILE), IDM_FILE_GIF, MF_BYCOMMAND | MF_UNCHECKED);
					}
					break;
				}
				case IDM_FILE_CLOSE:
					return SendMessage(hwnd, WM_CLOSE, 0, 0);
				case IDM_FILE_EXIT:
					if (calc_count() > 1) {
						TCHAR buf[256];
#ifdef WINVER
						StringCbPrintf(buf, sizeof(buf), _T("If you exit now, %d other running calculator(s) will be closed. \
															Are you sure you want to exit?"), calc_count() - 1);
#else
						sprintf(buf, "If you exit now, %d other running calculator(s) will be closed.  Are you sure you want to exit?", calc_count()-1);
#endif
						int res = MessageBox(NULL, buf, _T("Wabbitemu"), MB_YESNO);
						if (res == IDCANCEL || res == IDNO)
							break;
						PostQuitMessage(0);
					}
					SendMessage(hwnd, WM_CLOSE, 0, 0);
					break;
				case IDM_EDIT_COPY: {
					HLOCAL ans;
					ans = (HLOCAL) GetRealAns(&lpCalc->cpu);
					OpenClipboard(hwnd);
					EmptyClipboard();
					SetClipboardData(CF_TEXT, ans);
					CloseClipboard();
					break;
				}
				case IDM_EDIT_PASTE: {
					
					break;
				}
				case IDM_CALC_SKIN: {
					lpCalc->SkinEnabled = !lpCalc->SkinEnabled;
					gui_frame_update(lpCalc);
					break;
				}
				case IDM_CALC_SOUND: {
					togglesound(lpCalc->audio);
					CheckMenuItem(GetSubMenu(GetMenu(hwnd), 2), IDM_CALC_SOUND, MF_BYCOMMAND | (lpCalc->audio->enabled ? MF_CHECKED : MF_UNCHECKED));
					break;
				}
				case IDM_CALC_CONNECT: {
					/*if (!calcs[0].active || !calcs[1].active || link_connect(&calcs[0].cpu, &calcs[1].cpu))						
						MessageBox(NULL, _T("Connection Failed"), _T("Error"), MB_OK);					
					else*/
					link_connect_hub(lpCalc->slot, &lpCalc->cpu);
					//MessageBox(NULL, _T("Connection Successful"), _T("Success"), MB_OK);					
					break;
				}
				case IDM_CALC_PAUSE: {
					HMENU hmenu = GetMenu(hwnd);
					if (lpCalc->running) {
						CheckMenuItem(GetSubMenu(hmenu, 2), IDM_CALC_PAUSE, MF_BYCOMMAND | MF_CHECKED);
						lpCalc->running = FALSE;
					} else {
						CheckMenuItem(GetSubMenu(hmenu, 2), IDM_CALC_PAUSE, MF_BYCOMMAND | MF_UNCHECKED);
						lpCalc->running = TRUE;
					}
					break;
				}
				case IDM_CALC_VARIABLES:
					CreateVarTreeList();
					break;
				case IDM_CALC_KEYSPRESSED:
					if (IsWindow(hListDialog)) {
						SwitchToThisWindow(hListDialog, TRUE);
					} else {
						hListDialog = (HWND) CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_KEYS_LIST), hwnd, (DLGPROC) KeysListProc);
						ShowWindow(hListDialog, SW_SHOW);
					}
					break;
				case IDM_CALC_OPTIONS:
					DoPropertySheet(hwnd);
					break;
				case IDM_DEBUG_RESET: {
					calc_reset(lpCalc);
					//calc_turn_on(lpCalc);
					break;
				}
				case IDM_DEBUG_OPEN:
					gui_debug(lpCalc);
					break;
				case IDM_HELP_ABOUT:
					lpCalc->running = FALSE;
					DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DLGABOUT), hwnd, (DLGPROC) AboutDialogProc);					
					lpCalc->running = TRUE;					
					break;
				case IDM_HELP_WIZARD:
					DoWizardSheet(hwnd);
					break;
				case IDM_HELP_WEBSITE:					
					ShellExecute(NULL, _T("open"), g_szWebPage, NULL, NULL, SW_SHOWNORMAL);
					break;
				case IDM_FRAME_BTOGGLE:
					SendMessage(hwnd, WM_MBUTTONDOWN, MK_MBUTTON, MAKELPARAM(ctxtPt.x, ctxtPt.y));
					InvalidateRect(hwnd, &lpCalc->rectSkin, TRUE);
					UpdateWindow(hwnd);
					break;
				case IDM_FRAME_BUNLOCK: {
					RECT rc;
					keypad_t *kp = (keypad_t *) lpCalc->cpu.pio.devices[1].aux;
					int group,bit;
					GetClientRect(hwnd, &rc);
					for(group = 0; group < 7; group++) {
						for(bit = 0; bit < 8; bit++) {
							kp->keys[group][bit] &= (~KEY_LOCKPRESS);
						}
					}
					lpCalc->cpu.pio.keypad->on_pressed &= (~KEY_LOCKPRESS);

					InvalidateRect(hwnd, &lpCalc->rectSkin, TRUE);
					UpdateWindow(hwnd);
					SendMessage(hwnd, WM_SIZE, 0, 0);
					break;
				}
				case IDM_SPEED_QUARTER: {
					lpCalc->speed = 25;
					CheckMenuRadioItem(GetSubMenu(GetMenu(hwnd), 2), IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_QUARTER, MF_BYCOMMAND| MF_CHECKED);
					break;
				}
				case IDM_SPEED_HALF: {
					lpCalc->speed = 50;
					HMENU hmenu = GetMenu(hwnd);
					CheckMenuRadioItem(GetSubMenu(hmenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_HALF, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
				case IDM_SPEED_NORMAL: {
					lpCalc->speed = 100;
					HMENU hmenu = GetMenu(hwnd);
					CheckMenuRadioItem(GetSubMenu(hmenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_NORMAL, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
				case IDM_SPEED_DOUBLE: {
					lpCalc->speed = 200;
					HMENU hmenu = GetMenu(hwnd);
					CheckMenuRadioItem(GetSubMenu(hmenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_DOUBLE, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
				case IDM_SPEED_QUADRUPLE: {
					lpCalc->speed = 400;
					HMENU hmenu = GetMenu(hwnd);
					CheckMenuRadioItem(GetSubMenu(hmenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_QUADRUPLE, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
				case IDM_SPEED_MAX: {
					lpCalc->speed = MAX_SPEED;
					HMENU hmenu = GetMenu(hwnd);
					CheckMenuRadioItem(GetSubMenu(hmenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_MAX, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
				case IDM_SPEED_SET: {
					HMENU hmenu = GetMenu(hwnd);
					CheckMenuRadioItem(GetSubMenu(hmenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_SET, MF_BYCOMMAND | MF_CHECKED);
					DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_DLGSPEED), hwnd, (DLGPROC) SetSpeedProc, (LPARAM) lpCalc);
					SetFocus(hwnd);
					break;
				}
				case IDM_HELP_UPDATE: {
					TCHAR buffer[MAX_PATH];
					TCHAR *env;
					size_t envLen;
					_tdupenv_s(&env, &envLen, _T("appdata"));
					StringCbCopy(buffer, sizeof(buffer), env);
					free(env);
					StringCbCat(buffer, sizeof(buffer), _T("\\Revsoft.Autoupdater.exe"));
					HRSRC hrDumpProg = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_UPDATER), _T("UPDATER"));
					ExtractResource(buffer, hrDumpProg);

					TCHAR argBuf[MAX_PATH * 3];
					TCHAR filePath[MAX_PATH];
					GetModuleFileName(NULL, filePath, MAX_PATH);
					StringCbPrintf(argBuf, sizeof(argBuf), _T("\"%s\" -R \"%s\" \"%s\" \"%s\""), buffer, filePath, filePath, g_szDownload);
					STARTUPINFO si;
					PROCESS_INFORMATION pi;
					memset(&si, 0, sizeof(si)); 
					memset(&pi, 0, sizeof(pi)); 
					si.cb = sizeof(si);
					MessageBox(NULL, argBuf, _T("TEST"), MB_OK);
					if (!CreateProcess(NULL, argBuf,
						NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, 
						NULL, NULL, &si, &pi)) {
						MessageBox(NULL, _T("Unable to start the process. Try manually downloading the update."), _T("Error"), MB_OK);
						return FALSE;
					}
					exit(0);
					break;
				}
			}
			/*switch (HIWORD(wParam)) {
			}*/
			return 0;
		}
		//case WM_MOUSEMOVE:
		case WM_LBUTTONUP:
		case WM_LBUTTONDOWN:
		{
			int group, bit;
			static POINT pt;
			keypad_t *kp = lpCalc->cpu.pio.keypad;

			//CopySkinToButtons();
			if (Message == WM_LBUTTONDOWN) {
				SetCapture(hwnd);
				pt.x	= GET_X_LPARAM(lParam);
				pt.y	= GET_Y_LPARAM(lParam);
				if (lpCalc->bCutout) {
					pt.y += GetSystemMetrics(SM_CYCAPTION);	
					pt.x += GetSystemMetrics(SM_CXSIZEFRAME);
				}
			} else {
				ReleaseCapture();
			}

			for(group = 0; group < 7; group++) {
				for(bit = 0; bit < 8; bit++) {
					kp->keys[group][bit] &= (~KEY_MOUSEPRESS);
				}
			}

			lpCalc->cpu.pio.keypad->on_pressed &= ~KEY_MOUSEPRESS;

			if (wParam != MK_LBUTTON) {
				FinalizeButtons(lpCalc);
				return 0;
			}

			COLORREF c = GetPixel(lpCalc->hdcKeymap, pt.x, pt.y);
			if (GetRValue(c) == 0xFF) {
				FinalizeButtons(lpCalc);
				return 0;
			}

			if ( (GetGValue(c) >> 4) == 0x05 && (GetBValue(c) >> 4) == 0x00){
				lpCalc->cpu.pio.keypad->on_pressed |= KEY_MOUSEPRESS;
			} else {
				kp->keys[GetGValue(c) >> 4][GetBValue(c) >> 4] |= KEY_MOUSEPRESS;
				if ((kp->keys[GetGValue(c) >> 4][GetBValue(c) >> 4] & KEY_STATEDOWN) == 0) {
					DrawButtonState(lpCalc, lpCalc->hdcButtons, lpCalc->hdcKeymap, &pt, DBS_DOWN | DBS_PRESS);
					kp->keys[GetGValue(c) >> 4][GetBValue(c) >> 4] |= KEY_STATEDOWN;
					//SendMessage(hwnd, WM_SIZE, 0, 0);
				}
			}
		InvalidateRect(hwnd, &lpCalc->rectSkin, TRUE);
		UpdateWindow(hwnd);

		//extern POINT ButtonCenter83[64];
		//extern POINT ButtonCenter84[64];
			return 0;
		}
		case WM_MBUTTONDOWN:
		{
			int group,bit;
			POINT pt;
			keypad_t *kp = (keypad_t *) (&lpCalc->cpu)->pio.devices[1].aux;

			pt.x	= GET_X_LPARAM(lParam);
			pt.y	= GET_Y_LPARAM(lParam);

			COLORREF c = GetPixel(lpCalc->hdcKeymap, pt.x, pt.y);
			if (GetRValue(c) == 0xFF) return 0;
			group	= GetGValue(c)>>4;
			bit		= GetBValue(c)>>4;

			if (group== 0x05 && bit == 0x00) {
				lpCalc->cpu.pio.keypad->on_pressed ^= KEY_LOCKPRESS;
				if ( lpCalc->cpu.pio.keypad->on_pressed &  KEY_LOCKPRESS ) {
					DrawButtonState(lpCalc, lpCalc->hdcButtons, lpCalc->hdcKeymap, &pt, DBS_DOWN | DBS_LOCK);
				} else {
					DrawButtonState(lpCalc, lpCalc->hdcButtons, lpCalc->hdcKeymap, &pt, DBS_LOCK | DBS_UP);
				}
			}
			kp->keys[group][bit] ^= KEY_LOCKPRESS;
			if (kp->keys[group][bit] &  KEY_LOCKPRESS ) {
				DrawButtonState(lpCalc, lpCalc->hdcSkin, lpCalc->hdcKeymap, &pt, DBS_DOWN | DBS_LOCK);
			} else {
				DrawButtonState(lpCalc, lpCalc->hdcSkin, lpCalc->hdcKeymap, &pt, DBS_LOCK | DBS_UP);
			}
			InvalidateRect(hwnd, &lpCalc->rectSkin, TRUE);
			UpdateWindow(hwnd);
			SendMessage(hwnd, WM_SIZE, 0, 0);
			return 0;
		}

		case WM_KEYDOWN: {
			HandleKeyDown(lpCalc, wParam);
			return 0;
		}
		case WM_KEYUP:
			HandleKeyUp(lpCalc, wParam);
			return 0;
		case WM_SIZING:
		{
			if (lpCalc->SkinEnabled)
				return TRUE;
			RECT *prc = (RECT *) lParam;
			LONG ClientAdjustWidth, ClientAdjustHeight;
			LONG AdjustWidth, AdjustHeight;

			// Adjust for border and menu
			RECT rc = {0, 0, 0, 0};
			AdjustWindowRect(&rc, WS_CAPTION | WS_TILEDWINDOW, FALSE);
			if (GetMenu(hwnd) != NULL)
			{
				rc.bottom += GetSystemMetrics(SM_CYMENU);
			}

			RECT src;
			if (lpCalc->hwndStatusBar != NULL) {
				GetWindowRect(lpCalc->hwndStatusBar, &src);
				rc.bottom += src.bottom - src.top;
			}

			ClientAdjustWidth = rc.right - rc.left;
			ClientAdjustHeight = rc.bottom - rc.top;


			switch (wParam) {
			case WMSZ_BOTTOMLEFT:
			case WMSZ_LEFT:
			case WMSZ_TOPLEFT:
				prc->left -= 128 / 4;
				break;
			default:
				prc->right += 128 / 4;
				break;
			}

			switch (wParam) {
			case WMSZ_TOPLEFT:
			case WMSZ_TOP:
			case WMSZ_TOPRIGHT:
				prc->top -= 64 / 4;
				break;
			default:
				prc->bottom += 64 / 4;
				break;
			}


			// Make sure the width is a nice clean proportional sizing
			AdjustWidth = (prc->right - prc->left - ClientAdjustWidth) % 128;
			//AdjustHeight = (prc->bottom - prc->top) % lpCalc->cpu.pio.lcd->height;
			AdjustHeight = (prc->bottom - prc->top - ClientAdjustHeight) % 64;

			int cx_mult = (prc->right - prc->left - ClientAdjustWidth) / 128;
			int cy_mult = (prc->bottom - prc->top - ClientAdjustHeight) / 64;

			while (cx_mult < 2 || cy_mult < 2) {
				if (cx_mult < 2) {cx_mult++; AdjustWidth -= 128;}
				if (cy_mult < 2) {cy_mult++; AdjustHeight -= 64;}
			}

			if (cx_mult > cy_mult)
				AdjustWidth += (cx_mult - cy_mult) * 128;
			else if (cy_mult > cx_mult)
				AdjustHeight += (cy_mult - cx_mult) * 64;

			lpCalc->Scale = max(cx_mult, cy_mult);

			switch (wParam) {
			case WMSZ_BOTTOMLEFT:
			case WMSZ_LEFT:
			case WMSZ_TOPLEFT:
				prc->left += AdjustWidth;
				break;
			default:
				prc->right -= AdjustWidth;
				break;
			}

			switch (wParam) {
			case WMSZ_TOPLEFT:
			case WMSZ_TOP:
			case WMSZ_TOPRIGHT:
				prc->top += AdjustHeight;
				break;
			default:
				prc->bottom -= AdjustHeight;
				break;
			}
			RECT rect;
			GetClientRect(hwnd, &rect);
			InvalidateRect(hwnd, &rect, TRUE);
			return TRUE;
		}
		case WM_SIZE:
		{
			RECT rc, screen;
			GetClientRect(hwnd, &rc);
			HMENU hmenu = GetMenu(hwnd);
			int cyMenu;
			if (hmenu == NULL) {
				cyMenu = 0;
			} else {
				cyMenu = GetSystemMetrics(SM_CYMENU);
			}
			if ((lpCalc->bCutout && lpCalc->SkinEnabled))	
				rc.bottom += cyMenu;
			int desired_height;
			if (lpCalc->SkinEnabled)
				desired_height = lpCalc->rectSkin.bottom;
			else
				desired_height = 128;

			int status_height;
			if (lpCalc->hwndStatusBar == NULL) {
				status_height = 0;
			} else {
				RECT src;
				GetWindowRect(lpCalc->hwndStatusBar, &src);

				status_height = src.bottom - src.top;
				desired_height += status_height;
			}

			rc.bottom -= status_height;

			float xc, yc;
			if (lpCalc->SkinEnabled) {
				xc = 1/*((float)rc.right)/lpCalc->rectSkin.right*/;
				yc = 1/*((float)rc.bottom)/(lpCalc->rectSkin.bottom)*/;
			} else {
				xc = ((float) rc.right) / 256.0;
				yc = ((float) rc.bottom) / 128.0;
			}
			int width = lpCalc->rectLCD.right - lpCalc->rectLCD.left;
			SetRect(&screen,
				0, 0,
				(int) (width*xc),
				(int) (64*2*yc));

			if (lpCalc->SkinEnabled)
				OffsetRect(&screen, lpCalc->rectLCD.left, lpCalc->rectLCD.top);
			else
				OffsetRect(&screen, (int) ((rc.right - width*xc)/2), 0);

			if ((rc.right - rc.left) & 1) rc.right++;
			if ((rc.bottom - rc.top) & 1) rc.bottom++;

			RECT client;
			client.top = 0;
			client.left = 0;
			if (lpCalc->SkinEnabled && lpCalc->bCutout)
				GetWindowRect(hwnd, &client);
			if (lpCalc->SkinEnabled) {
				RECT correctSize = lpCalc->rectSkin;
				AdjustWindowRect(&correctSize, (WS_TILEDWINDOW |  WS_VISIBLE | WS_CLIPCHILDREN) & ~(WS_MAXIMIZEBOX), cyMenu);
				if (correctSize.left < 0)
					correctSize.right -= correctSize.left;
				if (correctSize.top < 0)	
					correctSize.bottom -= correctSize.top;
				SetWindowPos(hwnd, NULL, 0, 0, correctSize.right, correctSize.bottom , SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_DRAWFRAME);
			}
			MoveWindow(lpCalc->hwndLCD, screen.left + client.left, screen.top + client.top,
				screen.right-screen.left, screen.bottom-screen.top, FALSE);
			ValidateRect(hwnd, &screen);
			//printf("screen: %d\n", screen.right - screen.left);
			if (lpCalc->hwndStatusBar != NULL)
				SendMessage(lpCalc->hwndStatusBar, WM_SIZE, 0, 0);

			//force little buttons to be correct
			PositionLittleButtons(hwnd);
			UpdateWindow(lpCalc->hwndLCD);
			//InvalidateRect(hwnd, NULL, FALSE);
			return 0;
		}
		case WM_MOVE:
		//case WM_MOVING:
		{
			if (lpCalc->bCutout && lpCalc->SkinEnabled) {
				HDWP hdwp = BeginDeferWindowPos(3);
				RECT rc;
				GetWindowRect(hwnd, &rc);
				OffsetRect(&rc, lpCalc->rectLCD.left, lpCalc->rectLCD.top);
				DeferWindowPos(hdwp, lpCalc->hwndLCD, HWND_TOP, rc.left, rc.top, 0, 0, SWP_NOSIZE);
				EndDeferWindowPos(hdwp);
				PositionLittleButtons(hwnd);
			}
			return 0;
		}
		case WM_CONTEXTMENU:
		{
			ctxtPt.x = GET_X_LPARAM(lParam);
			ctxtPt.y = GET_Y_LPARAM(lParam);

			HMENU hmenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_FRAME_MENU));
		    // TrackPopupMenu cannot display the menu bar so get
		    // a handle to the first shortcut menu.
			hmenu = GetSubMenu(hmenu, 0);

			if (!OnContextMenu(hwnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), hmenu)) {
				DefWindowProc(hwnd, Message, wParam, lParam);
			}
			ScreenToClient(hwnd, &ctxtPt);
			DestroyMenu(hmenu);
			return 0;
		}
		case WM_GETMINMAXINFO: {
			if (lpCalc == NULL)
				return 0;
			if (!lpCalc->SkinEnabled)
				break;
			MINMAXINFO *info = (MINMAXINFO *) lParam;
			RECT rc = { 0, 0, 350, 725 };
			AdjustWindowRect(&rc, WS_CAPTION | WS_TILEDWINDOW, FALSE);
			info->ptMinTrackSize.x = rc.right - rc.left;
			info->ptMinTrackSize.y = rc.bottom - rc.top;
			info->ptMaxTrackSize.x = rc.right - rc.left;
			info->ptMaxTrackSize.y = rc.bottom - rc.top;
			return 0;
		}
		case WM_CLOSE:
			if (calc_count() == 1) {
				if (exit_save_state)
				{
					TCHAR temp_save[MAX_PATH];
#ifdef WINVER
					size_t len;
					TCHAR *path;
					_tdupenv_s(&path, &len, _T("appdata"));
					StringCbCopy(temp_save, sizeof(temp_save), path);
					free(path);
					StringCbCat(temp_save, sizeof(temp_save), _T("\\wabbitemu.sav"));
					StringCbCopy(lpCalc->rom_path, sizeof(lpCalc->rom_path), temp_save);
#else
					strcpy(temp_save, getenv("appdata"));
					strcat(temp_save, "\\wabbitemu.sav");
					strcpy(lpCalc->rom_path, temp_save);
#endif
					SAVESTATE_t *save = SaveSlot(lpCalc);
					WriteSave(temp_save, save, true);
					FreeSave(save);
				}

				_tprintf_s(_T("Saving registry settings\n"));
				SaveRegistrySettings(lpCalc);

			}
			DestroyWindow(hwnd);
			calc_slot_free(lpCalc);
			if (calc_count() == 0)
				PostQuitMessage(0);
			return 0;
		case WM_DESTROY:
			{
				_tprintf_s(_T("Releasing skin and keymap\n"));
				DeleteDC(lpCalc->hdcKeymap);
				DeleteDC(lpCalc->hdcSkin);
				lpCalc->hdcKeymap = NULL;
				lpCalc->hdcSkin = NULL;

				if (lpCalc->hwndDebug)
					DestroyWindow(lpCalc->hwndDebug);
				lpCalc->hwndDebug = NULL;

				if (lpCalc->hwndStatusBar)
					DestroyWindow(lpCalc->hwndStatusBar);
				lpCalc->hwndStatusBar = NULL;

				if (lpCalc->hwndSmallClose)
					DestroyWindow(lpCalc->hwndSmallClose);
				lpCalc->hwndSmallClose = NULL;

				if (lpCalc->hwndSmallMinimize)
					DestroyWindow(lpCalc->hwndSmallMinimize);
				lpCalc->hwndSmallMinimize = NULL;

				//for some reason this fails...:/
				if (!lpCalc->SkinEnabled || !lpCalc->bCutout)
					UnregisterDropWindow(hwnd, lpCalc->pDropTarget);
				lpCalc->pDropTarget = NULL;

				//if (link_connected(lpCalc->slot))
				//	link_disconnect(&lpCalc->cpu);

				lpCalc->hwndFrame = NULL;
				return 0;
			}
		case WM_NCHITTEST:
		{
			int htRet = (int) DefWindowProc(hwnd, Message, wParam, lParam);
			if (htRet != HTCLIENT) return htRet;

			POINT pt;
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);
			if (lpCalc->bCutout && lpCalc->SkinEnabled) {
				pt.y += GetSystemMetrics(SM_CYCAPTION);
				pt.x += GetSystemMetrics(SM_CXFIXEDFRAME);
			}
			ScreenToClient(hwnd, &pt);
			if (GetRValue(GetPixel(lpCalc->hdcKeymap, pt.x, pt.y)) != 0xFF)
				return htRet;
			return HTCAPTION;
		}
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

extern key_string ti83pkeystrings[KEY_STRING_SIZE];
extern key_string ti86keystrings[KEY_STRING_SIZE];
void LogKeypress(int group, int bit, UINT vk, BOOL keyDown, int model) {
	if (hListDialog) {
		int i;
		TCHAR buf[256];
		key_string *keystrings = model == TI_85 || model == TI_86 ? ti86keystrings : ti83pkeystrings;
		if (keyDown)
			StringCbCopy(buf, sizeof(buf), _T("Key down: "));
		else
			StringCbCopy(buf, sizeof(buf), _T("Key up: "));
		for (i = 0; i < KEY_STRING_SIZE; i++) {
			if (keystrings[i].group == group && keystrings[i].bit == bit)
				break;
		}
		StringCbCat(buf, sizeof(buf),keystrings[i].text);
		StringCbCat(buf, sizeof(buf), _T(" ("));
		TCHAR *name = NameFromVKey(vk);
		StringCbCat(buf, sizeof(buf), name);
		StringCbCat(buf, sizeof(buf), _T(")"));
		free(name);
		SendMessage(hListDialog, WM_USER, ADD_LISTBOX_ITEM, (LPARAM) &buf);
	}
}

INT_PTR CALLBACK AboutDialogProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
	case WM_INITDIALOG:
		return FALSE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			EndDialog(hwndDlg, IDOK);
			return TRUE;
		}
	case IDCANCEL:
		EndDialog(hwndDlg, IDCANCEL);
		break;
	}
	return FALSE;
}
