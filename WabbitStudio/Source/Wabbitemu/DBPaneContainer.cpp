#include "stdafx.h"

#include "guidebug.h"

static INT_PTR CALLBACK RegistersDlgProc(HWND, UINT uMsg, WPARAM, LPARAM)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			return TRUE;
		}
	default:
		{
			return FALSE;
		}
	}
}


static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		{
			HWND hwndDlg = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DEBUGGER_REGISTERS), hwnd, RegistersDlgProc);
			ShowWindow(hwndDlg, SW_SHOW);
			return 0;
		}
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			RECT rc;
			GetClientRect(hwnd, &rc);

			rc.top += (rc.bottom - rc.top) / 2;
			DrawText(hdc, _T("Problem?"), -1, &rc, DT_SINGLELINE);

			EndPaint(hwnd, &ps);
			return 0;
		}
	default:
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}

}


HWND CreatePaneContainer(HWND hwndParent)
{
	static ATOM Atom = 0;

	if (Atom == 0)
	{
		WNDCLASSEX wcx = {0};
		wcx.cbSize = sizeof(WNDCLASSEX);
		wcx.lpszClassName = _T("WABBIT_PANE_CONTAINER");
		wcx.lpfnWndProc = WndProc;
		wcx.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
		
		Atom = RegisterClassEx(&wcx);
	}

	return CreateWindowEx(0, (LPCTSTR) Atom, _T("Pane Container"), 
		WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
		0, 0, 0, 0,
		hwndParent, (HMENU) ID_PANECONTAINER, 
		(HINSTANCE) GetModuleHandle(NULL), NULL);
}