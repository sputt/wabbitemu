#include "stdafx.h"

#include "dbwatch.h"
#include "guidebug.h"
#include "device.h"
#include "label.h"
#include "registry.h"
#include "dbspriteviewer.h"

extern HINSTANCE g_hInst;
static WNDPROC wpOrigEditProc;
static const TCHAR *watchKey = _T("WatchLocsKey");
static const TCHAR *numWatchKey = _T("NumWatchKey");

static LRESULT APIENTRY EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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
static BOOL InsertListViewItems(HWND hWndListView, LPDEBUGWINDOWINFO lpDebugInfo, int cItems)
{
	LVITEM lvI;
	watchpoint_t *watchpoints = lpDebugInfo->watchpoints;

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
		watchpoint_t *watch = &watchpoints[lpDebugInfo->num_watch];
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
		lpDebugInfo->num_watch++;
		// Insert items into the list.
		if (ListView_InsertItem(hWndListView, &lvI) == -1) {
			return FALSE;
		}
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
		TCHAR buf[256];
		Edit_GetText(hwndEditControl, buf, ARRAYSIZE(buf));
		LPDEBUGWINDOWINFO lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(hwndEditControl, GWLP_USERDATA);
		int row_num = lpDebugInfo->edit_row;
		int col_num = lpDebugInfo->edit_col;
		if (col_num != 0 && _tcsicmp(buf, _T(""))) {
			watchpoint_t *watch = &lpDebugInfo->watchpoints[row_num];
			//automagically fill in label info where ever you enter it
			label_struct *label = lookup_label(lpCalc, buf);
			if (!isdigit(*buf) && *buf != '$' && *buf != '%' && label != NULL) {
				StringCbCopy(watch->label, sizeof(watch->label), buf);
				watch->waddr.addr = label->addr & 0xFFFF;
				watch->waddr.is_ram = label->IsRAM & 1;
				watch->waddr.page = label->page & 0xFF;

				watch->waddr_is_valid.page = TRUE;
				watch->waddr_is_valid.is_ram = TRUE;
				watch->waddr_is_valid.addr = TRUE;
			} else {
				int value = GetValue(buf);
				switch (col_num) {
					case 1: {
						int error = _stscanf_s(buf, _T("%dx%d"), &watch->width, &watch->height);
						if (error != EOF && error == 2) {
							watch->is_bitmap = TRUE;
						} else {
							watch->is_bitmap = FALSE;
							watch->size = value;
						}
						watch->size_is_valid = TRUE;
						break;
					}
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

static void CreateEditControl(LPDEBUGWINDOWINFO lpDebugInfo) {
	TCHAR buf[32];
	ListView_GetItemText(lpDebugInfo->hwndListView, lpDebugInfo->edit_row, lpDebugInfo->edit_col, buf, ARRAYSIZE(buf));
	RECT rc;
	LVITEMINDEX lvii;
	lvii.iItem = lpDebugInfo->edit_row;
	lvii.iGroup = 0;
	ListView_GetItemIndexRect(lpDebugInfo->hwndListView, &lvii, lpDebugInfo->edit_col, LVIR_BOUNDS, &rc);
	//rc is now the rect we want to use for the edit control
	lpDebugInfo->hwndEditControl = CreateWindow(_T("EDIT"), buf,
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		rc.left,
		rc.top,
		rc.right - rc.left,
		rc.bottom - rc.top,
		lpDebugInfo->hwndListView, 0, g_hInst, NULL);
	SetWindowFont(lpDebugInfo->hwndEditControl, lpDebugInfo->hfontSegoe, TRUE);
	wpOrigEditProc = (WNDPROC) SetWindowLongPtr(lpDebugInfo->hwndEditControl, GWLP_WNDPROC, (LONG_PTR) EditSubclassProc);
	Edit_SetSel(lpDebugInfo->hwndEditControl, 0, _tcslen(buf));
	Edit_LimitText(lpDebugInfo->hwndEditControl, 255);
	SetWindowLongPtr(lpDebugInfo->hwndEditControl, GWLP_USERDATA, (LONG_PTR) lpDebugInfo);
	SetFocus(lpDebugInfo->hwndEditControl);
}

static LRESULT APIENTRY EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { 
	switch (uMsg) {
		case WM_KEYDOWN: {
			LPDEBUGWINDOWINFO lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (wParam == VK_RETURN) {
				CloseSaveEdit(lpDebugInfo->lpCalc, hwnd);
				lpDebugInfo->hwndEditControl = NULL;
			} else if (wParam == VK_ESCAPE) {
				lpDebugInfo->hwndEditControl = NULL;
				DestroyWindow(hwnd);
			} else if (wParam == VK_TAB) {
				CloseSaveEdit(lpDebugInfo->lpCalc, hwnd);
				lpDebugInfo->hwndEditControl = NULL;
				if (GetKeyState(VK_SHIFT) & 0x80000000) {
					if (--lpDebugInfo->edit_col == 0) {
						lpDebugInfo->edit_col = 4;
					}
					CreateEditControl(lpDebugInfo);
				} else {
					if (++lpDebugInfo->edit_col == 5) {
						lpDebugInfo->edit_col = 1;
					}
					CreateEditControl(lpDebugInfo);
				}
			} else {
				return CallWindowProc(wpOrigEditProc, hwnd, uMsg, wParam, lParam);
			}
			return TRUE;
		}
		case WM_KILLFOCUS: {
			LPDEBUGWINDOWINFO lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			//we use hwndEditControl not hwnd so that we can't
			//call this after we have destroyed the window
			CloseSaveEdit(lpDebugInfo->lpCalc, lpDebugInfo->hwndEditControl);
			lpDebugInfo->hwndEditControl = NULL;
			return TRUE;
		}
		case WM_USER: {

		}
		default:
			return CallWindowProc(wpOrigEditProc, hwnd, uMsg, wParam, lParam); 
	}
} 

static void on_running_changed(LPCALC lpCalc, LPVOID lParam) {
	HWND hListView = (HWND)lParam;
	EnableWindow(hListView, !lpCalc->running);
	UpdateWindow(hListView);
}

LRESULT CALLBACK WatchProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	LPCALC lpCalc;
	LPDEBUGWINDOWINFO lpDebugInfo = (LPDEBUGWINDOWINFO)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (lpDebugInfo != NULL) {
		lpCalc = lpDebugInfo->lpCalc;
	}

	switch(Message) {
		case WM_CREATE: {
			lpDebugInfo = (LPDEBUGWINDOWINFO) ((LPCREATESTRUCT) lParam)->lpCreateParams;

			int i = 0;
			lpDebugInfo->hwndListView = CreateListView(hwnd);
			lpCalc = lpDebugInfo->lpCalc;
			LVCOLUMN listCol;
			memset(&listCol, 0, sizeof(LVCOLUMN));
			listCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			listCol.pszText = _T("Label");
			listCol.cx = 100;
			ListView_InsertColumn(lpDebugInfo->hwndListView, i++, &listCol);
			listCol.cx = 60;
			listCol.pszText = _T("Size");
			ListView_InsertColumn(lpDebugInfo->hwndListView, i++, &listCol);
			listCol.cx = 60;
			listCol.pszText = _T("Address");
			ListView_InsertColumn(lpDebugInfo->hwndListView, i++, &listCol);
			listCol.cx = 40;
			listCol.pszText = _T("Page");
			ListView_InsertColumn(lpDebugInfo->hwndListView, i++, &listCol);
			listCol.cx = 60;
			listCol.pszText = _T("Is Ram");
			ListView_InsertColumn(lpDebugInfo->hwndListView, i++, &listCol);
			listCol.cx = 145;
			listCol.pszText = _T("Value");
			ListView_InsertColumn(lpDebugInfo->hwndListView, i++, &listCol);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) lpDebugInfo);
			SetWindowFont(lpDebugInfo->hwndListView, lpDebugInfo->hfontSegoe, TRUE);

			TCHAR *temp = (TCHAR *) QueryDebugKey((TCHAR *) watchKey);
			TCHAR buf[256];
			memcpy(buf, temp, 256);
			//this way we dont have to worry about freeing later
			i = QueryDebugKey((TCHAR *) numWatchKey);
			lpDebugInfo->num_watch = 0;
			//its possible the registy gets screwed somehow
			//so dont assume we can read the data
			if (_tcslen(buf)) {
				TCHAR *ptr = buf;
				LVITEM lvI;
				lvI.pszText   = LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
				lvI.mask      = LVIF_TEXT;
				lvI.stateMask = 0;
				lvI.iSubItem  = 0;
				lvI.state     = 0;
				for (; lpDebugInfo->num_watch < i; lpDebugInfo->num_watch++) {
					watchpoint_t *watch = &lpDebugInfo->watchpoints[lpDebugInfo->num_watch];
					ZeroMemory(watch, sizeof(watchpoint_t));
					TCHAR *namePtr = watch->label;
					while (*ptr != ',') {
						*namePtr++ = *ptr++;
					}
					*namePtr = '\0';
					ptr++;

					//i dont know why i need this temp
					int addr, size;
					_stscanf_s(ptr, _T("%x,%x,%d,%d,%d|"), &addr, &watch->waddr.page, &watch->waddr.is_ram, &size, &watch->val);
					watch->waddr.addr = addr;
					watch->waddr_is_valid.addr = TRUE;
					watch->waddr_is_valid.page = TRUE;
					watch->waddr_is_valid.is_ram = TRUE;
					watch->size_is_valid = TRUE;
					if (size > 0xFF) {
						watch->is_bitmap = TRUE;
						watch->width = HIBYTE(size);
						watch->height = LOBYTE(size);
					} else {
						watch->is_bitmap = FALSE;
						watch->size = size;
					}
					while(*ptr++ != '|');

					lvI.iItem = lpDebugInfo->num_watch;
					ListView_InsertItem(lpDebugInfo->hwndListView, &lvI);
				}
			}

			calc_register_event(lpCalc, ROM_RUNNING_EVENT, &on_running_changed, lpDebugInfo->hwndListView);
			InsertListViewItems(lpDebugInfo->hwndListView, lpDebugInfo, 1);
			return FALSE;
		}
		case WM_SIZE: {
			MoveWindow(lpDebugInfo->hwndListView, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
			return FALSE;
		}
		case WM_COMMAND: {
			//switch (LOWORD(wParam)) {
			//}
			return FALSE;
		}
		case WM_NOTIFY: {
			switch (((LPNMHDR) lParam)->code) 
			{
				case LVN_ENDLABELEDIT: {
					NMLVDISPINFO *pdi = (NMLVDISPINFO *) lParam; 
					if (pdi->item.pszText != NULL) {
						label_struct *label = lookup_label(lpCalc, pdi->item.pszText);
						watchpoint_t *watch = &lpDebugInfo->watchpoints[pdi->item.iItem];
						TCHAR firstChar = *pdi->item.pszText;
						if (!isdigit(firstChar) && firstChar != '$' && firstChar != '%' && label != NULL) {
							StringCbCopy(watch->label, sizeof(watch->label), pdi->item.pszText);
							watch->waddr.addr = label->addr & 0xFFFF;
							watch->waddr.is_ram = label->IsRAM & 1;
							watch->waddr.page = label->page & 0xFF;

							watch->waddr_is_valid.page = TRUE;
							watch->waddr_is_valid.is_ram = TRUE;
							watch->waddr_is_valid.addr = TRUE;
						}
						StringCbCopy(lpDebugInfo->watchpoints[pdi->item.iItem].label, sizeof(lpDebugInfo->watchpoints[pdi->item.iItem].label), pdi->item.pszText);
					}
					break;
				}
				case LVN_ITEMCHANGING:
					CloseSaveEdit(lpCalc, lpDebugInfo->hwndEditControl);
					lpDebugInfo->hwndEditControl = NULL;
					break;
				case LVN_KEYDOWN: {
					LPNMLVKEYDOWN pnkd = (LPNMLVKEYDOWN) lParam;
					if (lpDebugInfo->hwndEditControl) {
						if (pnkd->wVKey == VK_ESCAPE) {
							DestroyWindow(lpDebugInfo->hwndEditControl);
							lpDebugInfo->hwndEditControl = NULL;
						} else {
							SendMessage(lpDebugInfo->hwndEditControl, WM_KEYDOWN, pnkd->wVKey, 0);
						}
					} else {
						if (pnkd->wVKey == VK_DELETE) {
							if (lpDebugInfo->num_watch == 1) {
								break;
							}
							int index = ListView_GetNextItem(lpDebugInfo->hwndListView, -1, LVNI_SELECTED);
							memmove(&lpDebugInfo->watchpoints[index], &lpDebugInfo->watchpoints[index+1], sizeof(watchpoint_t *) * (--lpDebugInfo->num_watch - index));
							ZeroMemory(&lpDebugInfo->watchpoints[lpDebugInfo->num_watch], sizeof(watchpoint_t));
							ListView_DeleteItem(lpDebugInfo->hwndListView, index);
							if (lpDebugInfo->num_watch == 0) {
								InsertListViewItems(lpDebugInfo->hwndListView, lpDebugInfo, 1);
							}
							Debug_UpdateWindow(hwnd);
						}
					}
					break;
				}
				case NM_DBLCLK: {
					NMITEMACTIVATE *lpnmitem = (NMITEMACTIVATE *)lParam;
					lpDebugInfo->edit_row = lpnmitem->iItem;
					lpDebugInfo->edit_col = lpnmitem->iSubItem;
					if (lpDebugInfo->edit_col == 0 || lpDebugInfo->edit_col == 5) {
						if (lpnmitem->iItem > lpDebugInfo->num_watch) {
							break;
						}
						watchpoint_t *watch = &lpDebugInfo->watchpoints[lpnmitem->iItem];

						if (!watch->is_bitmap) {
							break;
						}

						LPTABWINDOWINFO lpTabInfo = (LPTABWINDOWINFO) malloc(sizeof(TABWINDOWINFO));
						lpTabInfo->lpDebugInfo = lpDebugInfo;
						lpTabInfo->tabInfo = watch;
						HWND hwndSprite = CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_SPRITEVIEWER), hwnd, SpriteViewerDialogProc, (LPARAM) lpTabInfo);
						ShowWindow(hwndSprite, SW_SHOW);
						SendMessage(hwndSprite, WM_USER, DB_UPDATE, 0);
						int scale = lpCalc->model >= TI_84PCSE ? 1 : 2;
						SetWindowPos(hwndSprite, NULL, 0, 0, watch->width * scale + 30, watch->height * scale + 50, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
						lpDebugInfo->hwndSpriteViewer[lpnmitem->iItem] = hwndSprite;
						lpnmitem->lParam = (LPARAM) hwndSprite;
						break;
					}

					CreateEditControl(lpDebugInfo);
					break;
				}
				case LVN_GETDISPINFO: 
				{
					NMLVDISPINFO *plvdi = (NMLVDISPINFO *)lParam;
					if (plvdi->item.iItem > lpDebugInfo->num_watch) {
						break;
					}

					watchpoint_t *watch = &lpDebugInfo->watchpoints[plvdi->item.iItem];
					//if the last item is all valid add another one
					if (plvdi->item.iItem + 1 == lpDebugInfo->num_watch && watch->waddr_is_valid.addr == TRUE && watch->waddr_is_valid.page == TRUE &&
							watch->waddr_is_valid.is_ram == TRUE && watch->size_is_valid == TRUE) {
						InsertListViewItems(lpDebugInfo->hwndListView, lpDebugInfo, 1);
					}
					switch (plvdi->item.iSubItem)
					{
						case 0:
							StringCbCopy(plvdi->item.pszText, 65, watch->label);
							break;
						case 1:
							if (watch->size_is_valid) {
								if (watch->is_bitmap) {
									StringCbPrintf(plvdi->item.pszText, 20, _T("%dx%d"), watch->width, watch->height);
								} else {
									StringCbPrintf(plvdi->item.pszText, 4, _T("%d"), watch->size);
								}
							} else {
								plvdi->item.pszText = _T("");
							}
							break;
						case 2:
							if (watch->waddr_is_valid.addr) {
								StringCbPrintf(plvdi->item.pszText, 6, _T("$%04X"), watch->waddr.addr);
							} else {
								plvdi->item.pszText = _T("");
							}
							break;	
						case 3:
							if (watch->waddr_is_valid.page) {
								StringCbPrintf(plvdi->item.pszText, 4, _T("$%02X"), watch->waddr.page);
							} else {
								plvdi->item.pszText = _T("");
							}
							break;
						case 4:
							if (watch->waddr_is_valid.is_ram) {
								if (watch->waddr.is_ram) {
									StringCbCopy(plvdi->item.pszText, 5, _T("True")); 
								} else {
									StringCbCopy(plvdi->item.pszText, 6, _T("False"));
								}
							} else {
								plvdi->item.pszText = _T("");
							}
							break;
						case 5:
							if (watch->waddr_is_valid.addr == TRUE && watch->waddr_is_valid.page == TRUE &&
								watch->waddr_is_valid.is_ram == TRUE && watch->size_is_valid == TRUE) {
								if (watch->is_bitmap) {
									plvdi->item.pszText = _T("Double click for image");
									HWND hwndSprite = (HWND) plvdi->item.lParam;
									if (hwndSprite) {
										InvalidateRect(hwndSprite, NULL, FALSE);
									}
									return 0;
								}
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
									uint16_t value = wmem_read(lpCalc->cpu.mem_c, next);
									next.addr++;
									//handle the possiblity we pass page boundaries
									if (!(next.addr & 0x3FFF)) {
										next.page++;
									}
									if (watch->val == HEX4 || watch->val == DEC5) {
										value |= wmem_read(lpCalc->cpu.mem_c, next) << 8;
										next.addr++;
										//handle the possiblity we pass page boundaries
										if (!(next.page & 0x3FFF)) {
											next.page++;
										}
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

					for (int i = 0; i < MAX_WATCHPOINTS; i++) {
						HWND hwndSpriteViewer = lpDebugInfo->hwndSpriteViewer[i];
						if (hwndSpriteViewer) {
							InvalidateRect(hwndSpriteViewer, NULL, FALSE);
							UpdateWindow(hwndSpriteViewer);
						}
					}
					break;
				}
			}
			return TRUE;
		}
		case WM_DESTROY: {
			calc_unregister_event(lpCalc, ROM_RUNNING_EVENT, &on_running_changed, lpDebugInfo->hwndListView);

			TCHAR buf[512] = _T("");
			TCHAR watchString[256];
			for (int i = 0; i < lpDebugInfo->num_watch; i++)
			{
				watchpoint_t *watch = &lpDebugInfo->watchpoints[i];
				if (watch->size_is_valid && watch->waddr_is_valid.addr && watch->waddr_is_valid.is_ram && watch->waddr_is_valid.page) {
					int size;
					if (watch->is_bitmap) {
						size = MAKEWORD(watch->width, watch->height);
					} else {
						size = watch->size;
					}
					StringCbPrintf(watchString, sizeof(watchString), _T("%s,%x,%x,%d,%d,%d|"), watch->label,
										watch->waddr.addr, watch->waddr.page, watch->waddr.is_ram, size, watch->val);
					StringCbCat(buf, sizeof(buf), watchString);
				} else {
					lpDebugInfo->num_watch--;
				}
			}
			SaveDebugKey((TCHAR *) watchKey, REG_SZ, buf);
			SaveDebugKey((TCHAR *) numWatchKey, REG_DWORD, &lpDebugInfo->num_watch);
			return FALSE;
		}
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
}