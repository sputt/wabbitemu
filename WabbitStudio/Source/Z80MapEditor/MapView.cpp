#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>
#include <strsafe.h>
#include <assert.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include "MapView.h"
#include "MapHierarchy.h"
#include "MapObjects.h"
#include "Misc.h"
#include "AnimatedTiles.h"


#include "ObjectSelection.h"
#include "TilesToolbar.h"
#include "resource.h"
#include "utils.h"
#include "spasm.h"
#include "Layers.h"
#include "DrawQueue.h"
#include "Wabbitemu.h"
#include "StartLocation.h"
#include "ObjectProperties.h"
#include "UndoRedo.h"

#define MAPVIEW_CLASS _T("ZME_MapView")
extern HINSTANCE g_hInstance;
extern HDC g_hdcTiles;
extern HWND hwndTiles, hwndStatus, hwndMain;

extern MAPSETSETTINGS g_MapSet;
extern LAYER g_Layer;

static BOOL fSelectionDrag = FALSE;
static RECT rSelection;

#define CX_MARGIN 0
LPOBJECT CheckObjectMouse(LPMAPVIEWSETTINGS lpmvs, int x, int y);
LPMISC CheckMiscMouse(LPMAPVIEWSETTINGS lpmvs, int x, int y);

LPOBJECT SelectObjectIndex(HWND hwndMap, int Index) {
	LPMAPVIEWSETTINGS lpmvs = (LPMAPVIEWSETTINGS) GetWindowLongPtr(hwndMap, GWLP_USERDATA);

	if (Index < 0)
		return NULL;

	if (g_Layer == OBJECT_LAYER) {
		AddObjectToSelection(&lpmvs->ObjectArray[Index]);
	} else {
		AddObjectToSelection(&lpmvs->EnemyArray[Index]);
	}
	return &lpmvs->ObjectArray[Index];
}


/*
 * Draws a translucent selection rectangle to HDC
 * Courtesy of Wabbitemu's debugger
 */
void DrawSelectionRect(HDC hdc, RECT *r) {
	if (r->right < r->left) {
		LONG swap = r->left;
		r->left = r->right;
		r->right = swap;
	}
	
	if (r->bottom < r->top) {
		LONG swap = r->top;
		r->top = r->bottom;
		r->bottom = swap;
	}
	
	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.AlphaFormat = 0;	
	bf.SourceConstantAlpha = 100;
	
	SelectObject(hdc, GetStockObject(DC_PEN));
	SetDCPenColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
	SelectObject(hdc, GetStockObject(NULL_BRUSH));
	
	Rectangle(hdc, r->left, r->top, r->right, r->bottom);
	
	HDC hdcSel = CreateCompatibleDC(hdc);
	HBITMAP hbmSel = CreateCompatibleBitmap(hdc, 1, 1);
	SelectObject(hdcSel, hbmSel);

	SetPixel(hdcSel, 0, 0, GetSysColor(COLOR_HIGHLIGHT));
	
	AlphaBlend(	hdc, r->left+1, r->top+1, r->right - r->left - 2, r->bottom - r->top - 2,
				hdcSel, 0, 0, 1, 1,
				bf);
	
	DeleteObject(hbmSel);
	DeleteDC(hdcSel);
}

static struct {
	double Scale;
	HBITMAP hbm;
	Bitmap *bitmap;
} DrawCache = {0.0, NULL, NULL};

static void SelectObjectsInRect(LPMAPVIEWSETTINGS lpmvs, RECT *lpRect) {
	for (int i = 0; i < MAX_OBJECT; i++) {
		RemoveObjectFromSelection(&lpmvs->ObjectArray[i]);
	}
	for (int i = 0; i < MAX_OBJECT; i++) {
		POINT pt = {lpmvs->ObjectArray[i].x + lpmvs->ObjectArray[i].w/2, lpmvs->ObjectArray[i].y + lpmvs->ObjectArray[i].h/2};
		if (PtInRect(lpRect, pt) == TRUE) {
			AddObjectToSelection(&lpmvs->ObjectArray[i]);
		}
	}
}


static int PointToIndex(LPMAPVIEWSETTINGS lpmvs, int x, int y) {
	double cxTile = ((double) g_MapSet.cxTile) * g_MapSet.Scale;
	double cyTile = ((double) g_MapSet.cyTile) * g_MapSet.Scale;

	int col = min(((double) x) / cxTile, g_MapSet.cx - 1);
	int row = min(((double) y) / cyTile, g_MapSet.cy - 1);

	return col + row * g_MapSet.cx;
}

static void IndexToRect(LPMAPVIEWSETTINGS lpmvs, int index, RECT *lpRect) {
	int col = index % g_MapSet.cx;
	int row = index / g_MapSet.cx;

	RECT rs = {g_MapSet.cxTile * col, g_MapSet.cyTile * row, 
		g_MapSet.cxTile * (col + 1), g_MapSet.cyTile * (row + 1)};

	lpRect->left = rs.left * g_MapSet.Scale;
	lpRect->right = rs.right * g_MapSet.Scale;
	lpRect->top = rs.top * g_MapSet.Scale;
	lpRect->bottom = rs.bottom * g_MapSet.Scale;
}

