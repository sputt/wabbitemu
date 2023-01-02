#include "stdafx.h"

#include "gui.h"

#include "core.h"
#include "calc.h"
#include "83psehw.h"
#include "label.h"

#include "gif.h"
#include "screenshothandle.h"
#include "pngexport.h"

#include "var.h"
#include "keys.h"
#include "linksendvar.h"
#include "fileutilities.h"
#include "exportvar.h"

#include "dbmem.h"
#include "dbreg.h"
#include "dbtoolbar.h"
#include "dbdisasm.h"
#include "dbwatch.h"
#include "dbmonitor.h"
#include "dbcolorlcd.h"
#include "dbbreakpoints.h"

#include "guibuttons.h"
#include "guicommandline.h"
#include "guicontext.h"
#include "guicutout.h"
#include "guidebug.h"
#include "guidetached.h"
#include "guidialog.h"
#include "guiglow.h"
#include "guikeylist.h"
#include "guilcd.h"
#include "guiopenfile.h"
#include "guioptions.h"
#include "guisavestate.h"
#include "guisize.h"
#include "guiskin.h"
#include "guispeed.h"
#include "guiteacherview.h"
#include "guivartree.h"
#include "guiwizard.h"
#include "guiupdate.h"

#include "guidebug.h"
#include "DropTarget.h"
#include "expandpane.h"
#include "registry.h"
#include "sendfileswindows.h"
#include "state.h"
#include "ftp.h"

#include "CWabbitemu.h"

#define GIFGRAD_PEAK 15
#define GIFGRAD_TROUGH 10
#define KEY_TIMER 1
#define MIN_KEY_DELAY 400
#define MENU_FILE 0
#define BOOTFREE_VER_MAJOR 11
#define BOOTFREE_VER_MINOR 259
#define WM_FRAME_UPDATE WM_USER
#define WM_AVI_AUDIO_FRAME WM_FRAME_UPDATE+1
#define WM_OPEN_DEBUGGER WM_AVI_AUDIO_FRAME+1
#define WM_CLOSE_DEBUGGER WM_OPEN_DEBUGGER+1

int SKIN_WIDTH = 350;
int SKIN_HEIGHT = 725;

CWabbitemuModule _Module;

#ifdef _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

INT_PTR CALLBACK DlgVarlist(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
HINSTANCE g_hInst;
HACCEL hacceldebug;
HACCEL haccelmain;
BOOL gif_anim_advance;
BOOL silent_mode = FALSE;
BOOL is_exiting = FALSE;
HWND hwndCurrentDlg = NULL;

extern keyprog_t keygrps[256];
extern keyprog_t keysti83[256];
extern keyprog_t keysti86[256];

key_string_t ti86keystrings[KEY_STRING_SIZE] = {
	{ _T("F5"), 6, 0 }, { _T("F4"), 6, 1 }, { _T("F3"), 6, 2 }, { _T("F2"), 6, 3 }, { _T("F1"), 6, 4 }, { _T("2ND"), 6, 5 }, { _T("EXIT"), 6, 6 }, { _T("MORE"), 6, 7 },
	{ _T("ON"), 5, 0 }, { _T("STO"), 5, 1 }, { _T(","), 5, 2 }, { _T("x^2"), 5, 3 }, { _T("LN"), 5, 4 }, { _T("LOG"), 5, 5 }, { _T("GRAPH"), 5, 6 }, { _T("ALPHA"), 5, 7 },
	{ _T("0"), 4, 0 }, { _T("1"), 4, 1 }, { _T("4"), 4, 2 }, { _T("7"), 4, 3 }, { _T("EE"), 4, 4 }, { _T("SIN"), 4, 5 }, { _T("STAT"), 4, 6 }, { _T("x-Var"), 4, 7 },
	{ _T("."), 3, 0 }, { _T("2"), 3, 1 }, { _T("5"), 3, 2 }, { _T("8"), 3, 3 }, { _T("("), 3, 4 }, { _T("COS"), 3, 5 }, { _T("PRGM"), 3, 6 }, { _T("DEL"), 3, 7 },
	{ _T("(-)"), 2, 0 }, { _T("3"), 2, 1 }, { _T("6"), 2, 2 }, { _T("9"), 2, 3 }, { _T(")"), 2, 4 }, { _T("TAN"), 2, 5 }, { _T("CUSTOM"), 2, 6 }, { _T(""), 2, 7 },
	{ _T("ENTER"), 1, 0 }, { _T("+"), 1, 1 }, { _T("-"), 1, 2 }, { _T("x"), 1, 3 }, { _T("/"), 1, 4 }, { _T("^"), 1, 5 }, { _T("CLEAR"), 1, 6 }, { _T(""), 1, 7 },
	{ _T("DOWN"), 0, 0 }, { _T("LEFT"), 0, 1 }, { _T("RIGHT"), 0, 2 }, { _T("UP"), 0, 3 }, { _T(""), 0, 4 }, { _T(""), 0, 5 }, { _T(""), 0, 6 }, { _T(""), 0, 7 },
};

key_string_t ti83pkeystrings[KEY_STRING_SIZE] = {
	{ _T("GRAPH"), 6, 0 }, { _T("TRACE"), 6, 1 }, { _T("ZOOM"), 6, 2 }, { _T("WINDOW"), 6, 3 }, { _T("Y="), 6, 4 }, { _T("2ND"), 6, 5 }, { _T("MODE"), 6, 6 }, { _T("DEL"), 6, 7 },
	{ _T("ON"), 5, 0 }, { _T("STO"), 5, 1 }, { _T("LN"), 5, 2 }, { _T("LOG"), 5, 3 }, { _T("x^2"), 5, 4 }, { _T("x^-1"), 5, 5 }, { _T("MATH"), 5, 6 }, { _T("ALPHA"), 5, 7 },
	{ _T("0"), 4, 0 }, { _T("1"), 4, 1 }, { _T("4"), 4, 2 }, { _T("7"), 4, 3 }, { _T(","), 4, 4 }, { _T("SIN"), 4, 5 }, { _T("APPS"), 4, 6 }, { _T("X,T,0,n"), 4, 7 },
	{ _T("."), 3, 0 }, { _T("2"), 3, 1 }, { _T("5"), 3, 2 }, { _T("8"), 3, 3 }, { _T("("), 3, 4 }, { _T("COS"), 3, 5 }, { _T("PRGM"), 3, 6 }, { _T("STAT"), 3, 7 },
	{ _T("(-)"), 2, 0 }, { _T("3"), 2, 1 }, { _T("6"), 2, 2 }, { _T("9"), 2, 3 }, { _T(")"), 2, 4 }, { _T("TAN"), 2, 5 }, { _T("VARS"), 2, 6 }, { _T(""), 2, 7 },
	{ _T("ENTER"), 1, 0 }, { _T("+"), 1, 1 }, { _T("-"), 1, 2 }, { _T("x"), 1, 3 }, { _T("/"), 1, 4 }, { _T("^"), 1, 5 }, { _T("CLEAR"), 1, 6 }, { _T(""), 1, 7 },
	{ _T("DOWN"), 0, 0 }, { _T("LEFT"), 0, 1 }, { _T("RIGHT"), 0, 2 }, { _T("UP"), 0, 3 }, { _T(""), 0, 4 }, { _T(""), 0, 5 }, { _T(""), 0, 6 }, { _T(""), 0, 7 },
};

void gui_debug(LPCALC lpCalc, LPVOID lParam);

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ToolProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);


void gui_draw(LPCALC, LPVOID lParam) {
	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)lParam;
	if (lpMainWindow->hwndLCD != NULL) {
		InvalidateRect(lpMainWindow->hwndLCD, NULL, FALSE);
	}

	if (lpMainWindow->hwndDetachedLCD != NULL) {
		InvalidateRect(lpMainWindow->hwndDetachedLCD, NULL, FALSE);
	}

	if (lpMainWindow->gif_disp_state != GDS_IDLE) {
		static int skip = 0;
		if (skip == 0) {
			gif_anim_advance = TRUE;
			if (lpMainWindow->hwndFrame != NULL) {
				InvalidateRect(lpMainWindow->hwndFrame, NULL, FALSE);
			}
		}

		skip = (skip + 1) % 4;
	}
}

extern BITMAPINFO *bi, *colorbi, *contrastbi;
LPBITMAPINFO GetLCDColorPalette(CalcModel model, LCDBase_t *lcd) {
	BITMAPINFO *info = model >= TI_84PCSE ? colorbi : bi;
	if (model <= TI_84PSE && lcd->active && lcd->contrast > LCD_MAX_CONTRAST - 4) {
		info = contrastbi;
	}

	return info;
}

