#include "stdafx.h"

#include "expandpane.h"
#include "coretypes.h"
#include "dbcommon.h"
#include "registry.h"

extern HINSTANCE g_hInst;

#define EXPAND_PANE_BASE_ID	1137
static int IDArray[MAX_CALCS] = {
	EXPAND_PANE_BASE_ID,
	EXPAND_PANE_BASE_ID,
	EXPAND_PANE_BASE_ID,
	EXPAND_PANE_BASE_ID,
	EXPAND_PANE_BASE_ID,
	EXPAND_PANE_BASE_ID,
	EXPAND_PANE_BASE_ID,
	EXPAND_PANE_BASE_ID
};

#define MAX_SLIDE_SPEED 	40
#define SLIDE_ACCEL 		3.5

#define MAX_FADE (600/20)

typedef enum {
	EP_CLOSED,
	EP_OPENING,
	EP_CLOSING,
	EP_OPEN
} EXPAND_STATE;

typedef struct {
	BOOL bExpanded, bButtonHot, bHot;
	HWND contents;
	HWND hwndHeader;
	HWND hwndBtn;
	int dwHeight;
	DWORD DragCount;

	BOOL bFading;
	int FadeCount;

	BOOL bSliding;
	double SlideSpeed;

	EXPAND_STATE ExpandState;
	double VisibleHeight;
	LPDEBUGWINDOWINFO lpDebugInfo;
} expand_pane_settings, ep_settings;

HWND CreateExpandPane(HWND hwndParent, LPDEBUGWINDOWINFO lpDebugInfo, TCHAR *name, HWND contents) {

	expand_pane_settings *eps = (expand_pane_settings *) malloc(sizeof(expand_pane_settings));
	ZeroMemory(eps, sizeof(expand_pane_settings));

	DWORD status = (DWORD) QueryDebugKey(name);
	eps->contents = contents;
	eps->ExpandState = (EXPAND_STATE)status;
	eps->lpDebugInfo = lpDebugInfo;
	//eps->bFading = TRUE;

	if (IDArray[lpDebugInfo->lpCalc->slot] == EXPAND_PANE_BASE_ID) {
		lpDebugInfo->TotalPanes = 0;
	}

	HWND hwndExp = CreateWindow(
			g_szExpandPane,
			name,
			WS_VISIBLE | WS_CHILD,
			0, 0, 1, 1,
			hwndParent, (HMENU)IDArray[lpDebugInfo->lpCalc->slot]++, g_hInst,
			eps);
	if (hwndExp == NULL)
		return NULL;

	TEXTMETRIC tm;
	GetTextMetrics(GetDC(hwndExp), &tm);
	if (eps->ExpandState == EP_OPEN)
		eps->VisibleHeight = eps->dwHeight - tm.tmHeight*3/2;

	if (contents != NULL) {
		if (SetParent(contents, hwndExp) == NULL)
		{
			MessageBox(NULL, _T("Unable to set parent!\n"), _T("shit"), MB_OK);
		}
		ShowWindow(contents, SW_SHOW);
		SetWindowPos(contents, HWND_BOTTOM, 16, tm.tmHeight*3/2, 0, 0, SWP_NOSIZE);
		//DestroyWindow(contents);
	}

	lpDebugInfo->ExpandPanes[lpDebugInfo->TotalPanes++] = hwndExp;
	return hwndExp;
}

void ArrangeExpandPanes(LPDEBUGWINDOWINFO lpDebugInfo) {
	int i;
	for (i = 0; i < lpDebugInfo->TotalPanes; i++) {
		SendMessage(lpDebugInfo->ExpandPanes[i], WM_SIZE, 0, 0);
	}
}

void DrawExpandPanes(LPDEBUGWINDOWINFO lpDebugInfo) {
	int i;
	for (i = 0; i < lpDebugInfo->TotalPanes; i++) {
		InvalidateRect(lpDebugInfo->ExpandPanes[i], NULL, FALSE);
		SendMessage(lpDebugInfo->ExpandPanes[i], WM_PAINT, 0, 0);
	}
}

int GetExpandPanesHeight(LPDEBUGWINDOWINFO lpDebugInfo) {
	int i;
	lpDebugInfo->PanesHeight = 0;
	for (i = 0; i < lpDebugInfo->TotalPanes; i++) {
		SendMessage(lpDebugInfo->ExpandPanes[i], WM_SIZE, 0, 0);
	}
	return lpDebugInfo->PanesHeight;
}

