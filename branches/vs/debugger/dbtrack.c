#include <windows.h>
#include <windowsx.h>

#include "core.h"
#include "calc.h"

#define g_szTrackClass "WabbitTrackClass"

extern HINSTANCE g_hInst;
extern HFONT hfontSegoe;

typedef struct tag_DEBUGTRACK {
	TCHAR szName[256];
	waddr_t waddr;
	DWORD dwSize;
	TCHAR szFmt[32];
	struct tag_DEBUGTRACK *Next;
} DEBUGTRACK;

static HWND hwndTrack = NULL;
static DEBUGTRACK *dt = NULL, *last_dt = NULL;

static LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static TEXTMETRIC tm;
	
	switch (Message)
	{
	case WM_CREATE:
		{	
			HDC hdc = GetDC(hwnd);
			SelectObject(hdc, hfontSegoe);
			GetTextMetrics(hdc, &tm);
			ReleaseDC(hwnd, hdc);
			
			return 0;
		}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		RECT r, rc;
		DEBUGTRACK *curr_dt;
		
		GetClientRect(hwnd, &rc);
		hdc = BeginPaint(hwnd, &ps);
		SelectObject(hdc, hfontSegoe);
		
		SetBkMode(hdc, TRANSPARENT);
		
		SetRect(&r, 0, 0, rc.right, tm.tmHeight * 3 / 2);
		for (curr_dt = dt; curr_dt != NULL; curr_dt = curr_dt->Next, OffsetRect(&r, 0, tm.tmHeight * 3 / 2)) {
			TCHAR szValue[256];
			DWORD dwValue = 0;
			RECT dr;
			
			CopyRect(&dr, &r);
			dr.right = 100;
			DrawText(hdc, curr_dt->szName, -1, &dr, DT_SINGLELINE);
			
			OffsetRect(&dr, 100, 0);
			if (curr_dt->dwSize < 4) {
				int i;
				for (i = 0; i < curr_dt->dwSize; i++) {
					waddr_t waddr = curr_dt->waddr;
					waddr.addr += i;
					dwValue |= wmem_read(&calcs[gslot].mem_c, waddr) << (8 * i);
				}
				sprintf(szValue, curr_dt->szFmt, dwValue);
				DrawText(hdc, szValue, -1, &dr, DT_SINGLELINE);
			}
		}
		
		EndPaint(hwnd, &ps);
	}
					
	}
	return DefWindowProc(hwnd, Message, wParam, lParam);
}


static HWND CreateDebugTrack(void) {
	static BOOL FirstRun = TRUE;
	HWND hwnd;
	
	if (FirstRun) {
		WNDCLASSEX wcx;
		
		FirstRun = FALSE;
		
		ZeroMemory(&wcx, sizeof(wcx));
		wcx.cbSize = sizeof(wcx);
		wcx.lpfnWndProc = WndProc;
		wcx.lpszClassName = g_szTrackClass;
		wcx.hbrBackground =  (HBRUSH) (COLOR_BTNFACE+1);
		
		RegisterClassEx(&wcx);
	}
	
	hwnd = CreateWindowEx(
			WS_EX_TOOLWINDOW,
			g_szTrackClass,
			"Debug Tracking",
			WS_VISIBLE | WS_POPUP | WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 200, 400,
			NULL,
			NULL,
			g_hInst,
			NULL);
	
	return hwnd;
}


int AddDebugTrack(int slot, TCHAR *szName, waddr_t waddr, DWORD dwSize, TCHAR *szFmt) {
	if (hwndTrack == NULL)
		hwndTrack = CreateDebugTrack();
	
	DEBUGTRACK *new_dt = (DEBUGTRACK *) malloc(sizeof(DEBUGTRACK));

	strcpy(new_dt->szName, szName);
	new_dt->waddr = waddr;
	new_dt->dwSize = dwSize;
	strcpy(new_dt->szFmt, szFmt);
	new_dt->Next = NULL;
	
	// Add it to the list
	if (dt == NULL) {
		dt = new_dt;
		last_dt = dt;
	} else {
		last_dt->Next = new_dt;
		last_dt = new_dt;
	}
	return 0;
}

void UpdateDebugTrack(void) {
	if (hwndTrack != NULL)
		InvalidateRect(hwndTrack, NULL, TRUE);
}

