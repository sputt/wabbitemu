#include "stdafx.h"

#include "dbmonitor.h"
#include "guidebug.h"
#include "calc.h"
#include "resource.h"
#include "dbcommon.h"

extern HINSTANCE g_hInst;

INT_PTR CALLBACK ConditionsDialogProc(HWND, UINT, WPARAM, LPARAM);

void add_breakpoint(memc *mem, BREAK_TYPE type, waddr_t waddr)
{
	breakpoint_t *new_break = (breakpoint_t *) malloc(sizeof(breakpoint_t));
	new_break->active = TRUE;
	new_break->end_addr = waddr.addr % PAGE_SIZE;
	new_break->type = type;
	new_break->waddr = waddr;
	new_break->waddr.addr %= PAGE_SIZE;
	new_break->num_conditions = 0;
	StringCbPrintf(new_break->label, sizeof(new_break->label), _T("%04X"), waddr.addr);
	LPCALC lpCalc = calc_from_memc(mem);
	lpCalc->cond_breakpoints[waddr.is_ram][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] = new_break;
}

void rem_breakpoint(memc *mem, BREAK_TYPE type, waddr_t waddr)
{
	LPCALC lpCalc = calc_from_memc(mem);
	breakpoint_t *lpBreak = lpCalc->cond_breakpoints[waddr.is_ram][PAGE_SIZE * waddr.page + mc_base(waddr.addr)];
	if (lpBreak == NULL)
		return;
	free(lpBreak);
	lpCalc->cond_breakpoints[waddr.is_ram][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] = NULL;
}

// returns true if it should break, false otherwise
BOOL check_break_callback(memc *mem, BREAK_TYPE type, waddr_t waddr) {
	LPCALC lpCalc = calc_from_memc(mem);
	breakpoint_t *lpBreak = lpCalc->cond_breakpoints[waddr.is_ram][PAGE_SIZE * waddr.page + mc_base(waddr.addr)];
	if (lpBreak == NULL)
		return FALSE;

	//necessary because of page handling
	waddr.addr %= PAGE_SIZE;

	if (!lpBreak->active)
		return FALSE;
	int result = TRUE;
	for (int i = 0; i < lpBreak->num_conditions; i++) {
		switch (lpBreak->conditions[i].type) {
			case CONDITION_HIT_COUNT: {
				condition_hitcount_t *cond = (condition_hitcount_t *) lpBreak->conditions[i].data;
				waddr_t pcWaddr = addr_to_waddr(lpCalc->cpu.mem_c, lpCalc->cpu.pc);
				if (pcWaddr.addr % PAGE_SIZE == waddr.addr && pcWaddr.page == waddr.page && pcWaddr.is_ram == waddr.is_ram) {
					cond->hit_count++;
				}
				switch (cond->condition) {
					case TRIGGER_EQUAL:
						result = cond->hit_count == cond->trigger_value;
						break;
					case TRIGGER_GREATER_EQUAL:
						result = cond->hit_count >= cond->trigger_value;
						break;
					case TRIGGER_MULTIPLE_OF:
						result = !(cond->hit_count % cond->trigger_value);
						break;
				}
				break;
			}
			case CONDITION_REGISTER: {
				condition_register_t *cond = (condition_register_t *) lpBreak->conditions[i].data;
				int value = cond->is_word ? *((uint16_t *) cond->reg) : *((uint8_t *) cond->reg);
				switch (cond->condition) {
					case TRIGGER_EQUAL:
						result = value == cond->trigger_value;
						break;
					case TRIGGER_GREATER_EQUAL:
						result = value >= cond->trigger_value;
						break;
					case TRIGGER_MULTIPLE_OF:
						result = !(value % cond->trigger_value);
						break;
				}
				break;
			}
			case CONDITION_MEMORY: {
				condition_memory_t *cond = (condition_memory_t *) lpBreak->conditions[i].data;
				int value = cond->is_word ? wmem_read16(lpCalc->cpu.mem_c, cond->waddr) : 
											wmem_read(lpCalc->cpu.mem_c, cond->waddr);
				switch (cond->condition) {
					case TRIGGER_EQUAL:
						result = value == cond->trigger_value;
						break;
					case TRIGGER_GREATER_EQUAL:
						result = value >= cond->trigger_value;
						break;
					case TRIGGER_MULTIPLE_OF:
						result = !(value % cond->trigger_value);
						break;
				}
				break;
			}
		}
	}
	return TRUE;
}

