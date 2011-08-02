#include "stdafx.h"

#include "dbwatch.h"
#include "guidebug.h"
#include "calc.h"
#include "resource.h"
#include "dbcommon.h"
#include "device.h"
#include "label.h"
#include "registry.h"

extern HINSTANCE g_hInst;
extern HFONT hfontSegoe;
static HWND hwndEditControl;
static int edit_row, edit_col;
static WNDPROC wpOrigEditProc;
static const TCHAR *watchKey = _T("WatchLocsKey");
static const TCHAR *numWatchKey = _T("NumWatchKey");

static LRESULT APIENTRY EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#define MAX_WATCHPOINTS 20
typedef struct watchpoint {
	TCHAR label[64];
	waddr_t waddr;
	waddr_t waddr_is_valid;		//when we start the boxes are blank. This is tells us whether we should display the data
	//HWND hComboBox;				//we have to handle the combo box ourselves, this is its handle
	char size;					//number of elements to display
	BOOL size_is_valid;			//same as other is_valid
	VALUE_FORMAT val;			//how do we display these elements
} watchpoint_t;

static int num_watch;
static HWND hwndListView;

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
		/*watch->hComboBox = CreateWindow(_T("COMBOBOX"), _T("Byte"), WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
				   0, 0, 20, 20,
				   hWndListView, (HMENU) NULL,
				   g_hInst, NULL);
		ComboBox_AddString(watch->hComboBox, _T("Byte"));
		ComboBox_AddString(watch->hComboBox, _T("Word"));
		ComboBox_AddString(watch->hComboBox, _T("Char"));
		ComboBox_SetCurSel(watch->hComboBox, 0);
		SetWindowFont(watch->hComboBox, hfontSegoe, FALSE);*/
		RECT rc;
		//row doesnt matter, so just use first one.
		LVITEMINDEX lvii;
		lvii.iItem = 0;
		lvii.iGroup = 0;
		ListView_GetItemIndexRect(hWndListView, &lvii, 1, LVIR_BOUNDS, &rc);
		//SetWindowPos(watch->hComboBox, HWND_TOP, rc.left, rc.top, rc.right - rc.left, rc.bottom -  rc.top, SWP_SHOWWINDOW);
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
	} else if (*str == '$') {
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
		TCHAR buf[256];
		Edit_GetText(hwndEditControl, buf, ARRAYSIZE(buf));
		int value = GetWindowLongPtr(hwndEditControl, GWLP_USERDATA);
		int row_num = LOWORD(value);
		int col_num = HIWORD(value);
		if (col_num != 0 && _tcsicmp(buf, _T(""))) {
			watchpoint_t *watch = watchpoints[row_num];
			//automagically fill in label info where ever you enter it
			label_struct *label = lookup_label(lpDebuggerCalc, buf);
			if (!isdigit(*buf) && *buf != '$' && *buf != '%' && label != NULL) {
				StringCbCopy(watch->label, sizeof(watch->label), buf);
				watch->waddr.addr = label->addr & 0xFFFF;
				watch->waddr.is_ram = label->IsRAM & 1;
				watch->waddr.page = label->page & 0xFF;

				watch->waddr_is_valid.page = TRUE;
				watch->waddr_is_valid.is_ram = TRUE;
				watch->waddr_is_valid.addr = TRUE;
			} else {
				value = GetValue(buf);
				switch (col_num) {
					case 1:
						watch->size = value & 0xFF;
						watch->size_is_valid = TRUE;
						break;	
					case 2:
						watch->waddr.addr = value & 0xFFFF;
						watch->waddr_is_valid.addr = TRUE;
					
						break;
					case 3:
						watch->waddr.page = value &0xFF;
						watch->waddr_is_valid.page = TRUE;
						break;
					case 4:
						watch->waddr.is_ram = value & 0x1;
						watch->waddr_is_valid.is_ram = TRUE;
						break;
				}
			}
		}
		
		DestroyWindow(hwndEditControl);
	}
}

