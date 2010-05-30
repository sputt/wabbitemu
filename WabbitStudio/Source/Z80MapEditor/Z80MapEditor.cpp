// Z80MapEditor.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>
#include <strsafe.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include <crtdbg.h>
#include "Z80MapEditor.h"

#include "MapView.h"
#include "MapHierarchy.h"
#include "MapObjects.h"
#include "TilesToolbar.h"
#include "ObjectToolbar.h"
#include "ObjectGraphics.h"
#include "SaveFile.h"
#include "SaveFile.h"
#include "afxres.h"
#include "resource.h"
#include "spasm.h"
#include "Fonts.h"
#include "ObjectProperties.h"
#include "ObjectSelection.h"
#include "ObjectIDs.h"
#include "Layers.h"
#include "AnimatedTiles.h"
#include "Wabbitemu.h"
#include "DrawQueue.h"
#include "StartLocation.h"
#include "MapSetProperties.h"
#include "MiscProperties.h"
#include "Misc.h"
#include "ObjectOrder.h"
#include "utils.h"
#include "UndoRedo.h"

#if defined(_M_IX86)
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#define MAIN_CLASS _T("ZME_Main")

extern MAPSETSETTINGS g_MapSet;
extern LAYER g_Layer;

HINSTANCE g_hInstance;
HWND hwndMain;
HWND hwndHierarchy;
HWND hwndTiles, hwndObjects;
HWND hwndStatus;
HWND hwndToolbar;

HRESULT LoadMapDefaults(TCHAR *szFilename, LPMAPVIEWSETTINGS lpmvs);


static void UpdateWindowText(HWND hwnd) {
	TCHAR szTitle[MAX_PATH];
	StringCbPrintf(szTitle, sizeof(szTitle), _T("%s%s - Z80 Map Editor"), g_MapSet.szName, g_MapSet.fDirty ? _T("*") : _T(""));
	SetWindowText(hwnd, szTitle);
}

