#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <ole2.h>
#include <Shlobj.h>
#include <shobjidl.h>
#include <math.h>

#include "DropSource.h"
#include "DataObject.h"
#include "DropTarget.h"
#include "MapHierarchy.h"
#include "MapView.h"
#include "TilesToolbar.h"
#include "MapObjects.h"
#include "Fonts.h"
#include "Layers.h"

extern HINSTANCE g_hInstance;
extern HDC g_hdcTiles;
extern HWND hwndStatus;
extern LAYER g_Layer;

extern HFONT hfontTahoma;

#define OBJECT_TOOLBAR_CLASS _T("ZMEObjectsToolbar")

#define CX_MARGIN 10
#define CY_MARGIN 10
#define CX_PADDING 2

extern MAPSETSETTINGS g_MapSet;

extern OBJECTTYPE ObjectTypes[256];
extern DWORD ObjectTypeCount;

extern OBJECTTYPE EnemyTypes[256];
extern DWORD EnemyTypeCount;

static OBJECT ToolbarObjects[256];
static HWND hwndTab;

#define OBJECT_TOOLBAR_WIDTH 160

static void GetIndexRect(int Index, LPRECT lpr) {
	lpr->left = CX_MARGIN / 2;
	lpr->top = CX_MARGIN + (16 + CX_MARGIN) * Index - CX_MARGIN/2;
	lpr->right = lpr->left + OBJECT_TOOLBAR_WIDTH - CX_MARGIN * 2;
	lpr->bottom = lpr->top + 16 + CX_MARGIN;
}

