#include "stdafx.h"

#include "dbmonitor.h"
#include "guidebug.h"
#include "calc.h"
#include "resource.h"
#include "dbcommon.h"
#include "device.h"

#define PORT_MIN_COL_WIDTH 40
#define PORT_ROW_SIZE 15

extern HINSTANCE g_hInst;
static CPU_t *port_cpu = NULL;
static int port_map[0xFF];
static HWND hwndEditControl;
static WNDPROC wpOrigEditProc;
#define COLOR_BREAKPOINT		(RGB(230, 160, 180))
#define COLOR_SELECTION			(RGB(153, 222, 253))
#define DB_CREATE 0

// CreateListView: Creates a list-view control in report view.
// Returns the handle to the new control
// TO DO:  The calling procedure should determine whether the handle is NULL, in case 
// of an error in creation.
//
// HINST hInst: The global handle to the application instance.
// HWND  hWndParent: The handle to the control's parent window. 
//
static HWND CreateListView (HWND hwndParent) 
{
	RECT rcClient;                       // The parent window's client area.

	GetClientRect (hwndParent, &rcClient); 

	// Create the list-view window in report view with label editing enabled.
	HWND hWndListView = CreateWindow(WC_LISTVIEW, 
									 _T(""),
									 WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_NOSORTHEADER | WS_VISIBLE,
									 0, 0,
									 rcClient.right - rcClient.left,
									 rcClient.bottom - rcClient.top,
									 hwndParent,
									 (HMENU)NULL,
									 g_hInst,
									 NULL);
	
	ListView_SetExtendedListViewStyle(hWndListView, LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT);
	SetWindowTheme(hWndListView, L"explorer", NULL);

	return hWndListView;
}

// InsertListViewItems: Inserts items into a list view. 
// hWndListView:        Handle to the list-view control.
// cItems:              Number of items to insert.
// Returns TRUE if successful, and FALSE otherwise.
static BOOL InsertListViewItems(HWND hWndListView, int cItems)
{
	LVITEM lvI;

	// Initialize LVITEM members that are common to all items.
	lvI.pszText   = LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
	lvI.mask      = LVIF_TEXT;
	lvI.stateMask = 0;
	lvI.iSubItem  = 0;
	lvI.state     = 0;

	// Initialize LVITEM members that are different for each item.
	for (int index = 0; index < cItems; index++)
	{
		lvI.iItem = index;
	
		// Insert items into the list.
		if (ListView_InsertItem(hWndListView, &lvI) == -1)
			return FALSE;
	}

	return TRUE;
}

static int GetValue(TCHAR *str) 
{
	int value = 0;
	int len = _tcslen(str) - 1;
	BOOL is_bin = FALSE, is_hex = FALSE;
	if (!_tcsicmp(str, _T("True"))) {
		return TRUE;
	} else if (!_tcsicmp(str, _T("False"))) {
		return FALSE;
	} if (*str == '$') {
		is_hex = TRUE;
		str++;
	} else if (*str == '%') {
		str++;
		is_bin = TRUE;
	} else if (str[len] == 'b') {
		str[len] = '\0';
		is_bin = TRUE;
	} else if (str[len] == 'h') {
		str[len] = '\0';
		is_hex = TRUE;
	}
	if (is_hex) {
		value = xtoi(str);
		if (value == INT_MAX)
			value = 0;
	} else if (is_bin) {
		for (int i = 0; i < len; i++) {
			value <<= 1;
			if (str[i] == '1') {
				value += 1;
			} else if (str[i] != '0') {
				//error parsing assume 0
				value = 0;
				break;
			}
		}
	} else {
		value = _ttoi(str);
	}
	//handle error parsing
	if (value == INT_MAX || value == INT_MIN)
		value = 0;
	return value;
}

