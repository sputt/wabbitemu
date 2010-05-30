#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <commctrl.h>
#include <strsafe.h>

#include "MapHierarchy.h"
#include "AnimatedTiles.h"
#include "MapView.h"
#include "TilesToolbar.h"
#include "Fonts.h"
#include "Layers.h"

extern HINSTANCE g_hInstance;
extern HDC g_hdcTiles;
extern HWND hwndStatus;

#define TILES_TOOLBAR_CLASS _T("ZMETilesToolbar")

#define CX_MARGIN 10
#define CY_MARGIN 10
#define CX_PADDING 2

#define TILES_IN_ROW (384/g_MapSet.cyTile)
#define TILES_SCALE	3 / 2

extern MAPSETSETTINGS g_MapSet;

static HWND hwndTab;
static int SelectedTile = -1;
static int SelectedAnimate = -1, HotAnimate = -1;

static ANIMATE ToolbarAnimates[256];

extern int g_AnimateTypeCount;
extern ANIMATETYPE g_AnimateTypes[16];


void SizeTilesToolbar(HWND hwndToolbar2) {
	RECT rc, sr;
	GetClientRect(GetParent(hwndTab), &rc);
	GetWindowRect(hwndStatus, &sr);

	RECT tr;
	extern HWND hwndToolbar;
	GetWindowRect(hwndToolbar, &tr);
	SetWindowPos(hwndTab, HWND_TOP, 0, tr.bottom - tr.top, 160, (rc.bottom - rc.top) - (sr.bottom - sr.top) - (tr.bottom - tr.top), 0);
}

int GetSelectedTile(HWND hwndToolbar) {
	return SelectedTile;
}

LPANIMATETYPE GetSelectedAnimate(HWND hwndToolbar) {
	if (SelectedAnimate == -1)
		return NULL;
	return &g_AnimateTypes[SelectedAnimate];
}

void SetSelectedTile(HWND hwndToolbar, int Index) {
	SelectedTile = Index;
	InvalidateRect(hwndToolbar, NULL, FALSE);
}


static void GetIndexRect(int Index, LPRECT lpr) {
	lpr->left = CX_MARGIN / 2;
	lpr->top = CX_MARGIN + (16 + CX_MARGIN) * Index - CX_MARGIN/2;
	lpr->right = lpr->left + 160 - CX_MARGIN * 2;
	lpr->bottom = lpr->top + 16 + CX_MARGIN;
}

