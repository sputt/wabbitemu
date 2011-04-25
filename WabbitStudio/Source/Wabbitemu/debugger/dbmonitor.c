#include "stdafx.h"

#include "dbmonitor.h"
#include "guidebug.h"
#include "calc.h"
#include "resource.h"
#include "dbcommon.h"

#define PORT_MIN_COL_WIDTH 40
#define PORT_ROW_SIZE 15

extern HINSTANCE g_hInst;
extern HFONT hfontSegoe;
int PortHeaderWidths[4] = { 100, 90, 130, 110 };
int line_sel;
static CPU_t *port_cpu = NULL;
#define COLOR_BREAKPOINT		(RGB(230, 160, 180))
#define COLOR_SELECTION			(RGB(153, 222, 253))

CPU_t* CPU_clone(CPU_t *cpu) {
	CPU_t *new_cpu = (CPU_t *) malloc(sizeof(CPU_t));
	memcpy(new_cpu, cpu, sizeof(CPU_t));
	return new_cpu;
}

void HighlightLine(HDC hdc, RECT rc, COLORREF color) {
	HBRUSH hBrush = CreateSolidBrush(color);
	FillRect(hdc, &rc, hBrush);
}

// DoCreateHeader - creates a header control that is positioned along 
//     the top of the parent window's client area. 
// Returns the handle to the header control. 
// hwndParent - handle to the parent window.  
HWND DoCreateHeader(HWND hwndParent) {
	HWND hwndHeader;
    RECT rcParent; 
    HDLAYOUT hdl;
    WINDOWPOS wp;
 
    // Ensure that the common control DLL is loaded, and then create 
    // the header control. 
    InitCommonControls(); 
 
    if ((hwndHeader = CreateWindowEx(0, WC_HEADER, (LPCTSTR) NULL, 
            WS_CHILD | WS_BORDER | HDS_BUTTONS | HDS_HORZ, 
            0, 0, 0, 0, hwndParent, (HMENU) NULL, g_hInst, 
            (LPVOID) NULL)) == NULL) 
        return (HWND) NULL; 
 
    // Retrieve the bounding rectangle of the parent window's 
    // client area, and then request size and position values 
    // from the header control. 
    GetClientRect(hwndParent, &rcParent); 
 
    hdl.prc = &rcParent; 
    hdl.pwpos = &wp; 
    if (!Header_Layout(hwndHeader, &hdl)) 
        return (HWND) NULL; 
 
    // Set the size, position, and visibility of the header control. 
    SetWindowPos(hwndHeader, wp.hwndInsertAfter, wp.x, wp.y, 
        wp.cx, wp.cy, wp.flags | SWP_SHOWWINDOW); 
 
    return hwndHeader;
}

// DoInsertItem - inserts an item into a header control. 
// Returns the index of the new item. 
// hwndHeader - handle to the header control. 
// iInsertAfter - index of the previous item. 
// nWidth - width of the new item. 
// lpsz - address of the item string. 
int DoInsertItem(HWND hwndHeader, int iInsertAfter, int nWidth, LPTSTR lpsz) { 
    HDITEM hdi; 
    int index; 
 
    hdi.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH; 
    hdi.pszText = lpsz; 
    hdi.cxy = nWidth; 
    hdi.cchTextMax = sizeof(hdi.pszText) / sizeof(hdi.pszText[0]); 
    hdi.fmt = HDF_LEFT | HDF_STRING; 
 
    index = Header_InsertItem(hwndHeader, iInsertAfter, &hdi); 
 
    return index;
}