void GetExpandPaneState(LPDEBUGWINDOWINFO lpDebugInfo, ep_state *state) {
	state->total = lpDebugInfo->TotalPanes;

	int i;
	for (i = 0; i < lpDebugInfo->TotalPanes; i++)
	{
		ep_settings *eps = (ep_settings*) GetWindowLongPtr(lpDebugInfo->ExpandPanes[i], GWLP_USERDATA);
		if (eps) {
			state->state[i] = (eps->ExpandState == EP_OPEN);
		}
	}

	ArrangeExpandPanes(lpDebugInfo);
}

void SetExpandPaneState(LPDEBUGWINDOWINFO lpDebugInfo, const ep_state *state) {
	int i;
	for (i = 0; i < lpDebugInfo->TotalPanes; i++) {
		ep_settings *eps = (ep_settings*) GetWindowLongPtr(lpDebugInfo->ExpandPanes[i], GWLP_USERDATA);
		if (eps) {
			eps->bExpanded = state->state[i];
		}
	}

}

void DrawHeader(HDC hdc, HFONT hfontSegoe, TCHAR *s, int y, int width) {

	SelectObject(hdc, hfontSegoe);

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	RECT r = {8, y, width, y + tm.tmHeight*3/2};

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(0, 51, 153));
	DrawTextEx(hdc, s, -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE, NULL);

	DrawTextEx(hdc, s, -1, &r, DT_LEFT | DT_CALCRECT | DT_VCENTER | DT_SINGLELINE, NULL);
	SelectObject(hdc, GetStockObject(DC_PEN));
	SetDCPenColor(hdc, RGB(176, 191, 222));
	MoveToEx(hdc, r.right + tm.tmAveCharWidth/2, (r.bottom - r.top)/2 + 1, NULL);
	LineTo(hdc, width - 28, (r.bottom - r.top)/2 + 1);
}


static LRESULT CALLBACK HeaderProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message)
	{
	case WM_CREATE:
		return 0;
	case WM_VSCROLL:
		SendMessage(GetParent(hwnd), Message, wParam, lParam);
		return 0;
	case WM_PAINT:
	{
		HDC hdc;
		PAINTSTRUCT ps;
		TCHAR pszText[32];
		RECT rc;
		ep_settings *eps = (ep_settings*) GetWindowLongPtr(hwnd, GWLP_USERDATA);

		GetWindowText(GetParent(hwnd), pszText, sizeof(pszText));

		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rc);
		FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));
		if (eps->bHot) {
			DrawItemSelection(hdc, &rc, TRUE, FALSE, 130);
		}
		DrawHeader(hdc, eps->lpDebugInfo->hfontSegoe, pszText, 0, rc.right - rc.left);
		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_MOUSEWHEEL:
		SendMessage(GetParent(hwnd), Message, wParam, lParam);
		return 0;
	case WM_COMMAND: {
		ep_settings *eps = (ep_settings*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		switch (HIWORD(wParam)) {
			case BN_CLICKED:
			{
				switch (eps->ExpandState)
				{
				case EP_CLOSED:
					eps->ExpandState = EP_OPENING;
					SetTimer(GetParent(hwnd), 0, 20, NULL);
					eps->SlideSpeed = 0.0;
					break;
				case EP_OPEN:
					eps->ExpandState = EP_CLOSING;
					SetTimer(GetParent(hwnd), 0, 20, NULL);
					eps->SlideSpeed = 0.0;
					break;
				case EP_OPENING:
					eps->ExpandState = EP_CLOSING;
					break;
				case EP_CLOSING:
					eps->ExpandState = EP_OPENING;
					break;
				}

				RECT rc;
				GetClientRect(hwnd, &rc);

				//InvalidateRect(hwnd, NULL, TRUE);
				//InvalidateRect(GetParent(hwnd), NULL, TRUE);
				return 0;
			}
		}
		return 0;
	}
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
}

static WNDPROC OldButtonProc;
static HDC hdcButtons = NULL;