void PaintTiles(HWND hwnd, HDC hdc) {
	BITMAP bm;
	int TilesCount;

	RECT r, rc;
	GetClientRect(hwnd, &rc);
	CopyRect(&r, &rc);
	r.right = CX_MARGIN;
	FillRect(hdc, &r, GetSysColorBrush(COLOR_BTNHIGHLIGHT));

	CopyRect(&r, &rc);
	r.bottom = CY_MARGIN;
	FillRect(hdc, &r, GetSysColorBrush(COLOR_BTNHIGHLIGHT));

	if (g_MapSet.hwndRoot != NULL) {
		GetObject(GetCurrentObject(g_hdcTiles, OBJ_BITMAP), sizeof(BITMAP), &bm);
		TilesCount = bm.bmHeight * bm.bmWidth / (g_MapSet.cxTile * g_MapSet.cyTile);

		CopyRect(&r, &rc);
		r.left = CX_MARGIN + (g_MapSet.cxTile * TILES_SCALE + CX_PADDING) * ((TilesCount + TILES_IN_ROW - 1) / TILES_IN_ROW);
		FillRect(hdc, &r, GetSysColorBrush(COLOR_BTNHIGHLIGHT));

		CopyRect(&r, &rc);
		r.top = CX_MARGIN + (g_MapSet.cyTile * TILES_SCALE + CX_PADDING) * TILES_IN_ROW;
		FillRect(hdc, &r, GetSysColorBrush(COLOR_BTNHIGHLIGHT));
	}

	if (g_MapSet.hwndRoot != NULL) {
		RECT rSel = {0};
		for (int col = 0; col < (TilesCount + TILES_IN_ROW - 1) / TILES_IN_ROW; col++) {
			for (int row = 0; row < TILES_IN_ROW; row++) {
				RECT tr;
				int Index = (col * TILES_IN_ROW + row);
				tr.left = CX_MARGIN + g_MapSet.cxTile * TILES_SCALE * col + CX_PADDING * col;
				tr.top = CY_MARGIN + g_MapSet.cyTile * TILES_SCALE * row + CX_PADDING * row;

				tr.right = tr.left + g_MapSet.cxTile * TILES_SCALE;
				tr.bottom = tr.top + g_MapSet.cyTile * TILES_SCALE;
				if (Index == (SelectedTile % 128)) {
					CopyRect(&rSel, &tr);
				} else {
					if (Index >= TilesCount) {
						FillRect(hdc, &tr, GetSysColorBrush(COLOR_BTNHIGHLIGHT));
					} else {
						StretchBlt(hdc, tr.left, tr.top, tr.right - tr.left, tr.bottom - tr.top, 
							g_hdcTiles, 0, g_MapSet.cyTile * Index, g_MapSet.cxTile, g_MapSet.cyTile, SRCCOPY);
					}

					int OldLeft = tr.left;
					tr.left = tr.right;
					tr.right = tr.left + CX_PADDING;
					tr.bottom += CX_PADDING;
					FillRect(hdc, &tr, GetSysColorBrush(COLOR_BTNHIGHLIGHT));

					tr.left = OldLeft;
					tr.top = tr.bottom - CX_PADDING;
					FillRect(hdc, &tr, GetSysColorBrush(COLOR_BTNHIGHLIGHT));
				}

			}
		}

		if (SelectedTile != -1) {
			InflateRect(&rSel, (rSel.right - rSel.left) / 4 + CX_PADDING * 2, (rSel.bottom - rSel.top) / 4 + CX_PADDING * 2);
			FillRect(hdc, &rSel, GetSysColorBrush(COLOR_MENUHILIGHT));

			InflateRect(&rSel, -CX_PADDING * 2, -CX_PADDING * 2);
			SelectObject(hdc, GetStockObject(DC_BRUSH));
			if (SelectedTile & 0x80) {
				SetDCBrushColor(hdc, RGB(200, 45, 45));
			} else {
				SetDCBrushColor(hdc, RGB(0, 0, 0));
			}
			FillRect(hdc, &rSel, (HBRUSH) GetStockObject(DC_BRUSH));
			StretchBlt(hdc, rSel.left, rSel.top, rSel.right - rSel.left, rSel.bottom - rSel.top, 
				g_hdcTiles, 0, g_MapSet.cyTile * (SelectedTile % 128), g_MapSet.cxTile, g_MapSet.cyTile, SRCPAINT);
		}

	}
}

