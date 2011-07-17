#include "stdafx.h"

#include "dbwatch.h"
#include "guidebug.h"
#include "calc.h"
#include "resource.h"
#include "dbcommon.h"
#include "device.h"

extern HINSTANCE g_hInst;
extern HFONT hfontSegoe;
static HWND hwndEditControl;
static WNDPROC wpOrigEditProc;

#define MAX_WATCHPOINTS 20
typedef struct watchpoint {
	TCHAR label[64];
	waddr_t waddr;
	waddr_t waddr_is_valid;
} watchpoint_t;

int num_watch;

static watchpoint_t * watchpoints[MAX_WATCHPOINTS];

// CreateListView: Creates a list-view control in report view.
// Returns the handle to the new control
// TO DO:  The calling procedure should determine whether the handle is NULL, in case 
// of an error in creation.
//
// HINST hInst: The global handle to the applicadtion instance.
// HWND  hWndParent: The handle to the control's parent window. 
//
static HWND CreateListView (HWND hwndParent) 
{
	RECT rcClient;                       // The parent window's client area.

	GetClientRect (hwndParent, &rcClient); 

	// Create the list-view window in report view with label editing enabled.
	HWND hWndListView = CreateWindow(WC_LISTVIEW, 
									 _T(""),
									 WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_EDITLABELS | LVS_NOSORTHEADER | WS_VISIBLE,
									 0, 0,
									 rcClient.right - rcClient.left,
									 rcClient.bottom - rcClient.top,
									 hwndParent,
									 (HMENU)NULL,
									 g_hInst,
									 NULL);
	ListView_SetExtendedListViewStyle(hWndListView, LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT);

	return (hWndListView);
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
		watchpoints[num_watch] = (watchpoint_t *) malloc(sizeof(watchpoint_t));
		watchpoint_t *watch = watchpoints[num_watch];
		ZeroMemory(watch, sizeof(watchpoint_t));
		StringCbCopy(watch->label, sizeof(watch->label), _T("New Label"));
		num_watch++;
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
		if (xtoi(str, &value) != ERROR_SUCCESS)
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

static void CloseSaveEdit(HWND hwndEditControl) {
	if (hwndEditControl) {
		TCHAR buf[10];
		Edit_GetText(hwndEditControl, buf, ARRAYSIZE(buf));
		int value = GetWindowLongPtr(hwndEditControl, GWLP_USERDATA);
		int row_num = LOWORD(value);
		int col_num = HIWORD(value);
		if (col_num != 0)
			value = GetValue(buf);

		watchpoint_t *watch = watchpoints[row_num];

		switch (col_num) {
			case 0:	
				StringCbCopy(watch->label, sizeof(watch->label), buf);
				break;
			case 1:
				watch->waddr.addr = value & 0xFFFF;
				watch->waddr_is_valid.addr = TRUE;
				break;
			case 2:
				watch->waddr.page = value &0xFF;
				watch->waddr_is_valid.page = TRUE;
				break;
			case 3:
				watch->waddr.is_ram = value & 0x1;
				watch->waddr_is_valid.is_ram = TRUE;
				break;
		}
		
		DestroyWindow(hwndEditControl);
		hwndEditControl = NULL;
	}
}

static LRESULT APIENTRY EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { 
	switch (uMsg) {
		case WM_KEYDOWN:
			if (wParam == VK_RETURN)
				CloseSaveEdit(hwnd);
			else if (wParam == VK_ESCAPE) {
				hwndEditControl = NULL;
				DestroyWindow(hwnd);
			} else {
				return CallWindowProc(wpOrigEditProc, hwnd, uMsg, wParam, lParam);
			}
			return TRUE;
		case WM_KILLFOCUS:
			CloseSaveEdit(hwnd);
			return TRUE;
		default:
			return CallWindowProc(wpOrigEditProc, hwnd, uMsg, wParam, lParam); 
	}
} 

LRESULT CALLBACK WatchProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND hwndListView;
	switch(Message) {
		case WM_CREATE: {
			RECT rc, hdrRect;
			hwndListView = CreateListView(hwnd);
			LVCOLUMN listCol;
			memset(&listCol, 0, sizeof(LVCOLUMN));
			listCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			listCol.pszText = _T("Label");
			listCol.cx = 100;
			// Inserting Couloms as much as we want
			SendMessage(hwndListView,LVM_INSERTCOLUMN,0,(LPARAM)&listCol); // Insert/Show the coloum
			listCol.cx = 90;
			listCol.pszText = _T("Address");
			SendMessage(hwndListView,LVM_INSERTCOLUMN,1,(LPARAM)&listCol);
			listCol.cx = 130;
			listCol.pszText = _T("Page");
			SendMessage(hwndListView,LVM_INSERTCOLUMN,2,(LPARAM)&listCol);
			listCol.cx = 110;
			listCol.pszText = _T("Is Ram");
			SendMessage(hwndListView,LVM_INSERTCOLUMN,3,(LPARAM)&listCol);
			SendMessage(hwndListView, WM_SETFONT, (WPARAM) hfontSegoe, TRUE);

			num_watch = 0;
			InsertListViewItems(hwndListView, 1);
			return FALSE;
		}
		case WM_SIZE:
			MoveWindow(hwndListView, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
			return FALSE;
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
			}
			return FALSE;
		}
		case WM_NOTIFY: {
			switch (((LPNMHDR) lParam)->code) 
			{
				case LVN_ENDLABELEDIT:

					break;
				case LVN_ITEMCHANGING:
					CloseSaveEdit(hwndEditControl);
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
					if (col_num == 0)
						break;

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
					SendMessage(hwndEditControl, WM_SETFONT, (WPARAM) hfontSegoe, (LPARAM) TRUE);
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
					watchpoint_t *watch = plvdi->item.iItem > num_watch ? NULL : watchpoints[plvdi->item.iItem];
					switch (plvdi->item.iSubItem)
					{
						case 0:
							StringCbCopy(plvdi->item.pszText, 65, watch->label);
							break;
						case 1:
							if (watch->waddr_is_valid.addr)
								StringCbPrintf(plvdi->item.pszText, 6, _T("$%04X"), watch->waddr.addr);
							else
								plvdi->item.pszText = _T("");
							break;	
						case 2:
							if (watch->waddr_is_valid.page)
								StringCbPrintf(plvdi->item.pszText, 4, _T("$%02X"), watch->waddr.page);
							else
								plvdi->item.pszText = _T("");
							break;
						case 3:
							if (watch->waddr_is_valid.is_ram)
								if (watch->waddr.is_ram)
									StringCbCopy(plvdi->item.pszText, 5, _T("True")); 
								else
									StringCbCopy(plvdi->item.pszText, 6, _T("False"));
							else
								plvdi->item.pszText = _T("");
							break;
					}

					break;
				}
				//case NM_CUSTOMDRAW:
				//{
				//	int iRow;
				//	LPNMLVCUSTOMDRAW pListDraw = (LPNMLVCUSTOMDRAW)lParam; 
	//                switch(pListDraw->nmcd.dwDrawStage) 
	//                { 
	//                case CDDS_PREPAINT: 
	//                    SetWindowLongPtr(hwnd, DWLP_MSGRESULT, CDRF_NOTIFYITEMDRAW); 
	//                    return TRUE;
	//                case CDDS_ITEMPREPAINT: 
	//                case CDDS_ITEMPREPAINT | CDDS_SUBITEM: 
	//                    iRow = (int)pListDraw->nmcd.dwItemSpec; 
	//                    if (lpDebuggerCalc->cpu.pio.devices[port_map[iRow]].breakpoint) { 
	//                        // pListDraw->clrText   = RGB(252, 177, 0); 
	//                        pListDraw->clrTextBk = COLOR_BREAKPOINT; 
	//                        SetWindowLongPtr(hwnd, DWLP_MSGRESULT, CDRF_NEWFONT); 
	//                    }
	//                    return TRUE;
	//                default: 
	//                    SetWindowLongPtr(hwnd, DWLP_MSGRESULT, CDRF_DODEFAULT);
	//                    return TRUE;
	//                }
				//}
			}
			return FALSE;
		}
		case WM_USER: {
			switch (wParam) {
				case DB_UPDATE: {
					RECT rc;
					GetClientRect(hwnd, &rc);
					InvalidateRect(hwnd, &rc, FALSE);
					break;
				}
			}
			return TRUE;
		}
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
}