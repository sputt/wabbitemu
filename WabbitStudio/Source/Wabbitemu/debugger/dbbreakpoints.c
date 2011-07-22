#include "stdafx.h"

#include "dbmonitor.h"
#include "guidebug.h"
#include "calc.h"
#include "resource.h"
#include "dbcommon.h"

enum CONDITIONAL_BREAKPOINT_TYPE
{
	HIT_COUNT_CONDITION,
	REGISTER_CONDITION,
	MEMORY_CONDITION,
	TIMER_CONDITION,
	INTERRUPT_CONDITION
};

typedef struct breakpoint_condition
{
	CONDITIONAL_BREAKPOINT_TYPE type;
	void *data;

} breakpoint_condition_t;

typedef struct breakpoint
{
	TCHAR label[32];
	BREAK_TYPE type;
	waddr_t waddr;
	uint16_t end_addr;			//end of block memory 
	BOOL active;
	breakpoint *next;
	breakpoint_condition_t conditions[10];
} breakpoint_t;

extern HINSTANCE g_hInst;
extern HFONT hfontSegoe;
breakpoint_t *list_breakpoints[MAX_CALCS];

void add_breakpoint(memc *mem, BREAK_TYPE type, waddr_t waddr)
{
	breakpoint_t *new_break = (breakpoint_t *) malloc(sizeof(breakpoint_t));
	new_break->active = TRUE;
	new_break->end_addr = waddr.addr;
	new_break->next = NULL;
	new_break->type = type;
	new_break->waddr = waddr;
	StringCbPrintf(new_break->label, sizeof(new_break->label), _T("%04X"), waddr.addr);
	LPCALC lpCalc = NULL;
	int i;
	for (i = 0; i < MAX_CALCS; i++) {
		if (calcs[i].cpu.mem_c == mem) {
			lpCalc = &calcs[i];
			break;
		}
	}
	if (lpCalc == NULL)
		return;
	breakpoint_t *lpBreak = list_breakpoints[lpCalc->slot];
	if (lpBreak != NULL) {
		while (lpBreak->next != NULL)
			lpBreak = lpBreak->next;
		lpBreak->next = new_break;
	} else
		list_breakpoints[lpCalc->slot] = new_break;
}

void rem_breakpoint(memc *mem, BREAK_TYPE type, waddr_t waddr)
{
	LPCALC lpCalc = NULL;
	int i;
	for (i = 0; i < MAX_CALCS; i++) {
		if (calcs[i].cpu.mem_c == mem) {
			lpCalc = &calcs[i];
			break;
		}
	}
	if (lpCalc == NULL)
		return;
	breakpoint_t *lpPrevBreak, *lpBreak = list_breakpoints[lpCalc->slot];
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
		if (lpBreak == list_breakpoints[lpCalc->slot]) {
			list_breakpoints[lpCalc->slot] = lpBreak->next;
			free(lpBreak);
		} else {
			lpPrevBreak->next = lpBreak->next;
			free(lpBreak);
		}
	}
}

int GetWaddrData(HWND hwnd, waddr_t *waddr) {
	waddr->is_ram = Button_GetCheck(GetDlgItem(hwnd, IDC_CHK_RAM));
	int val;
	TCHAR buf[5];
	Edit_GetText(GetDlgItem(hwnd, IDC_EDT_ADDR), buf, ARRAYSIZE(buf));
	if (xtoi(buf, &val))
		return -1;
	waddr->addr = val;
	Edit_GetText(GetDlgItem(hwnd, IDC_EDT_PAGE), buf, ARRAYSIZE(buf));
	if (xtoi(buf, &val))
		return -1;
	waddr->page = val;
	return 0;
}