void PaintAnimate(HWND hwnd, HDC hdc) {
	RECT rc;
	GetClientRect(hwnd, &rc);

	MAPVIEWSETTINGS mvs;
	if (g_MapSet.hwndSelected != NULL) {
		GetMapViewSettings(g_MapSet.hwndSelected, &mvs);
	} else {
		GetMapViewSettings(g_MapSet.hwndRoot, &mvs);
	}

	SelectObject(hdc, g_Fonts.hfontMessage);
	SetBkMode(hdc, TRANSPARENT);

	for (int i = 0; i < g_AnimateTypeCount; i++) {
		RECT r;
		GetIndexRect(i, &r);
		if (i == HotAnimate || i == SelectedAnimate) {
			SelectObject(hdc, GetStockObject(DC_PEN));
			SetDCPenColor(hdc, GetSysColor(COLOR_BTNSHADOW));
			SelectObject(hdc, GetSysColorBrush(i == SelectedAnimate ? COLOR_BTNFACE : COLOR_BTNHIGHLIGHT));
		} else {
			SelectObject(hdc, GetStockObject(DC_PEN));
			SetDCPenColor(hdc, GetSysColor(COLOR_BTNHIGHLIGHT));
			SelectObject(hdc, GetSysColorBrush(HOLLOW_BRUSH));
		}

		Rectangle(hdc, r.left, r.top, r.right, r.bottom);

		DrawAnimate(hdc, &ToolbarAnimates[i], &mvs, 1.0);
		r.left += 30;
		DrawText(hdc, g_AnimateTypes[i].szName, -1, &r, DT_VCENTER | DT_SINGLELINE);
	}
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	
	switch (uMsg)
	{
	case WM_CREATE:
		// Create all of the animated tiles for the toolbar
		for (int i = 0; i < g_AnimateTypeCount; i++) {
			ZeroMemory(&ToolbarAnimates[i], sizeof(ANIMATE));
			CreateAnimate(&g_AnimateTypes[i], 0, 0, &ToolbarAnimates[i]);

			ToolbarAnimates[i].x = CX_MARGIN;
			ToolbarAnimates[i].y = CX_MARGIN + (16 + CX_MARGIN) * i;
			ToolbarAnimates[i].w = ToolbarAnimates[i].h = 16;
		}

		SetTimer(hwnd, 0, 25, NULL);
		return 0;
	case WM_TIMER:
		{
			if (TabCtrl_GetCurSel(hwndTab) == 1) {
				for (int i = 0; i < g_AnimateTypeCount; i++) {
					if (--ToolbarAnimates[i].TimeRemaining == 0) {
						ToolbarAnimates[i].Frame = (ToolbarAnimates[i].Frame + 1) % ToolbarAnimates[i].lpType->lpKey->FrameCount;
						ToolbarAnimates[i].TimeRemaining =  ToolbarAnimates[i].lpType->lpKey->Frames[ToolbarAnimates[i].Frame].Time;
					}
				}
				InvalidateRect(hwnd, NULL, FALSE);
			}
			return 0;
		}
	case WM_SIZE:
		{
			RECT r;
			GetWindowRect(hwndTab, &r);
			OffsetRect(&r, -r.left, -r.top);
			TabCtrl_AdjustRect(hwndTab, FALSE, &r);

			SetWindowPos(hwnd, NULL, r.left, r.top, r.right - r.left, r.bottom - r.top, SWP_NOZORDER);
			//SetWindowPos(hwnd, HWND_TOP, 0, 0, CX_MARGIN + ((TilesCount + TILES_IN_ROW - 1) / TILES_IN_ROW) * (g_MapSet.cxTile * 2 + CX_PADDING) + CX_MARGIN, rc.bottom - rc.top - (sr.bottom - sr.top), 0);
			return 0;
		}
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;

			MAPVIEWSETTINGS mvs;

			if (g_MapSet.hwndSelected != NULL) {
				GetMapViewSettings(g_MapSet.hwndSelected, &mvs);
				g_hdcTiles = mvs.hdcTiles;
			} else {
				GetMapViewSettings(g_MapSet.hwndRoot, &mvs);
				g_hdcTiles = mvs.hdcTiles;
			}

			hdc = BeginPaint(hwnd, &ps);
			if (TabCtrl_GetCurSel(hwndTab) == 0) {
				PaintTiles(hwnd, hdc);
			} else {
				PaintAnimate(hwnd, hdc);
			}
	
			EndPaint(hwnd, &ps);
			return 0;
		}
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_MOUSEMOVE:
		{
			if (TabCtrl_GetCurSel(hwndTab) == 0) {
				int x = GET_X_LPARAM(lParam) - CX_MARGIN;
				int y = GET_Y_LPARAM(lParam) - CY_MARGIN;

				if (uMsg == WM_MOUSEMOVE) {
					if ((wParam & MK_LBUTTON) == 0)
						return 0;
				}
				if (g_MapSet.cxTile == 0 || g_MapSet.cyTile == 0)
					return 0;

				if (x / ((g_MapSet.cxTile * TILES_SCALE) + CX_PADDING) > TILES_IN_ROW)
					return 0;

				if ((y / (g_MapSet.cyTile * TILES_SCALE + CX_PADDING)) >= TILES_IN_ROW)
					return 0;

				int OldSelected = SelectedTile;
				SelectedTile = ((x / (g_MapSet.cxTile * TILES_SCALE + CX_PADDING)) * TILES_IN_ROW) + (y / (g_MapSet.cyTile * TILES_SCALE + CX_PADDING));

				if (uMsg != WM_MOUSEMOVE) {
					SetEditorLayer(MAP_LAYER);
				}

				if (SelectedTile != OldSelected) {
					if (uMsg == WM_MBUTTONDOWN) {
						if (SelectedTile == OldSelected % 128) {
							SelectedTile = OldSelected ^ 0x80;
						} else {
							SelectedTile |= 0x80;
						}
					}
					InvalidateRect(hwnd, NULL, FALSE);
					TCHAR szStatus[32];
					StringCbPrintf(szStatus, sizeof(szStatus), _T("Tile %d"), SelectedTile);
					SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM) szStatus);
				} else if (uMsg == WM_MBUTTONDOWN) {
					SelectedTile |= 0x80;
					InvalidateRect(hwnd, NULL, FALSE);
				}
				return 0;
			} else {
				POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

				int OldIndex = HotAnimate;
				pt.y -= CX_MARGIN;
				int index = pt.y / (16 + CX_MARGIN);
				HotAnimate = index;

				if (HotAnimate >= g_AnimateTypeCount)
					HotAnimate = -1;

				if (uMsg != WM_MOUSEMOVE) {
					SetEditorLayer(MAP_LAYER);
				}

				RECT r;
				if (SelectedAnimate != HotAnimate && uMsg == WM_LBUTTONDOWN) {
					SelectedAnimate = HotAnimate;
				} else if (HotAnimate == OldIndex)
					return 0;

				if (OldIndex != -1) {
					GetIndexRect(OldIndex, &r);
					InvalidateRect(hwnd, &r, FALSE);
				}
				if (HotAnimate != -1) {
					GetIndexRect(HotAnimate, &r);
					InvalidateRect(hwnd, &r, FALSE);
				}
			}
		}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