HANDLE DDBToDIB(HBITMAP bitmap, DWORD dwCompression) {
	BITMAP			bm;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER 	lpbi;
	DWORD			dwLen;
	HANDLE			hDIB;
	HANDLE			handle;
	HDC 			hDC;


	// The function has no arg for bitfields
	if (dwCompression == BI_BITFIELDS)
		return NULL;

	// Get bitmap information
	GetObject(bitmap, sizeof(bm), (LPSTR)&bm);

	// Initialize the bitmapinfoheader
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bm.bmWidth;
	bi.biHeight = bm.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression = dwCompression;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if (nColors > 256)
		nColors = 0;
	dwLen = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = GetDC(NULL);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED, dwLen);

	if (!hDIB) {
		ReleaseDC(NULL, hDC);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver 
	// will calculate the biSizeImage field 
	GetDIBits(hDC, bitmap, 0L, (DWORD)bi.biHeight,
		(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0) {
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8)
			* bi.biHeight;

		// If a compression scheme is used the result may in fact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE);
	if (handle) {
		hDIB = handle;
	} else {
		GlobalFree(hDIB);
		ReleaseDC(NULL, hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits(hDC, bitmap,
		0L,				// Start scan line
		(DWORD)bi.biHeight,		// # of scan lines
		/*(LPBYTE)lpbi 			// address for bitmap bits
		+ (bi.biSize + nColors * sizeof(RGBQUAD))*/ (LPBYTE)hDIB,
		(LPBITMAPINFO)lpbi,		// address of bitmapinfo
		(DWORD)DIB_RGB_COLORS);		// Use RGB for color table

	if (!bGotBits) {
		GlobalFree(hDIB);

		ReleaseDC(NULL, hDC);
		return NULL;
	}

	ReleaseDC(NULL, hDC);
	return hDIB;
}

void handle_avi_video_frame(LPCALC, LPVOID lParam) {
	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)lParam;
	if (lpMainWindow == NULL || !lpMainWindow->m_IsRecording) {
		return;
	}

	HWND hwnd = lpMainWindow->hwndLCD;
	HDC hdc = GetDC(hwnd);
	RECT rc;
	GetClientRect(hwnd, &rc);
	HBITMAP hbm = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
	HDC newhdc = CreateCompatibleDC(hdc);
	SelectObject(newhdc, hbm);
	BitBlt(newhdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hdc, 0, 0, SRCCOPY);

	HRESULT hResult = lpMainWindow->m_CurrentAvi->AppendNewFrame(hbm);
	if (FAILED(hResult)) {
		TCHAR buf[512];
		StringCbCopy(buf, sizeof(buf), lpMainWindow->m_CurrentAvi->GetLastErrorMessage());
		SendMessage(lpMainWindow->hwndFrame, WM_COMMAND, MAKEWPARAM(IDM_FILE_AVI, 0), 0);
		MessageBox(lpMainWindow->hwndFrame, buf, _T("Error writing video data"), MB_OK | MB_ICONERROR);
	}

	DeleteObject(hbm);
}

void handle_avi_audio_frame(LPCALC, LPVOID lParam) {
	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)lParam;
	if (lpMainWindow == NULL || !lpMainWindow->m_IsRecording) {
		return;
	}

	// this will come in on a background thread, send it to the main thread to
	// avoid race conditions
	SendMessage(lpMainWindow->hwndFrame, WM_AVI_AUDIO_FRAME, 0, 0);
}

void avi_audio_frame(LPMAINWINDOW lpMainWindow) {
	if (lpMainWindow == NULL || !lpMainWindow->m_IsRecording) {
		return;
	}

	AUDIO_t *audio = lpMainWindow->lpCalc->audio;

	u_char *buffer[BANK_SIZE];
	unsigned char *dataout = (unsigned char *)&audio->buffer[audio->PlayPnt];
	unsigned char *datain = (unsigned char *)buffer;
	unsigned char *dataend = (unsigned char *)&audio->buffer[BUFFER_SMAPLES];
	for (int i = 0; i < BANK_SIZE; i++) {
		if (dataout >= dataend) {
			dataout = (unsigned char *)&audio->buffer[0];
		}

		datain[i] = dataout[0];
		dataout++;
	}

	HRESULT hResult = lpMainWindow->m_CurrentAvi->AppendAudioData(&audio->wfx,
		buffer, BANK_SIZE);
	if (FAILED(hResult)) {
		TCHAR buf[512];
		StringCbCopy(buf, sizeof(buf), lpMainWindow->m_CurrentAvi->GetLastErrorMessage());
		SendMessage(lpMainWindow->hwndFrame, WM_COMMAND, MAKEWPARAM(IDM_FILE_AVI, 0), 0);
		MessageBox(lpMainWindow->hwndFrame, buf, _T("Error writing audio data"), MB_OK | MB_ICONERROR);
	}
}

VOID CALLBACK TimerProc(HWND, UINT, UINT_PTR, DWORD dwTimer) {
	static long difference;
	static DWORD prevTimer;

	// How different the timer is from where it should be
	// guard from erroneous timer calls with an upper bound
	// that's the limit of time it will take before the
	// calc gives up and claims it lost time
	difference += ((dwTimer - prevTimer) & 0x003F) - TPF;
	prevTimer = dwTimer;

	// Are we greater than Ticks Per Frame that would call for
	// a frame skip?
	if (difference > -TPF) {
		calc_run_all();
		while (difference >= TPF) {
			calc_run_all();
			difference -= TPF;
		}
	} else {
		// Frame skip if we're too far ahead.
		difference += TPF;
	}
}

HWND gui_debug_hwnd(LPMAINWINDOW lpMainWindow) {
	if (lpMainWindow == NULL) {
		return NULL;
	}

	gui_debug(lpMainWindow->lpCalc, lpMainWindow);
	return lpMainWindow->hwndDebug;
}

void gui_debug(LPCALC lpCalc, LPVOID lParam) {
	if (lParam == NULL) {
		return;
	}

	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)lParam;
	TCHAR buf[256];
	if (link_connected_hub(lpCalc->slot)) {
		StringCbPrintf(buf, sizeof(buf), _T("Debugger (%d)"), lpCalc->slot + 1);
	} else {
		StringCbCopy(buf, sizeof(buf), _T("Debugger"));
	}
	if (lpCalc->audio != NULL) {
		pausesound(lpCalc->audio);
	}
	BOOL set_place = TRUE;
	int flags = 0;
	int iDpi = GetDpiForWindow(lpMainWindow->hwndFrame);

	RECT pos = {0, 0, MulDiv(800, iDpi, 96), MulDiv(600, iDpi, 96)};
	WINDOWPLACEMENT db_placement = {0};
	LPDEBUGWINDOWINFO lpDebugInfo = (LPDEBUGWINDOWINFO)GetWindowLongPtr(lpMainWindow->hwndDebug, GWLP_USERDATA);
	if (!lpDebugInfo) {
		db_placement.flags = SW_SHOWNORMAL;
		db_placement.length = sizeof(WINDOWPLACEMENT);
		CopyRect(&db_placement.rcNormalPosition, &pos);
		set_place = FALSE;
		flags = WS_VISIBLE;
	} else {
		db_placement = lpDebugInfo->db_placement;
	}

	calc_set_running(lpCalc, FALSE);
	calc_pause_linked();
	if (lpMainWindow->hwndDebug && IsWindow(lpMainWindow->hwndDebug)) {
		SwitchToThisWindow(lpMainWindow->hwndDebug, TRUE);
		waddr_t waddr = addr16_to_waddr(&lpCalc->mem_c, lpCalc->cpu.pc);
		while (!lpDebugInfo->is_ready) {
			Sleep(100);
		}

		SendMessage(lpMainWindow->hwndDebug, WM_COMMAND, MAKEWPARAM(DB_DISASM_GOTO_ADDR, 0), (LPARAM)&waddr);
		SendMessage(lpMainWindow->hwndDebug, WM_USER, DB_RESUME, 0);
		return;
	}

	lpMainWindow->hwndDebug = CreateWindowEx(
		WS_EX_CONTROLPARENT,
		g_szDebugName,
		buf,
		flags | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		db_placement.rcNormalPosition.left, db_placement.rcNormalPosition.top,
		db_placement.rcNormalPosition.right - db_placement.rcNormalPosition.left,
		db_placement.rcNormalPosition.bottom - db_placement.rcNormalPosition.top,
		0, 0, g_hInst, (LPVOID) lpMainWindow);
	if (set_place) {
		SetWindowPlacement(lpMainWindow->hwndDebug, &db_placement);
	}

	SendMessage(lpMainWindow->hwndDebug, WM_SIZE, 0, 0);
	Debug_UpdateWindow(lpMainWindow->hwndDebug);
}

POINT GetStartPoint() {
	LONG startX = (LONG)QueryWabbitKey(_T("startX"));
	LONG startY = (LONG)QueryWabbitKey(_T("startY"));

	RECT desktop = { 0 };
	POINT topLeftPt = { startX, startY };
	desktop.right = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	desktop.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	if (!PtInRect(&desktop, topLeftPt)) {
		//pt is not on the desktop
		startX = CW_USEDEFAULT;
		startY = CW_USEDEFAULT;
	}

	return topLeftPt;
}

LPMAINWINDOW gui_frame(LPCALC lpCalc) {
	RECT r;

	AdjustWindowRect(&r, WS_CAPTION | WS_TILEDWINDOW, TRUE);

	// this is to do some checks on some bad registry settings we may have saved
	// its also good for multiple monitors, in case wabbit was on a monitor that
	// no longer exists
	POINT startPoint = GetStartPoint();

	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)malloc(sizeof(MainWindow_t));
	ZeroMemory(lpMainWindow, sizeof(MainWindow_t));
	if (lpMainWindow == NULL) {
		return NULL;
	}

	if (!lpMainWindow->scale) {
		lpMainWindow->scale = 2;
	}

	if (lpMainWindow->skin_scale < DBL_EPSILON) {
		lpMainWindow->skin_scale = 1.0;
	}

	if (lpMainWindow->bSkinEnabled) {
		SetRect(&r, 0, 0, lpMainWindow->m_RectSkin.Width * lpMainWindow->scale, lpMainWindow->m_RectSkin.Height * lpMainWindow->scale);
	} else {
		SetRect(&r, 0, 0, 128 * lpMainWindow->scale, 64 * lpMainWindow->scale);
	}

	lpMainWindow->lpCalc = lpCalc;
	lpMainWindow->silent_mode = silent_mode;
	lpMainWindow->GIFAdd = 1;
	lpMainWindow->GIFGradientWidth = 1;
	lpMainWindow->dwDragCountdown = 0;
	lpMainWindow->hwndFrame = CreateWindowEx(
		WS_EX_APPWINDOW | (lpMainWindow->bCutout ? WS_EX_LAYERED : 0),
		g_szAppName,
		_T("Wabbitemu"),
		WS_TILEDWINDOW | WS_CLIPCHILDREN,
		startPoint.x, startPoint.y, r.right - r.left, r.bottom - r.top,
		NULL, 0, g_hInst, (LPVOID) lpMainWindow);

	if (lpMainWindow->hwndFrame == NULL) {
		return NULL;
	}

	lpCalc->speed = 100;
	lpMainWindow->hMenu = GetMenu(lpMainWindow->hwndFrame);
	CheckMenuRadioItem(GetSubMenu(lpMainWindow->hMenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_MAX, IDM_SPEED_NORMAL, MF_BYCOMMAND);
	return lpMainWindow;
}

/*
* Searches for a window with Wabbit's registered lcd class
*/
HWND find_existing_lcd(HWND hwndParent) 
{
	HWND FindChildhwnd = FindWindowEx(hwndParent, NULL, g_szLCDName, NULL);
	if (FindChildhwnd == NULL)
		FindChildhwnd = FindWindowEx(NULL, NULL, g_szLCDName, NULL);
	return FindChildhwnd;
}

void load_key_settings(LPCALC lpCalc, LPVOID) {
	keyprog_t *keys = lpCalc->model == TI_86 || lpCalc->model == TI_85 ? keysti86 : keysti83;
	memcpy(keygrps, keys, sizeof(keygrps));
}

void load_settings(LPCALC lpCalc, LPVOID lParam) {
	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)lParam;
	if (lpCalc->model < TI_84PCSE) {
		LCD_t *lcd = (LCD_t *) lpCalc->cpu.pio.lcd;
		lcd->shades = (u_int)QueryWabbitKey(_T("shades"));
		lcd->mode = (LCD_MODE)QueryWabbitKey(_T("lcd_mode"));
		lcd->steady_frame = 1.0 / QueryWabbitKey(_T("lcd_freq"));
		lcd->lcd_delay = (u_int)QueryWabbitKey(_T("lcd_delay"));
	}

	SendMessage(lpMainWindow->hwndFrame, WM_FRAME_UPDATE, 0, 0);
	if (lpMainWindow->hwndDebug != NULL) {
		SendMessage(lpMainWindow->hwndFrame, WM_CLOSE_DEBUGGER, 0, 0);
		SendMessage(lpMainWindow->hwndFrame, WM_OPEN_DEBUGGER, 0, 0);
	} else {
		lpCalc->running = TRUE;
	}
}