static void DrawMap(HDC hdcDest, LPMAPVIEWSETTINGS lpmvs, BOOL fWholeHot) {
	HDC hdc = CreateCompatibleDC(hdcDest);
	BYTE CurrentMap;

	Graphics graphics(hdcDest);

	if (DrawCache.Scale != g_MapSet.Scale) {
		DeleteBitmap(DrawCache.hbm);
		DrawCache.hbm = CreateCompatibleBitmap(hdcDest, g_MapSet.cx * g_MapSet.cxTile, g_MapSet.cy * g_MapSet.cyTile);
	}

	assert(DrawCache.hbm != NULL);
	HBITMAP hbmOld = (HBITMAP) SelectObject(hdc, DrawCache.hbm);
	SelectObject(hdc, GetStockObject(DC_BRUSH));

	LPBYTE lpMapData;
	
	if (g_MapSet.fTesting == TRUE) {
		ReadVariable("current_map", &CurrentMap, 1);
		if (g_MapSet.MapHierarchy[CurrentMap] == lpmvs->hwndMap) {
			lpMapData = GetZeldaMapData();
		} else {
			lpMapData = lpmvs->pMapData;
		}
	} else {
		lpMapData = lpmvs->pMapData;
	}
	for (int i = 0; i < g_MapSet.cx; i++) {
		for (int j = 0; j < g_MapSet.cy; j++) {
			int Index = i + j * g_MapSet.cx;
			RECT rs = {g_MapSet.cxTile * i, g_MapSet.cyTile * j, g_MapSet.cxTile * (i + 1), g_MapSet.cyTile * (j + 1)};

			if (g_MapSet.fTesting == TRUE) {
				SetDCBrushColor(hdc, RGB(0, 0, 0));
			} else {
				if (lpMapData[Index] & 0x80) {
					if (fWholeHot == TRUE || lpmvs->iHot == Index) {
						SetDCBrushColor(hdc, RGB(150, 120, 120));
					} else {
						SetDCBrushColor(hdc, RGB(200, 45, 45));
					}
				} else if (fWholeHot == TRUE || lpmvs->iHot == Index) {
					SetDCBrushColor(hdc, RGB(150, 150, 150));
				} else {
					SetDCBrushColor(hdc, RGB(0, 0, 0));
				}
			}

			FillRect(hdc, &rs, (HBRUSH) GetStockObject(DC_BRUSH));
			BitBlt(hdc, rs.left, rs.top, g_MapSet.cxTile, g_MapSet.cyTile,
				lpmvs->hdcTiles, 0, g_MapSet.cyTile * (lpMapData[Index] % 128), SRCPAINT);
		}
	}

	if (g_MapSet.fTesting == TRUE) {
		if (CurrentMap == 32) {
			CurrentMap = 0;
		}

		if (g_MapSet.MapHierarchy[CurrentMap] == lpmvs->hwndMap) {
			DrawMapAnimate(hdc, lpmvs, 1.0, TRUE);
			DrawZeldaDrawEntries(hdc);
			BYTE ScreenX, ScreenY;
			ReadVariable("screen_xc", &ScreenX, 1);
			ReadVariable("screen_yc", &ScreenY, 1);
			SelectObject(hdc, GetStockObject(DC_PEN));
			SetDCPenColor(hdc, RGB(255, 0, 0));
			SelectObject(hdc, GetStockBrush(HOLLOW_BRUSH));
			Rectangle(hdc, ScreenX, ScreenY, ScreenX+96, ScreenY+64);
		} else {
			DrawMapAnimate(hdc, lpmvs, 1.0, FALSE);
			DrawMapObjects(hdc, lpmvs);
		}
	} else {
		DrawMapAnimate(hdc, lpmvs, 1.0, FALSE);
		DrawMapObjects(hdc, lpmvs);
		DrawMapEnemies(hdc, lpmvs);
		DrawMapMisc(hdc, lpmvs);

		if (fSelectionDrag == TRUE) {
			DrawSelectionRect(hdc, &rSelection);
		}
	}
		
	if (g_MapSet.Scale > 1.5) {
		StretchBlt(hdcDest, 0, 0, (int) (g_MapSet.cx * g_MapSet.cxTile * g_MapSet.Scale), (int) (g_MapSet.cy * g_MapSet.cyTile * g_MapSet.Scale),
			hdc, 0, 0, g_MapSet.cx * g_MapSet.cxTile, g_MapSet.cy * g_MapSet.cyTile, SRCCOPY);
		SelectObject(hdc, hbmOld);
	} else {
		BitmapData *bmData = new BitmapData();
		if (DrawCache.Scale != g_MapSet.Scale) {
			if (DrawCache.bitmap != NULL)
				delete DrawCache.bitmap;
			DrawCache.bitmap = new Bitmap((int) g_MapSet.cx * g_MapSet.cxTile, (int) g_MapSet.cy * g_MapSet.cyTile, PixelFormat32bppRGB);
		}

		DrawCache.bitmap->LockBits(
			new Rect(0, 0, (int) g_MapSet.cx * g_MapSet.cxTile, (int) g_MapSet.cy * g_MapSet.cyTile),
			0, //ImageLockModeWrite | ImageLockModeRead,
			PixelFormat32bppRGB,
			bmData);

		SelectObject(hdc, hbmOld);
		GetBitmapBits(DrawCache.hbm, g_MapSet.cx * g_MapSet.cxTile * g_MapSet.cy * g_MapSet.cyTile * 4, bmData->Scan0);
		DrawCache.bitmap->UnlockBits(bmData);

		graphics.DrawImage(DrawCache.bitmap, (int) 0, (int) 0, 
			(int) (g_MapSet.cx * g_MapSet.cxTile * g_MapSet.Scale), 
			(int) (g_MapSet.cy * g_MapSet.cyTile * g_MapSet.Scale));

		delete bmData;
	}
	DeleteDC(hdc);

	if (g_MapSet.fTesting == FALSE) {
		if (g_MapSet.MapHierarchy != NULL) {
			if (g_MapSet.MapHierarchy[g_MapSet.StartMapOffset] == lpmvs->hwndMap) {
				DrawStartLocation(hdcDest, TRUE);
			}
		}
	}

	if (DrawCache.Scale != g_MapSet.Scale) {
		DrawCache.Scale = g_MapSet.Scale;
	}
}