HWND CreateTilesToolbar(HWND hwndParent) {
	WNDCLASSEX wcx = {0};

	wcx.cbSize = sizeof(wcx);
	wcx.hInstance = g_hInstance;
	wcx.lpfnWndProc = WndProc;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.lpszClassName = TILES_TOOLBAR_CLASS;

	RegisterClassEx(&wcx);

	RECT rc;
	GetClientRect(hwndParent, &rc);

	InitCommonControls();
	hwndTab = CreateWindow(WC_TABCONTROL, _T(""),
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
		0, 0, 160, rc.bottom, hwndParent, NULL, g_hInstance, NULL);
	SendMessage(hwndTab, WM_SETFONT, (WPARAM) g_Fonts.hfontMessage, (LPARAM) TRUE);

	TCITEM tci = {0};
	tci.mask = TCIF_TEXT | TCIF_IMAGE;
	tci.iImage = -1;
	tci.pszText = _T("Tiles");
	TabCtrl_InsertItem(hwndTab, 0, &tci);

	tci.pszText = _T("Animated tiles");
	TabCtrl_InsertItem(hwndTab, 1, &tci);

	HWND hwnd = CreateWindowEx(
		0,
		TILES_TOOLBAR_CLASS,
		_T("Tiles"),
		WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE,
		0, 0, 350, 400,
		hwndTab,
		NULL,
		g_hInstance,
		NULL);

	return hwnd;
}