void sync_calc_clock(LPCALC lpCalc, LPVOID) {
	if (lpCalc->model < TI_84P) {
		return;
	}

	SE_AUX_t *se_aux = lpCalc->cpu.pio.se_aux;
	if (se_aux == NULL) {
		return;
	}

	time_t now;
	time_t result;
	struct tm now_tm;
	time(&now);
	localtime_s(&now_tm, &now);
	result = mktime(&now_tm);

	struct tm ti_epoch = { 0 };
	ti_epoch.tm_mday = 1;
	ti_epoch.tm_isdst = now_tm.tm_isdst;
	ti_epoch.tm_year = 97;
	result -= mktime(&ti_epoch);

	se_aux->clock.set = (unsigned long)result;
	se_aux->clock.base = (unsigned long)result;
}

void check_bootfree_and_update(LPCALC lpCalc, LPVOID) {
	if (lpCalc->model < TI_73) {
		return;
	}

	u_char *bootFreeString = lpCalc->mem_c.flash + (lpCalc->mem_c.flash_pages - 1) * PAGE_SIZE + 0x0F;
	if (*bootFreeString != '1') {
		//not using bootfree
		return;
	}
	if (bootFreeString[1] == '.') {
		//using normal bootpage
		return;
	}

	int majorVer, minorVer;
	sscanf_s((char *)bootFreeString, "%d.%d", &majorVer, &minorVer);
	if (MAKELONG(BOOTFREE_VER_MINOR, BOOTFREE_VER_MAJOR) > MAKELONG(minorVer, majorVer)) {
		TCHAR hexFile[MAX_PATH];
		ExtractBootFree(lpCalc->model, hexFile);
		FILE *file;
		_tfopen_s(&file, hexFile, _T("rb"));
		writeboot(file, &lpCalc->mem_c, -1);
		fclose(file);
		_tfopen_s(&file, lpCalc->rom_path, _T("wb"));
		if (file) {
			fclose(file);
			MFILE *mfile = ExportRom(lpCalc->rom_path, lpCalc);
			mclose(mfile);
		}
	}
}

void create_bcall_labels(LPCALC lpCalc, LPVOID lParam) {
	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)lParam;
	if (lpCalc == NULL || lpCalc->model < TI_83P || 
		lpMainWindow == NULL || !lpMainWindow->bTIOSDebug)
	{
		return;
	}

	VoidLabels(lpCalc);
	bcall_t *bcalls = get_bcalls(lpCalc->model);
	bcall_t *ptr = bcalls;
	label_struct *label = lpCalc->labels;
	while (label < (lpCalc->labels + sizeof(lpCalc->labels)) && label->name != NULL) {
		label++;
	}

	while (ptr != NULL && ptr->address != -1) {
		int page;
		if (ptr->address & BIT(15)) {
			// its on the boot page
			page = lpCalc->mem_c.flash_pages - 1;
		} else if (ptr->address & BIT(14)) {
			// its on the bcall page
			switch (lpCalc->model) {
			case TI_84PCSE:
				page = 5;
				break;
			default:
				page = lpCalc->mem_c.flash_pages - 5;
				break;
			}
		} else {
			continue;
		}

		uint16_t bcall_address = ptr->address & 0x3FFF;
		waddr_t waddr;
		waddr.page = (uint8_t)page;
		waddr.addr = bcall_address;
		waddr.is_ram = FALSE;
		
		uint16_t real_address = wmem_read16(&lpCalc->mem_c, waddr);
		waddr.addr += 2;
		uint8_t real_page = wmem_read(&lpCalc->mem_c, waddr);

		size_t buffer_size = (_tcslen(ptr->name) + 1) * sizeof(TCHAR);
		TCHAR *name = (TCHAR *)malloc(buffer_size);
		ZeroMemory(name, buffer_size);
		// exclude the _
		StringCbCopy(name, buffer_size, ptr->name + 1);
		label->name = name;
		label->addr = real_address;
		label->page = real_page;
		label->IsRAM = FALSE;

		label++;
		ptr++;
	}
}

void update_calc_running(LPCALC lpCalc, LPVOID lParam) {
	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)lParam;
	if (lpMainWindow == NULL) {
		return;
	}

	HMENU hMenu = lpMainWindow->hMenu;
	if (!hMenu) {
		return;
	}

	if (lpCalc->running) {
		CheckMenuItem(GetSubMenu(hMenu, 2), IDM_CALC_PAUSE, MF_BYCOMMAND | MF_UNCHECKED);
	} else {
		CheckMenuItem(GetSubMenu(hMenu, 2), IDM_CALC_PAUSE, MF_BYCOMMAND | MF_CHECKED);
	}
}

void fire_com_breakpoint(LPCALC lpCalc, LPVOID lParam) {
	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)lParam;
	if (lpMainWindow == NULL) {
		return;
	}

	if (lpMainWindow->pWabbitemu != NULL) {
		waddr_t addr = addr16_to_waddr(lpCalc->cpu.mem_c, lpCalc->cpu.pc);
		lpMainWindow->pWabbitemu->Fire_OnBreakpoint(&addr);
	}
}

LPMAINWINDOW create_calc_frame_register_events() {
	LPCALC lpCalc = calc_slot_new();
	if (lpCalc == NULL) {
		return NULL;
	}

	LPMAINWINDOW lpMainWindow = _Module.CreateNewFrame(lpCalc);
	if (lpMainWindow == NULL) {
		return NULL;
	}

	calc_register_event(lpCalc, LCD_ENQUEUE_EVENT, &gui_draw, lpMainWindow);
	calc_register_event(lpCalc, ROM_LOAD_EVENT, &load_settings, lpMainWindow);
	calc_register_event(lpCalc, ROM_LOAD_EVENT, &load_key_settings, NULL);
	calc_register_event(lpCalc, ROM_LOAD_EVENT, &check_bootfree_and_update, NULL);
	calc_register_event(lpCalc, ROM_LOAD_EVENT, &sync_calc_clock, NULL);
	calc_register_event(lpCalc, ROM_LOAD_EVENT, &create_bcall_labels, lpMainWindow);
	calc_register_event(lpCalc, ROM_RUNNING_EVENT, &update_calc_running, lpMainWindow);
	calc_register_event(lpCalc, BREAKPOINT_EVENT, &fire_com_breakpoint, lpMainWindow);
	calc_register_event(lpCalc, GIF_FRAME_EVENT, &handle_screenshot, NULL);
	calc_register_event(lpCalc, AVI_VIDEO_FRAME_EVENT, &handle_avi_video_frame, lpMainWindow);
	calc_register_event(lpCalc, AVI_AUDIO_FRAME_EVENT, &handle_avi_audio_frame, lpMainWindow);
	if (!_Module.GetParsedCmdArgs()->no_create_calc) {
		calc_register_event(lpCalc, BREAKPOINT_EVENT, &gui_debug, lpMainWindow);
	}
	
	LoadRegistrySettings(lpMainWindow, lpCalc);
	return lpMainWindow;
}

void destroy_calc_frame(LPMAINWINDOW lpMainWindow) {
	if (lpMainWindow == NULL) {
		return;
	}

	DestroyWindow(lpMainWindow->hwndFrame);
	_Module.DestroyFrame(lpMainWindow);
}

/*
* Checks based on the existence of the main window and the LCD window whether we need
* to spawn a new process
* returns false if there is no existing process
* returns true if there is an existing process found
*/
bool check_no_new_process(HWND Findhwnd) {
	if (Findhwnd == NULL) {
		return false;
	} else {
		return find_existing_lcd(Findhwnd) != NULL;
	}
}

extern HWND hwndProp;
extern RECT PropRect;
extern int PropPageLast;

void RegisterWindowClasses(void) {
	WNDCLASSEX wc;

	wc.cbSize = sizeof(wc);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hInst;
	wc.hIcon = LoadIcon(g_hInst, _T("A"));
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

	// Debugger
	wc.lpfnWndProc = DebugProc;
	wc.style = CS_DBLCLKS;
	wc.lpszClassName = g_szDebugName;
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_DEBUG_MENU);
	wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE+1);
	RegisterClassEx(&wc);

	// Disassembly
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

	// Expanding Panes
	wc.style = 0;
	wc.lpfnWndProc = ExpandPaneProc;
	wc.lpszClassName = g_szExpandPane;
	RegisterClassEx(&wc);

	// Memory Viewer
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = MemProc;
	wc.lpszClassName = g_szMemName;
	wc.hbrBackground = NULL;
	RegisterClassEx(&wc);

	// Watchpoints
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = WatchProc;
	wc.lpszClassName = g_szWatchName;
	wc.hbrBackground = NULL;
	RegisterClassEx(&wc);

	// Port Monitor
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = PortMonitorProc;
	wc.lpszClassName = g_szPortMonitor;
	wc.hbrBackground = NULL;
	RegisterClassEx(&wc);

	// Color LCD Monitor
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = ColorLCDMonitorProc;
	wc.lpszClassName = g_szLCDMonitor;
	wc.hbrBackground = NULL;
	RegisterClassEx(&wc);

	// Detached LCD
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = DetachedProc;
	wc.lpszClassName = g_szDetachedName;
	wc.hbrBackground = NULL;
	RegisterClassEx(&wc);

	// Teacher View
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = TeacherViewProc;
	wc.lpszClassName = g_szTeacherViewName;
	wc.hbrBackground = NULL;
	RegisterClassEx(&wc);

	// Small cutout buttons
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = SmallButtonProc;
	wc.lpszClassName = g_szSmallButtonsName;
	wc.hbrBackground = NULL;
	RegisterClassEx(&wc);
}

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
										 const PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
										 const PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
										 const PMINIDUMP_CALLBACK_INFORMATION CallbackParam
										 );

static BOOL hasCrashed = FALSE;
extern int def(FILE *, FILE *, int);