static void ApplyMiscResize(int Edge, LPMISC lpm, int next_x, int next_y) {
	switch (Edge)
	{
	case WMSZ_LEFT:
		lpm->x = next_x;
		lpm->w = lpm->DragStartW + (lpm->DragStartX - next_x);
		break;
	case WMSZ_RIGHT:
		lpm->w = lpm->DragStartW - (lpm->DragStartX - next_x);
		break;
	case WMSZ_BOTTOM:
		lpm->h = lpm->DragStartH - (lpm->DragStartY - next_y);
		break;
	case WMSZ_TOP:
		lpm->y = next_y;
		lpm->h = lpm->DragStartH + (lpm->DragStartY - next_y);
		break;

	case WMSZ_TOPLEFT:
		ApplyMiscResize(WMSZ_LEFT, lpm, next_x, next_y);
		ApplyMiscResize(WMSZ_TOP, lpm, next_x, next_y);
		break;

	case WMSZ_TOPRIGHT:
		ApplyMiscResize(WMSZ_RIGHT, lpm, next_x, next_y);
		ApplyMiscResize(WMSZ_TOP, lpm, next_x, next_y);
		break;

	case WMSZ_BOTTOMLEFT:
		ApplyMiscResize(WMSZ_LEFT, lpm, next_x, next_y);
		ApplyMiscResize(WMSZ_BOTTOM, lpm, next_x, next_y);
		break;

	case WMSZ_BOTTOMRIGHT:
		ApplyMiscResize(WMSZ_BOTTOM, lpm, next_x, next_y);
		ApplyMiscResize(WMSZ_RIGHT, lpm, next_x, next_y);
		break;


	default:
		lpm->x = next_x;
		lpm->y = next_y;
		break;
	}

	lpm->x = max(0, lpm->x);
	lpm->y = max(0, lpm->y);
	lpm->x = min(255, lpm->x);
	lpm->y = min(255, lpm->y);

	lpm->w = max(1, lpm->w);
	lpm->w = min(256 - lpm->x, lpm->w);
	lpm->h = max(1, lpm->h);
	lpm->h = min(256 - lpm->y, lpm->h);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static BOOL fFirstDrag = FALSE;
	static BOOL fRightClickWithoutMove = TRUE;

	switch (uMsg)
	{
	case WM_CREATE:
		return 0;
	case WM_PAINT:
		{
			LPMAPVIEWSETTINGS lpmvs = (LPMAPVIEWSETTINGS) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);

			if (lpmvs != NULL) {
				DrawMap(hdc, lpmvs, 
					(g_Layer == MAP_OVERVIEW_LAYER &&
						((g_MapSet.hwndHot == hwnd) && (g_MapSet.HotOrientation == MP_NONE))) ? TRUE : FALSE);
			}
			EndPaint(hwnd, &ps);
			return 0;
		}
	case WM_MOUSEMOVE:
		{
			LPMAPVIEWSETTINGS lpmvs = (LPMAPVIEWSETTINGS) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (g_Layer == MAP_OVERVIEW_LAYER) {
				
			} else if (g_Layer == MAP_LAYER) {
				int Index = PointToIndex(lpmvs, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				if (Index != lpmvs->iHot) {
					RECT r;
					if (lpmvs->iHot != -1) {
						IndexToRect(lpmvs, lpmvs->iHot, &r);
						InflateRect(&r, 1, 1);
						InvalidateRect(hwnd, &r, FALSE);
					}
					IndexToRect(lpmvs, Index, &r);
					InvalidateRect(hwnd, &r, FALSE);
					InflateRect(&r, 1, 1);

					lpmvs->iHot = Index;
					UpdateWindow(hwnd);

					TRACKMOUSEEVENT EventTrack = {0};
					EventTrack.cbSize = sizeof(TRACKMOUSEEVENT);
					EventTrack.dwFlags = TME_LEAVE;
					EventTrack.hwndTrack = hwnd;

					TrackMouseEvent(&EventTrack);
				}
			} else if (g_Layer == MISC_LAYER) {
				LPMISC lpm = NULL;
				
				if ((lpm = CheckMiscMouse(lpmvs, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) != NULL) {
					if (GetCapture() != hwnd) {
						int Edge = GetMiscEdge(lpm, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
						SetCursor(GetCursorFromEdge(Edge));
					}
				}

				if (wParam & MK_LBUTTON) {
					for (int i = 0; i < GetSelectedObjectCount(); i++) {
						lpm = (LPMISC) GetSelectedObject(i);
						if (lpm == NULL || lpm->MagicNum != MISC_MAGIC_NUM)
							continue;

						if (fFirstDrag == TRUE) {
							lpm->DragStartX = lpm->x;
							lpm->DragStartY = lpm->y;
							lpm->DragStartW = lpm->w;
							lpm->DragStartH = lpm->h;
						}
						int next_x = lpm->DragStartX + (GET_X_LPARAM(lParam) - lpmvs->DragStartX) / g_MapSet.Scale;
						int next_y = lpm->DragStartY + (GET_Y_LPARAM(lParam) - lpmvs->DragStartY) / g_MapSet.Scale;


						// If they are holding shift, lock to the nearest 8
#define MISC_SNAP_RES	8
						if ((wParam & MK_SHIFT) == 0) {
							next_x = (next_x + (MISC_SNAP_RES/2)) / MISC_SNAP_RES * MISC_SNAP_RES;
							next_y = (next_y + (MISC_SNAP_RES/2)) / MISC_SNAP_RES * MISC_SNAP_RES;
						}

						ApplyMiscResize(lpm->EdgeDrag, lpm, next_x, next_y);

						if ((wParam & MK_SHIFT) == 0) {
							lpm->w = (lpm->w + (MISC_SNAP_RES/2)) / MISC_SNAP_RES * MISC_SNAP_RES;
							lpm->h = (lpm->h + (MISC_SNAP_RES/2)) / MISC_SNAP_RES * MISC_SNAP_RES;
						}
					}
					fFirstDrag = FALSE;
					InvalidateRect(hwnd, NULL, FALSE);
					UpdateWindow(hwnd);
				}
			} else if (g_Layer == OBJECT_LAYER || g_Layer == ENEMY_LAYER) {
				LPOBJECT lpo = NULL;
				int max_move;

				if (wParam & MK_RBUTTON)
					fRightClickWithoutMove = FALSE;

				if (fSelectionDrag == TRUE) {
					POINT p[2] = {{GET_X_LPARAM(lParam) / g_MapSet.Scale, GET_Y_LPARAM(lParam) / g_MapSet.Scale}, 
					{lpmvs->DragStartX / g_MapSet.Scale, lpmvs->DragStartY / g_MapSet.Scale}};
					//MapWindowPoints(NULL, hwnd, p, 2);
					SetRect(&rSelection, min(p[0].x, p[1].x), min(p[0].y, p[1].y), max(p[0].x, p[1].x), max(p[0].y, p[1].y));
					SelectObjectsInRect(lpmvs, &rSelection);

					InvalidateRect(hwnd, NULL, FALSE);
					UpdateWindow(hwnd);
					return 0;
				}

				if ((lpo = CheckObjectMouse(lpmvs, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))) != NULL)
					SetCursor(LoadCursor(NULL, IDC_SIZEALL));

				if ((wParam & MK_LBUTTON) && (((wParam & MK_CONTROL) == 0) && ((wParam & MK_SHIFT) == 0))) {
					int old_x, old_y;


					DWORD dwCount = GetSelectedObjectCount();
					if ((fFirstDrag == TRUE) && (dwCount == 0 || IsObjectSelected(lpo) == FALSE)) {
						ClearObjectSelection();
						if (lpo == NULL) {
							fFirstDrag = FALSE;
							fSelectionDrag = TRUE;
							SetCapture(hwnd);
						} else {
							AddObjectToSelection(lpo);
							dwCount++;
						}
					}

					for (int i = 0; i < dwCount; i++) {
						lpo = (LPOBJECT) GetSelectedObject(i);
						if (lpo == NULL)
							continue;

						if (fFirstDrag == TRUE) {
							lpo->DragStartX = lpo->x;
							lpo->DragStartY = lpo->y;
						}
						int next_x = lpo->DragStartX + (GET_X_LPARAM(lParam) - lpmvs->DragStartX) / g_MapSet.Scale;
						int next_y = lpo->DragStartY + (GET_Y_LPARAM(lParam) - lpmvs->DragStartY) / g_MapSet.Scale;

						SetCapture(hwnd);
						lpo->x = next_x;
						lpo->y = next_y;
						char szLine[256];
						CreateObjectOutput(lpo, szLine, sizeof(szLine), TRUE);

						ZOBJECT zobj = {0};
						OutputDebugStringA(szLine);
						OutputDebugStringA("\n");
						RunAssemblyWithArguments(szLine, (BYTE *) &zobj, sizeof(zobj));
						
						lpo->x = zobj.x;
						lpo->y = zobj.y;

					}

					fFirstDrag = FALSE;

					if (lpo != NULL) {
						//lpmvs->DragStartX += (lpo->x - old_x) * g_MapSet.Scale;
						//lpmvs->DragStartY += (lpo->y - old_y) * g_MapSet.Scale;
						InvalidateRect(hwnd, NULL, FALSE);
						UpdateWindow(hwnd);
					}
				}
			}
			if (wParam & MK_RBUTTON) {
				POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
				MapWindowPoints(hwnd, GetParent(hwnd), &pt, 1);
				SendMessage(GetParent(hwnd), uMsg, wParam, MAKELPARAM(pt.x, pt.y));
			}
			return 0;
		}
	case WM_MOUSELEAVE:
		{
			LPMAPVIEWSETTINGS lpmvs = (LPMAPVIEWSETTINGS) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (lpmvs->iHot != -1) {
				RECT r;
				IndexToRect(lpmvs, lpmvs->iHot, &r);
				lpmvs->iHot = -1;
				InvalidateRect(hwnd, &r, FALSE);
				UpdateWindow(hwnd);
			}
			return 0;
		}
	case WM_MBUTTONDOWN:
		{
			LPMAPVIEWSETTINGS lpmvs = (LPMAPVIEWSETTINGS) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			RECT r;

			SetFocus(hwnd);

			switch (g_Layer)
			{
			case MAP_LAYER:
				lpmvs->pMapData[lpmvs->iHot] ^= 0x80;
				IndexToRect(lpmvs, lpmvs->iHot, &r);
				InflateRect(&r, 1, 1);
				InvalidateRect(hwnd, &r, FALSE);
				UpdateWindow(hwnd);
				break;
			}

			return 0;
		}
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			LPMAPVIEWSETTINGS lpmvs = (LPMAPVIEWSETTINGS) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			SetFocus(hwnd);

			switch (g_Layer)
			{
			case MAP_LAYER:
				if (lpmvs->iHot != -1) {
					if ((wParam & MK_CONTROL) != 0) {
						int Index = PointToIndex(lpmvs, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
						SetSelectedTile(hwndTiles, lpmvs->pMapData[Index]);
						TabCtrl_SetCurSel(GetParent(hwndTiles), 0);
					} else {
						if (TabCtrl_GetCurSel(GetParent(hwndTiles)) == 0) {
							RECT r;
							for (int i = 0; i < lpmvs->AnimateCount; i++) {
								if (lpmvs->AnimateArray[i].x == ((lpmvs->iHot % g_MapSet.cx) * g_MapSet.cxTile) &&
									lpmvs->AnimateArray[i].y == ((lpmvs->iHot / g_MapSet.cy) * g_MapSet.cyTile)) {

										if (i != lpmvs->AnimateCount - 1) {
											memcpy(&lpmvs->AnimateArray[i], &lpmvs->AnimateArray[i + 1], (lpmvs->AnimateCount - 1 - i) * sizeof(lpmvs->AnimateArray[0]));
										}
										lpmvs->AnimateCount--;
										break;
								}
							}
							SaveUndoStep(hwnd);
							lpmvs->pMapData[lpmvs->iHot] = (BYTE) GetSelectedTile(hwndTiles);
							IndexToRect(lpmvs, lpmvs->iHot, &r);
							InflateRect(&r, 1, 1);
							InvalidateRect(hwnd, &r, FALSE);
							UpdateWindow(hwnd);
						} else {
							LPANIMATETYPE lpType = GetSelectedAnimate(hwndTiles);
							
							SaveUndoStep(hwnd);
							AddAnimate(lpmvs, lpType, GET_X_LPARAM(lParam) / g_MapSet.Scale, GET_Y_LPARAM(lParam) / g_MapSet.Scale);
							lpmvs->pMapData[lpmvs->iHot] = lpType->TileIndex;
						}
					}
				}
				break;

			case ENEMY_LAYER:
			case OBJECT_LAYER:
				{
					LPOBJECT lpo = NULL;
					POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

					lpmvs->DragStartX = pt.x;
					lpmvs->DragStartY = pt.y;

					if ((lpo = CheckObjectMouse(lpmvs, pt.x, pt.y)) != NULL) {
						SetCursor(LoadCursor(NULL, IDC_SIZEALL));
						fFirstDrag = TRUE;
						SaveUndoStep(hwnd);

						if (uMsg == WM_LBUTTONDBLCLK) {
							CreateObjectProperties(GetParent(hwnd), lpo);
						}
					} else {
						ClearObjectSelection();
						InvalidateRect(hwnd, NULL, FALSE);

						fFirstDrag = TRUE;
					}
					break;
				}

			case MISC_LAYER:
				{
					LPMISC lpm = NULL;
					POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

					lpmvs->DragStartX = pt.x;
					lpmvs->DragStartY = pt.y;
					

					if ((lpm = CheckMiscMouse(lpmvs, pt.x, pt.y)) != NULL) {
						int Edge = GetMiscEdge(lpm, pt.x, pt.y);
						SetCursor(GetCursorFromEdge(Edge));
						lpm->EdgeDrag = Edge;
						ClearObjectSelection();
						AddObjectToSelection(lpm);
						SaveUndoStep(hwnd);
						SetCapture(hwnd);
					} else {
						ClearObjectSelection();
						InvalidateRect(hwnd, NULL, FALSE);
					}
					fFirstDrag = TRUE;
					break;
				}
			}

			return 0;
		}
	case WM_LBUTTONUP:
		{
			LPMAPVIEWSETTINGS lpmvs = (LPMAPVIEWSETTINGS) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			SetFocus(hwnd);

			switch (g_Layer)
			{
			case MAP_OVERVIEW_LAYER:
				{
					g_MapSet.hwndSelected = hwnd;
					g_MapSet.SelectedOrientation = MP_NONE;

					InvalidateRect(GetParent(hwnd), NULL, FALSE);
					UpdateWindow(GetParent(hwnd));
					SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM) lpmvs->szName);
					// Cause a redraw of the tiles toolbar
					InvalidateRect(hwndTiles, NULL, TRUE);
					break;
				}

			case ENEMY_LAYER:
			case OBJECT_LAYER:
				{
					fFirstDrag = FALSE;

					if (GetCapture() == hwnd) {
						LPOBJECT lpo = CheckObjectMouse(lpmvs, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
						if (lpo != NULL) {
							SetCursor(LoadCursor(NULL, IDC_SIZEALL));
							lpmvs->DragStartX = -1.0;
							lpmvs->DragStartY = -1.0;
						}
						if (fSelectionDrag == TRUE) {
							fSelectionDrag = FALSE;
							SetRect(&rSelection, 0, 0, 0, 0);
							InvalidateRect(hwnd, NULL, FALSE);
						}
						ReleaseCapture();
					} else {
						LPOBJECT lpo;
						POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
						BOOL fInvalidate = FALSE;

						if (IsObjectSelected(NULL) == TRUE)
							fInvalidate = TRUE;

						if ((lpo = CheckObjectMouse(lpmvs, pt.x, pt.y)) != NULL) {
							SetCursor(LoadCursor(NULL, IDC_SIZEALL));

							if (!((wParam & MK_CONTROL) || (wParam & MK_SHIFT)))
								ClearObjectSelection();

							//if (IsObjectSelected(lpo) == FALSE) {
								fInvalidate = TRUE;

								if (IsObjectSelected(lpo) == FALSE) {
									AddObjectToSelection(lpo);
								} else {
									RemoveObjectFromSelection(lpo);
								}

								DWORD dwCount = GetSelectedObjectCount();
								if (dwCount == 1) {
									TCHAR szStatus[256];
									SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM) (lpo->lpType == NULL ? _T("") : lpo->lpType->szName));

									StringCbPrintf(szStatus, sizeof(szStatus), _T("\t%1.lf, %1.lf"), lpo->x, lpo->y);
									SendMessage(hwndStatus, SB_SETTEXT, 1, (LPARAM) szStatus);
								} else if (dwCount > 1) {
									TCHAR szStatus[256];
									StringCbPrintf(szStatus, sizeof(szStatus), _T("%d objects selected"), dwCount);
									SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM) szStatus);
								} else {
									SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM) _T(""));
								}
							//}
						} else {
							ClearObjectSelection();
							SetCursor(LoadCursor(NULL, IDC_ARROW));
						}
						if (fInvalidate == TRUE) {
							InvalidateRect(hwnd, NULL, FALSE);
							UpdateWindow(hwnd);
						}
					}
					break;
				}

			case MISC_LAYER:
				{
					if (g_MapSet.fPendingAdd == TRUE) {
						void AddMisc(LPMAPVIEWSETTINGS lpmvs, int x, int y);
						AddMisc(lpmvs, GET_X_LPARAM(lParam) / g_MapSet.Scale - 8, GET_Y_LPARAM(lParam) / g_MapSet.Scale - 8);
						g_MapSet.fPendingAdd = FALSE;
						InvalidateRect(hwnd, NULL, FALSE);
					} else {
						
						LPMISC lpm = CheckMiscMouse(lpmvs, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
						if (lpm != NULL) {
							SetCursor(LoadCursor(NULL, IDC_SIZEALL));
							AddObjectToSelection(lpm);
						} else {
							if (GetCapture() != hwnd) {
								ClearObjectSelection();
							}
						}
						ReleaseCapture();
						InvalidateRect(hwnd, NULL, FALSE);
					}
					break;
				}

			case START_LAYER:
				{
					POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
					MapWindowPoints(hwnd, GetParent(hwnd), &pt, 1);
					int MapID = SetStartLocation(pt.x, pt.y);

					if (g_MapSet.MapHierarchy[g_MapSet.StartMapOffset] != NULL) {
						InvalidateRect(g_MapSet.MapHierarchy[g_MapSet.StartMapOffset], NULL, FALSE);
					}

					LPMAPVIEWSETTINGS *lpMvsArray = NULL;
					int nWidth, nHeight;
					// Fill in the indexes
					FlattenMapTree(GetParent(hwnd), &lpMvsArray, &nWidth, &nHeight);

					for (int row = 0; row < nHeight; row++) {
						for (int col = 0; col < nWidth; col++) {
							if (lpMvsArray[row * nWidth + col] != NULL) {
								MAPVIEWSETTINGS mvs;
								GetMapViewSettings((HWND) lpMvsArray[row * nWidth + col], &mvs);
								if (MapID == mvs.Index) {
									g_MapSet.StartMapOffset = (row * nWidth) + col;
									break;
								}
							}
						}
					}
					RestoreStartLocationBackupLayer();
					break;
				}
			}
			return 0;
		}

	case WM_RBUTTONUP:
		{
			LPMAPVIEWSETTINGS lpmvs = (LPMAPVIEWSETTINGS) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

			if (fRightClickWithoutMove == TRUE) {
				switch (g_Layer)
				{
				case MISC_LAYER:
					{
						LPMISC lpm = CheckMiscMouse(lpmvs, pt.x, pt.y);
						if (lpm != NULL) {
							SetCursor(LoadCursor(NULL, IDC_SIZEALL));

							MapWindowPoints(hwnd, NULL, &pt, 1);
							if (IsObjectSelected(lpm) == FALSE) {
								ClearObjectSelection();
								AddObjectToSelection(lpm);
							}
							InvalidateRect(hwnd, NULL, FALSE);
							HMENU hmenu = GetSubMenu(LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MENUMISCCONTEXT)), 0);

							MENUITEMINFO mii = {0};
							mii.cbSize = sizeof(mii);
							mii.fMask = MIIM_SUBMENU;

							HMENU hSubMenu = CreatePopupMenu();
							extern MISCTYPE MiscTypes[256];
							extern DWORD MiscTypeCount;

							for (DWORD i = 0; i < MiscTypeCount; i++) {
								InsertMenu(hSubMenu, 0, 0, IDC_MISC_TYPE + i, MiscTypes[i].szName);
							}

							mii.hSubMenu = hSubMenu;

							SetMenuItemInfo(hmenu, 1, TRUE, &mii);

							CheckMenuItem(hSubMenu, IDC_MISC_TYPE + lpm->MiscID, MF_CHECKED);

							OnContextMenu(hwndMain, pt.x, pt.y, hmenu);
							return 0;
						}
						break;
					}
				case OBJECT_LAYER:
				case ENEMY_LAYER:
					{
						LPOBJECT lpo = CheckObjectMouse(lpmvs, pt.x, pt.y);
						if (lpo != NULL) {
							SetCursor(LoadCursor(NULL, IDC_SIZEALL));

							MapWindowPoints(hwnd, NULL, &pt, 1);
							if (IsObjectSelected(lpo) == FALSE) {
								ClearObjectSelection();
								AddObjectToSelection(lpo);
							}
							InvalidateRect(hwnd, NULL, FALSE);
							HMENU hmenu = GetSubMenu(LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MENUOBJECTCONTEXT)), 0);

							OnContextMenu(hwndMain, pt.x, pt.y, hmenu);
							return 0;
						}
						break;
					}
				default:
					break;
				}
			}

			MapWindowPoints(hwnd, GetParent(hwnd), &pt, 1);
			SendMessage(GetParent(hwnd), uMsg, wParam, MAKELPARAM(pt.x, pt.y));
			return 0;
		}
	case WM_RBUTTONDOWN:
		{
			LPMAPVIEWSETTINGS lpmvs = (LPMAPVIEWSETTINGS) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			SetFocus(hwnd);

			fRightClickWithoutMove = FALSE;
			switch (g_Layer)
			{
			case MISC_LAYER:
				{
					LPMISC lpm = CheckMiscMouse(lpmvs, pt.x, pt.y);
					if (lpm != NULL) {
						SetCursor(LoadCursor(NULL, IDC_SIZEALL));
						fRightClickWithoutMove = TRUE;
					}
					break;
				}
			case ENEMY_LAYER:
			case OBJECT_LAYER:
				{
					LPOBJECT lpo = CheckObjectMouse(lpmvs, pt.x, pt.y);
					if (lpo != NULL) {
						SetCursor(LoadCursor(NULL, IDC_SIZEALL));
						fRightClickWithoutMove = TRUE;
					}
					break;
				}
			}

			MapWindowPoints(hwnd, GetParent(hwnd), &pt, 1);
			SendMessage(GetParent(hwnd), uMsg, wParam, MAKELPARAM(pt.x, pt.y));
			return 0;
		}
	case WM_KEYDOWN:
		{
			LPMAPVIEWSETTINGS lpmvs = (LPMAPVIEWSETTINGS) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (g_MapSet.fTesting == TRUE) {
				SendMessage(g_MapSet.hwndTest, uMsg, wParam, lParam);
				return 0;
			}
			switch (g_Layer)
			{
			case MAP_OVERVIEW_LAYER:
				switch (wParam)
				{
				case VK_DELETE:
					BOOL RemoveMap(HWND hwndMap);
					g_MapSet.hwndSelected = NULL;
					InvalidateRect(GetParent(hwnd), NULL, TRUE);
					UpdateWindow(GetParent(hwnd));
					RemoveMap(hwnd);
					return 0;
				}
				return 0;

			case ENEMY_LAYER:
			case OBJECT_LAYER:
				{
					DWORD dwCount = GetSelectedObjectCount();

					if (dwCount > 0 && (wParam == VK_DELETE || wParam == VK_LEFT || wParam == VK_RIGHT || wParam == VK_UP || wParam == VK_DOWN)) {
						SaveUndoStep(hwnd);
					}

					for (int i = 0; i < dwCount; i++) {
						LPOBJECT lpo = (LPOBJECT) GetSelectedObject(i);
						if (lpo == NULL)
							break;

						if (wParam == VK_DELETE) {
							int idx;
							if (g_Layer == OBJECT_LAYER) {
								idx = lpo - lpmvs->ObjectArray;
								RemoveObjectFromSelection(lpo);
								if (idx != lpmvs->ObjectCount - 1) {
									memmove(&lpmvs->ObjectArray[idx], &lpmvs->ObjectArray[idx + 1],
										(lpmvs->ObjectCount - 1 - idx) * sizeof(OBJECT));
								}
								lpmvs->ObjectCount--;
							} else if (g_Layer == ENEMY_LAYER) {
								idx = lpo - lpmvs->EnemyArray;
								RemoveObjectFromSelection(lpo);
								if (idx != lpmvs->EnemyCount - 1) {
									memmove(&lpmvs->EnemyArray[idx], &lpmvs->EnemyArray[idx + 1],
										(lpmvs->EnemyCount - 1 - idx) * sizeof(OBJECT));
								}
								lpmvs->EnemyCount--;
							}
							for (int k = idx + 1; k < lpmvs->ObjectCount + 1; k++) {
								if (IsObjectSelected(&lpmvs->ObjectArray[k])) {
									RemoveObjectFromSelection(&lpmvs->ObjectArray[k]);
									AddObjectToSelection(&lpmvs->ObjectArray[k - 1]);
								}
							}
							i--;
							dwCount--;

						} else if (wParam == VK_LEFT || wParam == VK_RIGHT || wParam == VK_UP || wParam == VK_DOWN) {
							OBJECT TestObject = *lpo;

							for (int j = 0; j < 32; j++) {
								switch (wParam)
								{
								case VK_LEFT:
									TestObject.x -= j;
									break;
								case VK_RIGHT:
									TestObject.x += j;
									break;
								case VK_UP:
									TestObject.y -= j;
									break;
								case VK_DOWN:
									TestObject.y += j;
									break;
								}
								char szLine[256];
								CreateObjectOutput(&TestObject, szLine, sizeof(szLine), TRUE);

								ZENEMY zobj = {0};
								RunAssemblyWithArguments(szLine, (BYTE *) &zobj, sizeof(zobj));
								if (zobj.x != lpo->x || zobj.y != lpo->y) {
									lpo->x = zobj.x;
									lpo->y = zobj.y;
									break;
								}
							}

							SaveUndoStep(hwnd);
						}
					}

					InvalidateRect(hwnd, NULL, FALSE);
					UpdateWindow(hwnd);
					return 0;
				}
			case MISC_LAYER:
				{
					DWORD dwCount = GetSelectedObjectCount();

					for (int i = 0; i < dwCount; i++) {
						LPMISC lpm = (LPMISC) GetSelectedObject(i);
						if (lpm == NULL || lpm->MagicNum != MISC_MAGIC_NUM)
							continue;

						int next_x = lpm->x;
						int next_y = lpm->y;

						switch (wParam)
						{
						case VK_DELETE:
							{
								int idx = lpm - lpmvs->MiscArray;
								RemoveObjectFromSelection(lpm);
								if (idx != lpmvs->MiscCount - 1) {
									memmove(&lpmvs->MiscArray[idx], &lpmvs->MiscArray[idx + 1],
										(lpmvs->MiscCount - 1 - idx) * sizeof(MISC));
								}
								lpmvs->MiscCount--;
								lpm = NULL;
							}
							break;

						case VK_LEFT:
							next_x--;
							break;
						case VK_RIGHT:
							next_x++;
							break;
						case VK_UP:
							next_y--;
							break;
						case VK_DOWN:
							next_y++;
							break;
						}

						if (lpm != NULL) {
							ApplyMiscResize(0, lpm, next_x, next_y);
						}

						InvalidateRect(hwnd, NULL, FALSE);
						UpdateWindow(hwnd);
					}
					return 0;
				}
			}
		}
	case WM_KEYUP:
		{
			if (g_MapSet.fTesting == TRUE) {
				SendMessage(g_MapSet.hwndTest, uMsg, wParam, lParam);
				return 0;
			}
			return 0;
		}
	case WM_UPDATE:
		{
			LPMAPVIEWSETTINGS lpmvs = (LPMAPVIEWSETTINGS) GetWindowLongPtr(hwnd, GWLP_USERDATA);

			if (g_MapSet.hdwp != NULL) {
				g_MapSet.hdwp = DeferWindowPos(g_MapSet.hdwp, hwnd, NULL, 0, 0,
					(int) ((g_MapSet.cx * g_MapSet.cxTile + (g_MapSet.cx + 1) * CX_MARGIN) * g_MapSet.Scale),
					(int) ((g_MapSet.cy * g_MapSet.cyTile + (g_MapSet.cy + 1) * CX_MARGIN) * g_MapSet.Scale),
					SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
			}
			InvalidateRect(hwnd, NULL, FALSE);
			UpdateWindow(hwnd);
			return 0;
		}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

HWND CreateMapView(HWND hwndParent) {
	WNDCLASSEX wcx = {0};
	HWND hwnd;

	wcx.cbSize = sizeof(wcx);
	wcx.lpszClassName = MAPVIEW_CLASS;
	wcx.lpfnWndProc = WndProc;
	wcx.style = CS_DBLCLKS;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&wcx);

	hwnd = CreateWindow(MAPVIEW_CLASS, _T("Map View"), WS_CHILD | WS_VISIBLE, 0, 0, 100, 100, hwndParent, NULL, g_hInstance, NULL);
	return hwnd;
}

BOOL SetMapViewSettings(HWND hwndMap, LPMAPVIEWSETTINGS lpmvs) {
	LPMAPVIEWSETTINGS lpmvs_old = ((LPMAPVIEWSETTINGS) GetWindowLongPtr(hwndMap, GWLP_USERDATA));
	lpmvs->hwndMap = hwndMap;
	if (lpmvs_old != NULL) {
		memcpy(lpmvs_old, lpmvs, sizeof(MAPVIEWSETTINGS));
	} else {
		SetWindowLongPtr(hwndMap, GWLP_USERDATA, (LONG_PTR) lpmvs);
	}

	return TRUE;
}

BOOL GetMapViewSettings(HWND hwndMap, LPMAPVIEWSETTINGS lpmvs) {
	LPMAPVIEWSETTINGS lpmvs_map = (LPMAPVIEWSETTINGS) GetWindowLongPtr(hwndMap, GWLP_USERDATA);
	if (lpmvs_map == NULL)
		return FALSE;

	*lpmvs = *lpmvs_map;
	lpmvs->hwndMap = hwndMap;
	return TRUE;
}