LRESULT SendMouseMove(HWND hwnd) {
	POINT pt;
	GetCursorPos(&pt);
	MapWindowPoints(NULL, hwnd, &pt, 1);
	return SendMessage(hwnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
}

static HRESULT LoadMapHierarchy(HWND hwndParent, LPCTSTR lpszFileName) {
	DestroyWindow(hwndHierarchy);
	hwndHierarchy = CreateMapHierarchy(hwndParent);

	if (LoadMapSet(hwndHierarchy, lpszFileName) == TRUE) {
		SendMessage(hwndParent, WM_SIZE, 0, 0);
		SendMessage(hwndHierarchy, WM_MOUSEWHEEL, 0, NULL);
		ShowWindow(hwndTiles, SW_SHOW);
		InvalidateRect(hwndTiles, NULL, FALSE);
		InvalidateRect(hwndObjects, NULL, FALSE);
		UpdateWindowText(hwndParent);
		RECT rc, wr;
		GetClientRect(hwndParent, &rc);
		GetWindowRect(hwndHierarchy, &wr);
		SetWindowPos(hwndHierarchy, NULL, 
			((rc.right - rc.left) - (wr.right - wr.left)) / 2,
			((rc.bottom - rc.top) - (wr.bottom - wr.top)) / 2, 
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
		return S_OK;
	} else {
		MessageBox(NULL, _T("Failed to open the map for some reason.\n"), _T("Z80MapEditor"), MB_OK | MB_ICONERROR);
		return E_FAIL;
	}
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static IDropTarget *pDropTarget = NULL;

	switch (uMsg)
	{
	case WM_CREATE:
		{
			void RegisterDropWindow(HWND hwnd, IDropTarget **ppDropTarget);
			//RegisterDropWindow(hwnd, &pDropTarget);

			CreateFonts(hwnd, 8, NULL);
			ReadObjectGraphics(_T("graphics.asm"));

			extern OBJECTTYPE ObjectTypes[256];
			extern DWORD ObjectTypeCount;
			LoadObjectTypes(_T("objectdef.inc"), ObjectTypes, &ObjectTypeCount);

			extern OBJECTTYPE EnemyTypes[256];
			extern DWORD EnemyTypeCount;
			LoadObjectTypes(_T("enemydef.inc"), EnemyTypes, &EnemyTypeCount);

			LoadMiscTypes(_T("miscdef.inc"));
			InitObjectIDs(_T("project\\objects.asm"));
			InitAnimateKeys(_T("animatedef.inc"));
			LoadAnimateTypes(_T("animatedef.inc"));

			hwndStatus = CreateWindow(STATUSCLASSNAME, _T("Status bar"), SBARS_SIZEGRIP | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, g_hInstance, NULL);
			int iParts[] = {200, 300, -1};
			SendMessage(hwndStatus, SB_SETPARTS, sizeof(iParts) / sizeof(iParts[0]), (LPARAM) iParts);

			TBBUTTON tbButtons[] = {
				{MAKELONG(9, 0), ID_FILE_NEW, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR) _T("New")},
				{MAKELONG(8, 0), ID_FILE_OPEN, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR) _T("Open")},
				{MAKELONG(7, 0), ID_FILE_SAVE, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR) _T("Save")},

				{ MAKELONG(0, 0), 0, TBSTATE_ENABLED,
				TBSTYLE_SEP, {0}, NULL, NULL},

				{MAKELONG(0, 0), ID_LAYER_MAPOVERVIEW, 	TBSTATE_ENABLED, BTNS_AUTOSIZE | BTNS_CHECKGROUP, {0}, 0, (INT_PTR) _T("Mapset")},
				{MAKELONG(1, 0), ID_LAYER_MAP, 			TBSTATE_ENABLED, BTNS_AUTOSIZE | BTNS_CHECKGROUP,  {0}, 0, (INT_PTR) _T("Map")},
				{MAKELONG(2, 0), ID_LAYER_OBJECT, 			TBSTATE_ENABLED,BTNS_AUTOSIZE |  BTNS_CHECKGROUP,  {0}, 0,(INT_PTR) _T("Object")},
				{MAKELONG(3, 0), ID_LAYER_ENEMY, 			TBSTATE_ENABLED,BTNS_AUTOSIZE |  BTNS_CHECKGROUP,  {0}, 0, (INT_PTR) _T("Enemy")},
				{MAKELONG(4, 0), ID_LAYER_MISC, 			TBSTATE_ENABLED, BTNS_AUTOSIZE | BTNS_CHECKGROUP,  {0}, 0, (INT_PTR) _T("Misc")},
				{MAKELONG(5, 0), ID_LAYER_STARTLOCATIONS,	TBSTATE_ENABLED, BTNS_AUTOSIZE | BTNS_CHECKGROUP,  {0}, 0, (INT_PTR) _T("Start")},

				{ MAKELONG(0, 0), 0, TBSTATE_ENABLED,
				TBSTYLE_SEP, {0}, NULL, NULL},

				{MAKELONG(6, 0), ID_MAP_TESTMAP, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR) _T("Test")},
			};

			hwndToolbar = CreateWindow(
				TOOLBARCLASSNAME, 
				NULL, 
				WS_CHILD | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | TBSTYLE_TRANSPARENT | TBSTYLE_WRAPABLE, 
				0, 0, 0, 0, 
				hwnd, NULL, g_hInstance, NULL);

			SendMessage(hwndToolbar, CCM_SETVERSION, (WPARAM) 5, 0); 

			// Load the image list
			HIMAGELIST hImageList = ImageList_LoadImage(g_hInstance, MAKEINTRESOURCE(IDB_LAYERS), 16, 0, 
				RGB(168,230, 29), IMAGE_BITMAP, LR_CREATEDIBSECTION);
			SendMessage(hwndToolbar, TB_SETIMAGELIST, 0, (LPARAM) hImageList);

			SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
			SendMessage(hwndToolbar, TB_ADDBUTTONS, ARRAYSIZE(tbButtons), (LPARAM) &tbButtons);
			SendMessage(hwndToolbar, TB_SETMAXTEXTROWS, 0, NULL);

			SendMessage(hwndToolbar, TB_AUTOSIZE, 0, NULL);
			ShowWindow(hwndToolbar, SW_SHOW);

			hwndTiles = CreateTilesToolbar(hwnd);
			hwndObjects = CreateObjectToolbar(hwnd);
			InvalidateRect(hwnd, NULL, FALSE);

			SetEditorLayer(MISC_LAYER);
			return 0;
		}
		
	case WM_SIZE:
		{
			SendMessage(hwndToolbar, TB_AUTOSIZE, 0, NULL);
			SendMessage(hwndStatus, WM_SIZE, wParam, lParam);
			SizeTilesToolbar(hwndTiles);
			SizeObjectToolbar(hwndObjects);

			return 0;
		}
		
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_OPEN:
			{
				OPENFILENAME ofn;
				TCHAR *szFilename = (TCHAR *) malloc(MAX_PATH);

				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				StringCbCopy(szFilename, sizeof(szFilename), _T(""));
				ofn.lpstrFilter = _T("Map Sets (*.xml)\0*.xml\0All Files (*.*)\0*.*\0");
				ofn.lpstrFile = szFilename;
				ofn.nMaxFile = MAX_PATH;
				ofn.hInstance = g_hInstance;
				ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

				if (GetOpenFileName(&ofn) == TRUE) {
					LoadMapHierarchy(hwnd, szFilename);
				}
				free(szFilename);
				return 0;
			}
		case ID_FILE_SAVE:
			{
				FlattenMapTree(hwndHierarchy, (LPMAPVIEWSETTINGS **) &g_MapSet.MapHierarchy, &g_MapSet.cxMapHierarchy, &g_MapSet.cyMapHierarchy);

				SaveMapSet(g_MapSet.szFilename, &g_MapSet);

				FILE *file = _tfopen(_T("map_hierarchy.asm"), _T("w"));
				_ftprintf(file, _T("#if pageof($) = 1\nmap_hier_width = %d\n#else\n"), g_MapSet.cxMapHierarchy);
				for (int row = 0; row < g_MapSet.cyMapHierarchy; row++) {
					_ftprintf(file, _T(".db "));
					for (int col = 0; col < g_MapSet.cxMapHierarchy; col++) {
						int Index = 0;
						if (g_MapSet.MapHierarchy[row * g_MapSet.cxMapHierarchy + col] != NULL) {
							MAPVIEWSETTINGS mvs;
							GetMapViewSettings((HWND) g_MapSet.MapHierarchy[row * g_MapSet.cxMapHierarchy + col], &mvs);
							Index = mvs.Index;
						}
						_ftprintf(file, _T("%02d"), Index);
						if (col != g_MapSet.cxMapHierarchy - 1) {
							_ftprintf(file, _T(","));
						}
					}
					_ftprintf(file, _T("\n"));
				}
				_ftprintf(file, _T("#endif\n"));
				fclose(file);
				return 0;
			}
		case ID_FILE_PROPERTIES:
			CreateMapSetProperties(hwnd);
			return 0;

		case ID_EDIT_UNDO:
			RestoreUndoStep();
			return 0;

		// Layer switching
		case ID_LAYER_STARTLOCATIONS:
			SaveMapSet(g_MapSet.szFilename, &g_MapSet);
		case ID_LAYER_MAPOVERVIEW:
		case ID_LAYER_MAP:
		case ID_LAYER_OBJECT:
		case ID_LAYER_ENEMY:
		case ID_LAYER_MISC:
			SetEditorLayer((LAYER) LOWORD(wParam));
			return 0;

		case ID_SET_CHOOSESTARTLOCATION:
			{
				SaveMapSet(g_MapSet.szFilename, &g_MapSet);

				SetStartLocationBackupLayer(g_Layer);
				SetEditorLayer(START_LAYER);
				return 0;
			}

		case ID_MAP_NEW:
			{
				MAPVIEWSETTINGS prev_mvs;

				GetMapViewSettings(g_MapSet.hwndSelected, &prev_mvs);

				LPMAPVIEWSETTINGS lpmvs = (LPMAPVIEWSETTINGS) malloc(sizeof(MAPVIEWSETTINGS));
				ZeroMemory(lpmvs, sizeof(MAPVIEWSETTINGS));
				lpmvs->pMapData = (LPBYTE) malloc(g_MapSet.cx * g_MapSet.cy);
				memset(lpmvs->pMapData, (BYTE) GetSelectedTile(hwndTiles), g_MapSet.cx * g_MapSet.cy);
				lpmvs->iHot = -1;
				lpmvs->hwndMap = CreateMapView(hwndHierarchy);
				SetMapViewSettings(lpmvs->hwndMap, lpmvs);

				AddMap(&prev_mvs, lpmvs, g_MapSet.SelectedOrientation);
				SetMapViewSettings(g_MapSet.hwndSelected, &prev_mvs);
				SendMessage(hwndHierarchy, WM_MOUSEWHEEL, 0, 0);

				return 0;
			}

		case ID_MAP_LOADOBJECTS:
			{
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
					MAPVIEWSETTINGS mvs;

					GetMapViewSettings(g_MapSet.hwndSelected, &mvs);
					LoadMapDefaults(szFilename, &mvs);

					SetMapViewSettings(g_MapSet.hwndSelected, &mvs);
				}
				return 0;
			}
		
		case ID_MAP_SETTILESET:
			{
#include "SetTileset.h"
				LPMAPVIEWSETTINGS lpmvs = (LPMAPVIEWSETTINGS) malloc(sizeof(MAPVIEWSETTINGS));

				GetMapViewSettings(g_MapSet.hwndSelected, lpmvs);
				CreateSetTileset(hwnd, lpmvs);

				SetMapViewSettings(g_MapSet.hwndSelected, lpmvs);
				free(lpmvs);
				return 0;
			}

		case ID_MAP_PROPERTIES:
			{
#include "MapProperties.h"
				CreateMapProperties(hwnd);
				return 0;
			}

		case ID_MAP_TESTMAP:
			{
#include "Wabbitemu.h"
				if (g_MapSet.fTesting == TRUE) {
					DestroyWindow(g_MapSet.hwndTest);
					g_MapSet.fTesting = FALSE;
					UpdateAllMaps();
					return 0;
				} else {
					WCHAR wszBase[] = L"spasm.exe project\\zelda_all.asm " \
						L"mapeditor.8xk \"-Iproject;project\\scripts;project\\images;.\" -T -L -D_MAPEDITOR_TEST=1";
					WCHAR wszCommandLine[512];

					if (g_MapSet.StartMapOffset == 0) {
						StringCbCopyW(wszCommandLine, sizeof(wszCommandLine), wszBase);
					} else {
						StringCbPrintfW(wszCommandLine, sizeof(wszCommandLine), 
							L"%s -D_MAPEDITOR_MAPOFFSET=%d -D_MAPEDITOR_STARTX=%d -D_MAPEDITOR_STARTY=%d",
							wszBase, g_MapSet.StartMapOffset, g_MapSet.StartX, g_MapSet.StartY);
					}
					STARTUPINFOW si; 
					PROCESS_INFORMATION pi; 
					memset(&si, 0, sizeof(si)); 
					memset(&pi, 0, sizeof(pi)); 
	
					SetCursor(LoadCursor(NULL, IDC_WAIT));


					SECURITY_ATTRIBUTES saAttr; 
					saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
					saAttr.bInheritHandle = TRUE; 
					saAttr.lpSecurityDescriptor = NULL;

					HANDLE hStdOutRead, hStdOutWrite;
					CreatePipe(&hStdOutRead, &hStdOutWrite, &saAttr, 0);

					si.cb = sizeof(si);
					si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
					si.wShowWindow = SW_HIDE;
					si.hStdOutput = hStdOutWrite;
					si.hStdInput = hStdOutRead;

					if (!CreateProcessW(NULL, wszCommandLine, 
						&saAttr, &saAttr, TRUE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, 
						0, 0, 
						&si, &pi)) {
							ShowLastError(_T("Z80Build"));
					}

					char szBuffer[4096];
					DWORD dwBytesRead, dwTotalBytesAvail, dwBytesLeft;
					DWORD dwTotalBytesRead = 0;
					int n = 0;
					DWORD dwWaitResult;
					do
					{
						n++;
						dwWaitResult = WaitForSingleObject(pi.hProcess, 100);

						if (!PeekNamedPipe(hStdOutRead, &szBuffer[dwTotalBytesRead], sizeof(szBuffer), &dwBytesRead, &dwTotalBytesAvail, &dwBytesLeft)) {
							ShowLastError(_T("Z80Build"));
							break;
						} else if (dwBytesRead > 0) {
							ReadFile(hStdOutRead, &szBuffer[dwTotalBytesRead], dwBytesRead, &dwBytesRead, NULL);
							dwTotalBytesRead += dwBytesRead;
							szBuffer[dwTotalBytesRead] = '\0';
						}
					} while ((n < 150) && (dwWaitResult == WAIT_TIMEOUT));

					char *psz = strtok(szBuffer, "\r\n");
					while (psz != NULL) {
						OutputDebugStringA(psz);
						OutputDebugStringA("\r\n");

						psz = strtok(NULL, "\r\n");
					}

					CloseHandle(hStdOutRead);
					CloseHandle(hStdOutWrite);

					InitZeldaDrawQueue();

					CreateCalcPreview();

					SetCursor(LoadCursor(NULL, IDC_ARROW));
					SetFocus(g_MapSet.hwndRoot);

					return 0;
				}
			}

		case ID_OBJECT_ADD:
			{
				switch (g_Layer)
				{
				case MISC_LAYER:
					g_MapSet.fPendingAdd = TRUE;
					break;
				}
				return 0;
			}

		case ID_OBJECT_PROPERTIES:
			{
#include "ObjectProperties.h"

				LPVOID lpsel;
				if (GetSelectedObjectCount() == 1) {
					lpsel = GetSelectedObject(0);
					switch (g_Layer)
					{
					case OBJECT_LAYER:
					case ENEMY_LAYER:
						CreateObjectProperties(hwnd, (LPOBJECT) lpsel);
						InvalidateRect(((LPMAPVIEWSETTINGS) ((LPOBJECT) lpsel)->lpmvs)->hwndMap, NULL, FALSE);
						break;
					case MISC_LAYER:
						CreateMiscProperties(hwnd, (LPMISC) lpsel);
						break;
					}
				}
				return 0;
			}

		case ID_OBJECT_MANAGEORDER:
			{
				MAPVIEWSETTINGS mvs = {0};
				const LPMAPVIEWSETTINGS lpmvs = (LPMAPVIEWSETTINGS) GetWindowLongPtr(g_MapSet.hwndSelected, GWLP_USERDATA);
				SetEditorLayer(OBJECT_LAYER);
				ClearObjectSelection();
				GetMapViewSettings(g_MapSet.hwndSelected, &mvs);
				if (ManageObjectOrder(hwnd, lpmvs, &mvs) == IDOK) {
					SetMapViewSettings(g_MapSet.hwndSelected, &mvs);
				}
				return 0;
			}

		case ID_VIEW_SHOWGAP:
			{
				if (g_MapSet.cxMargin != 0) {
					g_MapSet.cxMargin = 0;
					CheckMenuItem(GetMenu(hwnd), ID_VIEW_SHOWGAP, MF_BYCOMMAND | MF_UNCHECKED);
				} else {
					g_MapSet.cxMargin = 5;
					CheckMenuItem(GetMenu(hwnd), ID_VIEW_SHOWGAP, MF_BYCOMMAND | MF_CHECKED);
				}
				SendMessage(hwndHierarchy, WM_MOUSEWHEEL, 0, NULL);
				SendMessage(hwndHierarchy, WM_SIZE, 0, 0);
				return 0;
			}
		case ID_VIEW_SHOWCALCPREVIEW:
			{
				break;
			}

		default:
			{
				extern DWORD MiscTypeCount;
				if (LOWORD(wParam) >= IDC_MISC_TYPE && LOWORD(wParam) < IDC_MISC_TYPE + MiscTypeCount) {
					LPMISC lpm = (LPMISC) GetSelectedObject(0);
					if (lpm->MagicNum == MISC_MAGIC_NUM) {
						lpm->MiscID = LOWORD(wParam) - IDC_MISC_TYPE;
					}
				}
				break;
			}
		}
		return 0;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rc;

			GetClientRect(hwnd, &rc);

			hdc = BeginPaint(hwnd, &ps);
			FillRect(hdc, &rc, (HBRUSH) GetSysColorBrush(COLOR_APPWORKSPACE));
			EndPaint(hwnd, &ps);
			return 0;
		}

	case WM_NOTIFY:
		{
			switch (((LPNMHDR) lParam)->code)
			{
			case TCN_SELCHANGE:
				InvalidateRect(((LPNMHDR) lParam)->hwndFrom, NULL, TRUE);
				return 0;
			}
			return 0;
		}

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hReserved, LPSTR lpCmdLine, int nCmdShow) {
	MSG Msg;
	WNDCLASSEX wcx = {0};
	RECT r = {0, 0, 1000, 750};
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

	wcx.cbSize = sizeof(wcx);
	wcx.lpszClassName = MAIN_CLASS;
	wcx.lpfnWndProc = WndProc;
	wcx.lpszMenuName = MAKEINTRESOURCE(IDR_MENUMAIN);
	wcx.hInstance = hInstance;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	
	RegisterClassEx(&wcx);

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	CoInitialize(NULL);
	OleInitialize(NULL);

	g_hInstance = hInstance;

	AddDefine("error", "0");
	AddDefine("_MAPEDITOR", "1");
	SetInputFile("objectdef.inc");
	SetOutputFile("test.bin");
	RunAssembly();

	AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW | WS_SIZEBOX, FALSE);
	hwndMain = CreateWindowEx(
		WS_EX_APPWINDOW,
		MAIN_CLASS,
		_T("Z80 Map Editor"),
		WS_OVERLAPPEDWINDOW | WS_SIZEBOX | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top,
		NULL, (HMENU) NULL,
		hInstance, NULL);

	ShowWindow(hwndMain, nCmdShow);

	int NumArgs;
	LPWSTR *wargv = CommandLineToArgvW(GetCommandLine(), &NumArgs);
	
	if (NumArgs > 1) {
		LoadMapHierarchy(hwndMain, wargv[1]);
	}

	HACCEL haccel = LoadAccelerators(g_hInstance, MAKEINTRESOURCE(IDR_ACCELERATORS));
	while (GetMessage(&Msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(hwndMain, haccel, &Msg)) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}

	void free_storage();
	free_storage();

	OleUninitialize();
	CoUninitialize();

	return (int) Msg.wParam;
}