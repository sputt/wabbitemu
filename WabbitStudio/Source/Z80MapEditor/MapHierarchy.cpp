#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>
#include <limits.h>
#include <ole2.h>
#include <Shlobj.h>
#include <shobjidl.h>
#include <math.h>
#include "MapHierarchy.h"

#include "ObjectSelection.h"
#include "ObjectToolbar.h"
#include "MapView.h"
#include "MapObjects.h"
#include "SaveFile.h"
#include "resource.h"
#include "DropTarget.h"
#include "Layers.h"
#include "utils.h"
#include "spasm.h"

#define MAPHIERARCHY_CLASS _T("ZME_MapHierarchy")

extern HINSTANCE g_hInstance;
extern HWND hwndStatus;
extern LAYER g_Layer;

HDC g_hdcTiles;
MAPSETSETTINGS g_MapSet = {FALSE, FALSE};

#define CX_MAPMARGIN g_MapSet.cxMargin


static BOOL CALLBACK EnumMapCount(HWND hwnd, LPARAM lParam) {
	LPINT pCount = (LPINT) lParam;
	(*pCount)++;
	return TRUE;
}

static int GetMapCount(HWND hwndHierarchy) {
	int Count = 0;
	EnumChildWindows(hwndHierarchy, EnumMapCount, (LPARAM) &Count);
	return Count;
}

static void ShiftFlatTreeRight(LPMAPVIEWSETTINGS *lpMvsArray, int nWidth, int nHeight) {
	for (int row = 0; row < nHeight; row++) {
		memmove(&lpMvsArray[nWidth * row + 1], &lpMvsArray[nWidth * row], (nWidth - 1) * sizeof(LPMAPVIEWSETTINGS));
		lpMvsArray[nWidth * row] = NULL;
	}
}

static void ShiftFlatTreeDown(LPMAPVIEWSETTINGS *lpMvsArray, int nWidth, int nHeight) {
	memmove(&lpMvsArray[nWidth], &lpMvsArray[0], (nHeight - 1) * nWidth * sizeof(LPMAPVIEWSETTINGS));
	memset(&lpMvsArray[0], 0, nWidth * sizeof(LPMAPVIEWSETTINGS));
}

static void FlattenMapTreeRecurse(HWND hwnd, LPMAPVIEWSETTINGS *lpMvsArray, int nWidth, int nHeight, int x, int y, LPINT lpShiftX, LPINT lpShiftY) {
	LPMAPVIEWSETTINGS lpmvs = (LPMAPVIEWSETTINGS) malloc(sizeof(MAPVIEWSETTINGS));
	GetMapViewSettings(hwnd, lpmvs);

	lpMvsArray[nWidth * y + x] = (LPMAPVIEWSETTINGS) lpmvs->hwndMap;

	int nShiftX = 0, nShiftY = 0;

	if (lpmvs->Above != NULL) {
		if (y + nShiftY == 0) {
			ShiftFlatTreeDown(lpMvsArray, nWidth, nHeight);
			nShiftY++;
		}

		FlattenMapTreeRecurse(lpmvs->Above, lpMvsArray, nWidth, nHeight, x + nShiftX, y - 1 + nShiftY, &nShiftX, &nShiftY);
	}

	if (lpmvs->Right != NULL) {
		FlattenMapTreeRecurse(lpmvs->Right, lpMvsArray, nWidth, nHeight, x + 1 + nShiftX, y + nShiftY, &nShiftX, &nShiftY);
	}

	if (lpmvs->Left != NULL) {
		if (x + nShiftX == 0) {
			ShiftFlatTreeRight(lpMvsArray, nWidth, nHeight);
			nShiftX++;
		}

		FlattenMapTreeRecurse(lpmvs->Left, lpMvsArray, nWidth, nHeight, x - 1 + nShiftX, y + nShiftY, &nShiftX, &nShiftY);
	}

	if (lpmvs->Below != NULL) {
		FlattenMapTreeRecurse(lpmvs->Below, lpMvsArray, nWidth, nHeight, x + nShiftX, y + 1 + nShiftY, &nShiftX, &nShiftY);
	}
	
	*lpShiftX += nShiftX;
	*lpShiftY += nShiftY;
}

