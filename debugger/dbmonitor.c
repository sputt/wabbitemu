#include "stdafx.h"

#include "dbmonitor.h"
#include "guidebug.h"
#include "calc.h"
#include "resource.h"
#include "dbcommon.h"

#define PORT_MIN_COL_WIDTH 40
#define PORT_ROW_SIZE 15

extern HINSTANCE g_hInst;
extern HFONT hfontSegoe;
int line_sel;
static CPU_t *port_cpu = NULL;
#define COLOR_BREAKPOINT		(RGB(230, 160, 180))
#define COLOR_SELECTION			(RGB(153, 222, 253))

CPU_t* CPU_clone(CPU_t *cpu) {
	CPU_t *new_cpu = (CPU_t *) malloc(sizeof(CPU_t));
	memcpy(new_cpu, cpu, sizeof(CPU_t));
	return new_cpu;
}

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
									 WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_NOSORTHEADER | WS_VISIBLE,
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
    
        // Insert items into the list.
        if (ListView_InsertItem(hWndListView, &lvI) == -1)
            return FALSE;
    }

    return TRUE;
}


#define DB_CREATE 0
LRESULT CALLBACK PortMonitorDialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND hwndListView;
	static int port_map[0xFF];
	static int start_row, last_port, cyHeader;
	switch(Message) {
		case WM_INITDIALOG: {
			RECT rc, hdrRect;
			start_row = 0;
			line_sel = 0;
			hwndListView = CreateListView(hwnd);
			int count = 0;
			for (int i = 0; i < 0xFF; i++) {
				if (lpDebuggerCalc->cpu.pio.devices[i].active) {
					port_map[count] = i;
					count++;
				}
			}
			LVCOLUMN listCol;
			memset(&listCol, 0, sizeof(LVCOLUMN));
			listCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			listCol.pszText = "Port Number";
			listCol.cx = 100;
			// Inserting Couloms as much as we want
			SendMessage(hwndListView,LVM_INSERTCOLUMN,0,(LPARAM)&listCol); // Insert/Show the coloum
			listCol.cx = 90;
			listCol.pszText="Hex";
			SendMessage(hwndListView,LVM_INSERTCOLUMN,1,(LPARAM)&listCol);
			listCol.cx = 130;
			listCol.pszText="Decimal";
			SendMessage(hwndListView,LVM_INSERTCOLUMN,2,(LPARAM)&listCol);
			listCol.cx = 110;
			listCol.pszText="Binary";
			SendMessage(hwndListView,LVM_INSERTCOLUMN,3,(LPARAM)&listCol);
			SendMessage(hwndListView, WM_SETFONT, (WPARAM) hfontSegoe, TRUE);

			InsertListViewItems(hwndListView, count);

			SendMessage(hwnd, WM_USER, DB_CREATE, 0);
			return TRUE;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDM_PORT_SETBREAKPOINT: {
					int port = port_map[ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED)];
					lpDebuggerCalc->cpu.pio.devices[port].breakpoint = !lpDebuggerCalc->cpu.pio.devices[port].breakpoint;
					break;
				}
				case IDM_PORT_EXIT:
					EndDialog(hwnd, IDCANCEL);
					break;
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
#define MAX_COUNT 10
			switch (((LPNMHDR) lParam)->code) 
			{
				case LVN_GETDISPINFO: 
				{
					NMLVDISPINFO *plvdi = (NMLVDISPINFO *)lParam;
					int port_num = port_map[plvdi->item.iItem];
					port_cpu->input = TRUE;
					port_cpu->pio.devices[port_num].code(port_cpu, &(port_cpu->pio.devices[port_num]));
					switch (plvdi->item.iSubItem)
					{
						case 0:
							StringCchPrintf(plvdi->item.pszText, MAX_COUNT, _T("%02X"), port_num);
							break;
						case 1:
							StringCbPrintf(plvdi->item.pszText, MAX_COUNT, _T("$%02X"), port_cpu->bus);
							break;	
						case 2:
							StringCbPrintf(plvdi->item.pszText, MAX_COUNT, _T("%d"), port_cpu->bus);
							break;
						case 3:
							StringCbPrintf(plvdi->item.pszText, MAX_COUNT, _T("%%%s"), byte_to_binary(port_cpu->bus)); 
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
				}
			}
			return FALSE;
		}
		case WM_USER: {
			switch (wParam) {
				case DB_CREATE:
					if (port_cpu != NULL)
						free(port_cpu);
					port_cpu = CPU_clone(&lpDebuggerCalc->cpu);
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