static void CreateEditControl(int row_num, int col_num) {
	TCHAR buf[32];
	ListView_GetItemText(hwndListView, row_num, col_num, buf, ARRAYSIZE(buf));
	RECT rc;
	LVITEMINDEX lvii;
	lvii.iItem = row_num;
	lvii.iGroup = 0;
	ListView_GetItemIndexRect(hwndListView, &lvii, col_num, LVIR_BOUNDS, &rc);
	//rc is now the rect we want to use for the edit control
	hwndEditControl = CreateWindow(_T("EDIT"), buf,
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		rc.left,
		rc.top,
		rc.right - rc.left,
		rc.bottom - rc.top,
		hwndListView, 0, g_hInst, NULL);
	SetWindowFont(hwndEditControl, hfontSegoe, TRUE);
	wpOrigEditProc = (WNDPROC) SetWindowLongPtr(hwndEditControl, GWLP_WNDPROC, (LONG_PTR) EditSubclassProc); 
	Edit_SetSel(hwndEditControl, 0, _tcslen(buf));
	Edit_LimitText(hwndEditControl, 255);
	SetWindowLongPtr(hwndEditControl, GWLP_USERDATA, MAKELPARAM(row_num, col_num));
	SetFocus(hwndEditControl);
}

static LRESULT APIENTRY EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { 
	switch (uMsg) {
		case WM_KEYDOWN:
			if (wParam == VK_RETURN) {
				CloseSaveEdit(hwnd);
				hwndEditControl = NULL;
			} else if (wParam == VK_ESCAPE) {
				hwndEditControl = NULL;
				DestroyWindow(hwnd);
			} else if (wParam == VK_TAB) {
				CloseSaveEdit(hwnd);
				hwndEditControl = NULL;
				if (GetKeyState(VK_SHIFT) & 0x80000000) {
					if (--edit_col == 0)
						edit_col = 4;
					CreateEditControl(edit_row, edit_col);
				} else {
					if (++edit_col == 5)
						edit_col = 1;
					CreateEditControl(edit_row, edit_col);
				}
			} else {
				return CallWindowProc(wpOrigEditProc, hwnd, uMsg, wParam, lParam);
			}
			return TRUE;
		case WM_KILLFOCUS:
			//we use hwndEditControl not hwnd so that we can't
			//call this after we have destroyed the window
			CloseSaveEdit(hwndEditControl);
			hwndEditControl = NULL;
			return TRUE;
		default:
			return CallWindowProc(wpOrigEditProc, hwnd, uMsg, wParam, lParam); 
	}
} 