void FlattenMapTree(HWND hwndHierarchy, LPMAPVIEWSETTINGS **lpMvsArray, LPINT lpWidth, LPINT lpHeight) {
	int nWidth = GetMapCount(hwndHierarchy) + 1;
	int nHeight = nWidth;
	LPMAPVIEWSETTINGS *lpmvs = (LPMAPVIEWSETTINGS *) calloc(nWidth * nHeight, sizeof(LPMAPVIEWSETTINGS));

	int ShiftX = 0, ShiftY = 0;
	FlattenMapTreeRecurse(g_MapSet.hwndRoot, lpmvs, nWidth, nHeight, 0, 0, &ShiftX, &ShiftY);
	ShiftFlatTreeRight(lpmvs, nWidth, nHeight);

	int nNewWidth = -1;
	int nNewHeight = -1;
	for (int row = 0; row < nHeight; row++) {
		int nThisWidth = -1;
		for (int col = 0; col < nWidth; col++) {
			if (lpmvs[nWidth * row + col] != NULL) {
				nThisWidth = col + 1;
			}
		}
		if (nThisWidth != -1) {
			if (nThisWidth > nNewWidth)
				nNewWidth = nThisWidth;
			nNewHeight = row + 1;
		}
	}

	*lpMvsArray = (LPMAPVIEWSETTINGS *) calloc(nNewWidth * nNewHeight, sizeof(LPMAPVIEWSETTINGS));
	for (int row = 0; row < nNewHeight; row++) {
		for (int col = 0; col < nNewWidth; col++) {
			(*lpMvsArray)[row * nNewWidth + col] = lpmvs[row * nWidth + col];
		}
	}
	free(lpmvs);
	if (lpWidth != NULL)
		*lpWidth = nNewWidth;
	if (lpHeight != NULL)
		*lpHeight = nNewHeight;
}

/*
 * Takes coordinates relative to the map hierarchy window and locates the hwnd of the map view
 * that you clicked in (or returns FALSE)
 * If there is no map at that point, it will return the closest map and the orientation from it
 */
BOOL IsMapSlot(HWND hwndHierarchy, int x, int y, HWND *phwnd, MP_ORIENTATION *pOrientation) {
	int cx = (int) ((g_MapSet.cx * g_MapSet.cxTile + CX_MAPMARGIN) * g_MapSet.Scale);
	int cy = (int) ((g_MapSet.cy * g_MapSet.cyTile + CX_MAPMARGIN) * g_MapSet.Scale);

	POINT pt = {x, y};
	HWND hwnd;

	hwnd = ChildWindowFromPoint(hwndHierarchy, pt);
	if (hwnd != hwndHierarchy) {
		if (phwnd != NULL)
			*phwnd = hwnd;
		if (pOrientation != NULL)
			*pOrientation = MP_NONE;
		return TRUE;
	}

	pt.x = x + cx;
	pt.y = y;
	hwnd = ChildWindowFromPoint(hwndHierarchy, pt);
	if (hwnd != NULL && hwnd != hwndHierarchy) {
		if (phwnd != NULL)
			*phwnd = hwnd;
		if (pOrientation != NULL)
			*pOrientation = MP_LEFT;
		return TRUE;
	}

	pt.x = x - cx;
	pt.y = y;
	hwnd = ChildWindowFromPoint(hwndHierarchy, pt);
	if (hwnd != NULL && hwnd != hwndHierarchy) {
		if (phwnd != NULL)
			*phwnd = hwnd;
		if (pOrientation != NULL)
		*pOrientation = MP_RIGHT;
		return TRUE;
	}

	pt.x = x;
	pt.y = y + cy;
	hwnd = ChildWindowFromPoint(hwndHierarchy, pt);
	if (hwnd != NULL && hwnd != hwndHierarchy) {
		if (phwnd != NULL)
			*phwnd = hwnd;
		if (pOrientation != NULL)
			*pOrientation = MP_ABOVE;
		return TRUE;
	}

	pt.x = x;
	pt.y = y - cy;
	hwnd = ChildWindowFromPoint(hwndHierarchy, pt);
	if (hwnd != NULL && hwnd != hwndHierarchy) {
		if (phwnd != NULL)
			*phwnd = hwnd;
		if (pOrientation != NULL)
			*pOrientation = MP_BELOW;
		return TRUE;
	}

	return FALSE;
}

