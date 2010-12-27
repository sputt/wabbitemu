#include "stdafx.h"
#include "dbmonitor.h"
#include "guidebug.h"
#include "calc.h"
#define PORT_MIN_COL_WIDTH 40
#define PORT_ROW_SIZE 20

extern HINSTANCE g_hInst;
extern HFONT hfontSegoe;
int PortHeaderWidths[4] = { 100, 90, 130, 110 };

LRESULT CALLBACK PortMonitorDialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND hwndHeader;
	static int start_row, last_port;
	switch(Message) {
		case WM_INITDIALOG: {
			
			start_row = 0;
			hwndHeader = DoCreateHeader(hwnd);
			SendMessage(hwndHeader, WM_SETFONT, (WPARAM) hfontSegoe, TRUE);
			DoInsertItem(hwndHeader, 0, PortHeaderWidths[0], _T("Port Number"));
			DoInsertItem(hwndHeader, 1, PortHeaderWidths[1], _T("Value (Hex)"));
			DoInsertItem(hwndHeader, 2, PortHeaderWidths[2], _T("Value (Decimal)"));
			DoInsertItem(hwndHeader, 3, PortHeaderWidths[3], _T("Value (Binary)"));
			return TRUE;
		}
		case WM_COMMAND: {

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
		case WM_MOUSEWHEEL: {
			int i, zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			WPARAM sbtype;
			if (zDelta > 0) 
				sbtype = SB_LINEUP;
			else
				sbtype = SB_LINEDOWN;

			for (i = 0; i < abs(zDelta); i += WHEEL_DELTA)
				SendMessage(hwnd, WM_VSCROLL, sbtype, 0);
			return 0;
		}
		case WM_PAINT: {
			RECT rc, hdrRect, tr;
			PAINTSTRUCT ps;
			HDC hdc, hdcDest;
			HBITMAP hbm;
			HDITEM hdi = {0};
			int iItem, iSize, max_right = 1, cyHeader;

			GetClientRect(hwnd, &rc);

			hdcDest = BeginPaint(hwnd, &ps);
			//paint the background
			FillRect(hdcDest, &rc, GetStockBrush(WHITE_BRUSH));
			iSize = Header_GetItemCount(hwndHeader);
			if (iSize == 0)
				return 0;
			GetWindowRect(hwndHeader, &hdrRect);
			cyHeader = hdrRect.bottom - hdrRect.top;

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
			//Set rect for each row to draw
			CopyRect(&tr, &rc);
			tr.bottom = PORT_ROW_SIZE;
			OffsetRect(&tr, 5, cyHeader);

			int i = (ps.rcPaint.top - cyHeader) / PORT_ROW_SIZE;
			OffsetRect(&tr, 0, PORT_ROW_SIZE * i);

			int end_i = (ps.rcPaint.bottom - cyHeader + PORT_ROW_SIZE - 1) / PORT_ROW_SIZE;

			for (; i < end_i; i++, OffsetRect(&tr, 0, PORT_ROW_SIZE)) {
				//draw the item data
 				for (iItem = 0; iItem < iSize; iItem++, tr.left = tr.right) {
					Header_GetItem(hwndHeader, Header_OrderToIndex(hwndHeader, iItem), &hdi);
					TCHAR buf[256];
					tr.right = tr.left + PortHeaderWidths[iItem];
					switch (iItem) {
						case 0:
							StringCbPrintf(buf, sizeof(buf), _T("%X"), i);
							DrawText(hdcDest, buf, -1, &tr, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
							break;
						case 1:
							break;
						case 2:
							break;
						case 3:
							break;
					}
				}
				tr.left = 5;
 				tr.right = rc.right;
			}
			EndPaint(hwnd, &ps);
			return FALSE;
		}
		/*case WM_VSCROLL: {
			RECT rc;
			GetClientRect(hwnd, &rc);
			switch (LOWORD(wParam)) {
				case SB_TOP:
					start_row = 0;
					break;
				case SB_BOTTOM:
					start_row = lpDebuggerCalc->cpu.pio.d - (4*dps->nRows);
					break;
				case SB_LINEUP:
				
					break;
				case SB_LINEDOWN:
					if (zinf[0].addr + dps->nPage == 0x10000) return 0;

					if (zinf[0].size) {
						dps->nPane += zinf[0].size;
					} else {
						// label
						dps->nPane += zinf[1].size;
					}
					dps->iSel--;
					break;
				case SB_THUMBTRACK:
					dps->nPane = HIWORD(wParam);
					break;
				case SB_PAGEDOWN:
					last_pagedown = zinf[dps->nRows - 2].addr - dps->nPane;
					dps->nPane += last_pagedown;
					break;
				case SB_PAGEUP:
					dps->nPane -= last_pagedown;
					break;
			}
			break;
		}*/
		case WM_CLOSE:
			DestroyWindow(hwnd);
			return FALSE;
	}
	return FALSE;// DefWindowProc(hwnd, Message, wParam, lParam);
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
int DoInsertItem(HWND hwndHeader, int iInsertAfter, int nWidth, LPSTR lpsz) { 
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


