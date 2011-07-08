#include "stdafx.h"

#include "dbmonitor.h"
#include "guidebug.h"
#include "calc.h"
#include "resource.h"
#include "dbcommon.h"

typedef struct breakpoint
{
	TCHAR label[32];
	BREAK_TYPE type;
	waddr_t waddr;
	uint16_t end_addr;			//end of block memory 
	BOOL active;
	breakpoint *next;
} breakpoint_t;

extern HINSTANCE g_hInst;
extern HFONT hfontSegoe;
breakpoint_t *list_breakpoints[MAX_CALCS];


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

void add_breakpoint(memc *mem, BREAK_TYPE type, waddr_t waddr) {
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
		if (calcs[i].cpu.mem_c == mem)
			lpCalc = &calcs[i];
	}
	if (lpCalc == NULL)
		return;
	breakpoint_t *lpBreak = list_breakpoints[lpCalc->slot];
	if (lpBreak != NULL) {
		while (lpBreak->next != NULL)
			lpBreak = lpBreak->next;
		lpBreak->next = new_break;
	}
	else
		list_breakpoints[lpCalc->slot] = new_break;
}


#define DB_CREATE 0
LRESULT CALLBACK BreakpointsDialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND hwndListView;
	static int port_map[0xFF];
	static int start_row, last_port, cyHeader;
	switch(Message) {
		case WM_INITDIALOG: {
			hwndListView = GetDlgItem(hwnd, IDC_LIST_BREAKS);
			LVCOLUMN listCol;
			memset(&listCol, 0, sizeof(LVCOLUMN));
			listCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			listCol.pszText = "Label";
			listCol.cx = 112;
			SendMessage(hwndListView, LVM_INSERTCOLUMN, 0, (LPARAM)&listCol);
			listCol.cx = 110;
			listCol.pszText = "Type";
			SendMessage(hwndListView, LVM_INSERTCOLUMN, 1, (LPARAM)&listCol);
			listCol.cx = 60;
			listCol.pszText = "Address";
			SendMessage(hwndListView, LVM_INSERTCOLUMN, 2, (LPARAM)&listCol);
			listCol.cx = 40;
			listCol.pszText = "Page";
			SendMessage(hwndListView, LVM_INSERTCOLUMN, 3, (LPARAM)&listCol);
			listCol.cx = 60;
			listCol.pszText = "In Ram";
			SendMessage(hwndListView, LVM_INSERTCOLUMN, 4, (LPARAM)&listCol);
			SendMessage(hwndListView, WM_SETFONT, (WPARAM) hfontSegoe, TRUE);

			ListView_SetExtendedListViewStyle(hwndListView, LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

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
			InsertListViewItems(hwndListView, count);
			for (int i = 0; i < count; i++) {
				ListView_SetCheckState(hwndListView, i, lpBreak->active);
				lpBreak = lpBreak->next;
			}

			SendMessage(hwnd, WM_USER, DB_CREATE, 0);
			return TRUE;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				
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
				case NM_CLICK: {
					NMITEMACTIVATE *plvdi = (NMITEMACTIVATE *) lParam; 
					breakpoint_t *lpBreak = list_breakpoints[lpDebuggerCalc->slot];
					for (int i = plvdi->iItem; i > 0; i--)
						lpBreak = lpBreak->next;
					TCHAR temp[32];
					StringCbPrintf(temp,sizeof(temp), _T("%04X"), lpBreak->waddr.addr);
					Edit_SetText(GetDlgItem(hwnd, IDC_EDT_ADDR), temp);
					StringCbPrintf(temp,sizeof(temp), _T("%02X"), lpBreak->waddr.page);
					Edit_SetText(GetDlgItem(hwnd, IDC_EDT_PAGE), temp);
					Button_SetCheck(GetDlgItem(hwnd, IDC_CHK_RAM), lpBreak->waddr.is_ram);
					
					Button_SetCheck(GetDlgItem(hwnd, IDC_BREAK_NORMAL), lpBreak->type == NORMAL_BREAK ? BST_CHECKED : BST_UNCHECKED);
					Button_SetCheck(GetDlgItem(hwnd, IDC_BREAK_WRITE), lpBreak->type == MEM_WRITE_BREAK ? BST_CHECKED : BST_UNCHECKED);
					Button_SetCheck(GetDlgItem(hwnd, IDC_BREAK_READ), lpBreak->type == MEM_READ_BREAK ? BST_CHECKED : BST_UNCHECKED);
					break;
				}
				case LVN_GETDISPINFO: 
				{
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
				/*case NM_CUSTOMDRAW:
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
                        if (lpDebuggerCalc->cpu.pio.devices[port_map[iRow]].breakpoint) { 
                            // pListDraw->clrText   = RGB(252, 177, 0); 
                            pListDraw->clrTextBk = COLOR_BREAKPOINT; 
                            SetWindowLongPtr(hwnd, DWLP_MSGRESULT, CDRF_NEWFONT); 
                        }
                        return TRUE;
                    default: 
                        SetWindowLongPtr(hwnd, DWLP_MSGRESULT, CDRF_DODEFAULT);
                        return TRUE;
                    }
				}*/
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