int GetWaddrData(HWND hwnd, waddr_t *waddr) {
	waddr->is_ram = Button_GetCheck(GetDlgItem(hwnd, IDC_CHK_RAM));
	TCHAR buf[5];
	Edit_GetText(GetDlgItem(hwnd, IDC_EDT_ADDR), buf, ARRAYSIZE(buf));
	int val = xtoi(buf);
	if (val == INT_MAX)
		return -1;
	waddr->addr = val;
	Edit_GetText(GetDlgItem(hwnd, IDC_EDT_PAGE), buf, ARRAYSIZE(buf));
	val = xtoi(buf);
	if (val == INT_MAX)
		return -1;
	waddr->page = val;
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
		if (ListView_InsertItem(hwndListView, &lvI) == -1)
			return;
		lpBreak->active = active;
		ListView_SetCheckState(hwndListView, i, lpBreak->active);
	}
	for (int j = 0; j < lpCalc->cpu.mem_c->ram_size; j++) {
		if (lpCalc->ram_cond_break[j] == NULL)
			continue;
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
		if (ListView_InsertItem(hwndListView, &lvI) == -1)
			return;
		lpBreak->active = active;
		ListView_SetCheckState(hwndListView, j, lpBreak->active);
	}
	is_updating = FALSE;
}

