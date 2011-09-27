#include "stdafx.h"

#include "dbmonitor.h"
#include "guidebug.h"
#include "calc.h"
#include "resource.h"
#include "dbcommon.h"

extern HINSTANCE g_hInst;
extern HFONT hfontSegoe;
static BOOL is_updating_break = FALSE;

INT_PTR CALLBACK ConditionsDialogProc(HWND, UINT, WPARAM, LPARAM);

void add_breakpoint(memc *mem, BREAK_TYPE type, waddr_t waddr)
{
	if (is_updating_break)
		return;
	breakpoint_t *new_break = (breakpoint_t *) malloc(sizeof(breakpoint_t));
	new_break->active = TRUE;
	new_break->end_addr = waddr.addr % PAGE_SIZE;
	new_break->next = NULL;
	new_break->type = type;
	new_break->waddr = waddr;
	new_break->waddr.addr %= PAGE_SIZE;
	StringCbPrintf(new_break->label, sizeof(new_break->label), _T("%04X"), waddr.addr);
	LPCALC lpCalc = calc_from_memc(mem);
	breakpoint_t *lpBreak = lpCalc->cond_breakpoints;
	if (lpBreak != NULL) {
		while (lpBreak->next != NULL)
			lpBreak = lpBreak->next;
		lpBreak->next = new_break;
	} else {
		lpCalc->cond_breakpoints = new_break;
	}
}

void rem_breakpoint(memc *mem, BREAK_TYPE type, waddr_t waddr)
{
	//we just want to clear in the break list,
	//not in the manager. I like this better than creating
	//another set of functions in core.c. If we ever
	//have more than one debugger open we may need to fix this
	if (is_updating_break)
		return;
	LPCALC lpCalc = calc_from_memc(mem);
	breakpoint_t *lpPrevBreak, *lpBreak = lpCalc->cond_breakpoints;
	while (lpBreak->next != NULL) {
		if (lpBreak->waddr.addr == waddr.addr && lpBreak->waddr.page == waddr.page &&
			lpBreak->waddr.is_ram == waddr.is_ram && lpBreak->type == type) {
			break;
		} else {
			lpPrevBreak = lpBreak;
			lpBreak = lpBreak->next;
		}
	}
	if (lpBreak->waddr.addr == waddr.addr && lpBreak->waddr.page == waddr.page &&
		lpBreak->waddr.is_ram == waddr.is_ram && lpBreak->type == type) {
		if (lpBreak == lpCalc->cond_breakpoints) {
			lpCalc->cond_breakpoints = lpBreak->next;
			free(lpBreak);
		} else {
			lpPrevBreak->next = lpBreak->next;
			free(lpBreak);
		}
	}
}