static BOOL is_updating = FALSE;
void UpdateItemsListView(HWND hwndListView) {
	is_updating = TRUE;
	ListView_DeleteAllItems(hwndListView);
	int count = 0;
	breakpoint_t *lpBreak = list_breakpoints[lpDebuggerCalc->slot];
	if (lpBreak != NULL) {
		count++;
		while (lpBreak->next != NULL) {
			lpBreak = lpBreak->next;
			count++;
		}
	}

	lpBreak = list_breakpoints[lpDebuggerCalc->slot];
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

//returns true if it should break, false otherwise
BOOL check_break_callback(memc *mem, BREAK_TYPE type, waddr_t waddr) {
	breakpoint_t *lpBreak = list_breakpoints[lpDebuggerCalc->slot];
	if (lpBreak == NULL)
		return FALSE;
	
	while (lpBreak->next != NULL && (lpBreak->waddr.addr != waddr.addr ||
		lpBreak->waddr.page != waddr.page || lpBreak->waddr.is_ram != waddr.is_ram || lpBreak->type != type)) {
		lpBreak = lpBreak->next;
	}
	if (lpBreak->waddr.addr != waddr.addr || lpBreak->waddr.page != waddr.page || lpBreak->waddr.is_ram != waddr.is_ram)
		return FALSE;
	if (!lpBreak->active)
		return FALSE;
	return TRUE;
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

			SendMessage(hwnd, WM_USER, DB_CREATE, 0);
			return TRUE;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDC_REM_BREAK: {
					breakpoint_t *lpBreak = list_breakpoints[lpDebuggerCalc->slot];
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
					SendMessage(GetParent(hwnd), WM_USER, DB_UPDATE, 0);
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
					SendMessage(GetParent(hwnd), WM_USER, DB_UPDATE, 0);
					break;
				}
				case IDC_UPDATE_BREAK: {
					waddr_t waddr;
					if (GetWaddrData(hwnd, &waddr) == -1)
						break;

					breakpoint_t *lpBreak, *lpNextBreak, *lpPrevBreak = list_breakpoints[lpDebuggerCalc->slot];
					for (int i = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED) - 1; i > 0; i--)
						lpPrevBreak = lpPrevBreak->next;
					//were the first element in the list there is no previous
					if (lpPrevBreak == list_breakpoints[lpDebuggerCalc->slot])
						lpBreak = lpPrevBreak;
					else
						lpBreak = lpPrevBreak->next;
					lpNextBreak = lpBreak->next;

					TCHAR label[32];
					StringCchCopy(label, ARRAYSIZE(label), lpBreak->label);
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
					lpBreak = NULL;

					if (Button_GetCheck(GetDlgItem(hwnd, IDC_BREAK_NORMAL))) {
						set_break(lpDebuggerCalc->cpu.mem_c, waddr);
					} else if (Button_GetCheck(GetDlgItem(hwnd, IDC_BREAK_WRITE))) {
						set_mem_write_break(lpDebuggerCalc->cpu.mem_c, waddr);
					} else if (Button_GetCheck(GetDlgItem(hwnd, IDC_BREAK_READ))) {
						set_mem_read_break(lpDebuggerCalc->cpu.mem_c, waddr);
					}

					//ok i know this is really hacky but now we need to go find the last one 
					//(the one we just added) and move it to where the old one was so it doesnt
					//look like the list was fucked
					lpBreak = list_breakpoints[lpDebuggerCalc->slot];
					breakpoint_t *lpNewLastBreak = lpBreak;
					while (lpBreak->next != NULL) {
						lpNewLastBreak = lpBreak;
						lpBreak = lpBreak->next;
					}
					//thank god this is a linked list and not a dumbass array
					lpNewLastBreak->next = NULL;
					if (lpPrevBreak != lpBreak) {
						lpBreak->next = lpPrevBreak->next;
						lpPrevBreak->next = lpBreak;
					} else {
						lpBreak->next = list_breakpoints[lpDebuggerCalc->slot];
						list_breakpoints[lpDebuggerCalc->slot] = lpBreak;
					}
					StringCchCopy(lpBreak->label, ARRAYSIZE(lpBreak->label), label);

					UpdateItemsListView(hwndListView);
					SendMessage(GetParent(hwnd), WM_USER, DB_UPDATE, 0);
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
					breakpoint_t *lpBreak = list_breakpoints[lpDebuggerCalc->slot];
					for (int i = plvdi->item.iItem; i > 0; i--)
						lpBreak = lpBreak->next;
					StringCbCopy(lpBreak->label, sizeof(lpBreak->label), plvdi->item.pszText);
					break;
				}
				case LVN_ITEMCHANGED: {
					NMLISTVIEW *pnmv = (NMLISTVIEW *) lParam;
					breakpoint_t *lpBreak = list_breakpoints[lpDebuggerCalc->slot];
					for (int i = pnmv->iItem; i > 0; i--)
						lpBreak = lpBreak->next;
					
					if (pnmv->uChanged & LVIF_STATE && !is_updating &&
						(pnmv->uNewState & LVIS_STATEIMAGEMASK) != (pnmv->uOldState & LVIS_STATEIMAGEMASK)) {
						if ((pnmv->uNewState & LVIS_STATEIMAGEMASK) >> 12 == 1)
							lpBreak->active = FALSE;
						else
							lpBreak->active = TRUE;
						ListView_SetCheckState(hwndListView, pnmv->iItem, lpBreak->active);
						SendMessage(GetParent(hwnd), WM_USER, DB_UPDATE, 0);
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

					breakpoint_t *lpBreak = list_breakpoints[lpDebuggerCalc->slot];
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
					breakpoint_t *lpBreak = list_breakpoints[lpDebuggerCalc->slot];
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