#define DB_CREATE 0
LRESULT CALLBACK PortMonitorDialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND hwndHeader;
	static int start_row, last_port, cyHeader;
	switch(Message) {
		case WM_INITDIALOG: {
			RECT rc, hdrRect;
			start_row = 0;
			line_sel = 0;
			hwndHeader = DoCreateHeader(hwnd);
			GetWindowRect(hwndHeader, &hdrRect);
			cyHeader = hdrRect.bottom - hdrRect.top;
			SendMessage(hwndHeader, WM_SETFONT, (WPARAM) hfontSegoe, TRUE);
			DoInsertItem(hwndHeader, 0, PortHeaderWidths[0], _T("Port Number"));
			DoInsertItem(hwndHeader, 1, PortHeaderWidths[1], _T("Value (Hex)"));
			DoInsertItem(hwndHeader, 2, PortHeaderWidths[2], _T("Value (Decimal)"));
			DoInsertItem(hwndHeader, 3, PortHeaderWidths[3], _T("Value (Binary)"));
			SendMessage(hwnd, WM_USER, DB_CREATE, 0);
			return TRUE;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDM_PORT_SETBREAKPOINT: {
					int port = line_sel;
					int i = 0;
					while (port) {
						if (lpDebuggerCalc->cpu.pio.devices[i].active)
							port--;
						i++;
					}
					lpDebuggerCalc->cpu.pio.devices[i].breakpoint = !lpDebuggerCalc->cpu.pio.devices[i].breakpoint;
					break;
				}
			}
			return FALSE;
		}
		case WM_KEYDOWN: {
			//TODO: make other params right
			if (wParam == VK_F2)
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDM_PORT_SETBREAKPOINT, 0), 0);
			return FALSE;
		}
		case WM_LBUTTONUP: {
			RECT hdrRect, rc;
			GetWindowRect(hwndHeader, &hdrRect);
			int cyHeader = hdrRect.bottom - hdrRect.top;
			int y_coord = HIWORD(lParam);
			line_sel = (y_coord - cyHeader) / PORT_ROW_SIZE + start_row;
			GetClientRect(hwnd, &rc);
			InvalidateRect(hwnd, &rc, FALSE);
		}
		case WM_MOUSEWHEEL: {
			int i, zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			WPARAM sbtype;
			if (zDelta > 0) 
				sbtype = SB_LINEUP;
			else
				sbtype = SB_LINEDOWN;

			for (i = 0; i < abs(zDelta); i += WHEEL_DELTA)
				SendMessage(hwnd, WM_VSCROLL, sbtype, 0);
			return FALSE;
		}
		case WM_NOTIFY: {
			LPNMHEADER nmheader = (LPNMHEADER) lParam;
			switch (nmheader->hdr.code) {
				case HDN_ENDTRACK: {
					LPHDITEM lphdi = nmheader->pitem;
					static BOOL in_changing = FALSE;
					HDITEM hdi = {0};
					RECT rc;

					if (in_changing) return FALSE;
					in_changing = TRUE;

 					hdi.mask = HDI_LPARAM;
					Header_GetItem(hwndHeader, nmheader->iItem, &hdi);

 					GetClientRect(hwnd, &rc);
 					if (lphdi->cxy > rc.right - rc.left)
						lphdi->cxy = rc.right - rc.left - 6;

 					if (lphdi->cxy < PORT_MIN_COL_WIDTH)
 						lphdi->cxy = PORT_MIN_COL_WIDTH;
 					PortHeaderWidths[nmheader->iItem] = lphdi->cxy;
					GetClientRect(hwnd, &rc);
					InvalidateRect(hwnd, &rc, TRUE);
					in_changing = FALSE;
					return FALSE;
				}
			}
			return FALSE;
		}
		case WM_PAINT: {
			RECT rc, hdrRect, tr;
			PAINTSTRUCT ps;
			HDC hdc, hdcDest;
			HBITMAP hbm;
			HDITEM hdi = {0};
			int iItem, iSize, max_right = 1;

			GetClientRect(hwnd, &rc);

			hdc = BeginPaint(hwnd, &ps);
			hdcDest = CreateCompatibleDC(hdc);
			hbm = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
			SelectObject(hdcDest, hbm);
			SetBkMode(hdcDest, TRANSPARENT);
			//paint the background
			FillRect(hdcDest, &rc, GetStockBrush(WHITE_BRUSH));
			SelectObject(hdcDest, hfontSegoe);
			iSize = Header_GetItemCount(hwndHeader);
			if (iSize == 0)
				return 0;

			//Set rect for each row to draw
			CopyRect(&tr, &rc);
			tr.bottom = PORT_ROW_SIZE;
			OffsetRect(&tr, 5, cyHeader);

			int i = start_row;//(rc.top - cyHeader) / PORT_ROW_SIZE;
			//OffsetRect(&tr, 0, PORT_ROW_SIZE * i);

			int end_i = (rc.bottom - cyHeader + PORT_ROW_SIZE - 1) / PORT_ROW_SIZE;

			for (int drawn = 0; i < 255 && drawn < end_i; i++) {
				if (!port_cpu->pio.devices[i].active)
					continue;
				//highlight the line if necessary
				if (lpDebuggerCalc->cpu.pio.devices[i].breakpoint) {
					tr.left = 0;
					HighlightLine(hdcDest, tr, COLOR_BREAKPOINT);
					tr.left = 5;
				}
				if (drawn == line_sel - start_row) {
					tr.left = 0;
					HighlightLine(hdcDest, tr, COLOR_SELECTION);
					tr.left = 5;
				}
				//get the data onto the bus
				port_cpu->input = TRUE;
				port_cpu->pio.devices[i].code(port_cpu, &(port_cpu->pio.devices[i]));
				//draw the item data
 				for (iItem = 0; iItem < iSize; iItem++, tr.left = tr.right) {
					Header_GetItem(hwndHeader, Header_OrderToIndex(hwndHeader, iItem), &hdi);
					TCHAR buf[256];
					tr.right = tr.left + PortHeaderWidths[iItem];
					switch (iItem) {
						case 0:
							StringCbPrintf(buf, sizeof(buf), _T("%02X"), i);
							break;
						case 1:
							StringCbPrintf(buf, sizeof(buf), _T("$%02X"), port_cpu->bus);
							break;	
						case 2:
							StringCbPrintf(buf, sizeof(buf), _T("%d"), port_cpu->bus);
							break;
						case 3:
							StringCbPrintf(buf, sizeof(buf), _T("%%%s"), byte_to_binary(port_cpu->bus)); 
							break;
					}
					DrawText(hdcDest, buf, -1, &tr, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
				}
				tr.left = 5;
 				tr.right = rc.right;
				OffsetRect(&tr, 0, PORT_ROW_SIZE);
				drawn++;
			}

			//draw lines for the headers
			for (iItem = 0; iItem < iSize; iItem++) {
				int iCol;
				Header_GetItem(hwndHeader, Header_OrderToIndex(hwndHeader, iItem), &hdi);
				iCol = (int) hdi.lParam;
				if (iCol != -1) {
					max_right += PortHeaderWidths[iItem];
					MoveToEx(hdcDest, max_right - 1, cyHeader, NULL);
					LineTo(hdcDest, max_right - 1, rc.bottom);
				}
			}
			BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcDest, 0, 0, SRCCOPY);
			DeleteObject(hbm);
			DeleteDC(hdcDest);
			EndPaint(hwnd, &ps);
			return FALSE;
		}
		case WM_VSCROLL: {
			RECT rc;
			GetClientRect(hwnd, &rc);
			int last_dev = 0;
			for (int i = 0; i < 255; i++)
				if (port_cpu->pio.devices[i].active)
					last_dev = i;
			int total_lines = (rc.bottom - cyHeader + PORT_ROW_SIZE - 1) / PORT_ROW_SIZE;
			switch (LOWORD(wParam)) {
				case SB_TOP:
					start_row = 0;
					break;
				case SB_BOTTOM:
					start_row = last_dev - total_lines;
					break;
				case SB_LINEUP:
					start_row--;
					if (start_row < 0)
						start_row = 0;
					break;
				case SB_LINEDOWN:
					if (start_row  < last_dev - total_lines)
						start_row++;
					break;
				case SB_THUMBTRACK:
					//dps->nPane = HIWORD(wParam);
					break;
				case SB_PAGEDOWN:
					start_row += total_lines;
					if (start_row > last_dev)
						start_row = last_dev - total_lines;
					break;
				case SB_PAGEUP:
					start_row -= total_lines;
					if (start_row < 0)
						start_row = 0;
					break;
			}
			rc.top = cyHeader;
			GetClientRect(hwnd, &rc);
			InvalidateRect(hwnd, &rc, FALSE);
			return FALSE;
		}
		case WM_USER: {
			switch (wParam) {
				case DB_CREATE:
					if (port_cpu != NULL)
						free(port_cpu);
					port_cpu = CPU_clone(&lpDebuggerCalc->cpu);
					break;
				case DB_UPDATE: {
					RECT rc;
					GetClientRect(hwnd, &rc);
					InvalidateRect(hwnd, &rc, FALSE);
					break;
				}
			}
			return TRUE;
		}
		case WM_CLOSE:
			DestroyWindow(hwnd);
			return FALSE;
	}
	return FALSE;
}