static void CloseSaveEdit(LPCALC lpCalc, HWND hwndEditControl) {
	if (hwndEditControl) {
		TCHAR buf[10];
		Edit_GetText(hwndEditControl, buf, ARRAYSIZE(buf));
		int value = GetWindowLongPtr(hwndEditControl, GWLP_USERDATA);
		int row_num = LOWORD(value);
		int col_num = HIWORD(value);
		//handles getting the user input and converting it to an int
		//can convert bin, hex, and dec
		value = GetValue(buf) & 0xFF;
		int port_num = port_map[row_num];
		BOOL output_backup = lpCalc->cpu.output;
		int bus_backup = lpCalc->cpu.bus;
		lpCalc->cpu.bus = value;
		lpCalc->cpu.output = TRUE;
		lpCalc->cpu.pio.devices[port_num].code(&lpCalc->cpu, &(lpCalc->cpu.pio.devices[port_num]));
		lpCalc->cpu.output = output_backup;
		lpCalc->cpu.bus = bus_backup;

		if (port_cpu != NULL)
			free(port_cpu);
		port_cpu = CPU_clone(&lpCalc->cpu);

		DestroyWindow(hwndEditControl);
	}
}

static LRESULT APIENTRY EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { 
	switch (uMsg) {
		case WM_KEYDOWN:
			if (wParam == VK_RETURN) {
				LPCALC lpCalc = (LPCALC) GetWindowLongPtr(GetParent(GetParent(hwnd)), GWLP_USERDATA);
				CloseSaveEdit(lpCalc, hwnd);
				hwndEditControl = NULL;
			} else if (wParam == VK_ESCAPE) {
				hwndEditControl = NULL;
				DestroyWindow(hwnd);
			} else {
				return CallWindowProc(wpOrigEditProc, hwnd, uMsg, wParam, lParam);
			}
			return TRUE;
		default:
			return CallWindowProc(wpOrigEditProc, hwnd, uMsg, wParam, lParam); 
	}
} 