static BOOL CALLBACK EnumMapAdjust(HWND hwnd, LPARAM lParam) {
	int dx = GET_X_LPARAM(lParam);
	int dy = GET_Y_LPARAM(lParam);

	RECT r;
	GetWindowRect(hwnd, &r);
	MapWindowRect(NULL, GetParent(hwnd), &r);

	r.left += dx;
	r.top += dy;

	SetWindowPos(hwnd, NULL, r.left, r.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	return TRUE;
}

static void AdjustMaps(HWND hwndHierarchy, HWND hwndMap, RECT *pr, double *pWindowX, double *pWindowY) {
	if (hwndMap == NULL)
		return;

	int cx = (int) ((g_MapSet.cx * g_MapSet.cxTile + (2 * CX_MAPMARGIN)) * g_MapSet.Scale);
	int cy = (int) ((g_MapSet.cy * g_MapSet.cyTile + (2 * CX_MAPMARGIN)) * g_MapSet.Scale);

	LPARAM lParam = MAKELPARAM(cx - pr->left, cy - pr->top);
	EnumChildWindows(hwndHierarchy, EnumMapAdjust, lParam);

	RECT wr;
	GetWindowRect(hwndHierarchy, &wr);
	MapWindowRect(NULL, GetParent(hwndHierarchy), &wr);

	SetWindowPos(hwndHierarchy, NULL, wr.left - (cx - pr->left), wr.top - (cy - pr->top), (pr->right + cx + (cx - pr->left)), (pr->bottom + cy + (cy - pr->top)), SWP_NOZORDER | SWP_NOREDRAW);
	if (pWindowX != NULL)
		*pWindowX -= (cx - pr->left);
	if (pWindowY != NULL)
		*pWindowY -= (cy - pr->top);
}

static void UpdateMaps(HWND hwndMap, RECT *pr, int x, int y) {
	MAPVIEWSETTINGS mvs;

	if (hwndMap == NULL)
		return;

	GetMapViewSettings(hwndMap, &mvs);	
	int cx = (int) ((g_MapSet.cx * g_MapSet.cxTile) * g_MapSet.Scale);
	int cy = (int) ((g_MapSet.cy * g_MapSet.cyTile) * g_MapSet.Scale);

	if (pr != NULL) {
		if (x < pr->left)
			pr->left = x;
		if (x + cx > pr->right)
			pr->right = x + cx;
		if (y < pr->top)
			pr->top = y;
		if (y + cy > pr->bottom)
			pr->bottom = y + cy;
	}

	if (g_MapSet.hdwp != NULL)
		g_MapSet.hdwp = DeferWindowPos(g_MapSet.hdwp, hwndMap, NULL, x, y, cx, cy, SWP_NOZORDER | SWP_NOCOPYBITS);
	else
		SetWindowPos(hwndMap, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	if (mvs.Left != NULL)
		UpdateMaps(mvs.Left, pr, x - cx - (int) (CX_MAPMARGIN * g_MapSet.Scale), y);
	if (mvs.Right != NULL)
		UpdateMaps(mvs.Right, pr, x + cx + (int) (CX_MAPMARGIN * g_MapSet.Scale), y);
	if (mvs.Above != NULL)
		UpdateMaps(mvs.Above, pr, x, y - cy - (int) (CX_MAPMARGIN * g_MapSet.Scale));
	if (mvs.Below != NULL)
		UpdateMaps(mvs.Below, pr, x, y + cy + (int) (CX_MAPMARGIN * g_MapSet.Scale));
}

static BOOL CALLBACK EnumMapUpdate(HWND hwnd, LPARAM lParam) {
	return TRUE;
}

BOOL CALLBACK EnumMapInvalidate(HWND hwnd, LPARAM lParam) {
	InvalidateRect(hwnd, NULL, FALSE);
	UpdateWindow(hwnd);
	return TRUE;
}

void UpdateAllMaps(void) {
	extern HWND hwndHierarchy;
	EnumChildWindows(hwndHierarchy, EnumMapInvalidate, NULL);
}

static void OffsetRectOrientation(RECT *pr, MP_ORIENTATION Orientation) {
	int cx = (int) ((g_MapSet.cx * g_MapSet.cxTile + CX_MAPMARGIN) * g_MapSet.Scale);
	int cy = (int) ((g_MapSet.cy * g_MapSet.cyTile + CX_MAPMARGIN) * g_MapSet.Scale);

	switch (Orientation)
	{
	case MP_LEFT:
		OffsetRect(pr, -cx, 0);
		break;
	case MP_RIGHT:
		OffsetRect(pr, cx, 0);
		break;
	case MP_ABOVE:
		OffsetRect(pr, 0, -cy);
		break;
	case MP_BELOW:
		OffsetRect(pr, 0, cy);
		break;
	}
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static MAPVIEWSETTINGS mvs;
	static POINT ptDragStart = {-1, -1};
	static double WindowX = 0.0f, WindowY = 0.0f;
	static BOOL IsZooming = FALSE;
	static double ZoomX, ZoomY;
	static CDropTarget *pDropTarget = NULL;
	switch (uMsg)
	{
	case WM_CREATE:
		{
			FORMATETC fmtetcObject = {RegisterClipboardFormat(_T("ZMEObject")), 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
			FORMATETC fmtetcEnemy = {RegisterClipboardFormat(_T("ZMEEnemy")), 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
			LPMAPVIEWSETTINGS lpmvs;
			LPMAPVIEWSETTINGS prev_lpmvs;

			void RegisterDropWindow(HWND hwnd, IDropTarget **ppDropTarget);
			RegisterDropWindow(hwnd, (IDropTarget **) &pDropTarget);

			pDropTarget->AddAcceptedFormat(&fmtetcObject);
			pDropTarget->AddAcceptedFormat(&fmtetcEnemy);

			g_MapSet.cxMargin = 5;

			//g_hdcTiles = CreateCompatibleDC(NULL);

			SetFocus(hwnd);
			return 0;
		}
	case WM_MOVE:
		{
			WindowX = (double)(short) LOWORD(lParam);
			WindowY = (double)(short) HIWORD(lParam);
			IsZooming = FALSE;
			return 0;
		}
	case WM_LBUTTONDOWN:
		{
			HWND hwndMap;
			MP_ORIENTATION Orientation;
			SetFocus(hwnd);

			switch (g_Layer)
			{
			case MAP_OVERVIEW_LAYER:
				if (IsMapSlot(hwnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), &hwndMap, &Orientation) == TRUE) {
					g_MapSet.hwndSelected = hwndMap;
					g_MapSet.SelectedOrientation = Orientation;
					InvalidateRect(hwnd, NULL, FALSE);
				}
				break;
			}
			return 0;
		}

	case WM_RBUTTONDOWN:
		ptDragStart.x = GET_X_LPARAM(lParam);
		ptDragStart.y = GET_Y_LPARAM(lParam);
		//SetFocus(hwnd);
		return 0;
	case WM_RBUTTONUP:
		ptDragStart.x = -1;
		ptDragStart.y = -1;
		ReleaseCapture();
		return 0;
	case WM_LBUTTONDBLCLK:
		{
			HWND hwndMap;
			MP_ORIENTATION Orientation;
			if (IsMapSlot(hwnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), &hwndMap, &Orientation) == TRUE) {
				OPENFILENAME ofn;
				TCHAR *szFilename = (TCHAR *) malloc(MAX_PATH);

				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				StringCbCopy(szFilename, sizeof(szFilename), _T(""));
				ofn.lpstrFilter = _T("Assembly Files (*.asm, *.z80)\0*.asm;*.z80\0All Files (*.*)\0*.*\0");
				ofn.lpstrFile = szFilename;
				ofn.nMaxFile = MAX_PATH;
				ofn.hInstance = g_hInstance;
				ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

				if (GetOpenFileName(&ofn) == TRUE) {
					MAPVIEWSETTINGS mvs_prev;
					GetMapViewSettings(hwndMap, &mvs_prev);

					LPMAPVIEWSETTINGS lpmvs = LoadMap(hwnd, szFilename, NULL, NULL);

					if (lpmvs != NULL) {
						lpmvs->hdcTiles = mvs_prev.hdcTiles;
						StringCbCopy(lpmvs->szTilesFilename, sizeof(lpmvs->szTilesFilename), mvs_prev.szTilesFilename);
						AddMap(&mvs_prev, lpmvs, Orientation);
						SetMapViewSettings(hwndMap, &mvs_prev);

						g_MapSet.hwndSelected = lpmvs->hwndMap;
						g_MapSet.SelectedOrientation = MP_NONE;
						SendMessage(hwnd, WM_MOUSEWHEEL, 0, NULL);
					}
				}
			}
			return 0;
		}
	case WM_MOUSEMOVE:
		{
			if (g_MapSet.fPendingAdd) {
				SetCursor(LoadCursor(NULL, IDC_NO));

			}
			if (wParam & MK_RBUTTON) {
				POINT ptDragEnd = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

				SetCapture(hwnd);
				WindowX += ptDragEnd.x - ptDragStart.x;
				WindowY += ptDragEnd.y - ptDragStart.y;

				SetWindowPos(hwnd, NULL, (int) WindowX, (int) WindowY, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
				IsZooming = FALSE;
				EnumChildWindows(hwnd, EnumMapInvalidate, (LPARAM) &mvs);
				UpdateWindow(hwnd);
				UpdateWindow(GetParent(hwnd));
			} else {
				if (g_Layer == MAP_OVERVIEW_LAYER) {
					RECT wrOld = {0};
					HWND hwndOld = NULL;
					if (g_MapSet.hwndHot != NULL) {
						if (g_MapSet.HotOrientation == MP_NONE) {
							hwndOld = g_MapSet.hwndHot;
						} else {
							GetWindowRect(g_MapSet.hwndHot, &wrOld);
							MapWindowRect(NULL, hwnd, &wrOld);
							OffsetRectOrientation(&wrOld, g_MapSet.HotOrientation);
						}
					}
					BOOL fIsMap = IsMapSlot(hwnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), &g_MapSet.hwndHot, &g_MapSet.HotOrientation);
					
					if (fIsMap == TRUE && g_MapSet.HotOrientation != MP_NONE) {

						RECT wrNew;
						GetWindowRect(g_MapSet.hwndHot, &wrNew);
						MapWindowRect(NULL, hwnd, &wrNew);

						OffsetRectOrientation(&wrNew, g_MapSet.HotOrientation);

						if (g_MapSet.hwndHot != NULL && memcmp(&wrOld, &wrNew, sizeof(RECT)) != 0) {
							InvalidateRect(hwnd, &wrNew, TRUE);
							if (hwndOld == NULL)
								InvalidateRect(hwnd, &wrOld, TRUE);
							else
								InvalidateRect(hwndOld, NULL, FALSE);
						}

					} else {

						if (fIsMap == TRUE) {
							InvalidateRect(g_MapSet.hwndHot, NULL, FALSE);
						} else {
							g_MapSet.hwndHot = NULL;
							g_MapSet.HotOrientation = MP_NONE;
						}
						if (hwndOld == NULL)
							InvalidateRect(hwnd, &wrOld, TRUE);
						else
							InvalidateRect(hwndOld, NULL, FALSE);
					}
					UpdateWindow(hwnd);
				}
			}
			
			return 0;
		}
	case WM_MOUSEWHEEL:
		{
			WORD fwKeys = GET_KEYSTATE_WPARAM(wParam);
			short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			double OldScale = g_MapSet.Scale;
			RECT rc;
			POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

			if (IsZooming == FALSE) {
				GetClientRect(GetParent(hwnd), &rc);
				IsZooming = TRUE;
				ZoomX = (-WindowX + (rc.right - rc.left) / 2) / g_MapSet.Scale;
				ZoomY = (-WindowY + (rc.bottom - rc.top) / 2) / g_MapSet.Scale;
			}

			g_MapSet.Scale += g_MapSet.Scale * (((double) zDelta) * 0.001);

			if (g_MapSet.Scale < 0.6)
				g_MapSet.Scale = 0.6;
			if (g_MapSet.Scale > 6.0)
				g_MapSet.Scale = 6.0;

			//g_MapSet.hdwp = NULL;
			g_MapSet.hdwp = BeginDeferWindowPos(4);

			EnumChildWindows(hwnd, EnumMapUpdate, (LPARAM) &mvs);
			RECT r = {LONG_MAX, LONG_MAX, LONG_MIN, LONG_MIN};
			RECT RootMapRect;
			GetWindowRect(g_MapSet.hwndRoot, &RootMapRect);
			MapWindowRect(NULL, hwnd, &RootMapRect);

			UpdateMaps(g_MapSet.hwndRoot, &r, 
				RootMapRect.left * (g_MapSet.Scale / OldScale), 
				RootMapRect.top * (g_MapSet.Scale / OldScale));

			WindowX -= ZoomX * (g_MapSet.Scale - OldScale);
			WindowY -= ZoomY * (g_MapSet.Scale - OldScale);
			SetWindowPos(hwnd, NULL, (int) WindowX, (int) WindowY, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOREDRAW);

			EndDeferWindowPos(g_MapSet.hdwp);
			g_MapSet.hdwp = NULL;

			AdjustMaps(hwnd, g_MapSet.hwndRoot, &r, &WindowX, &WindowY);

			InvalidateRect(hwnd, NULL, FALSE);
			InvalidateRect(GetParent(hwnd), NULL, FALSE);
			EnumChildWindows(hwnd, EnumMapInvalidate, (LPARAM) &mvs);
			UpdateWindow(hwnd);
			UpdateWindow(GetParent(hwnd));
			return 0;
		}
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rc;

			GetClientRect(hwnd, &rc);

			hdc = BeginPaint(hwnd, &ps);
			FillRect(hdc, &rc, (HBRUSH) GetSysColorBrush(COLOR_APPWORKSPACE));

			if (g_Layer == MAP_OVERVIEW_LAYER) {
				if (g_MapSet.hwndHot != NULL) {
					RECT r;
					GetWindowRect(g_MapSet.hwndHot, &r);
					MapWindowRect(NULL, hwnd, &r);
					OffsetRectOrientation(&r, g_MapSet.HotOrientation);

					SelectObject(hdc, GetStockObject(NULL_PEN));
					SelectObject(hdc, GetStockObject(DC_BRUSH));
					SetDCBrushColor(hdc, RGB(185, 185, 185));
					Rectangle(hdc, r.left, r.top, r.right, r.bottom);
				}

				if (g_MapSet.hwndSelected != NULL) {
					RECT r;
					GetWindowRect(g_MapSet.hwndSelected, &r);
					MapWindowRect(NULL, hwnd, &r);

					OffsetRectOrientation(&r, g_MapSet.SelectedOrientation);
					ExcludeClipRect(hdc, r.left, r.top, r.right, r.bottom);

					InflateRect(&r, (CX_MAPMARGIN * 2 / 3) * g_MapSet.Scale, (CX_MAPMARGIN * 2 / 3) * g_MapSet.Scale);
					r.bottom++; r.right++;

					SelectObject(hdc, GetStockObject(NULL_PEN));
					SelectObject(hdc, GetSysColorBrush(COLOR_MENUHILIGHT));
					Rectangle(hdc, r.left, r.top, r.right, r.bottom);
				}
			}
			EndPaint(hwnd, &ps);
			return 0;
		}
	case WM_CREATEOBJECT:
	case WM_CREATEENEMY:
		{
			POINT pt;
			LPMAPVIEWSETTINGS lpmvs;
			LPOBJECTTYPE lpot = (LPOBJECTTYPE) lParam;

			pt.x = GET_X_LPARAM(wParam);
			pt.y = GET_Y_LPARAM(wParam);

			MapWindowPoints(NULL, hwnd, &pt, 1);

			HWND hwndMap;
			IsMapSlot(hwnd, pt.x, pt.y, &hwndMap, NULL);

			RECT wr;
			GetWindowRect(hwndMap, &wr);
			MapWindowRect(NULL, hwnd, &wr);

			int ox = (int) (pt.x - wr.left);
			int oy = (int) (pt.y - wr.top);

			OBJECT SizeObject;
			CreateObject(lpot, 0, 0, &SizeObject);

			ox = round((double) ox / g_MapSet.Scale);
			oy = round((double) oy / g_MapSet.Scale);

			ox -= SizeObject.w / 2;
			oy -= (SizeObject.lpImage->cyImage / 2) - (SizeObject.lpImage->cyImage - SizeObject.h);

			SizeObject.x = ox;
			SizeObject.y = oy;

			SizeObject.y += SizeObject.z;
			char szLine[256];
			CreateObjectOutput(&SizeObject, szLine, sizeof(szLine), FALSE);
			ZOBJECT zobj = {0};
			RunAssemblyWithArguments(szLine, (LPBYTE) &zobj, sizeof(zobj));

			oy = zobj.y;
			ox = zobj.x;

			lpmvs = (LPMAPVIEWSETTINGS) GetWindowLongPtr(hwndMap, GWLP_USERDATA);
			int new_object;
			
			if (uMsg == WM_CREATEOBJECT) {
				new_object = AddObject(lpmvs, lpot, ox, oy);
				SetEditorLayer(OBJECT_LAYER);
			} else if (uMsg == WM_CREATEENEMY) {
				new_object = AddEnemy(lpmvs, lpot, ox, oy);
				SetEditorLayer(ENEMY_LAYER);
			}
			if (new_object != -1) {
				ClearObjectSelection();
				LPOBJECT SelectObjectIndex(HWND hwndMap, int Index);
				SelectObjectIndex(hwndMap, new_object);
				InvalidateRect(hwndMap, NULL, FALSE);
				SetFocus(hwndMap);
			}
			return 0;
		}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

BOOL AddMap(MAPVIEWSETTINGS *PrevMap, MAPVIEWSETTINGS *NewMap, MP_ORIENTATION mpOrient) {
	if (PrevMap != NULL) {
		switch (mpOrient)
		{
		case MP_LEFT:
			PrevMap->Left = NewMap->hwndMap;
			break;
		case MP_RIGHT:
			PrevMap->Right = NewMap->hwndMap;
			break;
		case MP_BELOW:
			PrevMap->Below = NewMap->hwndMap;
			break;
		case MP_ABOVE:
			PrevMap->Above  = NewMap->hwndMap;
			break;
		}
	} else {
		g_MapSet.hwndRoot = NewMap->hwndMap;
	}
	return TRUE;
}



static BOOL CALLBACK EnumMapFindParent(HWND hwnd, LPARAM lParam) {
	MAPVIEWSETTINGS mvs;
	HWND *phwnd = (HWND *) lParam;

	GetMapViewSettings(hwnd, &mvs);

	if (mvs.Left == *phwnd || mvs.Right == *phwnd  || mvs.Above == *phwnd  || mvs.Below == *phwnd) {
		*phwnd = hwnd;
		return FALSE;
	}
}

BOOL RemoveMap(HWND hwndMap) {
	MAPVIEWSETTINGS mvs;
	HWND hwndResult = hwndMap;

	GetMapViewSettings(hwndMap, &mvs);

	if (mvs.Left != NULL || mvs.Right != NULL || mvs.Above != NULL || mvs.Below != NULL)
		return FALSE;


	EnumChildWindows(GetParent(hwndMap), EnumMapFindParent, (LPARAM) &hwndResult);
	if (hwndResult == NULL)
		return FALSE;

	GetMapViewSettings(hwndResult, &mvs);
	if (mvs.Left == hwndMap)
		mvs.Left = NULL;
	else if (mvs.Right == hwndMap)
		mvs.Right = NULL;
	else if (mvs.Above == hwndMap)
		mvs.Above = NULL;
	else if (mvs.Below == hwndMap)
		mvs.Below = NULL;

	SetMapViewSettings(hwndResult, &mvs);
	DestroyWindow(hwndMap);
	return TRUE;
}


HWND CreateMapHierarchy(HWND hwndParent) {
	WNDCLASSEX wcx = {0};
	HWND hwnd;

	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_DBLCLKS;
	wcx.lpszClassName = MAPHIERARCHY_CLASS;
	wcx.lpfnWndProc = WndProc;
	wcx.hbrBackground = NULL; // (HBRUSH) (COLOR_MENUHILIGHT + 1);
	wcx.lpszMenuName = NULL;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&wcx);

	hwnd = CreateWindow(
		MAPHIERARCHY_CLASS, 
		_T("Map View"),
		WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		0, 0, 4000, 4000,
		hwndParent, NULL, 
		g_hInstance, NULL);

	return hwnd;
}