LONG WINAPI ExceptionFilter(_EXCEPTION_POINTERS *pExceptionInfo) {
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOALIGNMENTFAULTEXCEPT | SEM_NOGPFAULTERRORBOX);
	if (hasCrashed) {
		return EXCEPTION_EXECUTE_HANDLER;
	}

	hasCrashed = TRUE;
	TCHAR szDumpPath[MAX_PATH], szTempDumpPath[MAX_PATH];
	GetStorageString(szDumpPath, sizeof(szDumpPath));
	StringCbCopy(szTempDumpPath, sizeof(szTempDumpPath), szDumpPath);
	StringCbCat(szTempDumpPath, sizeof(szDumpPath), _T("Wabbitemu.dmp"));
	StringCbCat(szDumpPath, sizeof(szDumpPath), _T("Wabbitemu.dmp.zip"));
	HANDLE hFile = CreateFile(szTempDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL );

	if (hFile != INVALID_HANDLE_VALUE) {
		_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

		ExInfo.ThreadId = GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExceptionInfo;
		ExInfo.ClientPointers = NULL;

		_MINIDUMP_TYPE dumpType;

		VS_FIXEDFILEINFO thisFileInfo;
		UINT dwBytes;
		DWORD dwLen = GetFileVersionInfoSize(_T("DbgHelp.dll"), NULL);
		LPBYTE versionData = (LPBYTE) malloc(dwLen);
		GetFileVersionInfo(_T("DbgHelp.dll"), 0, dwLen, versionData);
		VerQueryValue(versionData, _T("\\"), (LPVOID *) &thisFileInfo, &dwBytes);
		if (HIWORD(thisFileInfo.dwFileVersionMS) == 6) {
			if (LOWORD(thisFileInfo.dwFileVersionMS) <= 1) {
				dumpType = (_MINIDUMP_TYPE) (MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithHandleData | MiniDumpWithDataSegs);
			} else {
				dumpType = (_MINIDUMP_TYPE) (MiniDumpWithFullMemoryInfo | MiniDumpWithDataSegs);
			}
		} else {
			dumpType = (_MINIDUMP_TYPE) (MiniDumpWithHandleData | MiniDumpScanMemory | MiniDumpWithDataSegs);
		}

		// write the dump
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, dumpType, &ExInfo, NULL, NULL);
		CloseHandle(hFile);
		FILE *dumpFile, *zipFile;
		_tfopen_s(&dumpFile, szTempDumpPath, _T("rb"));
		_tfopen_s(&zipFile, szDumpPath, _T("wb"));
		def(dumpFile, zipFile, 1);
		fclose(dumpFile);
		fclose(zipFile);

		if (MessageBox(NULL, _T("Unfortunately Wabbitemu has appeared to have crashed. Would you like to send a crash report to the developers so they can fix it?"),
			_T("Crash"), MB_YESNO) == IDYES) {

				DialogBox(g_hInst, MAKEINTRESOURCE(IDD_REPORT_BUG), NULL, (DLGPROC) BugReportDialogProc);

				HINTERNET hInternet = OpenFtpConnection();
				//Get the file friendly time string
				TCHAR timeStringText[MAX_PATH];
				time_t timeUploaded;
				time(&timeUploaded);
				TCHAR timeString[256];
				_tctime_s(timeString, sizeof(timeString), &timeUploaded);
				for (int i = _tcslen(timeString); i >= 0; i--) {
					if (timeString[i] == ':') {
						timeString[i] = '_';
					}
				}
				//get rid of newline
				timeString[_tcslen((timeString)) - 1] = '\0';
				StringCbCopy(timeStringText, sizeof(timeStringText), timeString);
				StringCbCat(timeStringText, sizeof(timeStringText), _T(".dmp.gz"));

				FtpPutFile(hInternet, szDumpPath, timeStringText, FTP_TRANSFER_TYPE_BINARY, NULL);
				InternetCloseHandle(hInternet);
		}
	}
	return EXCEPTION_EXECUTE_HANDLER;
}


bool CWabbitemuModule::ParseCommandLine(LPCTSTR lpszCommandLine, HRESULT *phres)
{
	ParseCommandLineArgs(&m_parsedArgs);
	return __super::ParseCommandLine(lpszCommandLine, phres);
}

LPMAINWINDOW CWabbitemuModule::CreateNewFrame(LPCALC lpCalc) {
	LPMAINWINDOW lpMainWindow = gui_frame(lpCalc);
	if (lpMainWindow == NULL) {
		MessageBox(NULL, _T("Failed to create main window"), _T("Error"), MB_OK | MB_ICONERROR);
		return NULL;
	}

	lpMainWindow->keys_pressed = new list<key_string_t>;
	m_lpMainWindows.push_back(lpMainWindow);
	return lpMainWindow;
}

void CWabbitemuModule::DestroyFrame(LPMAINWINDOW lpMainWindow) {
	for (auto it = m_lpMainWindows.begin(); it != m_lpMainWindows.end(); it++) {
		if (*it == lpMainWindow) {
			delete lpMainWindow->keys_pressed;
			m_lpMainWindows.erase(it);
			break;
		}
	}
}

HRESULT CWabbitemuModule::PreMessageLoop(int nShowCmd)
{
	HRESULT hr =  __super::PreMessageLoop(nShowCmd);

	g_hInst = _AtlBaseModule.GetModuleInstance();
	RegisterWindowClasses();
	InitCommonControls();
	OleInitialize(NULL);

	// Initialize GDI+.
	GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::Status gdiStatus = GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	if (gdiStatus != Gdiplus::Ok) {
		MessageBox(NULL , _T("Unable to start GDI+"), _T("Error"), MB_OK);
		return E_FAIL;
	}

	if (m_parsedArgs.no_create_calc) {
		return hr; 
	}

	bool alreadyRunningWabbit = false;

	//Create our appdata folder
	TCHAR appData[MAX_PATH];
	GetStorageString(appData, sizeof(appData));
	int error = CreateDirectory(appData, NULL);
	if (!error) {
		error = GetLastError();
		if (error != ERROR_ALREADY_EXISTS) {
			MessageBox(NULL , _T("Unable to create appdata folder"), _T("Error"), MB_OK);
			return E_FAIL;
		}
	}

#ifndef _DEBUG
	SetUnhandledExceptionFilter(ExceptionFilter);
#endif

	CheckSetIsPortableMode();

	//this is here so we get our load_files_first setting
	new_calc_on_load_files = QueryWabbitKey(_T("load_files_first")) || m_parsedArgs.force_new_instance;

	HWND alreadyRunningHwnd = NULL;
	alreadyRunningHwnd = FindWindow(g_szAppName, NULL);
	alreadyRunningWabbit = check_no_new_process(alreadyRunningHwnd);
	// If there is a setting to load files into a new calc each time and there is a calc already running
	// ask it to create a new core to load into
	if (new_calc_on_load_files && alreadyRunningHwnd) {
		HWND tempHwnd;
		SendMessage(alreadyRunningHwnd, WM_COMMAND, IDM_FILE_NEW, 0);
		for (int i = MAX_CALCS; i > 0; i--) {
			tempHwnd = FindWindow(g_szAppName, NULL);
			if (tempHwnd != alreadyRunningHwnd) {
				break;
			}
		}
		alreadyRunningHwnd = tempHwnd;
	}

	if (alreadyRunningWabbit) {
		LoadCommandlineFiles(&m_parsedArgs, (LPARAM) find_existing_lcd(alreadyRunningHwnd), LoadAlreadyExistingWabbit);
		if (m_parsedArgs.force_focus) {
			SwitchToThisWindow(alreadyRunningHwnd, TRUE);
		}

		exit(0);
	}

	silent_mode = m_parsedArgs.silent_mode;

	LPMAINWINDOW lpMainWindow = NULL;
	LPCALC lpCalc = NULL;

	BOOL loadedRom = FALSE;
	// ROMs are special, we need to load them first before anything else
	if (m_parsedArgs.num_rom_files > 0) {
		lpMainWindow = create_calc_frame_register_events();
		if (lpMainWindow == NULL) {
			return E_UNEXPECTED;
		}

		m_lpMainWindows.push_back(lpMainWindow);

		lpCalc = lpMainWindow->lpCalc;
		if (lpCalc == NULL) {
			return E_UNEXPECTED;
		}

		for (int i = 0; i < m_parsedArgs.num_rom_files; i++) {
			// if you specify more than one rom file to be loaded, only the first is loaded
			if (rom_load(lpCalc, m_parsedArgs.rom_files[i])) {
				loadedRom = TRUE;
				break;
			}
		}
	}


	TCHAR rom_path[MAX_PATH];
	StringCbCopy(rom_path, sizeof(rom_path), (TCHAR *)QueryWabbitKey(_T("rom_path")));
	if (!loadedRom) {
		lpMainWindow = create_calc_frame_register_events();
		if (lpMainWindow == NULL) {
			return E_UNEXPECTED;
		}

		lpCalc = lpMainWindow->lpCalc;
		if (lpCalc == NULL) {
			return E_UNEXPECTED;
		}

		if (rom_load(lpCalc, lpCalc->rom_path) == FALSE) {
			destroy_calc_frame(lpMainWindow);

			if (!loadedRom) {
				calc_slot_free(lpCalc);

				// if we lost our ROM we don't know what the default skin scale was
				// so we need to clear our old skin scale
				lpMainWindow->skin_scale = 0.0;
				lpMainWindow = DoWizardSheet(NULL);
				// save wizard show
				SaveWabbitKey(_T("rom_path"), REG_SZ, &lpCalc->rom_path);
				if (lpMainWindow == NULL) {
					// this is likely caused by a user cancel
					return E_ABORT;
				}
			}
		}
	}

	//Check for any updates
	if (check_updates) {
		CreateThread(NULL, 0, CheckForUpdates, lpMainWindow->hwndFrame, NULL, NULL);
	}
	if (show_whats_new) {
		ShowWhatsNew(FALSE);
	}

	LoadCommandlineFiles(&m_parsedArgs, (LPARAM) lpMainWindow, LoadToLPCALC);

	// Set the one global timer for all calcs
	SetTimer(NULL, 0, TPF, TimerProc);

	hacceldebug = LoadAccelerators(g_hInst, _T("DisasmAccel"));
	if (!haccelmain) {
		haccelmain = LoadAccelerators(g_hInst, _T("Z80Accel"));
	}

	return hr;
}

HWND CWabbitemuModule::CheckValidFrameHandle(HWND hwndToCheck) {
	for (auto it = m_lpMainWindows.begin(); it != m_lpMainWindows.end(); it++) {
		LPMAINWINDOW lpMainWindow = *it;
		if (lpMainWindow->hwndFrame == hwndToCheck || lpMainWindow->hwndLCD == hwndToCheck) {
			return lpMainWindow->hwndLCD;
		}
	}

	return NULL;
}

HWND CWabbitemuModule::CheckValidOtherHandle(HWND hwndToCheck) {
	for (auto it = m_lpMainWindows.begin(); it != m_lpMainWindows.end(); it++) {
		LPMAINWINDOW lpMainWindow = *it;
		// we only need to worry about this case if the cutout skin is enabled
		if (lpMainWindow->bCutout && lpMainWindow->bSkinEnabled) {
			continue;
		}

		if (lpMainWindow->hwndSmallClose == hwndToCheck || lpMainWindow->hwndSmallMinimize == hwndToCheck) {
			return lpMainWindow->hwndLCD;
		}
	}

	return NULL;
}

HWND CWabbitemuModule::CheckValidDebugHandle(HWND hwndToCheck) {
	for (auto it = m_lpMainWindows.begin(); it != m_lpMainWindows.end(); it++) {
		LPMAINWINDOW lpMainWindow = *it;
		if (lpMainWindow->hwndDebug == hwndToCheck) {
			return lpMainWindow->hwndLCD;
		}
	}
	
	return NULL;
}

void CWabbitemuModule::SetGIFState(gif_disp_state state) {
	for (auto it = m_lpMainWindows.begin(); it != m_lpMainWindows.end(); it++) {
		LPMAINWINDOW lpMainWindow = *it;
		if (lpMainWindow != NULL && lpMainWindow->lpCalc->active) {
			lpMainWindow->gif_disp_state = state;
		}
	}
}