LRESULT CALLBACK PortMonitorDialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND hwndListView;
	static LPCALC lpCalc;
	static LPDEBUGWINDOWINFO lpDebugInfo;
	switch(Message) {
		case WM_INITDIALOG: {
			HICON hIcon = LoadIcon(g_hInst, _T("w"));
			SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)	hIcon);
			DeleteObject(hIcon);

			lpCalc = (LPCALC) lParam;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, lParam);
			lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);

			hwndListView = CreateListView(hwnd);
			int count = 0;
			for (int i = 0; i < 0xFF; i++) {
				if (lpCalc->cpu.pio.devices[i].active) {
					port_map[count] = i;
					count++;
				}
			}
			LVCOLUMN listCol;
			memset(&listCol, 0, sizeof(LVCOLUMN));
			listCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			listCol.pszText = _T("Port Number");
			listCol.cx = 100;
			ListView_InsertColumn(hwndListView, 0, &listCol);
			listCol.cx = 90;
			listCol.pszText = _T("Hex");
			ListView_InsertColumn(hwndListView, 1, &listCol);
			listCol.cx = 130;
			listCol.pszText = _T("Decimal");
			ListView_InsertColumn(hwndListView, 2, &listCol);
			listCol.cx = 110;
			listCol.pszText = _T("Binary");
			ListView_InsertColumn(hwndListView, 3, &listCol);
			SetWindowFont(hwndListView, lpDebugInfo->hfontSegoe, TRUE);

			InsertListViewItems(hwndListView, count);

			Debug_CreateWindow(hwnd);
			return TRUE;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDM_PORT_SETBREAKPOINT: {
					int port = port_map[ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED)];
					lpCalc->cpu.pio.devices[port].breakpoint = !lpCalc->cpu.pio.devices[port].breakpoint;
					break;
				}
				case IDM_PORT_EXIT:
					SendMessage(hwnd, WM_CLOSE, 0, 0);
					break;
			}
			return FALSE;
		}
		case WM_NOTIFY: {
			switch (((LPNMHDR) lParam)->code) 
			{
				case LVN_ITEMCHANGING:
					CloseSaveEdit(lpCalc, hwndEditControl);
					hwndEditControl = NULL;
					break;
				case LVN_KEYDOWN: {
					LPNMLVKEYDOWN pnkd = (LPNMLVKEYDOWN) lParam;
					if (hwndEditControl) {
						if (pnkd->wVKey == VK_ESCAPE) {
							DestroyWindow(hwndEditControl);
							hwndEditControl = NULL;
						} else {
							SendMessage(hwndEditControl, WM_KEYDOWN, pnkd->wVKey, 0);
						}
					}
					break;
				}
				case NM_DBLCLK: {
					NMITEMACTIVATE *lpnmitem = (NMITEMACTIVATE *)lParam;
					int row_num = lpnmitem->iItem;
					int col_num = lpnmitem->iSubItem;
					//no editing the port num
					if (col_num == 0)
						return FALSE;

					TCHAR buf[32];
					ListView_GetItemText(hwndListView, row_num, col_num, buf, ARRAYSIZE(buf));
					RECT rc;
					LVITEMINDEX lvii;
					lvii.iItem = row_num;
					lvii.iGroup = 0;
					ListView_GetItemIndexRect(hwndListView, &lvii, col_num, LVIR_BOUNDS, &rc);
					//rc is now the rect we want to use for the edit control
					hwndEditControl = CreateWindow(_T("EDIT"), buf,
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT| ES_MULTILINE,
						rc.left,
						rc.top,
						rc.right - rc.left,
						rc.bottom - rc.top,
						hwndListView, 0, g_hInst, NULL);
					SetWindowFont(hwndEditControl, lpDebugInfo->hfontSegoe, TRUE);
					wpOrigEditProc = (WNDPROC) SetWindowLongPtr(hwndEditControl, GWLP_WNDPROC, (LONG_PTR) EditSubclassProc); 
					Edit_LimitText(hwndEditControl, 9);
					Edit_SetSel(hwndEditControl, 0, _tcslen(buf));
					SetWindowLongPtr(hwndEditControl, GWLP_USERDATA, MAKELPARAM(row_num, col_num));
					SetFocus(hwndEditControl);
					break;
				}
				case LVN_GETDISPINFO: 
				{
					NMLVDISPINFO *plvdi = (NMLVDISPINFO *)lParam;
					int port_num = port_map[plvdi->item.iItem];
					port_cpu->input = TRUE;
					port_cpu->pio.devices[port_num].code(port_cpu, &(port_cpu->pio.devices[port_num]));
					switch (plvdi->item.iSubItem)
					{
						case 0:
							StringCchPrintf(plvdi->item.pszText, 10, _T("%02X"), port_num);
							break;
						case 1:
							StringCbPrintf(plvdi->item.pszText, 10, _T("$%02X"), port_cpu->bus);
							break;	
						case 2:
							StringCbPrintf(plvdi->item.pszText, 10, _T("%d"), port_cpu->bus);
							break;
						case 3:
							StringCbPrintf(plvdi->item.pszText, 10, _T("%%%s"), byte_to_binary(port_cpu->bus)); 
							break;
					}

					break;
				}
				case NM_CUSTOMDRAW:
				{
					int iRow;
					LPNMLVCUSTOMDRAW pListDraw = (LPNMLVCUSTOMDRAW)lParam; 
					switch(pListDraw->nmcd.dwDrawStage) 
					{ 
					case CDDS_PREPAINT: 
						SetWindowLongPtr(hwnd, DWLP_MSGRESULT, CDRF_NOTIFYITEMDRAW); 
						return TRUE;
					case CDDS_ITEMPREPAINT: 
					case CDDS_ITEMPREPAINT | CDDS_SUBITEM: 
						iRow = (int)pListDraw->nmcd.dwItemSpec; 
						if (lpCalc->cpu.pio.devices[port_map[iRow]].breakpoint) { 
							// pListDraw->clrText   = RGB(252, 177, 0); 
							pListDraw->clrTextBk = COLOR_BREAKPOINT; 
							SetWindowLongPtr(hwnd, DWLP_MSGRESULT, CDRF_NEWFONT); 
						}
						return TRUE;
					default: 
						SetWindowLongPtr(hwnd, DWLP_MSGRESULT, CDRF_DODEFAULT);
						return TRUE;
					}
				}
			}
			return FALSE;
		}
		case WM_USER: {
			switch (wParam) {
				case DB_CREATE:
					if (port_cpu != NULL) {
						free(port_cpu);
					}
					port_cpu = CPU_clone(&lpCalc->cpu);
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
			lpDebugInfo->hPortMon = NULL;
			EndDialog(hwnd, IDOK);
			return FALSE;
	}
	return FALSE;
}