void SizeObjectToolbar(HWND hwnd) {
	RECT rc, sr;
	GetClientRect(GetParent(hwnd), &rc);
	GetWindowRect(hwndStatus, &sr);

	RECT tr;
	extern HWND hwndToolbar;
	GetWindowRect(hwndToolbar, &tr);

	SetWindowPos(hwnd, HWND_TOP, rc.right - OBJECT_TOOLBAR_WIDTH, tr.bottom - tr.top, 
		OBJECT_TOOLBAR_WIDTH, rc.bottom - rc.top - (sr.bottom - sr.top) - (tr.bottom - tr.top), 0);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static int SelectedObject = -1;
	static int HotObject = -1;
	static CDropTarget *pDropTargetNull = NULL;
	
	switch (uMsg)
	{
	case WM_CREATE:
		{
			void RegisterDropWindow(HWND hwnd, IDropTarget **ppDropTarget);
			RegisterDropWindow(hwnd, (IDropTarget **) &pDropTargetNull);

			int i;
			// Create all of the objects for the toolbar
			for (i = 0; i < ObjectTypeCount + EnemyTypeCount; i++) {
				ZeroMemory(&ToolbarObjects[i], sizeof(OBJECT));
				if (i < ObjectTypeCount) {
					CreateObject(&ObjectTypes[i], 0, 0, &ToolbarObjects[i]);
					ToolbarObjects[i].y = CX_MARGIN + (16 + CX_MARGIN) * i;
				} else {
					CreateEnemy(&EnemyTypes[i - ObjectTypeCount], 0, 0, &ToolbarObjects[i]);
					ToolbarObjects[i].y = CX_MARGIN + (16 + CX_MARGIN) * (i - ObjectTypeCount);
				}

				ToolbarObjects[i].x = CX_MARGIN;
				ToolbarObjects[i].w = ToolbarObjects[i].h = 16;
				ToolbarObjects[i].z = 0;
				ToolbarObjects[i].d = 0;
			}
			return 0;
		}

	case WM_SIZE:
		{
			RECT rc, sr;

			GetClientRect(GetParent(hwnd), &rc);
			GetWindowRect(hwndStatus, &sr);

			RECT r;
			GetWindowRect(hwndTab, &r);
			
			TabCtrl_AdjustRect(hwndTab, FALSE, &r);
			MapWindowRect(NULL, GetParent(hwnd), &r);

			SetWindowPos(hwnd, NULL, r.left, r.top, r.right - r.left, r.bottom - r.top, SWP_NOZORDER);
			return 0;
		}
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			BITMAP bm;

			MAPVIEWSETTINGS mvs;

			GetMapViewSettings(g_MapSet.hwndRoot, &mvs);
			g_hdcTiles = mvs.hdcTiles;

			hdc = BeginPaint(hwnd, &ps);
			RECT rc;
			GetClientRect(hwnd, &rc);

			SelectObject(hdc, g_Fonts.hfontMessage);
			SetBkMode(hdc, TRANSPARENT);

			int nStart, nEnd;
			COLORREF cr;
			if (TabCtrl_GetCurSel(hwndTab) == 0) {
				nStart = 0;
				nEnd = ObjectTypeCount;
				cr = RGB(0, 0, 255);
			} else {
				nStart = ObjectTypeCount;
				nEnd = ObjectTypeCount + EnemyTypeCount;
				cr = RGB(250, 120, 40);
			}
			for (int j = nStart; j < nEnd; j++) {
				int i = j - nStart;
				RECT r;
				GetIndexRect(i, &r);
				if (i == HotObject || i == SelectedObject) {
					SelectObject(hdc, GetStockObject(DC_PEN));
					SetDCPenColor(hdc, GetSysColor(COLOR_BTNSHADOW));
					SelectObject(hdc, GetSysColorBrush(i == SelectedObject ? COLOR_BTNFACE : COLOR_BTNHIGHLIGHT));
				} else {
					SelectObject(hdc, GetStockObject(DC_PEN));
					SetDCPenColor(hdc, GetSysColor(COLOR_BTNHIGHLIGHT));
					SelectObject(hdc, GetSysColorBrush(HOLLOW_BRUSH));
				}

				Rectangle(hdc, r.left, r.top, r.right, r.bottom);

				DrawObject(hdc, &ToolbarObjects[j], cr);
				r.left += 30;
				DrawText(hdc, ToolbarObjects[j].lpType->szName, -1, &r, DT_VCENTER | DT_SINGLELINE);
			}

			EndPaint(hwnd, &ps);
			return 0;
		}
	case WM_MOUSEMOVE:
		{
			static int DragCount = 0;
			if (wParam != MK_LBUTTON) {
				DragCount = 0;
			} else {
				if (++DragCount > GetSystemMetrics(SM_CXDRAG)) {
					IDragSourceHelper *pDragSourceHelper;
					IDataObject *pDataObject;
					IDropSource *pDropSource;

					FORMATETC fmtetc[1] = {NULL, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
					STGMEDIUM stgmed[1] = {TYMED_HGLOBAL, {0}, 0};

					if (g_Layer == OBJECT_LAYER) {
						fmtetc[0].cfFormat = RegisterClipboardFormat(_T("ZMEObject"));
					} else {
						fmtetc[0].cfFormat = RegisterClipboardFormat(_T("ZMEEnemy"));
					}

					stgmed[0].hGlobal = GlobalAlloc(GHND, sizeof(LPOBJECTTYPE));
					LPOBJECTTYPE *ppot = (LPOBJECTTYPE *) GlobalLock(stgmed[0].hGlobal);
					if (g_Layer == ENEMY_LAYER) {
						*ppot = &EnemyTypes[SelectedObject];
					} else if (g_Layer == OBJECT_LAYER) {
						*ppot = &ObjectTypes[SelectedObject];
					}
					GlobalUnlock(stgmed[0].hGlobal);

					HRESULT CreateDataObject(FORMATETC *pFormatEtc, STGMEDIUM *pStgMedium, DWORD dwCount, IDataObject **ppDataObject);
					HRESULT CreateDropSource(IDropSource **ppDropSource);

					CreateDataObject(fmtetc, stgmed, 1, &pDataObject);
					CreateDropSource(&pDropSource);

					HRESULT hr = CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER, 
						IID_IDragSourceHelper, (LPVOID *) &pDragSourceHelper);

					if (SUCCEEDED(hr)) {
						SHDRAGIMAGE shdi;
						HBITMAP hbmDrag, hbmOld;
						HDC hdc;

						OBJECT obj = {0};
						if (g_Layer == ENEMY_LAYER) {
							CreateEnemy(&EnemyTypes[SelectedObject], 0, 0, &obj);
						} else if (g_Layer == OBJECT_LAYER) {
							CreateObject(&ObjectTypes[SelectedObject], 0, 0, &obj);
						}

						
						shdi.sizeDragImage.cx = ceil((double) obj.lpImage->cxImage * g_MapSet.Scale);
						shdi.sizeDragImage.cy = ceil((double) obj.lpImage->cyImage * g_MapSet.Scale);
						shdi.ptOffset.x = shdi.sizeDragImage.cx  / 2;
						shdi.ptOffset.y = shdi.sizeDragImage.cy / 2;
						shdi.crColorKey = RGB(255, 0, 0);

						hdc = CreateCompatibleDC(NULL);
						hbmDrag = CreateBitmap(shdi.sizeDragImage.cx, shdi.sizeDragImage.cy, 1, 32, NULL);
						hbmOld = (HBITMAP) SelectObject(hdc, hbmDrag);

						HDC hdcScale = CreateCompatibleDC(hdc);
						HBITMAP hbmScale = CreateCompatibleBitmap(hdc, obj.lpImage->cxImage, obj.lpImage->cyImage);

						SelectObject(hdcScale, hbmScale);

						SelectObject(hdc, GetStockObject(DC_BRUSH));
						SetDCBrushColor(hdc, shdi.crColorKey);
						RECT r = {0, 0, shdi.sizeDragImage.cx, shdi.sizeDragImage.cy};
						FillRect(hdc, &r, (HBRUSH) GetStockObject(DC_BRUSH));

						obj.x = obj.y = 0;
						obj.z = obj.d = 0;
						obj.w = obj.lpImage->cxImage;
						obj.h = obj.lpImage->cyImage;
						if (g_Layer == ENEMY_LAYER) {
							obj.lpType = &EnemyTypes[SelectedObject];
						} else if (g_Layer == OBJECT_LAYER) {
							obj.lpType = &ObjectTypes[SelectedObject];
						}

						DrawObject(hdcScale, &obj, RGB(0, 0, 255));

						StretchBlt(hdc, 0, 0, shdi.sizeDragImage.cx, shdi.sizeDragImage.cy, 
							hdcScale, 0, 0, obj.lpImage->cxImage, obj.lpImage->cyImage, SRCCOPY);

						DeleteObject(hbmScale);
						DeleteDC(hdcScale);

						SelectObject(hdc, hbmOld);
						DeleteDC(hdc);
						shdi.hbmpDragImage = hbmDrag;

						pDragSourceHelper->InitializeFromBitmap(&shdi, pDataObject);

						DWORD dwEffect = DROPEFFECT_NONE;
						hr = DoDragDrop(pDataObject, pDropSource, DROPEFFECT_COPY, &dwEffect);
						if (FAILED(hr)) {
							OutputDebugString(_T("Failed to drag and drop!\n"));
						}

						pDragSourceHelper->Release();
					}

					pDataObject->Release();
					pDropSource->Release();
					return 0;
				}
			}

			POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

			int OldIndex = HotObject;
			pt.y -= CX_MARGIN;
			int index = pt.y / (16 + CX_MARGIN);
			HotObject = index;

			if (TabCtrl_GetCurSel(hwndTab) == 0) {
				if (HotObject >= ObjectTypeCount)
					HotObject = -1;
			} else if (TabCtrl_GetCurSel(hwndTab) == 1) {
				if (HotObject >= EnemyTypeCount)
					HotObject = -1;
			}

			if (HotObject == OldIndex)
				return 0;

			RECT r;
			if (OldIndex != -1) {
				GetIndexRect(OldIndex, &r);
				InvalidateRect(hwnd, &r, FALSE);
			}
			if (HotObject != -1) {
				GetIndexRect(HotObject, &r);
				InvalidateRect(hwnd, &r, FALSE);
			}
			return 0;
		}
	case WM_LBUTTONDOWN:
		if (TabCtrl_GetCurSel(hwndTab) == 0)
		{
			SetEditorLayer(OBJECT_LAYER);
		}
		else
		{
			SetEditorLayer(ENEMY_LAYER);
		}
		SetCapture(hwnd);
	case WM_RBUTTONDOWN:
		{
			POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

			IDragSourceHelper *pDragSourceHelper;
			IDataObject *pDataObject;
			IDropSource *pDropSource;

			int OldIndex = SelectedObject;
			pt.y -= CX_MARGIN;
			int index = pt.y / (16 + CX_MARGIN);
			SelectedObject = index;

			if (SelectedObject >= ObjectTypeCount)
				SelectedObject = -1;

			if (SelectedObject == OldIndex)
				return 0;

			RECT r;
			if (OldIndex != -1) {
				GetIndexRect(OldIndex, &r);
				InvalidateRect(hwnd, &r, FALSE);
			}
			if (SelectedObject != -1) {
				GetIndexRect(SelectedObject, &r);
				InvalidateRect(hwnd, &r, FALSE);
			}
			return 0;
		}

	case WM_LBUTTONUP:
		ReleaseCapture();
		return 0;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

HWND CreateObjectToolbar(HWND hwndParent) {
	WNDCLASSEX wcx = {0};

	wcx.cbSize = sizeof(wcx);
	wcx.hInstance = g_hInstance;
	wcx.lpfnWndProc = WndProc;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.lpszClassName = OBJECT_TOOLBAR_CLASS;

	RegisterClassEx(&wcx);

	RECT rc;
	GetClientRect(hwndParent, &rc);

	InitCommonControls();
	hwndTab = CreateWindow(WC_TABCONTROL, _T(""),
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
		rc.right - OBJECT_TOOLBAR_WIDTH, 0, OBJECT_TOOLBAR_WIDTH, rc.bottom, hwndParent, NULL, g_hInstance, NULL);
	SendMessage(hwndTab, WM_SETFONT, (WPARAM) g_Fonts.hfontMessage, (LPARAM) TRUE);

	TCITEM tci = {0};
	tci.mask = TCIF_TEXT | TCIF_IMAGE;
	tci.iImage = -1;
	tci.pszText = _T("Objects");
	TabCtrl_InsertItem(hwndTab, 0, &tci);

	tci.pszText = _T("Enemies");
	TabCtrl_InsertItem(hwndTab, 1, &tci);

	HWND hwnd = CreateWindowEx(
		0,
		OBJECT_TOOLBAR_CLASS,
		_T("Objects"),
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
		0, 0, 350, 400,
		hwndTab,
		NULL,
		g_hInstance,
		NULL);

	return hwndTab;
}