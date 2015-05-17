#include "stdafx.h"

#include "dbmonitor.h"
#include "guidebug.h"
#include "calc.h"
#include "breakpoint.h"
#include "resource.h"
#include "dbcommon.h"

extern HINSTANCE g_hInst;

INT_PTR CALLBACK ConditionsDialogProc(HWND, UINT, WPARAM, LPARAM);

int GetWaddrData(HWND hwnd, waddr_t *waddr, memc *mem) {
	waddr->is_ram = Button_GetCheck(GetDlgItem(hwnd, IDC_CHK_RAM));
	TCHAR buf[32];
	Edit_GetText(GetDlgItem(hwnd, IDC_EDT_ADDR), buf, ARRAYSIZE(buf));
	int val = StringToValue(buf);
	if (val == INT_MAX) {
		return -1;
	}

	waddr->addr = (uint16_t) val;
	Edit_GetText(GetDlgItem(hwnd, IDC_EDT_PAGE), buf, ARRAYSIZE(buf));
	val = StringToValue(buf);
	if (val == INT_MAX) {
		return -1;
	}

	int max_pages = waddr->is_ram ? mem->ram_pages : mem->flash_pages;
	waddr->page = (uint8_t) val % max_pages;
	return 0;
}

static BOOL is_updating = FALSE;
void UpdateItemsListView(LPCALC lpCalc, HWND hwndListView) {
	if (is_updating) {
		return;
	}

	is_updating = TRUE;
	ListView_DeleteAllItems(hwndListView);
	int i;

	for (i = 0; i < lpCalc->cpu.mem_c->flash_size; i++) {
		if (lpCalc->flash_cond_break[i] == NULL) {
			continue;
		}

		LPBREAKPOINT lpBreak = lpCalc->flash_cond_break[i];
		LVITEM lvI;
		// Initialize LVITEM members that are common to all items.
		lvI.pszText		= LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
		lvI.mask		= LVIF_TEXT | LVIF_PARAM;
		lvI.stateMask	= 0;
		lvI.iSubItem	= 0;
		lvI.state		= 0;
		lvI.iItem		= i;
		lvI.lParam		= (LPARAM) lpBreak;
	
		BOOL active = lpBreak->active;
		// Insert items into the list.
		if (ListView_InsertItem(hwndListView, &lvI) == -1) {
			return;
		}

		lpBreak->active = active;
		ListView_SetCheckState(hwndListView, i, lpBreak->active);
	}

	for (int j = 0; j < lpCalc->cpu.mem_c->ram_size; j++) {
		if (lpCalc->ram_cond_break[j] == NULL) {
			continue;
		}

		LPBREAKPOINT lpBreak = lpCalc->ram_cond_break[j];
		LVITEM lvI;
		// Initialize LVITEM members that are common to all items.
		lvI.pszText		= LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
		lvI.mask		= LVIF_TEXT | LVIF_PARAM;
		lvI.stateMask	= 0;
		lvI.iSubItem	= 0;
		lvI.state		= 0;
		lvI.iItem		= i + j;
		lvI.lParam		= (LPARAM) lpBreak;
	
		BOOL active = lpBreak->active;
		// Insert items into the list.
		if (ListView_InsertItem(hwndListView, &lvI) == -1) {
			return;
		}

		lpBreak->active = active;
		ListView_SetCheckState(hwndListView, j, lpBreak->active);
	}

	is_updating = FALSE;
}