void CWabbitemuModule::RunMessageLoop()
{
	MSG Msg;
	while (GetMessage(&Msg, NULL, 0, 0)) {
		HACCEL haccel = haccelmain;
		HWND hwndtop = GetForegroundWindow();
		if (hwndtop != NULL) {
			if (CheckValidDebugHandle(hwndtop)) {
				haccel = hacceldebug;
			} else {
				HWND newhwndtop = CheckValidFrameHandle(hwndtop);
				if (newhwndtop != NULL) {
					haccel = haccelmain;
					SetForegroundWindow(hwndtop);
					hwndtop = newhwndtop;
				} else {
					newhwndtop = CheckValidOtherHandle(hwndtop);
					if (newhwndtop != NULL) {
						haccel = haccelmain;
						hwndtop = newhwndtop;
					} else {
						haccel = NULL;
					}
				}
			}
		}

		if (hwndProp != NULL) {
			HWND propPage = PropSheet_GetCurrentPageHwnd(hwndProp);
			if (propPage == NULL) {
				GetWindowRect(hwndProp, &PropRect);
				DestroyWindow(hwndProp);
				hwndProp = NULL;
			}
		}

		if (Msg.hwnd != NULL && Msg.hwnd == hwndProp) {
			// Get the current tab
			HWND hwndPropTabCtrl = PropSheet_GetTabControl(hwndProp);
			PropPageLast = TabCtrl_GetCurSel(hwndPropTabCtrl);

			if (Msg.message == WM_KEYDOWN) {
				OutputDebugString(_T("Hard key down man"));
			}

			if (PropSheet_IsDialogMessage(hwndProp, &Msg) == TRUE) {
				continue;
			}
		}

		if (!TranslateAccelerator(hwndtop, haccel, &Msg) && 
			(hwndCurrentDlg == NULL || !IsDialogMessage(hwndCurrentDlg, &Msg)))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}

}

HRESULT CWabbitemuModule::PostMessageLoop() {
	// Make sure the GIF has terminated
	if (gif_write_state == GIF_FRAME) {
		gif_write_state = GIF_END;
		handle_screenshot(NULL, NULL);
	}

	// Shutdown GDI+
	GdiplusShutdown(m_gdiplusToken);

	return __super::PostMessageLoop();
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int nCmdShow)
{
	// SetProcessDPIAware();
	return _Module.WinMain(nCmdShow);
}