//returns true if it should break, false otherwise
BOOL check_break_callback(memc *mem, BREAK_TYPE type, waddr_t waddr) {
	LPCALC lpCalc = calc_from_memc(mem);
	breakpoint_t *lpBreak = lpCalc->cond_breakpoints;
	if (lpBreak == NULL)
		return FALSE;

	//necessary because of page handling
	waddr.addr %= PAGE_SIZE;

	while (lpBreak->next != NULL && (lpBreak->waddr.addr != waddr.addr ||
		lpBreak->waddr.page != waddr.page || lpBreak->waddr.is_ram != waddr.is_ram || lpBreak->type != type)) {
		lpBreak = lpBreak->next;
	}
	if (lpBreak->waddr.addr != waddr.addr || lpBreak->waddr.page != waddr.page || lpBreak->waddr.is_ram != waddr.is_ram)
		return FALSE;
	if (!lpBreak->active)
		return FALSE;
	int result = TRUE;
	for (int i = 0; i < lpBreak->num_conditions; i++) {
		switch (lpBreak->conditions[i].type) {
			case CONDITION_HIT_COUNT: {
				condition_hitcount_t *cond = (condition_hitcount_t *) lpBreak->conditions[i].data;
				cond->hit_count++;
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
				int value = cond->is_word ? wmem_read16(lpDebuggerCalc->cpu.mem_c, cond->waddr) : 
											wmem_read(lpDebuggerCalc->cpu.mem_c, cond->waddr);
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
void UpdateItemsListView(HWND hwndListView) {
	is_updating = TRUE;
	ListView_DeleteAllItems(hwndListView);
	int count = 0;
	breakpoint_t *lpBreak = lpDebuggerCalc->cond_breakpoints;
	if (lpBreak != NULL) {
		count++;
		while (lpBreak->next != NULL) {
			lpBreak = lpBreak->next;
			count++;
		}
	}

	lpBreak = lpDebuggerCalc->cond_breakpoints;
	for (int i = 0; i < count; i++) {
		LVITEM lvI;
		// Initialize LVITEM members that are common to all items.
		lvI.pszText		= LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
		lvI.mask		= LVIF_TEXT;
		lvI.stateMask	= 0;
		lvI.iSubItem	= 0;
		lvI.state		= 0;
		lvI.iItem		= i;
	
		BOOL active = lpBreak->active;
		// Insert items into the list.
		if (ListView_InsertItem(hwndListView, &lvI) == -1)
			return;
		lpBreak->active = active;
		ListView_SetCheckState(hwndListView, i, lpBreak->active);
		lpBreak = lpBreak->next;
	}
	is_updating = FALSE;
}

#define DB_CREATE 0
LRESULT CALLBACK BreakpointsDialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndListView;
	static int port_map[0xFF];
	static int start_row, last_port, cyHeader;
	switch(Message) {
		case WM_INITDIALOG: {
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
			SetWindowFont(hwndListView, hfontSegoe, TRUE);

			ListView_SetExtendedListViewStyle(hwndListView, LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

			UpdateItemsListView(hwndListView);

			Debug_CreateWindow(hwnd);
			return TRUE;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDC_REM_BREAK: {
					breakpoint_t *lpBreak = lpDebuggerCalc->cond_breakpoints;
					for (int i = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED); i > 0; i--)
						lpBreak = lpBreak->next;
					switch (lpBreak->type) {
						case NORMAL_BREAK:
							clear_break(lpDebuggerCalc->cpu.mem_c, lpBreak->waddr);
							break;
						case MEM_READ_BREAK:
							clear_mem_read_break(lpDebuggerCalc->cpu.mem_c, lpBreak->waddr);
							break;
						case MEM_WRITE_BREAK:
							clear_mem_write_break(lpDebuggerCalc->cpu.mem_c, lpBreak->waddr);
							break;
					}
					UpdateItemsListView(hwndListView);
					Debug_UpdateWindow(GetParent(hwnd));
					break;
				}
				case IDC_ADD_BREAK: {
					waddr_t waddr;
					if (GetWaddrData(hwnd, &waddr) == -1)
						break;

					if (Button_GetCheck(GetDlgItem(hwnd, IDC_BREAK_NORMAL))) {
						set_break(lpDebuggerCalc->cpu.mem_c, waddr);
					} else if (Button_GetCheck(GetDlgItem(hwnd, IDC_BREAK_WRITE))) {
						set_mem_write_break(lpDebuggerCalc->cpu.mem_c, waddr);
					} else if (Button_GetCheck(GetDlgItem(hwnd, IDC_BREAK_READ))) {
						set_mem_read_break(lpDebuggerCalc->cpu.mem_c, waddr);
					}
					
					UpdateItemsListView(hwndListView);
					Debug_UpdateWindow(GetParent(hwnd));
					break;
				}
				case IDC_UPDATE_BREAK: {
					waddr_t waddr;
					if (GetWaddrData(hwnd, &waddr) == -1)
						break;

					breakpoint_t *lpBreak = lpDebuggerCalc->cond_breakpoints;
					for (int i = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED); i > 0; i--)
						lpBreak = lpBreak->next;

					//this is a small hack that lets us keep our current pointer
					is_updating_break = TRUE;
					switch (lpBreak->type) {
						case NORMAL_BREAK:
							clear_break(lpDebuggerCalc->cpu.mem_c, lpBreak->waddr);
							break;
						case MEM_READ_BREAK:
							clear_mem_read_break(lpDebuggerCalc->cpu.mem_c, lpBreak->waddr);
							break;
						case MEM_WRITE_BREAK:
							clear_mem_write_break(lpDebuggerCalc->cpu.mem_c, lpBreak->waddr);
							break;
					}

					if (Button_GetCheck(GetDlgItem(hwnd, IDC_BREAK_NORMAL))) {
						set_break(lpDebuggerCalc->cpu.mem_c, waddr);
						lpBreak->type = NORMAL_BREAK;
					} else if (Button_GetCheck(GetDlgItem(hwnd, IDC_BREAK_WRITE))) {
						set_mem_write_break(lpDebuggerCalc->cpu.mem_c, waddr);
						lpBreak->type = MEM_WRITE_BREAK;
					} else if (Button_GetCheck(GetDlgItem(hwnd, IDC_BREAK_READ))) {
						set_mem_read_break(lpDebuggerCalc->cpu.mem_c, waddr);
						lpBreak->type = MEM_READ_BREAK;
					}
					lpBreak->waddr = waddr;
					is_updating_break = FALSE;

					UpdateItemsListView(hwndListView);
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
					breakpoint_t *lpBreak = lpDebuggerCalc->cond_breakpoints;
					for (int i = plvdi->item.iItem; i > 0; i--)
						lpBreak = lpBreak->next;
					StringCbCopy(lpBreak->label, sizeof(lpBreak->label), plvdi->item.pszText);
					break;
				}
				case LVN_ITEMCHANGED: {
					NMLISTVIEW *pnmv = (NMLISTVIEW *) lParam;
					breakpoint_t *lpBreak = lpDebuggerCalc->cond_breakpoints;
					for (int i = pnmv->iItem; i > 0; i--)
						lpBreak = lpBreak->next;
					
					if (pnmv->uChanged & LVIF_STATE && !is_updating &&
						(pnmv->uNewState & LVIS_STATEIMAGEMASK) != (pnmv->uOldState & LVIS_STATEIMAGEMASK)) {
						if ((pnmv->uNewState & LVIS_STATEIMAGEMASK) >> 12 == 1)
							lpBreak->active = FALSE;
						else
							lpBreak->active = TRUE;
						ListView_SetCheckState(hwndListView, pnmv->iItem, lpBreak->active);
						Debug_UpdateWindow(GetParent(hwnd));
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

					breakpoint_t *lpBreak = lpDebuggerCalc->cond_breakpoints;
					for (int i = plvdi->iItem; i > 0; i--)
						lpBreak = lpBreak->next;
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
					breakpoint_t *lpBreak = lpDebuggerCalc->cond_breakpoints;
					for (int i = plvdi->item.iItem; i > 0; i--)
						lpBreak = lpBreak->next;

					switch (plvdi->item.iSubItem)
					{
						case 0:
							StringCbPrintf(plvdi->item.pszText, MAX_COUNT, _T("%s"), lpBreak->label);
							break;
						case 1:
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
						case 2:
							StringCbPrintf(plvdi->item.pszText, MAX_COUNT, _T("$%04X"), lpBreak->waddr.addr);
							break;	
						case 3:
							StringCbPrintf(plvdi->item.pszText, MAX_COUNT, _T("$%02X"), lpBreak->waddr.page);
							break;
						case 4:
							if (lpBreak->waddr.is_ram)
								StringCbPrintf(plvdi->item.pszText, MAX_COUNT, _T("%s"), _T("True")); 
							else
								StringCbPrintf(plvdi->item.pszText, MAX_COUNT, _T("%s"), _T("False")); 
							break;
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

					UpdateItemsListView(hwndListView);
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