LRESULT CALLBACK BreakpointsDialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static int start_row, last_port, cyHeader;
	HWND hwndListView = GetDlgItem(hwnd, IDC_LIST_BREAKS);
	LPDEBUGWINDOWINFO lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(hwnd, GWLP_USERDATA);
	LPCALC lpCalc;
	if (lpDebugInfo != NULL) {
		lpCalc = lpDebugInfo->lpCalc;
	}

	switch(Message) {
		case WM_INITDIALOG: {
			lpDebugInfo = (LPDEBUGWINDOWINFO)lParam;
			lpCalc = lpDebugInfo->lpCalc;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) lpDebugInfo);

			LVCOLUMN listCol;
			memset(&listCol, 0, sizeof(LVCOLUMN));
			listCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			listCol.pszText = _T("Label");
			listCol.cx = 112;
			ListView_InsertColumn(hwndListView, 0, &listCol);
			listCol.cx = 110;
			listCol.pszText = _T("Type");
			ListView_InsertColumn(hwndListView, 1, &listCol);
			listCol.cx = 60;
			listCol.pszText = _T("Address");
			ListView_InsertColumn(hwndListView, 2, &listCol);
			listCol.cx = 40;
			listCol.pszText = _T("Page");
			ListView_InsertColumn(hwndListView, 3, &listCol);
			listCol.cx = 60;
			listCol.pszText = _T("In Ram");
			ListView_InsertColumn(hwndListView, 4, &listCol);
			SetWindowFont(hwndListView, lpDebugInfo->hfontSegoe, TRUE);

			ListView_SetExtendedListViewStyle(hwndListView, LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
			SetWindowTheme(hwndListView, L"explorer", NULL);

			UpdateItemsListView(lpCalc, hwndListView);

			Debug_CreateWindow(hwnd);
			return TRUE;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;

			hdc = BeginPaint(hwnd, &ps);

			RECT rc;
			GetClientRect(hwnd, &rc);

			FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));

			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLORBTN:
		{
			return (LRESULT)GetStockObject(WHITE_BRUSH);
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDC_REM_BREAK: {
					int selIndex = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED);
					LVITEM item;
					item.mask = LVIF_PARAM;
					item.iItem = selIndex;
					item.iSubItem = 0;
					ListView_GetItem(hwndListView, &item);
					if (item.iItem == -1) {
						break;
					}

					LPBREAKPOINT lpBreak = (LPBREAKPOINT) item.lParam;
					switch (lpBreak->type) {
						case NORMAL_BREAK:
							clear_break(lpCalc->cpu.mem_c, lpBreak->waddr);
							break;
						case MEM_READ_BREAK:
							clear_mem_read_break(lpCalc->cpu.mem_c, lpBreak->waddr);
							break;
						case MEM_WRITE_BREAK:
							clear_mem_write_break(lpCalc->cpu.mem_c, lpBreak->waddr);
							break;
					}

					ListView_DeleteItem(hwndListView, selIndex);
					if (!ListView_GetItemCount(hwndListView)) {
						Button_Enable(GetDlgItem(hwnd, IDC_REM_BREAK), FALSE);
						Button_Enable(GetDlgItem(hwnd, IDC_UPDATE_BREAK), FALSE);
					}

					InvalidateRect(hwnd, NULL, FALSE);
					break;
				}
				case IDC_ADD_BREAK: {
					waddr_t waddr;
					memc *memc = lpCalc->cpu.mem_c;
					if (GetWaddrData(hwnd, &waddr, memc) == -1) {
						break;
					}

					if (Button_GetCheck(GetDlgItem(hwnd, IDC_BREAK_NORMAL))) {
						if (check_break(memc, waddr)) {
							break;
						}

						set_break(memc, waddr);
					} else if (Button_GetCheck(GetDlgItem(hwnd, IDC_BREAK_WRITE))) {
						if (check_mem_write_break(memc, waddr)) {
							break;
						}

						set_mem_write_break(lpCalc->cpu.mem_c, waddr);
					} else if (Button_GetCheck(GetDlgItem(hwnd, IDC_BREAK_READ))) {
						if (check_mem_read_break(memc, waddr)) {
							break;
						}

						set_mem_read_break(lpCalc->cpu.mem_c, waddr);
					} else {
						break;
					}

					LPBREAKPOINT lpBreak = lpCalc->cond_breakpoints[waddr.is_ram][PAGE_SIZE * waddr.page + mc_base(waddr.addr)];
					int index = ListView_GetItemCount(hwndListView);

					LVITEM item;
					item.mask = LVIF_PARAM;
					item.iItem = index;
					item.iSubItem = 0;
					item.lParam = (LPARAM) lpBreak;
					ListView_InsertItem(hwndListView, &item);

					Button_Enable(GetDlgItem(hwnd, IDC_REM_BREAK), TRUE);
					Button_Enable(GetDlgItem(hwnd, IDC_UPDATE_BREAK), TRUE);
					
					InvalidateRect(hwnd, NULL, FALSE);
					break;
				}
				case IDC_UPDATE_BREAK: {
					waddr_t waddr;
					memc *memc = lpCalc->cpu.mem_c;
					if (GetWaddrData(hwnd, &waddr, memc) == -1) {
						break;
					}

					TCHAR old_desc[32];
					int selIndex = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED);
					LVITEM item;
					item.mask = LVIF_PARAM;
					item.iItem = selIndex;
					item.iSubItem = 0;
					ListView_GetItem(hwndListView, &item);
					if (item.iItem == -1) {
						break;
					}

					LPBREAKPOINT lpBreak = (LPBREAKPOINT) item.lParam;
					StringCbCopy(old_desc, sizeof(old_desc), lpBreak->label);
					switch (lpBreak->type) {
						case NORMAL_BREAK:
							clear_break(lpCalc->cpu.mem_c, lpBreak->waddr);
							break;
						case MEM_READ_BREAK:
							clear_mem_read_break(lpCalc->cpu.mem_c, lpBreak->waddr);
							break;
						case MEM_WRITE_BREAK:
							clear_mem_write_break(lpCalc->cpu.mem_c, lpBreak->waddr);
							break;
					}

					
					if (IsDlgButtonChecked(hwnd, IDC_BREAK_NORMAL)) {
						set_break(lpCalc->cpu.mem_c, waddr);
						lpBreak->type = NORMAL_BREAK;
					} else if (IsDlgButtonChecked(hwnd, IDC_BREAK_WRITE)) {
						set_mem_write_break(lpCalc->cpu.mem_c, waddr);
						lpBreak->type = MEM_WRITE_BREAK;
					} else if (IsDlgButtonChecked(hwnd, IDC_BREAK_READ)) {
						set_mem_read_break(lpCalc->cpu.mem_c, waddr);
						lpBreak->type = MEM_READ_BREAK;
					}

					StringCbCopy(lpBreak->label, sizeof(lpBreak->label), old_desc);
					lpBreak->waddr = waddr;

					InvalidateRect(hwnd, NULL, FALSE);
					break;
				}
				case IDC_COND_BREAK: {
					INT_PTR result  = DialogBox(g_hInst, MAKEINTRESOURCE(IDD_CONDITION), hwnd, (DLGPROC) ConditionsDialogProc);
					if (result == IDOK) {

					}
					break;
				}
			}
			return FALSE;
		}
		
		case WM_MOUSEWHEEL: {
			WPARAM sbtype;
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			sbtype = zDelta > 0 ?  SB_LINEUP : SB_LINEDOWN;

			for (int i = 0; i < abs(zDelta); i += WHEEL_DELTA) {
				SendMessage(hwnd, WM_VSCROLL, sbtype, 0);
			}

			return FALSE;
		}
		case WM_NOTIFY: {
#define MAX_COUNT 32
			switch (((LPNMHDR) lParam)->code) 
			{
				case LVN_ENDLABELEDIT: {
					NMLVDISPINFO *plvdi = (NMLVDISPINFO *) lParam; 
					if (plvdi->item.pszText == NULL) {
						return TRUE;
					}

					LPBREAKPOINT lpBreak = (LPBREAKPOINT) plvdi->item.lParam;
					StringCbCopy(lpBreak->label, sizeof(lpBreak->label), plvdi->item.pszText);
					InvalidateRect(hwnd, NULL, FALSE);
					break;
				}
				case LVN_ITEMCHANGED: {
					NMLISTVIEW *pnmv = (NMLISTVIEW *) lParam;

					LPBREAKPOINT lpBreak = (LPBREAKPOINT) pnmv->lParam;
					
					if (pnmv->uChanged & LVIF_STATE && !is_updating &&
						(pnmv->uNewState & LVIS_STATEIMAGEMASK) != (pnmv->uOldState & LVIS_STATEIMAGEMASK)) {
						lpBreak->active = (pnmv->uNewState & LVIS_STATEIMAGEMASK) >> 12 != 1;
						ListView_SetCheckState(hwndListView, pnmv->iItem, lpBreak->active);
						ListView_Update(hwndListView, pnmv->iItem);
					}
					break;
				}
				case NM_CLICK: {
					NMITEMACTIVATE *plvdi = (NMITEMACTIVATE *) lParam; 
					if (plvdi->iItem == -1) {
						Button_Enable(GetDlgItem(hwnd, IDC_REM_BREAK), FALSE);
						Button_Enable(GetDlgItem(hwnd, IDC_UPDATE_BREAK), FALSE);

						SetDlgItemText(hwnd, IDC_EDT_ADDR, _T(""));
						SetDlgItemText(hwnd, IDC_EDT_PAGE, _T(""));
						CheckDlgButton(hwnd, IDC_CHK_RAM, FALSE);
						CheckDlgButton(hwnd, IDC_BREAK_NORMAL, BST_UNCHECKED);
						CheckDlgButton(hwnd, IDC_BREAK_WRITE, BST_UNCHECKED);
						CheckDlgButton(hwnd, IDC_BREAK_READ, BST_UNCHECKED);
						break;
					}

					Button_Enable(GetDlgItem(hwnd, IDC_REM_BREAK), TRUE);
					Button_Enable(GetDlgItem(hwnd, IDC_UPDATE_BREAK), TRUE);

					LVITEM item;
					item.mask = LVIF_PARAM;
					item.iItem = plvdi->iItem;
					item.iSubItem = 0;
					ListView_GetItem(hwndListView, &item);
					LPBREAKPOINT lpBreak = (LPBREAKPOINT) item.lParam;
					TCHAR temp[32];
					
					StringCbPrintf(temp, sizeof(temp), _T("$%04X"), lpBreak->waddr.addr);
					SetDlgItemText(hwnd, IDC_EDT_ADDR, temp);
					StringCbPrintf(temp, sizeof(temp), _T("$%02X"), lpBreak->waddr.page);
					SetDlgItemText(hwnd, IDC_EDT_PAGE, temp);
					Button_SetCheck(GetDlgItem(hwnd, IDC_CHK_RAM), lpBreak->waddr.is_ram);
					
					CheckDlgButton(hwnd, IDC_BREAK_NORMAL, lpBreak->type == NORMAL_BREAK ? BST_CHECKED : BST_UNCHECKED);
					CheckDlgButton(hwnd, IDC_BREAK_WRITE, lpBreak->type == MEM_WRITE_BREAK ? BST_CHECKED : BST_UNCHECKED);
					CheckDlgButton(hwnd, IDC_BREAK_READ, lpBreak->type == MEM_READ_BREAK ? BST_CHECKED : BST_UNCHECKED);
					break;
				}
				case LVN_GETDISPINFO: {
					NMLVDISPINFO *plvdi = (NMLVDISPINFO *)lParam;
					LPBREAKPOINT lpBreak = (LPBREAKPOINT) plvdi->item.lParam;

					switch (plvdi->item.iSubItem)
					{
						case 0: {
							StringCbPrintf(plvdi->item.pszText, MAX_COUNT, _T("%s"), lpBreak->label);
							break;
						}
						case 1: {
							switch (lpBreak->type) {
								case NORMAL_BREAK:
									StringCchPrintf(plvdi->item.pszText, MAX_COUNT, _T("%s"), _T("Execution"));
									break;
								case MEM_READ_BREAK:
									StringCchPrintf(plvdi->item.pszText, MAX_COUNT, _T("%s"), _T("Memory Read"));
									break;
								case MEM_WRITE_BREAK:
									StringCchPrintf(plvdi->item.pszText, MAX_COUNT, _T("%s"), _T("Memory Write"));
									break;
							}
							break;
						}
						case 2: {
							StringCbPrintf(plvdi->item.pszText, MAX_COUNT, _T("$%04X"), lpBreak->waddr.addr);
							break;
						}
						case 3: {
							StringCbPrintf(plvdi->item.pszText, MAX_COUNT, _T("$%02X"), lpBreak->waddr.page);
							break;
						}
						case 4: {
							if (lpBreak->waddr.is_ram) {
								StringCbPrintf(plvdi->item.pszText, MAX_COUNT, _T("%s"), _T("True")); 
							} else {
								StringCbPrintf(plvdi->item.pszText, MAX_COUNT, _T("%s"), _T("False")); 
							}
							break;
						}
					}
					ListView_SetCheckState(hwndListView, plvdi->item.iItem, lpBreak->active);
					break;
				}
			}
			return FALSE;
		}
		case WM_USER: {
			switch (wParam) {
				case DB_CREATE:
					break;
				case DB_UPDATE: {
					RECT rc;
					GetClientRect(hwnd, &rc);
					InvalidateRect(hwnd, &rc, FALSE);

					LPDEBUGWINDOWINFO lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					UpdateItemsListView(lpDebugInfo->lpCalc, hwndListView);
					break;
				}
			}
			return TRUE;
		}
		case WM_CLOSE:
			DestroyWindow(hwnd);
			return FALSE;
	}
	return DefWindowProc(hwnd, Message, wParam, lParam);
}

INT_PTR CALLBACK ConditionsDialogProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM) {
	switch (Message) {
	case WM_INITDIALOG: {
		HWND hConditionType = GetDlgItem(hwndDlg, IDC_COND_TYPE);
		ComboBox_AddString(hConditionType, _T("Hit count"));
		ComboBox_AddString(hConditionType, _T("Register"));
		ComboBox_AddString(hConditionType, _T("Memory"));
		ComboBox_SetCurSel(hConditionType, 0);

		HWND hCompType = GetDlgItem(hwndDlg, IDC_COMP_TYPE);
		ComboBox_AddString(hCompType, _T("="));
		ComboBox_AddString(hCompType, _T(">="));
		ComboBox_AddString(hCompType, _T("Multiple of"));
		ComboBox_SetCurSel(hCompType, 0);
		return TRUE;
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case IDOK:
			EndDialog(hwndDlg, IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hwndDlg, IDCANCEL);
			return TRUE;
		}
	}
	}
	return FALSE;
}