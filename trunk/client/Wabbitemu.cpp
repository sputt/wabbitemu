// Wabbitemu.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Wabbitemu.h"
#include "Wabbitemu_h.h"
#include "shellapi.h"
#include "shobjidl.h"
#include "shlobj.h"

#define MAX_LOADSTRING 100
#define MAX_SHADES	255
#define LCD_HIGH	255

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WABBITEMU, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
	
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WABBITEMU));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WABBITEMU));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WABBITEMU);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

VOID CALLBACK TimerProc(HWND hwnd, UINT Message, UINT_PTR idEvent, DWORD dwTimer) {
	InvalidateRect(hwnd, NULL, FALSE);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	static IClassFactory *pClassFactory;
	static IWabbitemu *pWabbitemu;
	static BITMAPINFO *bi;
	switch (message)
	{
	case WM_CREATE:
	{
		CLSID IID_IWabbitemu, CLSID_Wabbitemu;
		HRESULT hr;
		int i;
		
		CLSIDFromString((LPOLESTR) L"{8cc953bc-a879-492b-ad22-a2f4dfcd0e19}", &CLSID_Wabbitemu);
		CLSIDFromString((LPOLESTR) L"{13b5c004-4377-4c94-a8f9-efc1fdaeb31c}", &IID_IWabbitemu);
	
		CoInitialize(NULL);
		
		hr = CoCreateInstance(CLSID_Wabbitemu, NULL, CLSCTX_LOCAL_SERVER, IID_IWabbitemu, (void **) &pWabbitemu);

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

		for (i = 0; i <= MAX_SHADES; i++) {
			bi->bmiColors[i].rgbRed = (0x9E*(256-(LCD_HIGH/MAX_SHADES)*i))/255;
			bi->bmiColors[i].rgbGreen = (0xAB*(256-(LCD_HIGH/MAX_SHADES)*i))/255;
			bi->bmiColors[i].rgbBlue = (0x88*(256-(LCD_HIGH/MAX_SHADES)*i))/255;				
		}	
		
		SetTimer(hWnd, 0, 30, TimerProc);
		break;
	}

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
	{
		int total = 0;
		TCHAR szText[32];
		RECT r;
		int i;
		double calc_time = 0.0;
		BYTE screen[128*64];
		
		hdc = BeginPaint(hWnd, &ps);

		pWabbitemu->DrawScreen(0, screen);
		
		StretchDIBits(
			hdc,
			//rc.left, rc.top, rc.right - rc.left,  rc.bottom - rc.top,
			0, 0, 96*2, 64*2,
			0, 0, 96, 64,
			screen,
			bi,
			DIB_RGB_COLORS,
			SRCCOPY);

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_KEYDOWN:
		pWabbitemu->KeyPress(0, wParam);
		break;
	case WM_KEYUP:
		pWabbitemu->KeyRelease(0, wParam);
		break;
	case WM_LBUTTONDOWN:
		pWabbitemu->KeyPress(0, VK_F12);
		break;
	case WM_LBUTTONUP:
		pWabbitemu->KeyRelease(0, VK_F12);
		break;
	case WM_RBUTTONDOWN:
	{
		APPENTRY AppList[96];
		DWORD Count, i;
		
		ShellExecute(NULL, L"open", L"C:\\Wabbitemu\\Release\\wabbitemu.exe", L"D:\\app\\zelda.8xk", NULL, SW_HIDE);
		Sleep(1000);
		pWabbitemu->GetAppList(0, AppList, &Count);
		
		for (i = 0; i < Count; i++) {
			if (strncmp((const char *) AppList[i].name, "Zelda", 5) == 0) {
				pWabbitemu->SetBreakpoint(0, hWnd, FALSE, AppList[i].page, 0x416A);
			}
		}

		break;
	}
	case WM_USER:
		Sleep(5000);
		pWabbitemu->ClearBreakpoint(0, 0, wParam, lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