static LRESULT CALLBACK ExpandButtonProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
		case WM_VSCROLL:
			SendMessage(GetParent(hwnd), Message, wParam, lParam);
			return 0;
		case WM_MOUSEMOVE: {
			ep_settings *eps = (ep_settings*) GetWindowLongPtr(hwnd, GWLP_USERDATA);

			if (eps->bButtonHot == FALSE) {
				eps->bButtonHot = TRUE;

				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);

				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(tme);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hwnd;
				tme.dwHoverTime = 1;
				TrackMouseEvent(&tme);
			}

			return 0;
		}
		case WM_MOUSELEAVE: {
			ep_settings *eps = (ep_settings*) GetWindowLongPtr(hwnd, GWLP_USERDATA);

			eps->bButtonHot = FALSE;

			InvalidateRect(eps->hwndHeader, NULL, TRUE);
			UpdateWindow(eps->hwndHeader);
			return 0;
		}
		case WM_PAINT: {
			ep_settings *eps = (ep_settings*) GetWindowLongPtr(hwnd, GWLP_USERDATA);

			if (hdcButtons == NULL) {
				HBITMAP hbmButtons = LoadBitmap(g_hInst, _T("expandpanebuttons"));
				hdcButtons = CreateCompatibleDC(GetDC(hwnd));
				SelectObject(hdcButtons, hbmButtons);
				DeleteObject(hbmButtons);
			}

			HDC hdcDest, hdc;
			HBITMAP hbm;
			PAINTSTRUCT ps;

			RECT rc;
			GetClientRect(hwnd, &rc);

			hdcDest = BeginPaint(hwnd, &ps);
			if (eps->bFading) {
				hdc = CreateCompatibleDC(hdcDest);
				hbm = CreateCompatibleBitmap(hdcDest, rc.right - rc.left, rc.bottom - rc.top);
				SelectObject(hdc, hbm);
				FillRect(hdc, &rc, (HBRUSH) GetStockObject(WHITE_BRUSH));
			} else {
				hdc = hdcDest;
			}

			int base_x = 0;
			if (eps->ExpandState == EP_OPEN || eps->ExpandState == EP_OPENING)
				base_x = 30;

			if (eps->bButtonHot)
				base_x += 15;

			BitBlt(hdc, (rc.right - rc.left - 15)/2,
					(rc.bottom - rc.top - 15)/2, 15, 15, hdcButtons, base_x, 15, SRCAND);

			BitBlt(hdc, (rc.right - rc.left - 15)/2,
					(rc.bottom - rc.top - 15)/2, 15, 15, hdcButtons, base_x, 0, SRCPAINT);

			if (eps->bFading) {
				// Blit the copy to the actual hdc
				BLENDFUNCTION bf;
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				bf.SourceConstantAlpha = 255 * ((eps->FadeCount % MAX_FADE) + 1) / MAX_FADE;
				bf.AlphaFormat = 0;

				FillRect(hdcDest, &rc, (HBRUSH) GetStockObject(WHITE_BRUSH));

				AlphaBlend(
						hdcDest, 	0, 0, rc.right - rc.left, rc.bottom - rc.top,
						hdc, 		0, 0, rc.right - rc.left, rc.bottom - rc.top, bf);

				DeleteObject(hbm);
				DeleteDC(hdc);

			}
			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_DESTROY: 
			if (hdcButtons) {
				DeleteObject(hdcButtons);
				hdcButtons = NULL;
			}
		default:
			return CallWindowProc(OldButtonProc, hwnd, Message, wParam, lParam);
	}
}