void press_key(LPCALC lpCalc, int group, int bit) {
	lpCalc->fake_running = TRUE;
	keypad_press(&lpCalc->cpu, group, bit);
	calc_run_tstates(lpCalc, lpCalc->cpu.timer_c->freq / 8);
	keypad_release(&lpCalc->cpu, group, bit);
	calc_run_tstates(lpCalc, lpCalc->cpu.timer_c->freq / 8);
	lpCalc->fake_running = FALSE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	LPCALC lpCalc = NULL;
	if (lpMainWindow != NULL) {
		lpCalc = lpMainWindow->lpCalc;
	}

	switch (Message) {
	case WM_CREATE: {
		lpMainWindow = (LPMAINWINDOW)((LPCREATESTRUCT)lParam)->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) lpMainWindow);
		return 0;
					}
	case WM_FRAME_UPDATE: {
		gui_frame_update(lpMainWindow);
		break;
	}
	case WM_AVI_AUDIO_FRAME: {
		avi_audio_frame(lpMainWindow);
		break;
	}
	case WM_OPEN_DEBUGGER: {
		gui_debug(lpMainWindow->lpCalc, lpMainWindow);
		break;
	}
	case WM_CLOSE_DEBUGGER: {
		DestroyWindow(lpMainWindow->hwndDebug);
		lpMainWindow->hwndDebug = NULL;
		break;
	}
	case WM_PAINT: {
		if (gif_anim_advance) {
			switch (lpMainWindow->gif_disp_state) {
			case GDS_STARTING:
				if (lpMainWindow->GIFGradientWidth > 15) {
					lpMainWindow->gif_disp_state = GDS_RECORDING;
					lpMainWindow->GIFAdd = -1;
				} else {
					lpMainWindow->GIFGradientWidth++;
				}
				break;
			case GDS_RECORDING:
				lpMainWindow->GIFGradientWidth += lpMainWindow->GIFAdd;
				if (lpMainWindow->GIFGradientWidth > GIFGRAD_PEAK) {
					lpMainWindow->GIFAdd = -1;
				} else if (lpMainWindow->GIFGradientWidth < GIFGRAD_TROUGH) {
					lpMainWindow->GIFAdd = 1;
				}
				break;
			case GDS_ENDING:
				if (lpMainWindow->GIFGradientWidth) {
					lpMainWindow->GIFGradientWidth--;
				} else {
					lpMainWindow->gif_disp_state = GDS_IDLE;							
					gui_frame_update(lpMainWindow);
				}						
				break;
			case GDS_IDLE:
				break;
			}
			gif_anim_advance = FALSE;
		}

		if (lpMainWindow->gif_disp_state != GDS_IDLE) {
			RECT screen;
			GetWindowRect(lpMainWindow->hwndLCD, &screen);
			MapWindowRect(NULL, lpMainWindow->hwndFrame, &screen);

			int grayred = (int)(((double)lpMainWindow->GIFGradientWidth / GIFGRAD_PEAK) * 50);
			HDC hWindow = GetDC(hwnd);
			HDC hdcSkin = CreateCompatibleDC(hWindow);
			HBITMAP hbm = CreateCompatibleBitmap(hWindow, 
				lpMainWindow->m_RectSkin.Width, lpMainWindow->m_RectSkin.Height);
			SelectObject(hdcSkin, hbm);
			Graphics g(hdcSkin);
			g.DrawImage(lpMainWindow->m_lpBitmapRenderedSkin, lpMainWindow->m_RectSkin);
			DrawGlow(hdcSkin, hWindow, &screen, RGB(127 - grayred, 127 - grayred, 127 + grayred),
				lpMainWindow->GIFGradientWidth, lpMainWindow->bSkinEnabled, 1.0 / lpMainWindow->skin_scale);
			ReleaseDC(hwnd, hWindow);
			InflateRect(&screen, lpMainWindow->GIFGradientWidth, lpMainWindow->GIFGradientWidth);
			ValidateRect(hwnd, &screen);

			DeleteObject(hbm);
			DeleteDC(hdcSkin);
			ReleaseDC(hwnd, hWindow);
		}

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		if (lpMainWindow->bSkinEnabled) {
			if (lpMainWindow->bCutout) {
				// still pretty resource intensive
				// UpdateWabbitemuLayeredWindow(lpMainWindow);
			} else {
				LONG updateWidth = ps.rcPaint.right - ps.rcPaint.left;
				LONG updateHeight = ps.rcPaint.bottom - ps.rcPaint.top;

				double skin_scale = lpMainWindow->skin_scale;
				Rect updateRect(ps.rcPaint.left, ps.rcPaint.top, updateWidth, updateHeight);
				Graphics g(hdc);
				g.SetInterpolationMode(InterpolationModeLowQuality);
				g.DrawImage(lpMainWindow->m_lpBitmapRenderedSkin,
					updateRect,
					(INT) (ps.rcPaint.left / skin_scale),
					(INT) (ps.rcPaint.top / skin_scale),
					(INT) (updateWidth / skin_scale), 
					(INT) (updateHeight / skin_scale),
					UnitPixel);
			}

			delete lpMainWindow->m_lpBitmapRenderedSkin;
			lpMainWindow->m_lpBitmapRenderedSkin = lpMainWindow->m_lpBitmapSkin
				->Clone(lpMainWindow->m_RectSkin, PixelFormat32bppARGB);
		} else {
			FillRect(hdc, &ps.rcPaint, GetStockBrush(GRAY_BRUSH));
		}

		ReleaseDC(hwnd, hdc);
		EndPaint(hwnd, &ps);

		return 0;
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case IDM_FILE_NEW: {
			LPMAINWINDOW lpNewWindow = create_calc_frame_register_events();
			if (lpNewWindow == NULL || lpNewWindow->lpCalc == NULL) {
				MessageBox(hwnd, _T("Unable to open any more calculators"), _T("Error"), MB_OK | MB_ICONERROR);
				return TRUE;
			}

			LPCALC lpCalcNew = lpNewWindow->lpCalc;
			if (rom_load(lpCalcNew, lpCalc->rom_path) || 
				rom_load(lpCalcNew, (LPCTSTR) QueryWabbitKey(_T("rom_path"))))
			{
				lpNewWindow->bSkinEnabled = lpMainWindow->bSkinEnabled;
				lpNewWindow->bCutout = lpMainWindow->bCutout;
				lpNewWindow->bCustomSkin = lpMainWindow->bCustomSkin;
				lpNewWindow->scale = lpMainWindow->scale;
				lpNewWindow->skin_scale = lpMainWindow->skin_scale;
				lpNewWindow->default_skin_scale = lpMainWindow->default_skin_scale;
				lpNewWindow->m_FaceplateColor = lpMainWindow->m_FaceplateColor;
				lpNewWindow->bAlphaBlendLCD = lpMainWindow->bAlphaBlendLCD;
				gui_frame_update(lpNewWindow);

				if (lpCalcNew->model < TI_84PCSE) {
					LCD_t *lcd = (LCD_t *) lpCalcNew->cpu.pio.lcd;
					lcd->shades = ((LCD_t *)lpCalc->cpu.pio.lcd)->shades;
				}

				RECT rc;
				GetWindowRect(hwnd, &rc);
				RECT newrc;
				GetWindowRect(lpMainWindow->hwndFrame, &newrc);
				SetWindowPos(lpNewWindow->hwndFrame, NULL, newrc.left + rc.right - rc.left, newrc.top, 0, 0,
					SWP_NOSIZE | SWP_NOZORDER);
			} else {
				calc_slot_free(lpCalcNew);
				SendMessage(hwnd, WM_COMMAND, IDM_HELP_WIZARD, 0);
			}
			break;
						   }
		case IDM_FILE_OPEN: {
			GetOpenSendFileName(hwnd, lpCalc);
			SetWindowText(hwnd, _T("Wabbitemu"));
			break;
		}
		case IDM_FILE_SAVE: {
			TCHAR FileName[MAX_PATH];
			const TCHAR lpstrFilter[] = _T("Known File types ( *.sav; *.rom; *.bin) \0*.sav;*.rom;*.bin\0\
										   Save States  (*.sav)\0*.sav\0\
										   ROMS  (*.rom; .bin)\0*.rom;*.bin\0\
										   OSes (*.8xu)\0*.8xu\0\
										   All Files (*.*)\0*.*\0\0");
			ZeroMemory(FileName, MAX_PATH);
			BOOL running = lpCalc->running;
			lpCalc->running = FALSE;
			if (!SaveFile(FileName, (TCHAR *) lpstrFilter, _T("Wabbitemu Save State"), _T("sav"), OFN_PATHMUSTEXIST, 0)) {
				TCHAR extension[5] = _T("");
				const TCHAR *pext = _tcsrchr(FileName, _T('.'));
				if (pext != NULL) {
					StringCbCopy(extension, sizeof(extension), pext);
				}
				
				if (!_tcsicmp(extension, _T(".rom")) || !_tcsicmp(extension, _T(".bin"))) {
					MFILE *file = ExportRom(FileName, lpCalc);
					if (file == NULL) {
						MessageBox(hwnd, _T("There was an error writing to the file"), _T("Error"), MB_OK | MB_ICONERROR);
					}
					StringCbCopy(lpCalc->rom_path, sizeof(lpCalc->rom_path), FileName);
					mclose(file);
				} else if (!_tcsicmp(extension, _T(".8xu"))) {
					HWND hExportOS = CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_EXPORT_OS), hwnd, 
						(DLGPROC) ExportOSDialogProc, (LPARAM) FileName);
					ShowWindow(hExportOS, SW_SHOW);
				} else {
					gui_savestate(hwnd, FileName, lpCalc);
				}
			}
			lpCalc->running = running;
			break;
							}
		case IDM_FILE_GIF: {
			HMENU hmenu = lpMainWindow->hMenu;
			static BOOL calcBackupRunning[MAX_CALCS];
			if (gif_write_state == GIF_IDLE) {
				int i;
				for (i = 0; i < MAX_CALCS; i++) {
					if (calcs[i].active == TRUE) {
						calcBackupRunning[i] = calcs[i].running;
						calcs[i].running = FALSE;
					}
				}
				BOOL start_screenshot = get_screenshot_filename(gifext);
				FILE *file;
				_tfopen_s(&file, screenshot_file_name, _T("wb"));
				if (!file && start_screenshot) { 
					MessageBox(hwnd, _T("Invalid file name."), _T("Error"), MB_OK);
					start_screenshot = FALSE;
				} else if (file) {
					fclose(file);
				}

				if (!start_screenshot) {
					int i;
					for (i = 0; i < MAX_CALCS; i++) {
						if (calcs[i].active == TRUE) {
							calcs[i].running = calcBackupRunning[i];
						}
					}
					break;
				}

				_Module.SetGIFState(GDS_STARTING);
				gif_write_state = GIF_START;
				CheckMenuItem(hmenu, IDM_FILE_GIF, MF_BYCOMMAND | MF_CHECKED);
				lpCalc->running = TRUE;
			} else {
				gif_write_state = GIF_END;
				_Module.SetGIFState(GDS_ENDING);
				CheckMenuItem(hmenu, IDM_FILE_GIF, MF_BYCOMMAND | MF_UNCHECKED);
			}
			break;
						   }
		case IDM_FILE_STILLGIF: {
			lpCalc->running = FALSE;
			BOOL start_screenshot = get_screenshot_filename(pngext);
			if (start_screenshot) {
				export_png(lpCalc, screenshot_file_name);
			}
			lpCalc->running = TRUE;
			break;
		}
		case IDM_FILE_AVI: {
			HMENU hmenu = lpMainWindow->hMenu;
			if (lpMainWindow->m_IsRecording) {
				lpMainWindow->m_IsRecording = FALSE;
				delete lpMainWindow->m_CurrentAvi;
				lpMainWindow->m_CurrentAvi = NULL;
				CheckMenuItem(GetSubMenu(hmenu, MENU_FILE), IDM_FILE_AVI, MF_BYCOMMAND | MF_UNCHECKED);
			} else {
				lpCalc->running = FALSE;
				TCHAR lpszFile[MAX_PATH];
				if (!SaveFile(lpszFile, _T("AVIs (*.avi)\0*.avi\0All Files (*.*)\0*.*\0\0"),
					_T("Wabbitemu Export AVI"), _T("avi"), OFN_PATHMUSTEXIST, 0)) {

					LPBITMAPINFO info = GetLCDColorPalette(lpCalc->model, lpCalc->cpu.pio.lcd);
					COMPVARS compVar = { 0 };
					compVar.cbSize = sizeof(COMPVARS);

					if (ICCompressorChoose(hwnd, 0, info, NULL, &compVar, NULL)) {
						lpMainWindow->m_CurrentAvi = new CAviFile(lpszFile, compVar.fccHandler, AVI_FPS, compVar.lQ);
						lpMainWindow->m_IsRecording = TRUE;
						CheckMenuItem(GetSubMenu(hmenu, MENU_FILE), IDM_FILE_AVI, MF_BYCOMMAND | MF_CHECKED);
					}
				}
				lpCalc->running = TRUE;
			}
			break;
		}
		case IDM_FILE_CLOSE:
			return SendMessage(hwnd, WM_CLOSE, 0, 0);
		case IDM_FILE_EXIT:
			if (calc_count() > 1) {
				TCHAR buf[256];
				StringCbPrintf(buf, sizeof(buf), _T("If you exit now, %d other running calculator(s) will be closed. \
													Are you sure you want to exit?"), calc_count() - 1);
				int res = MessageBox(NULL, buf, _T("Wabbitemu"), MB_YESNO);
				if (res == IDCANCEL || res == IDNO) {
					break;
				}
				is_exiting = TRUE;
			} else {
				SendMessage(hwnd, WM_CLOSE, 0, 0);
			}
			PostQuitMessage(0);
			break;
		case IDM_CALC_COPY: {
			HLOCAL ans;
			TCHAR *buffer = (TCHAR *)LocalAlloc(LMEM_FIXED, 2048);
			ans = (HLOCAL) GetRealAns(&lpCalc->cpu, buffer);
			BOOL success = OpenClipboard(hwnd);
			if (!success) {
				MessageBox(hwnd, _T("Unable to open clipboard"), _T("Error"), MB_OK);
				break;
			}

			success = EmptyClipboard();
			if (!success) {
				MessageBox(hwnd, _T("Unable to empty clipboard"), _T("Error"), MB_OK);
				break;
			}

			HANDLE hData = SetClipboardData(CF_TEXT, ans);
			if (hData == NULL) {
				MessageBox(hwnd, _T("Unable to set clipboard data"), _T("Error"), MB_OK);
				break;
			}

			success = CloseClipboard();
			if (!success) {
				MessageBox(hwnd, _T("Unable to open clipboard"), _T("Error"), MB_OK);
				break;
			}
			break;
							}
		case IDM_CALC_PASTE: {
			OpenClipboard(hwnd);
			HANDLE hClipboardData = GetClipboardData(CF_TEXT);
			if (!hClipboardData) {
				break;
			}

			char *charData = (char *) GlobalLock(hClipboardData);

			for (u_int i = 0; i < strlen(charData); i++) {
				char num = charData[i];

#define CALC_FLAGS 0x89F0
#define SHIFT_FLAGS CALC_FLAGS + 18
#define LOWERCASE_FLAGS  CALC_FLAGS + 36

#define LWR_CASE_ACTIVE (1 << 3)

#define SHIFT_2ND (1 << 3)
#define SHIFT_ALPHA (1 << 4)
#define SHIFT_LWR_ALPHA (1 << 5)
#define SHIFT_KEEP_ALPHA (1 << 7)

				// we use tolower here so we just get the key not the modifier
				BOOL press_shift = FALSE;
				BOOL press_alpha = !isdigit(num);
				int group = 0, bit = 0;
				keyprog_t *keys = lpCalc->model == TI_86 || lpCalc->model == TI_85 ? keysti86 : keysti83;
				for (int i = 0; i < MAX_KEY_MAPPINGS; i++) {
					if (keys[i].vk == (UINT) toupper(num)) {
						group = keys[i].group;
						bit = keys[i].bit;
						break;
					}
				}

				if (num == '.') {
					press_alpha = FALSE;
					group = 3;
					bit = 0;
				} else if (num == '-') {
					press_alpha = FALSE;
					group = 1;
					bit = 2;
				}

				if (group == 0 && bit == 0) {
					switch (num) {
					case ']':
						press_shift = TRUE;
						group = 1;
						bit = 2;
						break;
					case '[':
						press_shift = TRUE;
						group = 1;
						bit = 3;
						break;
					case '{':
						press_shift = TRUE;
						group = 3;
						bit = 4;
						break;
					case '(':
						press_alpha = FALSE;
						group = 3;
						bit = 4;
						break;
					case '}':
						press_shift = TRUE;
						group = 2;
						bit = 4;
						break;
					case ')':
						press_alpha = FALSE;
						group = 2;
						bit = 4;
						break;
					case ',':
						press_alpha = FALSE;
						group = 4;
						bit = 4;
						break;
					case ':':
						group = 3;
						bit = 0;
						break;
					case '?':
						group = 2;
						bit = 0;
						break;
					case '^':
						press_alpha = FALSE;
						group = 1;
						bit = 5;
						break;
					case '/':
						press_alpha = FALSE;
						group = 1;
						bit = 4;
						break;
					case '*':
						press_alpha = FALSE;
						group = 1;
						bit = 3;
						break;
					case '+':
						press_alpha = FALSE;
						group = 1;
						bit = 1;
						break;
					}
				}

				if (group == 0 && bit == 0) {
					continue;
				}

				BYTE shiftFlags = mem_read(&lpCalc->mem_c, SHIFT_FLAGS);
				if (shiftFlags & SHIFT_2ND) {
					press_key(lpCalc, 6, 5);
				}

				BOOL isLowercaseEnabled = mem_read(&lpCalc->mem_c, LOWERCASE_FLAGS) & LWR_CASE_ACTIVE;
				if (press_shift) {
					press_key(lpCalc, 6, 5);
				} else if (press_alpha) {
					if (!(shiftFlags & SHIFT_ALPHA)) {
						press_key(lpCalc, 5, 7);
					}

					if (IsCharLower(num) && isLowercaseEnabled) {
						press_key(lpCalc, 5, 7);
					}
				} else if (shiftFlags & SHIFT_ALPHA) {
					press_key(lpCalc, 5, 7);
					press_key(lpCalc, 5, 7);
					// if lowercase is enabled we have to press alpha again
					if (isLowercaseEnabled) {
						press_key(lpCalc, 5, 7);
					}
				}

				press_key(lpCalc, group, bit);
				// rather than put this in a thread and create who know what for
				// race conditions, just force and lcd update. This should be plenty of
				// user feedback, although it will be slower
				gui_draw(lpCalc, lpMainWindow);
			}
			GlobalUnlock(hClipboardData);
			CloseClipboard();

			break;
							 }
		case IDM_VIEW_SKIN: {
			lpMainWindow->bSkinEnabled = !lpMainWindow->bSkinEnabled;
			gui_frame_update(lpMainWindow);
			break;
							}
		case IDM_VIEW_LCD: {
			if (lpMainWindow->hwndDetachedLCD != NULL || lpMainWindow->hwndDetachedFrame != NULL) {
				break;
			}

			RECT r;
			int lcdWidth = lpCalc->cpu.pio.lcd->display_width;
			int lcdHeight = lpCalc->cpu.pio.lcd->height;
			SetRect(&r, 0, 0, lcdWidth * lpMainWindow->scale, lcdHeight * lpMainWindow->scale);
			AdjustWindowRect(&r, WS_CAPTION | WS_TILEDWINDOW, FALSE);

			POINT startPoint = GetStartPoint();
			lpMainWindow->hwndDetachedFrame  = CreateWindowEx(
				0,
				g_szDetachedName,
				_T("LCD"),
				(WS_TILEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN) & ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX),
				startPoint.x, startPoint.y, r.right - r.left, r.bottom - r.top,
				NULL, 0, g_hInst, (LPVOID) lpMainWindow);

			if (lpMainWindow->hwndDetachedFrame == NULL) {
				return -1;
			}

			break;
		 }
		case IDM_VIEW_TEACHERVIEW: {
			if (lpMainWindow->hwndTeacherView != NULL) {
				break;
			}

			if (lpMainWindow->teacher_view_init == FALSE) {
				lpMainWindow->teacher_views[0][1] = WINDOW_SCREEN;
				lpMainWindow->teacher_views[1][0] = GRAPH_SCREEN;
				lpMainWindow->teacher_views[1][1] = TABLE_SCREEN;
				lpMainWindow->teacher_view_init = TRUE;
			}

			int dpi = GetDpiForWindow(hwnd);
			RECT r;
			int lcdWidth = lpCalc->cpu.pio.lcd->display_width;
			int lcdHeight = lpCalc->cpu.pio.lcd->height;
			int scale = lpCalc->model < TI_84PCSE ? TEACHER_VIEW_SCALE : 1;
			SetRect(&r, 0, 0,
				MulDiv((lcdWidth* scale* TEACHER_VIEW_COLS), dpi, 96),
				MulDiv((lcdHeight* scale* TEACHER_VIEW_ROWS) +
					(TEACHER_VIEW_CAPTION_SIZE * (TEACHER_VIEW_ROWS)), dpi, 96));
			AdjustWindowRect(&r, WS_CAPTION | WS_SYSMENU, FALSE);

			POINT startPoint = GetStartPoint();
			lpMainWindow->hwndTeacherView = CreateWindowEx(
				0,
				g_szTeacherViewName,
				_T("Teacher View"),
				(WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_CLIPCHILDREN) & ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX),
				startPoint.x, startPoint.y, r.right - r.left, r.bottom - r.top,
				NULL, 0, g_hInst, (LPVOID)lpMainWindow);

			break;
		}
		case IDM_CALC_SOUND: {
			togglesound(lpCalc->audio);
			CheckMenuItem(GetSubMenu(GetMenu(hwnd), 2), IDM_CALC_SOUND, MF_BYCOMMAND | (lpCalc->audio->enabled ? MF_CHECKED : MF_UNCHECKED));
			break;
							 }
		case ID_DEBUG_TURNONCALC:
			{
				calc_turn_on(lpCalc);
				break;
			}
		case IDM_CALC_CONNECT: {
			/*if (!calcs[0].active || !calcs[1].active || link_connect(&calcs[0].cpu, &calcs[1].cpu))						
			MessageBox(NULL, _T("Connection Failed"), _T("Error"), MB_OK);					
			else*/
			link_connect_hub(lpCalc->slot, &lpCalc->cpu);
			TCHAR buf[64];
			StringCbCopy(buf, sizeof(buf), calc_get_model_string(lpCalc->model));
			StringCbCat(buf, sizeof(buf), _T(" Connected"));
			SendMessage(lpMainWindow->hwndStatusBar, SB_SETTEXT, 1, (LPARAM)buf);
			StringCbPrintf(buf, sizeof(buf), _T("Wabbitemu (%d)"), lpCalc->slot + 1);
			SetWindowText(hwnd, buf);			
			break;
							   }
		case IDM_CALC_PAUSE: {
			if (lpCalc->running) {
				calc_set_running(lpCalc, FALSE);
			} else {
				calc_set_running(lpCalc, TRUE);
			}
			break;
							 }
		case IDM_VIEW_VARIABLES:
			CreateVarTreeList(hwnd, lpMainWindow);
			break;
		case IDM_VIEW_KEYSPRESSED:
			if (IsWindow(lpMainWindow->hwndKeyListDialog)) {
				SwitchToThisWindow(lpMainWindow->hwndKeyListDialog, TRUE);
			} else {
				lpMainWindow->hwndKeyListDialog = (HWND)CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_KEYS_LIST),
					hwnd, (DLGPROC) KeysListProc, (LPARAM) lpMainWindow);
				ShowWindow(lpMainWindow->hwndKeyListDialog, SW_SHOW);
			}
			break;
		case IDM_CALC_OPTIONS:
			DoPropertySheet(hwnd, lpMainWindow);
			break;
		case IDM_DEBUG_RESET: {
			calc_reset(lpCalc);
			break;
							  }
		case IDM_DEBUG_OPEN:
			gui_debug_hwnd(lpMainWindow);
			break;
		case IDM_HELP_ABOUT:
			lpCalc->running = FALSE;
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DLGABOUT), hwnd, (DLGPROC) AboutDialogProc);					
			lpCalc->running = TRUE;
			break;
		case IDM_HELP_WHATSNEW:
			ShowWhatsNew(TRUE);
			break;
		case IDM_HELP_WIZARD: {
			LPMAINWINDOW lpNewWindow = DoWizardSheet(hwnd);
			if (lpNewWindow != NULL) {
				SetFocus(lpNewWindow->hwndFrame);
				RECT rc;
				GetWindowRect(hwnd, &rc);
				RECT newrc;
				GetWindowRect(lpMainWindow->hwndFrame, &newrc);
				SetWindowPos(lpNewWindow->hwndFrame, NULL, newrc.left + rc.right - rc.left, newrc.top, 0, 0,
					SWP_NOSIZE | SWP_NOZORDER | (silent_mode ? SWP_HIDEWINDOW : SWP_SHOWWINDOW));
			}
			break;
							  }
		case IDM_HELP_WEBSITE:					
			ShellExecute(NULL, _T("open"), g_szWebPage, NULL, NULL, SW_SHOWNORMAL);
			break;
		case IDM_FRAME_BTOGGLE: {
			POINT ctxtPt = lpMainWindow->ctxtPt;
			SendMessage(hwnd, WM_MBUTTONDOWN, MK_MBUTTON, MAKELPARAM(ctxtPt.x, ctxtPt.y));
			break;
		}
		case IDM_FRAME_BUNLOCK: {
			RECT rc;
			keypad_t *kp = (keypad_t *) lpCalc->cpu.pio.devices[1].aux;
			int group, bit;
			GetClientRect(hwnd, &rc);
			for(group = 0; group < 7; group++) {
				for(bit = 0; bit < 8; bit++) {
					kp->keys[group][bit] &= (~KEY_LOCKPRESS);
				}
			}
			lpCalc->cpu.pio.keypad->on_pressed &= (~KEY_LOCKPRESS);

			FinalizeButtons(lpMainWindow);
			break;
								}
		case IDM_SPEED_QUARTER: {
			lpCalc->speed = 25;
			HMENU hmenu = lpMainWindow->hMenu;
			CheckMenuRadioItem(GetSubMenu(hmenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_QUARTER, MF_BYCOMMAND | MF_CHECKED);
			break;
								}
		case IDM_SPEED_HALF: {
			lpCalc->speed = 50;
			HMENU hmenu = lpMainWindow->hMenu;
			CheckMenuRadioItem(GetSubMenu(hmenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_HALF, MF_BYCOMMAND | MF_CHECKED);
			break;
							 }
		case IDM_SPEED_NORMAL: {
			lpCalc->speed = 100;
			HMENU hmenu = lpMainWindow->hMenu;
			CheckMenuRadioItem(GetSubMenu(hmenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_NORMAL, MF_BYCOMMAND | MF_CHECKED);
			break;
							   }
		case IDM_SPEED_DOUBLE: {
			lpCalc->speed = 200;
			HMENU hmenu = lpMainWindow->hMenu;
			CheckMenuRadioItem(GetSubMenu(hmenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_DOUBLE, MF_BYCOMMAND | MF_CHECKED);
			break;
							   }
		case IDM_SPEED_QUADRUPLE: {
			lpCalc->speed = 400;
			HMENU hmenu = lpMainWindow->hMenu;
			CheckMenuRadioItem(GetSubMenu(hmenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_QUADRUPLE, MF_BYCOMMAND | MF_CHECKED);
			break;
								  }
		case IDM_SPEED_MAX: {
			lpCalc->speed = MAX_SPEED;
			HMENU hmenu = lpMainWindow->hMenu;
			CheckMenuRadioItem(GetSubMenu(hmenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_MAX, MF_BYCOMMAND | MF_CHECKED);
			break;
							}
		case IDM_SPEED_SET: {
			int dialog = DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_DLGSPEED), hwnd, (DLGPROC) SetSpeedProc, (LPARAM) lpCalc);
			if (dialog == IDOK) {
				HMENU hMenu = lpMainWindow->hMenu;
				switch(lpCalc->speed)
				{
				case 25:
					CheckMenuRadioItem(hMenu, IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_QUARTER, MF_BYCOMMAND| MF_CHECKED);
					break;
				case 50:
					CheckMenuRadioItem(hMenu, IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_HALF, MF_BYCOMMAND| MF_CHECKED);
					break;
				case 100:
					CheckMenuRadioItem(hMenu, IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_NORMAL, MF_BYCOMMAND| MF_CHECKED);
					break;
				case 200:
					CheckMenuRadioItem(hMenu, IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_DOUBLE, MF_BYCOMMAND| MF_CHECKED);
					break;
				case 400:
					CheckMenuRadioItem(hMenu, IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_QUADRUPLE, MF_BYCOMMAND| MF_CHECKED);
					break;
				default:
					CheckMenuRadioItem(hMenu, IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_SET, MF_BYCOMMAND| MF_CHECKED);
					break;
				}
			}
			SetFocus(hwnd);
			break;
							}
		case IDM_HELP_UPDATE: {
			BOOL hasUpdates = CheckForUpdates(hwnd);
			if (!hasUpdates) {
				MessageBox(hwnd, _T("No update is available"), _T("Wabbitemu"), MB_OK);
			}
			if (!SUCCEEDED(hasUpdates)) {
				MessageBox(hwnd, _T("Error checking for updates"), _T("Error"), MB_OK);
			}
			break;
							  }
		case IDM_HELP_BUG: {
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_REPORT_BUG), hwnd, (DLGPROC) BugReportDialogProc);
			break;
						   }
		}
		return 0;
	}
	case WM_LBUTTONUP: {
		if (lpCalc == NULL) {
			break;
		}

		BOOL repostMessage = FALSE;
		keypad_t *kp = lpCalc->cpu.pio.keypad;

		ReleaseCapture();
		KillTimer(hwnd, KEY_TIMER);

		for (int group = 0; group < 7; group++) {
			for (int bit = 0; bit < 8; bit++) {
				if (kp->last_pressed[group][bit] - lpCalc->cpu.timer_c->tstates >= MIN_KEY_DELAY || !lpCalc->running) {
					kp->keys[group][bit] &= (~KEY_MOUSEPRESS);
				} else {
					repostMessage = TRUE;
				}
			}
		}

		if (kp->on_last_pressed - lpCalc->cpu.timer_c->tstates >= MIN_KEY_DELAY || !lpCalc->running) {
			lpCalc->cpu.pio.keypad->on_pressed &= ~KEY_MOUSEPRESS;
		} else {
			repostMessage = TRUE;
		}

		if (repostMessage) {
			SetTimer(hwnd, KEY_TIMER, 50, NULL);
		}

		FinalizeButtons(lpMainWindow);
		return 0;
	}
	case WM_LBUTTONDOWN: {
		if (lpCalc == NULL) {
			break;
		}

		int group, bit;
		POINT pt;
		keypad_t *kp = lpCalc->cpu.pio.keypad;
		if (kp == NULL) {
			break;
		}

		SetCapture(hwnd);
		pt.x	= GET_X_LPARAM(lParam);
		pt.y	= GET_Y_LPARAM(lParam);

		for (group = 0; group < 7; group++) {
			for (bit = 0; bit < 8; bit++) {
				kp->keys[group][bit] &= (~KEY_MOUSEPRESS);
			}
		}

		kp->on_pressed &= ~KEY_MOUSEPRESS;

		LONG x = (LONG)(pt.x / lpMainWindow->skin_scale);
		LONG y = (LONG)(pt.y / lpMainWindow->skin_scale);
		Color c;
		lpMainWindow->m_lpBitmapKeymap->GetPixel(x, y, &c);
		if (c.GetRed() == 0xFF) {
			FinalizeButtons(lpMainWindow);
			return 0;
		}

		group = c.GetGreen() >> 4;
		bit	= c.GetBlue() >> 4;
		HandleTeacherViewKey(lpMainWindow->hwndTeacherView, lpCalc);
		LogKeypress(lpMainWindow, lpCalc->model, group, bit);
		if (group == KEYGROUP_ON && bit == KEYBIT_ON){
			kp->on_pressed |= KEY_MOUSEPRESS;
			kp->on_last_pressed = lpCalc->cpu.timer_c->tstates;
		} else {
			kp->keys[group][bit] |= KEY_MOUSEPRESS;
			if ((kp->keys[group][bit] & KEY_STATEDOWN) == 0) {
				kp->keys[group][bit] |= KEY_STATEDOWN;
				kp->last_pressed[group][bit] = lpCalc->cpu.timer_c->tstates;
			}
		}
		FinalizeButtons(lpMainWindow);
		return 0;
	}
	case WM_TIMER: {
		if (wParam == KEY_TIMER) {
			PostMessage(hwnd, WM_LBUTTONUP, 0, 0);
		}
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	case WM_MBUTTONDOWN: {
		if (lpCalc == NULL) {
			break;
		}

		int group,bit;
		POINT pt;
		keypad_t *kp = lpCalc->cpu.pio.keypad;

		pt.x	= GET_X_LPARAM(lParam);
		pt.y	= GET_Y_LPARAM(lParam);

		// convert to the scale it is displayed at
		LONG x = (LONG)(pt.x / lpMainWindow->skin_scale);
		LONG y = (LONG)(pt.y / lpMainWindow->skin_scale);
		Color c;
		lpMainWindow->m_lpBitmapKeymap->GetPixel(x, y, &c);
		if (c.GetRed() == 0xFF) return 0;
		group	= c.GetGreen() >> 4;
		bit		= c.GetBlue() >> 4;

		if (group == KEYGROUP_ON && bit == KEYBIT_ON) {
			lpCalc->cpu.pio.keypad->on_pressed ^= KEY_LOCKPRESS;
		} else {
			kp->keys[group][bit] ^= KEY_LOCKPRESS;
		}
		FinalizeButtons(lpMainWindow);
		return 0;
	}
	case WM_KEYDOWN: {
		HandleKeyDown(lpMainWindow, wParam);
		return 0;
	}
	case WM_KEYUP: {
		if (wParam) {
			HandleKeyUp(lpMainWindow, wParam);
		}
		return 0;
	}
	case WM_SIZING: {
		if (lpCalc == NULL) {
			break;
		}

		if (lpMainWindow->bSkinEnabled) {
			return HandleSkinSizingMessage(hwnd, lpMainWindow, wParam, (RECT *)lParam);
		}

		return HandleLCDSizingMessage(hwnd, lpMainWindow->hwndStatusBar, lpMainWindow,
			wParam, (RECT *)lParam, lpCalc->cpu.pio.lcd->width);
	}
	case WM_SIZE: {
		RECT rc = { 0 };
		rc.bottom = HIWORD(lParam);
		rc.right = LOWORD(lParam);

		// HACK: For minimize bug
		// If you minimize the cutout window the screen in cutout mode, when you restore
		// it you receive the already adjusted window rect, so when you adjust it again
		// the screen size is larger than it should be.
		if (rc.bottom == 0 && rc.right == 0) {
			lpMainWindow->bIsMinimized = true;
			return 0;
		}

		if (lpMainWindow->bIsMinimized) {
			lpMainWindow->bIsMinimized = false;
			return 0;
		}

		AdjustWindowRect(&rc, WS_CAPTION | WS_TILEDWINDOW, TRUE);
		if (lpMainWindow->bSkinEnabled) {
			HandleSkinSizingMessage(hwnd, lpMainWindow, WMSZ_BOTTOMRIGHT, &rc);
		} else {
			HandleLCDSizingMessage(hwnd, lpMainWindow->hwndStatusBar, lpMainWindow,
				WMSZ_BOTTOMRIGHT, &rc, lpCalc->cpu.pio.lcd->width);
		}

		if (!lpMainWindow->bCutout || !lpMainWindow->bSkinEnabled) {
			LONG newWidth = rc.right - rc.left;
			LONG newHeight = rc.bottom - rc.top;
			SetWindowPos(hwnd, NULL, 0, 0, newWidth, newHeight, SWP_NOMOVE | SWP_NOZORDER);
		}

		return HandleSizeMessage(hwnd, lpMainWindow->hwndLCD, lpMainWindow, lpCalc,
			lpMainWindow->bSkinEnabled, lpMainWindow->bCutout);
	}
	case WM_MOVE: {
		if (lpCalc == NULL) {
			break;
		}

		return HandleMoveMessage(hwnd, lpMainWindow);
	}
	case WM_CONTEXTMENU: {
		lpMainWindow->ctxtPt.x = GET_X_LPARAM(lParam);
		lpMainWindow->ctxtPt.y = GET_Y_LPARAM(lParam);

		HMENU hmenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_FRAME_MENU));
		// TrackPopupMenu cannot display the menu bar so get
		// a handle to the first shortcut menu.
		hmenu = GetSubMenu(hmenu, 0);

		if (!OnContextMenu(hwnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), hmenu)) {
			DefWindowProc(hwnd, Message, wParam, lParam);
		}
		ScreenToClient(hwnd, &lpMainWindow->ctxtPt);
		DestroyMenu(hmenu);
		return 0;
						 }
	case WM_GETMINMAXINFO: {
		MINMAXINFO *info = (MINMAXINFO *)lParam;
		return GetMinMaxInfo(hwnd, lpMainWindow, info);
	}
	case WM_KILLFOCUS: {
		if (lpCalc == NULL) {
			break;
		}
		keypad_t *keypad = lpCalc->cpu.pio.keypad;
		if (keypad == NULL) {
			break;
		}
		
		for (int group = 0; group < 8; group++) {
			for (int bit = 0; bit < 8; bit++) {
				if (keypad->keys[group][bit] == '\0') {
					continue;
				}

				keyprog_t *key = keypad_keyprog_from_groupbit(&lpCalc->cpu, group, bit);
				if (key == NULL) {
					continue;
				}

				HandleKeyUp(lpMainWindow, key->vk);
			}
		}
		return 0;
					   }
	case WM_CLOSE:
		{
			BOOL fIsCOMFrame = (BOOL) GetProp(hwnd, _T("COMObjectFrame"));
			if (fIsCOMFrame)
			{
				DestroyWindow(hwnd);
				lpMainWindow->pWabbitemu->Fire_OnClose();
			}
			else
			{
				if (calc_count() == 1) {
					if (exit_save_state)
					{
						TCHAR tempSave[MAX_PATH] = {0};
						if (portable_mode) {
							StringCbCopy(tempSave, sizeof(tempSave), portSettingsPath);
							for (u_int i = _tcslen(portSettingsPath) - 1; i >= 0; i--) {
								if (tempSave[i] == '\\') {
									tempSave[i] = '\0';
									break;
								}
								tempSave[i] = '\0';
							}
						} else {
							GetStorageString(tempSave, sizeof(tempSave));
						}
						StringCbCat(tempSave, sizeof(tempSave), _T("wabbitemu.sav"));
						StringCbCopy(lpCalc->rom_path, sizeof(lpCalc->rom_path), tempSave);
						SAVESTATE_t *save = SaveSlot(lpCalc, _T("Auto save state"),
							_T("This save was automatically generated by Wabbitemu"));
						WriteSave(tempSave, save, true);
						FreeSave(save);
					}

					SaveRegistrySettings(lpMainWindow, lpCalc);

					is_exiting = TRUE;
					free(link_hub);
					link_hub = NULL;
				}

				destroy_calc_frame(lpMainWindow);

				if (calc_count() == 0) {
					PostQuitMessage(0);
				}
			}
			return 0;
		}
	case WM_DESTROY: {
		if (lpMainWindow->hwndLCD != NULL) {
			DestroyWindow(lpMainWindow->hwndLCD);
			lpMainWindow->hwndLCD = NULL;
		}

		if (lpMainWindow->hwndDebug != NULL) {
			DestroyWindow(lpMainWindow->hwndDebug);
			lpMainWindow->hwndDebug = NULL;
		}

		if (lpMainWindow->hwndStatusBar != NULL) {
			DestroyWindow(lpMainWindow->hwndStatusBar);
			lpMainWindow->hwndStatusBar = NULL;
		}

		if (lpMainWindow->hwndSmallClose != NULL) {
			DestroyWindow(lpMainWindow->hwndSmallClose);
			lpMainWindow->hwndSmallClose = NULL;
		}

		if (lpMainWindow->hwndSmallMinimize != NULL) {
			DestroyWindow(lpMainWindow->hwndSmallMinimize);
			lpMainWindow->hwndSmallMinimize = NULL;
		}

		CancelFileThreadSend(lpCalc);
		calc_slot_free(lpCalc);

		//if (link_connected(lpCalc->slot))
		//	link_disconnect(&lpCalc->cpu);
		return 0;
	}
	case WM_NCHITTEST:
		{
			int htRet = (int) DefWindowProc(hwnd, Message, wParam, lParam);
			switch (htRet) {
			case HTLEFT:
			case HTRIGHT:
			case HTTOP:
			case HTBOTTOM:
				return HTNOWHERE;
			default:
				if (htRet == HTCLIENT) {
					break;
				}
				return htRet;
			}

			POINT pt;
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);

			ScreenToClient(hwnd, &pt);
			LONG x = (LONG)(pt.x / lpMainWindow->skin_scale);
			LONG y = (LONG)(pt.y / lpMainWindow->skin_scale);
			Color c;
			lpMainWindow->m_lpBitmapKeymap->GetPixel(x, y, &c);
			if (c.GetRed() != 0xFF) {
				return htRet;
			}

			return HTCAPTION;
		}
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}