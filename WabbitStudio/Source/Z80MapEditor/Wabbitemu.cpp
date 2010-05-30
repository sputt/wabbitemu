#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

#include "wabbitemu.h"
#include "MapHierarchy.h"

#define MAX_SHADES 255

extern HINSTANCE g_hInstance;
extern MAPSETSETTINGS g_MapSet;
extern HWND hwndHierarchy;

static BITMAPINFO *bi;
static unsigned char *pMapData;
static HWND hwndPrev = NULL;

static void PaintLCD(HDC hdc) {
	LPBYTE pScreen = GetLCDImage();	
	StretchDIBits(
		hdc,
		0, 0, 96 * 2, 64 * 2,
		0, 0, 96, 64,
		pScreen,
		bi,
		DIB_RGB_COLORS,
		SRCCOPY);
}

static void FrameCallback(void) {
	BYTE CurrentMap;
	ReadVariable("current_map", &CurrentMap, 1);
	MAPVIEWSETTINGS mvs;
	GetMapViewSettings(g_MapSet.MapHierarchy[CurrentMap], &mvs);
	//if (pMapData != NULL) {
	//	memcpy(mvs.pMapData, pMapData, 16 * 16);
	//}

	if (g_MapSet.fTesting == FALSE) {
		g_MapSet.fTesting = TRUE;
		UpdateAllMaps();
	} else {
		InvalidateRect(g_MapSet.MapHierarchy[CurrentMap], NULL, FALSE);
		UpdateWindow(g_MapSet.MapHierarchy[CurrentMap]);
	}

	if (hwndPrev != g_MapSet.MapHierarchy[CurrentMap]) {
		if (hwndPrev != NULL) {
			InvalidateRect(hwndPrev, NULL, FALSE);
		}
		hwndPrev = g_MapSet.MapHierarchy[CurrentMap];
	}
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg)
	{
	case WM_CREATE:
		{
			bi = (BITMAPINFO *) malloc(sizeof(BITMAPINFOHEADER) + (MAX_SHADES+1)*sizeof(RGBQUAD));
			bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bi->bmiHeader.biWidth = 128;
			bi->bmiHeader.biHeight = -64;
			bi->bmiHeader.biPlanes = 1;
			bi->bmiHeader.biBitCount = 8;
			bi->bmiHeader.biCompression = BI_RGB;
			bi->bmiHeader.biSizeImage = 0;
			bi->bmiHeader.biXPelsPerMeter = 0;
			bi->bmiHeader.biYPelsPerMeter = 0;
			bi->bmiHeader.biClrUsed = MAX_SHADES+1;
			bi->bmiHeader.biClrImportant = MAX_SHADES+1;
			
			//#define LCD_LOW (RGB(0x9E, 0xAB, 0x88))			
			int i;
			#define LCD_HIGH	255
			for (i = 0; i <= MAX_SHADES; i++) {
				bi->bmiColors[i].rgbRed = (0x9E*(256-(LCD_HIGH/MAX_SHADES)*i))/255;
				bi->bmiColors[i].rgbGreen = (0xAB*(256-(LCD_HIGH/MAX_SHADES)*i))/255;
				bi->bmiColors[i].rgbBlue = (0x88*(256-(LCD_HIGH/MAX_SHADES)*i))/255;				
			}

			int slot = rom_load(0, "mapeditor.sav");
			if (slot == -1) {
				MessageBox(NULL, _T("Failed to load a ROM for wabbitemu"), _T("Z80 Map Editor"), MB_OK | MB_ICONERROR);
				return 1;
			}
			EnableCalc(TRUE);
			SendFile("mapeditor.8xk", 1);
			SendFile("mapeditor.lab", 0);

			SetTimer(hwnd, 1, TPF, NULL);
			PressKey('2');
			for (int i = 0; i < FPS / 3; i++) {
				calc_run_all();
			}
			ReleaseKey('2');
			HookZelda(FrameCallback);

			pMapData = GetZeldaMapData();
			return 0;
		}
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			PaintLCD(hdc);
			EndPaint(hwnd, &ps);
			return 0;
		}
	case WM_KEYDOWN:
		if (wParam == VK_SHIFT) {
			if (GetKeyState(VK_LSHIFT) & 0xFF00) {
				wParam = VK_LSHIFT;
			} else {
				wParam = VK_RSHIFT;
			}
		}
		PressKey(wParam);
		return 0;
	case WM_KEYUP:
		if (wParam == VK_SHIFT) {
			ReleaseKey(VK_LSHIFT);
			ReleaseKey(VK_RSHIFT);
		} else {
			ReleaseKey(wParam);
		}
		return 0;
	case WM_TIMER:
		{
			static long difference = 0;
			static DWORD prevTimer = 0;
			DWORD dwTimer = GetTickCount();

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

				InvalidateRect(hwnd, NULL, FALSE);
				UpdateWindow(hwnd);
			// Frame skip if we're too far ahead.
			} else difference += TPF;
			return 0;
		}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

void CreateCalcPreview(void) {
	WNDCLASS wc = {0};
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = _T("CalcPreview");
	wc.hInstance = g_hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&wc);

	RECT r = {0, 0, 96 * 2, 64 * 2};
	AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);

	g_MapSet.hwndTest = CreateWindowEx(
		0,
		_T("CalcPreview"),
		_T("Map preview"),
		WS_OVERLAPPEDWINDOW
		, //| WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top,
		NULL, NULL, g_hInstance, NULL);
}