LRESULT CALLBACK ExpandPaneProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static TEXTMETRIC tm;
	static BOOL bFirstRun = TRUE;

	switch (Message) {
		case WM_CREATE:
		{
			expand_pane_settings *eps;
			
			eps = (expand_pane_settings *) ((CREATESTRUCT*)lParam)->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) eps);

			if (bFirstRun == TRUE) {
				WNDCLASSEX wcx;

				bFirstRun = FALSE;

				ZeroMemory(&wcx, sizeof(wcx));
				wcx.cbSize = sizeof(wcx);
				wcx.lpfnWndProc = HeaderProc;
				wcx.lpszClassName = _T("WabbitExpPaneHeader");
				wcx.hInstance = g_hInst;
				wcx.hbrBackground = NULL; //(HBRUSH) (COLOR_MENU+1);

				RegisterClassEx(&wcx);
			}

			eps->hwndHeader = CreateWindowEx(
					0,
					_T("WabbitExpPaneHeader"),
					_T("Test"),
					WS_CHILD | WS_VISIBLE,
					0, 0, 40, 20,
					hwnd, (HMENU) 10,
					g_hInst, NULL);
			SetWindowPos(eps->hwndHeader, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

			eps->hwndBtn = CreateWindowEx(
					WS_EX_TRANSPARENT,	// this lets the other shit behind it get drawn first
					_T("BUTTON"),
					_T("V"),
					BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
					0, 0, 1, 1,
					eps->hwndHeader, (HMENU) 1500, g_hInst, NULL);

			eps->bButtonHot = FALSE;
			eps->bHot = FALSE;
			eps->bExpanded = eps->ExpandState == EP_OPEN ? TRUE : FALSE;

			OldButtonProc = (WNDPROC) GetWindowLongPtr(eps->hwndBtn, GWLP_WNDPROC);
			SetWindowLongPtr(eps->hwndBtn, GWLP_WNDPROC, (LONG_PTR) ExpandButtonProc);
			SetWindowLongPtr(eps->hwndBtn, GWLP_USERDATA, (LONG_PTR) eps);

			SetWindowLongPtr(eps->hwndHeader, GWLP_USERDATA, (LONG_PTR) eps);
			HDC hdc = GetDC(hwnd);
			GetTextMetrics(hdc, &tm);
			ReleaseDC(hwnd, hdc);
			eps->dwHeight = 100;
			SendMessage(hwnd, WM_SIZE, 0, 0);
			return 0;
		}
		case WM_MOUSEWHEEL:
			SendMessage(GetParent(hwnd), Message, wParam, lParam);
			break;
		case WM_VSCROLL:
			SendMessage(GetParent(hwnd), Message, wParam, lParam);
			break;
		case WM_TIMER:
		{
			ep_settings *eps = (ep_settings*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (eps->bFading) {
				eps->FadeCount++;
				InvalidateRect(hwnd, NULL, FALSE);
			}

			// if I started decelerating now, how far would I get
			double FramesLeft = ceil(eps->SlideSpeed / SLIDE_ACCEL);

			double HowFar = eps->VisibleHeight;
			double FakeVel = eps->SlideSpeed;

			int i;
			switch (eps->ExpandState)
			{
			case EP_OPENING:
				for (i = 0; i < FramesLeft; i++) {
					FakeVel -= SLIDE_ACCEL;
					HowFar += FakeVel;
				}
				if ((HowFar + tm.tmHeight*3/2) >= eps->dwHeight) {
					eps->SlideSpeed -= SLIDE_ACCEL;
					if (eps->SlideSpeed < 0.0)
						eps->SlideSpeed = 0.0;
				} else {
					eps->SlideSpeed += SLIDE_ACCEL;
					if (eps->SlideSpeed > MAX_SLIDE_SPEED)
						eps->SlideSpeed = MAX_SLIDE_SPEED;
				}
				eps->VisibleHeight+=eps->SlideSpeed;
				ArrangeExpandPanes(eps->lpDebugInfo);
				break;
			case EP_CLOSING:
				for (i = 0; i < FramesLeft; i++) {
					FakeVel += SLIDE_ACCEL;
					HowFar -= FakeVel;
				}
				if (HowFar <= 0.0) {
					eps->SlideSpeed -= SLIDE_ACCEL;
					if (eps->SlideSpeed < 0.0)
						eps->SlideSpeed = 0.0;
				} else {
					eps->SlideSpeed += SLIDE_ACCEL;
					if (eps->SlideSpeed > MAX_SLIDE_SPEED)
						eps->SlideSpeed = MAX_SLIDE_SPEED;
				}
				eps->VisibleHeight-=eps->SlideSpeed;
				ArrangeExpandPanes(eps->lpDebugInfo);
				break;
			default:
				break;
			}
			return 0;
		}
		case WM_SIZE:
		{
			ep_settings *eps = (ep_settings*) GetWindowLongPtr(hwnd, GWLP_USERDATA);

			HDC hdc = GetDC(hwnd);
			SelectObject(hdc, eps->lpDebugInfo->hfontSegoe);

			ReleaseDC(hwnd, hdc);

			RECT prc;
			GetWindowRect(GetParent(hwnd), &prc);

			DWORD dwHeight = tm.tmHeight*3/2;
			DWORD dwWidth = prc.right - prc.left - GetSystemMetrics(SM_CXVSCROLL);
			int index = GetWindowLongPtr(hwnd, GWLP_ID) - EXPAND_PANE_BASE_ID;

			// Add in all the previous windows
			int cy = eps->lpDebugInfo->reg_panes_yoffset;
			int i;
			for (i = 0; i < index; i++) {
				RECT rc;
				GetWindowRect(eps->lpDebugInfo->ExpandPanes[i], &rc);
				//printf("windowrect: %ld %ld %ld %ld\n", rc.left, rc.top ,rc.right, rc.bottom);
				cy += rc.bottom - rc.top;
				cy += tm.tmHeight/4;
			}

			if (eps->contents) {
				SendMessage(eps->contents, WM_SIZE, 0, 0);

				WINDOWINFO wi;
				GetWindowInfo(eps->contents, &wi);

				eps->dwHeight = wi.rcClient.bottom - wi.rcClient.top;
				if ((eps->ExpandState == EP_OPENING) && ((eps->VisibleHeight + tm.tmHeight*3/2) > eps->dwHeight)) {
					KillTimer(hwnd, 0);
					eps->ExpandState = EP_OPEN;
					eps->VisibleHeight = eps->dwHeight - tm.tmHeight*3/2;
					printf("Pane is open\n");
					HWND hParent = GetParent(hwnd);
					if (hParent != NULL)
						SendMessage(hParent, WM_SIZE, 0, 0);
				} else if ((eps->ExpandState == EP_CLOSING) && (eps->VisibleHeight <= 0)) {
					KillTimer(hwnd, 0);
					eps->ExpandState = EP_CLOSED;
					eps->VisibleHeight = 0;
					printf("Pane is closed\n");
					HWND hParent = GetParent(hwnd);
					if (hParent != NULL)
						SendMessage(hParent, WM_SIZE, 0, 0);
				}

				SetWindowPos(eps->contents, HWND_BOTTOM, 16, tm.tmHeight*3 + (int) eps->VisibleHeight - eps->dwHeight, 0, 0, SWP_NOSIZE);
				InvalidateRect(eps->contents, NULL, FALSE);
				UpdateWindow(eps->contents);
			}

			if (eps->ExpandState != EP_CLOSED)
				dwHeight = tm.tmHeight*3/2 + (DWORD) eps->VisibleHeight;
			//accounts for a gap the MoveWindow y = 2 below
			eps->lpDebugInfo->PanesHeight += dwHeight + 2;

			RECT rc;
			MoveWindow(hwnd, 2, cy, dwWidth - 4, dwHeight, TRUE);
			GetClientRect(hwnd, &rc);
			SetWindowPos(eps->hwndHeader, HWND_TOP, 0, 0, dwWidth, tm.tmHeight*3/2, 0);
			SetWindowPos(eps->hwndBtn, NULL, dwWidth - 24 - 8, (tm.tmHeight*3/2 - 20)/2, 20, 20, SWP_NOZORDER);

			return 0;
		}
		case WM_PAINT:
		{
			ep_settings *eps = (ep_settings*) GetWindowLongPtr(hwnd, GWLP_USERDATA);

			InvalidateRect(eps->hwndBtn, NULL, FALSE);
			InvalidateRect(eps->hwndHeader, NULL, FALSE);
			PAINTSTRUCT ps;
			HDC hdc;
			HBRUSH hbr = GetStockBrush(WHITE_BRUSH);

			hdc = BeginPaint(hwnd, &ps);
			RECT rc;
			GetClientRect(hwnd, &rc);
			RECT r;
			CopyRect(&r, &rc);
			r.bottom = r.top + tm.tmHeight*3/2;
			FillRect(hdc, &r, hbr);
			CopyRect(&r, &rc);
			r.top += tm.tmHeight*3/2;
			r.right = 16;
			FillRect(hdc, &r, hbr);

			RECT rContents;
			GetClientRect(eps->contents, &rContents);
			CopyRect(&r, &rc);
			r.left = rContents.right;
			r.top += tm.tmHeight*3/2;
			FillRect(hdc, &r, hbr);

			EndPaint(hwnd, &ps);
			InvalidateRect(eps->contents, NULL, FALSE);
			UpdateWindow(eps->contents);
			return 0;
		}
		case WM_DESTROY: {
			ep_settings *eps = (ep_settings*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			TCHAR name[256];
			GetWindowText(hwnd, name, ARRAYSIZE(name));
			SaveDebugKey(name, REG_DWORD, &eps->ExpandState);
			IDArray[eps->lpDebugInfo->lpCalc->slot]--;
			free(eps);
		}
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}