#define DB_CREATE 0
LRESULT CALLBACK BreakpointsDialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndListView;
	static int port_map[0xFF];
	static int start_row, last_port, cyHeader;
	static LPCALC lpCalc;
	switch(Message) {
		case WM_INITDIALOG: {
			lpCalc = (LPCALC) lParam;
			LPDEBUGWINDOWINFO lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);
			hwndListView = GetDlgItem(hwnd, IDC_LIST_BREAKS);
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
					//UpdateItemsListView(hwndListView);
					Debug_UpdateWindow(GetParent(hwnd));
					break;
				}
				case IDC_ADD_BREAK: {
					waddr_t waddr;
					if (GetWaddrData(hwnd, &waddr) == -1)
						break;

					if (Button_GetCheck(GetDlgItem(hwnd, IDC_BREAK_NORMAL))) {
						set_break(lpCalc->cpu.mem_c, waddr);
					} else if (Button_GetCheck(GetDlgItem(hwnd, IDC_BREAK_WRITE))) {
						set_mem_write_break(lpCalc->cpu.mem_c, waddr);
					} else if (Button_GetCheck(GetDlgItem(hwnd, IDC_BREAK_READ))) {
						set_mem_read_break(lpCalc->cpu.mem_c, waddr);
					}
					
					//UpdateItemsListView(hwndListView);
					Debug_UpdateWindow(GetParent(hwnd));
					break;
				}
				case IDC_UPDATE_BREAK: {
					waddr_t waddr;
					if (GetWaddrData(hwnd, &waddr) == -1)
						break;

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

					if (Button_GetCheck(GetDlgItem(hwnd, IDC_BREAK_NORMAL))) {
						set_break(lpCalc->cpu.mem_c, waddr);
						lpBreak->type = NORMAL_BREAK;
					} else if (Button_GetCheck(GetDlgItem(hwnd, IDC_BREAK_WRITE))) {
						set_mem_write_break(lpCalc->cpu.mem_c, waddr);
						lpBreak->type = MEM_WRITE_BREAK;
					} else if (Button_GetCheck(GetDlgItem(hwnd, IDC_BREAK_READ))) {
						set_mem_read_break(lpCalc->cpu.mem_c, waddr);
						lpBreak->type = MEM_READ_BREAK;
					}
					lpBreak->waddr = waddr;

					//UpdateItemsListView(hwndListView);
					Debug_UpdateWindow(GetParent(hwnd));
					break;
				}
				case IDC_COND_BREAK: {
					HWND hConditions = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_CONDITION), hwnd, (DLGPROC) ConditionsDialogProc);
					ShowWindow(hConditions, SW_SHOW);
					break;
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
			return FALSE;
		}
		case WM_NOTIFY: {
#define MAX_COUNT 32
			switch (((LPNMHDR) lParam)->code) 
			{
				case LVN_ENDLABELEDIT: {
					NMLVDISPINFO *plvdi = (NMLVDISPINFO *) lParam; 
					if (plvdi->item.pszText == NULL)
						return TRUE;
					LPBREAKPOINT lpBreak = (LPBREAKPOINT) plvdi->item.lParam;
					StringCbCopy(lpBreak->label, sizeof(lpBreak->label), plvdi->item.pszText);
					break;
				}
				case LVN_ITEMCHANGED: {
					NMLISTVIEW *pnmv = (NMLISTVIEW *) lParam;

					LPBREAKPOINT lpBreak = (LPBREAKPOINT) pnmv->lParam;
					
					if (pnmv->uChanged & LVIF_STATE && !is_updating &&
						(pnmv->uNewState & LVIS_STATEIMAGEMASK) != (pnmv->uOldState & LVIS_STATEIMAGEMASK)) {
						if ((pnmv->uNewState & LVIS_STATEIMAGEMASK) >> 12 == 1) {
							lpBreak->active = FALSE;
						} else {
							lpBreak->active = TRUE;
						}
						ListView_SetCheckState(hwndListView, pnmv->iItem, lpBreak->active);
						ListView_Update(hwndListView, pnmv->iItem);
					}
					break;
				}
				case NM_CLICK: {
					NMITEMACTIVATE *plvdi = (NMITEMACTIVATE *) lParam; 
					if (plvdi->iItem == -1) {
						Button_Enable(GetDlgItem(hwnd, IDC_UPDATE_BREAK), FALSE);

						Edit_SetText(GetDlgItem(hwnd, IDC_EDT_ADDR), _T(""));
						Edit_SetText(GetDlgItem(hwnd, IDC_EDT_PAGE), _T(""));
						Button_SetCheck(GetDlgItem(hwnd, IDC_CHK_RAM), FALSE);
						Button_SetCheck(GetDlgItem(hwnd, IDC_BREAK_NORMAL), BST_UNCHECKED);
						Button_SetCheck(GetDlgItem(hwnd, IDC_BREAK_WRITE), BST_UNCHECKED);
						Button_SetCheck(GetDlgItem(hwnd, IDC_BREAK_READ), BST_UNCHECKED);
						break;
					}
					Button_Enable(GetDlgItem(hwnd, IDC_UPDATE_BREAK), TRUE);

					LVITEM item;
					item.mask = LVIF_PARAM;
					item.iItem = plvdi->iItem;
					item.iSubItem = 0;
					ListView_GetItem(hwndListView, &item);
					LPBREAKPOINT lpBreak = (LPBREAKPOINT) item.lParam;
					TCHAR temp[32];
					StringCbPrintf(temp, sizeof(temp), _T("%04X"), lpBreak->waddr.addr);
					Edit_SetText(GetDlgItem(hwnd, IDC_EDT_ADDR), temp);
					StringCbPrintf(temp, sizeof(temp), _T("%02X"), lpBreak->waddr.page);
					Edit_SetText(GetDlgItem(hwnd, IDC_EDT_PAGE), temp);
					Button_SetCheck(GetDlgItem(hwnd, IDC_CHK_RAM), lpBreak->waddr.is_ram);
					
					Button_SetCheck(GetDlgItem(hwnd, IDC_BREAK_NORMAL), lpBreak->type == NORMAL_BREAK ? BST_CHECKED : BST_UNCHECKED);
					Button_SetCheck(GetDlgItem(hwnd, IDC_BREAK_WRITE), lpBreak->type == MEM_WRITE_BREAK ? BST_CHECKED : BST_UNCHECKED);
					Button_SetCheck(GetDlgItem(hwnd, IDC_BREAK_READ), lpBreak->type == MEM_READ_BREAK ? BST_CHECKED : BST_UNCHECKED);
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

					UpdateItemsListView(lpCalc, hwndListView);
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

INT_PTR CALLBACK ConditionsDialogProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
		case WM_INITDIALOG:
			return FALSE;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK:
					EndDialog(hwndDlg, IDOK);
					return TRUE;
				case IDCANCEL:
					EndDialog(hwndDlg, IDCANCEL);
					break;
			}
	}
	return FALSE;
}