LRESULT CALLBACK WatchProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch(Message) {
		case WM_CREATE: {
			RECT rc, hdrRect;
			int i = 0;
			hwndListView = CreateListView(hwnd);
			LVCOLUMN listCol;
			memset(&listCol, 0, sizeof(LVCOLUMN));
			listCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			listCol.pszText = _T("Label");
			listCol.cx = 100;
			ListView_InsertColumn(hwndListView, i++, &listCol);
			listCol.cx = 60;
			listCol.pszText = _T("Size");
			ListView_InsertColumn(hwndListView, i++, &listCol);
			listCol.cx = 60;
			listCol.pszText = _T("Address");
			ListView_InsertColumn(hwndListView, i++, &listCol);
			listCol.cx = 40;
			listCol.pszText = _T("Page");
			ListView_InsertColumn(hwndListView, i++, &listCol);
			listCol.cx = 60;
			listCol.pszText = _T("Is Ram");
			ListView_InsertColumn(hwndListView, i++, &listCol);
			listCol.cx = 145;
			listCol.pszText = _T("Value");
			ListView_InsertColumn(hwndListView, i++, &listCol);
			SetWindowFont(hwndListView, hfontSegoe, TRUE);

			TCHAR *temp = (TCHAR *) QueryDebugKey((TCHAR *) watchKey);
			TCHAR buf[256];
			memcpy(buf, temp, 256);
			//this way we dont have to worry about freeing later
			i = QueryDebugKey((TCHAR *) numWatchKey);
			num_watch = 0;
			//its possible the registy gets screwed somehow
			//so dont assume we can read the data
			if (strlen(buf)) {
				TCHAR *ptr = buf;
				LVITEM lvI;
				lvI.pszText   = LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
				lvI.mask      = LVIF_TEXT;
				lvI.stateMask = 0;
				lvI.iSubItem  = 0;
				lvI.state     = 0;
				for (; num_watch < i; num_watch++) {
					watchpoint_t *watch = watchpoints[num_watch];
					if (watch)
						free(watch);
					watchpoints[num_watch] = (watchpoint_t *) malloc(sizeof(watchpoint_t));
					watch = watchpoints[num_watch];
					TCHAR *namePtr = watch->label;
					while (*ptr != ',') {
						*namePtr++ = *ptr++;
					}
					*namePtr = '\0';
					ptr++;

					//i dont know why i need this temp
					int addr;
					sscanf(ptr, _T("%x,%x,%d,%d,%d|"), &addr, &watch->waddr.page, &watch->waddr.is_ram, &watch->size, &watch->val);
					watch->waddr.addr = addr;
					watch->waddr_is_valid.addr = TRUE;
					watch->waddr_is_valid.page = TRUE;
					watch->waddr_is_valid.is_ram = TRUE;
					watch->size_is_valid = TRUE;
					while(*ptr++ != '|');

					lvI.iItem = num_watch;
					ListView_InsertItem(hwndListView, &lvI);
				}
			}

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
				//case HDN_ENDTRACK: {
				//	int count = ListView_GetItemCount(hwndListView);
				//	for (int i = 0; i < count; i++) {
				//		RECT rc;
				//		//row doesnt matter, so just use first one.
				//		LVITEMINDEX lvii;
				//		lvii.iItem = 0;
				//		lvii.iGroup = 0;
				//		ListView_GetItemIndexRect(hwndListView, &lvii, 1, LVIR_BOUNDS, &rc);
				//		SetWindowPos(watchpoints[i]->hComboBox, HWND_TOP, rc.left, rc.top, rc.right - rc.left, rc.bottom -  rc.top, SWP_SHOWWINDOW);
				//	}
				//	break;
				//}
				case LVN_ENDLABELEDIT: {
					NMLVDISPINFO *pdi = (NMLVDISPINFO *) lParam; 
					if (pdi->item.pszText != NULL) {
						label_struct *label = lookup_label(lpDebuggerCalc, pdi->item.pszText);
						watchpoint_t *watch = watchpoints[pdi->item.iItem];
						char firstChar = *pdi->item.pszText;
						if (!isdigit(firstChar) && firstChar != '$' && firstChar != '%' && label != NULL) {
							StringCbCopy(watch->label, sizeof(watch->label), pdi->item.pszText);
							watch->waddr.addr = label->addr & 0xFFFF;
							watch->waddr.is_ram = label->IsRAM & 1;
							watch->waddr.page = label->page & 0xFF;

							watch->waddr_is_valid.page = TRUE;
							watch->waddr_is_valid.is_ram = TRUE;
							watch->waddr_is_valid.addr = TRUE;
						}
						StringCbCopy(watchpoints[pdi->item.iItem]->label, sizeof(watchpoints[pdi->item.iItem]->label), pdi->item.pszText);
					}
					break;
				}
				case LVN_ITEMCHANGING:
					CloseSaveEdit(hwndEditControl);
					hwndEditControl = NULL;
					break;
				case LVN_KEYDOWN: {
					LPNMLVKEYDOWN pnkd = (LPNMLVKEYDOWN) lParam;
					if (hwndEditControl) {
						if (pnkd->wVKey == VK_ESCAPE) {
							DestroyWindow(hwndEditControl);
							hwndEditControl = NULL;
						} else{
							SendMessage(hwndEditControl, WM_KEYDOWN, pnkd->wVKey, 0);
						}
					} else {
						if (pnkd->wVKey == VK_DELETE) {
							int index = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED);
							free(watchpoints[index]);
							memmove(&watchpoints[index], &watchpoints[index+1], sizeof(watchpoint_t *) * (--num_watch - index));
							watchpoints[num_watch] = NULL;
							ListView_DeleteItem(hwndListView, index);
							if (num_watch == 0)
								InsertListViewItems(hwndListView, 1);
							Debug_UpdateWindow(hwnd);
						}
					}
					break;
				}
				case NM_DBLCLK: {
					NMITEMACTIVATE *lpnmitem = (NMITEMACTIVATE *)lParam;
					edit_row = lpnmitem->iItem;
					edit_col = lpnmitem->iSubItem;
					if (edit_col == 0 || edit_col == 5)
						break;

					CreateEditControl(edit_row, edit_col);
					break;
				}
				case LVN_GETDISPINFO: 
				{
					NMLVDISPINFO *plvdi = (NMLVDISPINFO *)lParam;
					watchpoint_t *watch = plvdi->item.iItem > num_watch ? NULL : watchpoints[plvdi->item.iItem];
					//if the last item is all valid add another one
					if (plvdi->item.iItem + 1 == num_watch && watch->waddr_is_valid.addr == TRUE && watch->waddr_is_valid.page == TRUE &&
							watch->waddr_is_valid.is_ram == TRUE && watch->size_is_valid == TRUE)
						InsertListViewItems(hwndListView, 1);
					switch (plvdi->item.iSubItem)
					{
						case 0:
							StringCbCopy(plvdi->item.pszText, 65, watch->label);
							break;
						case 1:
							if (watch->size_is_valid)
								StringCbPrintf(plvdi->item.pszText, 4, _T("%d"), watch->size);
							else
								plvdi->item.pszText = _T("");
							break;
						case 2:
							if (watch->waddr_is_valid.addr)
								StringCbPrintf(plvdi->item.pszText, 6, _T("$%04X"), watch->waddr.addr);
							else
								plvdi->item.pszText = _T("");
							break;	
						case 3:
							if (watch->waddr_is_valid.page)
								StringCbPrintf(plvdi->item.pszText, 4, _T("$%02X"), watch->waddr.page);
							else
								plvdi->item.pszText = _T("");
							break;
						case 4:
							if (watch->waddr_is_valid.is_ram)
								if (watch->waddr.is_ram)
									StringCbCopy(plvdi->item.pszText, 5, _T("True")); 
								else
									StringCbCopy(plvdi->item.pszText, 6, _T("False"));
							else
								plvdi->item.pszText = _T("");
							break;
						case 5:
							if (watch->waddr_is_valid.addr == TRUE && watch->waddr_is_valid.page == TRUE &&
								watch->waddr_is_valid.is_ram == TRUE && watch->size_is_valid == TRUE) {
								TCHAR format[10];
								TCHAR output[513] = _T("");
								TCHAR temp[5];
								waddr_t next = watch->waddr;
								switch (watch->val) {
										case HEX2:
											StringCbCopy(format, sizeof(format), _T("%02X "));
											break;
										case HEX4:
											StringCbCopy(format, sizeof(format), _T("%04X "));
											break;
										case DEC3:
										case DEC5:
											StringCbCopy(format, sizeof(format), _T("%0d "));
											break;
										case CHAR1:
											StringCbCopy(format, sizeof(format), _T("%c "));
											break;
									}
								for (int i = 0; i < watch->size; i++) {
									uint16_t value = wmem_read(lpDebuggerCalc->cpu.mem_c, next);
									next.addr++;
									//handle the possiblity we pass page boundaries
									if (!(next.addr & 0x3FFF))
										next.page++;
									if (watch->val == HEX4 || watch->val == DEC5) {
										value |= wmem_read(lpDebuggerCalc->cpu.mem_c, next) << 8;
										next.addr++;
										//handle the possiblity we pass page boundaries
										if (!(next.page & 0x3FFF))
											next.page++;
									}
									StringCbPrintf(temp, 513, format, value);
									StringCbCat(output, sizeof(output), temp);
								}
								plvdi->item.pszText = output;
							}
							break;
					}

					break;
				}
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
		case WM_DESTROY: {
			TCHAR buf[512] = _T("");
			TCHAR watchString[256];
			for (int i = 0; i < num_watch; i++)
			{
				watchpoint_t *watch = watchpoints[i];
				if (watch->size_is_valid && watch->waddr_is_valid.addr && watch->waddr_is_valid.is_ram && watch->waddr_is_valid.page) {
					StringCbPrintf(watchString, sizeof(watchString), _T("%s,%x,%x,%d,%d,%d|"), watch->label,
										watch->waddr.addr, watch->waddr.page, watch->waddr.is_ram, watch->size, watch->val);
					StringCbCat(buf, sizeof(buf), watchString);
				} else {
					num_watch--;
				}
			}
			SaveDebugKey((TCHAR *) watchKey, REG_SZ, buf);
			SaveDebugKey((TCHAR *) numWatchKey, REG_DWORD, &num_watch);
			return FALSE;
		}
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return FALSE;
}