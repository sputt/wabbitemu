#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include "guidebug.h"

#include "calc.h"
#include "dbmem.h"
#include "dbdisasm.h"
#include "dbreg.h"
#include "expandpane.h"
#include "resource.h"

extern HINSTANCE g_hInst;

RECT db_rect = {-1, -1, -1, -1};
HFONT hfontSegoe, hfontLucida, hfontLucidaBold;

#define CY_TOOLBAR 32
static unsigned int cyGripper = 10;
static unsigned int cyDisasm = 350, cyMem;

#define MAX_TABS 5
static ep_state expand_pane_state = {0};
static HWND hdisasm, hreg, hmem;
static int total_mem_pane;
static HWND hmemlist[MAX_TABS];


BOOL CALLBACK EnumDebugResize(HWND hwndChild, LPARAM lParam) {
	int idChild;
	RECT rc;
	RECT *rcParent = &rc;
	GetClientRect((HWND) lParam, rcParent);

	// Is it a first level child?
	if ((HWND) lParam != GetParent(hwndChild))
		return TRUE;

	idChild = GetWindowLongPtr(hwndChild, GWL_ID);
	switch (idChild) {
	case ID_TOOLBAR:
		MoveWindow(hwndChild, 0, 0, rcParent->right, CY_TOOLBAR, TRUE);
		break;
	case ID_DISASM:
		MoveWindow(hwndChild, 0, CY_TOOLBAR, rcParent->right - REG_PANE_WIDTH, cyDisasm - CY_TOOLBAR, TRUE);
		break;
	case ID_MEMTAB: {
		MoveWindow(hwndChild, 3, cyDisasm + cyGripper, rcParent->right - 103 - REG_PANE_WIDTH - 3, cyMem - cyGripper - 3, TRUE);
		//TabCtrl_AdjustRect(hwndChild, FALSE, &rcParent);
		//SetWindowPos(hmem, HWND_TOP, rcParent->left, rcParent->top, rcParent->right - rcParent->left, rcParent->bottom - rcParent->top, 0);
		HWND curTab = hmemlist[TabCtrl_GetCurSel(hwndChild)];
		MoveWindow(curTab, 3, 26, rcParent->right - REG_PANE_WIDTH - 113, rcParent->bottom, TRUE);
		SendMessage(curTab, WM_SIZE, 0, 0);
		SendMessage(curTab, WM_USER, DB_UPDATE, 0);
		break;
	}
	/*case ID_MEM:
		MoveWindow(hwndChild, 0, 0, rcParent->right, rcParent->bottom, TRUE);
		break;*/
	case ID_STACK:
		MoveWindow(hwndChild, rcParent->right - 100 - REG_PANE_WIDTH, cyDisasm + cyGripper, 100, cyMem- cyGripper, TRUE);
		break;
	case ID_REG:
		//printf("Reg: left: %d, top:% d, height: %d\n", rcParent->right - REG_PANE_WIDTH, CY_TOOLBAR, rcParent->bottom);
		SetWindowPos(hwndChild, HWND_TOP, rcParent->right - REG_PANE_WIDTH, CY_TOOLBAR, REG_PANE_WIDTH, rcParent->bottom - CY_TOOLBAR, 0);
		//MoveWindow(hwndChild, rcParent->right - REG_PANE_WIDTH, CY_TOOLBAR, REG_PANE_WIDTH, rcParent->bottom, TRUE);
		break;
	}
	SendMessage(hwndChild, WM_USER, DB_UPDATE, 0);
	return TRUE;
}


BOOL CALLBACK EnumDebugUpdate(HWND hwndChild, LPARAM lParam) {
	SendMessage(hwndChild, WM_USER, DB_UPDATE, lParam);
	return TRUE;
}

int CALLBACK EnumFontFamExProc(
  ENUMLOGFONTEX *lpelfe,    // logical-font data
  NEWTEXTMETRICEX *lpntme,  // physical-font data
  DWORD FontType,           // type of font
  LPARAM lParam             // application-defined data
) {
	LOGFONT *lplf = &lpelfe->elfLogFont;
	lplf->lfHeight = -MulDiv(9, GetDeviceCaps(GetDC(NULL), LOGPIXELSY), 72);
	lplf->lfWidth = 0;
	*((HFONT *) lParam) = CreateFontIndirect(lplf);
	return 0;
}

extern HWND hwndPrev;

LRESULT CALLBACK DebugProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static double ratioDisasm;

	static mp_settings mps[1 + MAX_TABS];
	static dp_settings dps = {
		0, 0, 0, 0, 0, 0, 0,
		{0, 0},
		0, 0, 0,
		NULL, NULL, NULL, NULL,
		{{0, 0, 7, "Addr", &sprint_addr, 0, NULL},
		{1, 0, 11, "Data", &sprint_data, 0, NULL},
		{2, 0, 23, "Disassembly", &sprint_command, 0, NULL},
		{3, 0, 6, "Size", &sprint_size, DT_CENTER, NULL},
		{4, 0, 8, "Clocks", &sprint_clocks, DT_CENTER, NULL},
		{-1, 0, 0, "", &sprint_addr, 0, NULL},
		{-1, 0, 0, "", &sprint_addr, 0, NULL},
		{-1, 0, 0, "", &sprint_addr, 0, NULL},},
		NULL,
		{0, 0, 0, 0}};

	static BOOL bDrag = FALSE, bHot = FALSE;
	static int offset_click;
	static BOOL top_locked = FALSE;
	static BOOL bottom_locked = FALSE;



	switch (Message) {
		case WM_CREATE:
		{
			LOGFONT lf;
			memset(&lf, 0, sizeof(LOGFONT));
			strcpy(lf.lfFaceName, "Lucida Console");

			EnumFontFamiliesEx(
					GetDC(NULL),
					&lf,
					(FONTENUMPROC) EnumFontFamExProc,
					(LPARAM) &hfontLucida,
					0);

			GetObject(hfontLucida, sizeof(LOGFONT), &lf);
			lf.lfWeight = FW_BOLD;

			hfontLucidaBold = CreateFontIndirect(&lf);

			LOGFONT lfSegoe;
			memset(&lfSegoe, 0, sizeof(LOGFONT));
			strcpy(lfSegoe.lfFaceName, "Segoe UI");

			if (EnumFontFamiliesEx(GetDC(NULL), &lfSegoe, (FONTENUMPROC) EnumFontFamExProc, (LPARAM) &hfontSegoe, 0) != 0) {
				strcpy(lfSegoe.lfFaceName, "Tahoma");
				EnumFontFamiliesEx(GetDC(NULL), &lfSegoe, (FONTENUMPROC) EnumFontFamExProc, (LPARAM) &hfontSegoe, 0);
			}

			/* Create diassembly window */

			//ZeroMemory(&dps, sizeof(dps));
			dps.nSel = calcs[gslot].cpu.pc;

			hdisasm =
			CreateWindow(
				g_szDisasmName,
				"disasm",
				WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
				0, 0, 1, 1,
				hwnd,
				(HMENU) ID_DISASM,
				g_hInst, &dps);

			CreateWindow(
				g_szToolbar,
				"toolbar",
				WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
				0, 0, 1, 1,
				hwnd,
				(HMENU) ID_TOOLBAR,
				g_hInst, NULL);

			hmem =
			CreateWindow(
				WC_TABCONTROL, "",
			    WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
			    0, 0, 1, 1,
			    hwnd,
			    (HMENU) ID_MEMTAB,
			    g_hInst, NULL);
			SetWindowFont(hmem, hfontSegoe, TRUE);
			mps[1].addr = 0x0000;
			mps[1].mode = MEM_BYTE;
			mps[1].sel = 0x000;
			mps[1].track = -1;

			hmemlist[0] = CreateWindow(
				g_szMemName,
				"Memory",
				WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS,
				3, 20, 100, 100,
				hmem,
				(HMENU) ID_MEM,
				g_hInst, &mps[1]);
			TCITEM tie;
			tie.mask = TCIF_TEXT | TCIF_IMAGE;
			tie.iImage = -1;
			tie.pszText = "Mem 1";
			tie.lParam = (LPARAM)hmem;
			TabCtrl_InsertItem(hmem, 0, &tie);
			total_mem_pane = 1;

			hreg =
			CreateWindow(
				g_szRegName,
				"reg",
				WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL,
				0, 0, 100, 300,
				hwnd,
				(HMENU) ID_REG,
				g_hInst, NULL);

			mps[0].addr = calcs[gslot].cpu.sp;
			mps[0].mode = MEM_WORD;
			mps[0].sel = mps[1].addr;
			mps[0].track = offsetof(struct CPU, sp);

			CreateWindow(
				g_szMemName,
				"Stack",
				WS_VISIBLE | WS_CHILD,
				0, 0, 1, 1,
				hwnd,
				(HMENU) ID_STACK,
				g_hInst, &mps[0]);

			RECT rc;
			GetClientRect(hwnd, &rc);
			ratioDisasm = (double) cyDisasm / rc.bottom;

			if (expand_pane_state.total != 0) {
				SetExpandPaneState(&expand_pane_state);
				SendMessage(hreg, WM_SIZE, 0, 0); // didn't help
			}

			if (calcs[gslot].profiler.running)
				CheckMenuItem(GetSubMenu(GetMenu(hwnd), 3), IDM_TOOLS_PROFILE, MF_BYCOMMAND | MF_CHECKED);

			hwndPrev = hdisasm;
			SetFocus(hdisasm);
			SendMessage(hwnd, WM_SIZE, 0, 0);
			SendMessage(hwnd, WM_USER, DB_UPDATE, 0);
			return 0;
		}
		case WM_SIZING:
		{
			RECT *r = (RECT *) lParam;
			int cyCaption = GetSystemMetrics(SM_CYCAPTION);
			int cyBottomFrame = GetSystemMetrics(SM_CYSIZEFRAME);
			int minHeight = CY_TOOLBAR + cyGripper + cyCaption + cyBottomFrame*2;
			if (r->bottom - r->top < minHeight) {
				r->bottom = r->top + minHeight;
				return TRUE;
			}

			return FALSE;
		}
		case WM_SIZE:
		{
			RECT rc;
			GetClientRect(hwnd, &rc);

			if (!bDrag) {
				if (top_locked) {
					cyDisasm = CY_TOOLBAR;
				} else if (bottom_locked) {
					cyDisasm = rc.bottom - cyGripper;
				} else {
					int y = ratioDisasm * rc.bottom;
					if (y < CY_TOOLBAR) y = CY_TOOLBAR;
					if (y > rc.bottom - cyGripper) y = rc.bottom - cyGripper;
					cyDisasm = y;
				}
				cyMem = rc.bottom - cyDisasm;
			}

			EnumChildWindows(hwnd, EnumDebugResize, (LPARAM) hwnd);
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			int y = GET_Y_LPARAM(lParam) + offset_click;
			RECT rc;
			GetClientRect(hwnd, &rc);

			if (bDrag) {
				if (y < CY_TOOLBAR + cyGripper) {
					top_locked = TRUE;
					bottom_locked = FALSE;
					y = CY_TOOLBAR;
				}
				else if (y > rc.bottom - cyGripper) {
					bottom_locked = TRUE;
					top_locked = FALSE;
					y = rc.bottom - cyGripper;
				} else {
					top_locked = FALSE;
					bottom_locked = FALSE;
				}
				cyDisasm = y;
				cyMem = rc.bottom - cyDisasm;
				SendMessage(hwnd, WM_SIZE, 0, 0);
			}
			HCURSOR hcursor = LoadCursor(NULL, IDC_SIZENS);
			int dy = abs((int)(y - (cyDisasm - (cyGripper/2))));

			if (dy < 16) {
				SetCursor(hcursor);
				if (bHot == FALSE) {
					bHot = TRUE;
					RECT r;
					GetClientRect(hwnd, &r);
					r.left = 0; r.right -= REG_PANE_WIDTH;
					r.top = cyDisasm;
					r.bottom = r.top + cyGripper;
					InvalidateRect(hwnd, &r, FALSE);
				}

				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(tme);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hwnd;
				tme.dwHoverTime = 1;
				TrackMouseEvent(&tme);
			} else if (bHot) {
				bHot = FALSE;
				RECT r;
				GetClientRect(hwnd, &r);
				r.left = 0; r.right -= REG_PANE_WIDTH;
				r.top = cyDisasm;
				r.bottom = r.top + cyGripper;
				InvalidateRect(hwnd, &r, FALSE);
			}

			ratioDisasm = (double) cyDisasm / (double) rc.bottom;
			return 0;
		}
		case WM_MOUSELEAVE:
		{
			bHot = FALSE;
			RECT r;
			GetClientRect(hwnd, &r);
			r.left = 0; r.right -= REG_PANE_WIDTH;
			r.top = cyDisasm;
			r.bottom = r.top + cyGripper;
			InvalidateRect(hwnd, &r, FALSE);
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			int y = GET_Y_LPARAM(lParam);
			int dy = abs((int) (y - (cyDisasm + (cyGripper/2))));

			if (dy < 16) {
				bDrag = TRUE;
				offset_click = cyDisasm - y;
				SetCapture(hwnd);
				HCURSOR hcursor = LoadCursor(NULL, IDC_SIZENS);
				SetCursor(hcursor);
				RECT r;
				GetClientRect(hwnd, &r);
				r.left = 0; r.right -= REG_PANE_WIDTH;
				r.top = cyDisasm;
				r.bottom = r.top + cyGripper;
				InvalidateRect(hwnd, &r, FALSE);
			}
			return 0;
		}
		case WM_LBUTTONUP:
		{
			ReleaseCapture();
			bDrag = FALSE;
			RECT r;
			GetClientRect(hwnd, &r);
			r.left = 0; r.right -= REG_PANE_WIDTH;
			r.top = cyDisasm;
			r.bottom = r.top + cyGripper;
			InvalidateRect(hwnd, &r, FALSE);
			return 0;
		}
		case WM_LBUTTONDBLCLK:
		{
			int y = GET_Y_LPARAM(lParam);
			int dy = abs((int) (y - (cyDisasm + (cyGripper/2))));

			if (dy < 16) {
				ratioDisasm = 0.5;
				SendMessage(hwnd, WM_SIZE, 0, 0);
			}
			return 0;
		}
		case WM_COMMAND:
		{
			printf("Got a command\n");
			switch (wParam) {
			case IDM_TOOLS_PROFILE: {
				calcs[gslot].profiler.running = !calcs[gslot].profiler.running;
				HMENU hmenu = GetMenu(hwnd);
				if (calcs[gslot].profiler.running) {
					DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DLGPROFILE), hwnd, (DLGPROC)ProfileDialogProc);
					memset(calcs[gslot].profiler.data, 0, MIN_BLOCK_SIZE * sizeof(long));
					CheckMenuItem(GetSubMenu(hmenu, 3), IDM_TOOLS_PROFILE, MF_BYCOMMAND | MF_CHECKED);
				} else {
					FILE* file;
					int i;
					double data;
					char buffer[256];
					GetCurrentDirectory(ARRAYSIZE(buffer), (char*)&buffer);
					strcat((char*)&buffer, "\\profile.txt");
					file = fopen(buffer, "wb");
					fprintf(file, "Total Tstates: %i\r\n", calcs[gslot].profiler.totalTime);
					for(i = calcs[gslot].profiler.lowAddress / calcs[gslot].profiler.blockSize;
							i < ARRAYSIZE(calcs[gslot].profiler.data) &&
							i < (calcs[gslot].profiler.highAddress / calcs[gslot].profiler.blockSize); i++) {
						data = (double)calcs[gslot].profiler.data[i] / (double)calcs[gslot].profiler.totalTime;
						if (data != 0.0)
							fprintf(file, "$%04X - $%04X: %f%% %d tstates\r\n", i * calcs[gslot].profiler.blockSize, ((i + 1) * calcs[gslot].profiler.blockSize) - 1, data, calcs[gslot].profiler.data[i]);
					}
					fclose(file);
					CheckMenuItem(GetSubMenu(hmenu, 3), IDM_TOOLS_PROFILE, MF_BYCOMMAND | MF_UNCHECKED);
				}
				break;
			}
			case IDM_VIEW_ADDMEM: {
				HMENU hMenu = GetMenu(hwnd);
				EnableMenuItem(hMenu, IDM_VIEW_DELMEM, MF_BYCOMMAND | MF_ENABLED);
				if (total_mem_pane + 1 > MAX_TABS)
					break;
				ShowWindow(hmemlist[TabCtrl_GetCurSel(hmem)], SW_HIDE);
				mps[total_mem_pane + 1].addr = 0x0000;
				mps[total_mem_pane + 1].mode = MEM_BYTE;
				mps[total_mem_pane + 1].sel = 0x000;
				mps[total_mem_pane + 1].track = -1;

				hmemlist[total_mem_pane] = CreateWindow(
					g_szMemName,
					"Memory",
					WS_VISIBLE | WS_CHILD,
					3, 20, 100, 100,
					hmem,
					(HMENU) ID_MEM,
					g_hInst, &mps[total_mem_pane + 1]);
				char buffer[64];
				sprintf(buffer, "Mem %i", total_mem_pane + 1);
				TCITEM tie;
				tie.mask = TCIF_TEXT | TCIF_IMAGE;
				tie.iImage = -1;
				tie.pszText = buffer;
				tie.lParam = (LPARAM)hmemlist[total_mem_pane];
				TabCtrl_InsertItem(hmem, total_mem_pane, &tie);
				TabCtrl_SetCurSel(hmem, total_mem_pane);
				total_mem_pane++;
				if (total_mem_pane == MAX_TABS)
					EnableMenuItem(hMenu, IDM_VIEW_ADDMEM, MF_BYCOMMAND | MF_DISABLED);
				SendMessage(hwnd, WM_SIZE, 0, 0);
				SendMessage(hwnd, WM_USER, DB_UPDATE, 0);
				break;
			}
			case IDM_VIEW_DELMEM: {
				HMENU hMenu = GetMenu(hwnd);
				EnableMenuItem(hMenu, IDM_VIEW_ADDMEM, MF_BYCOMMAND | MF_ENABLED);
				if (total_mem_pane == 1)
					break;
				total_mem_pane--;
				if (total_mem_pane == 1)
					EnableMenuItem(hMenu, IDM_VIEW_DELMEM, MF_BYCOMMAND | MF_DISABLED);
				TabCtrl_DeleteItem(hmem, total_mem_pane);
				TabCtrl_SetCurSel(hmem, total_mem_pane - 1);
				hmemlist[total_mem_pane] = NULL;
				SendMessage(hwnd, WM_USER, DB_UPDATE, 0);
				break;
			}
			default:
				//if (GetFocus() == hdisasm || GetFocus() == htoolbar)
				SendMessage(hdisasm, Message, wParam, lParam);
				/*else if (GetFocus() == hmem)
					SendMessage(hdisasm, Message, wParam, lParam);*/
				break;
			}
			break;
		}
		case WM_NOTIFY: {
			LPNMHDR header = (LPNMHDR)lParam;
			switch (header->code) {
				case TCN_SELCHANGE: {
					if(header->hwndFrom == hmem) {
						int i;
						int index = TabCtrl_GetCurSel(hmem);
						for (i = 0; i < total_mem_pane; i++) {
							if (i == index)
								ShowWindow(hmemlist[i], SW_SHOW);
							else
								ShowWindow(hmemlist[i], SW_HIDE);
						}
						SendMessage(hwnd, WM_SIZE, 0 , 0);
					} else if (header->hwndFrom == hdisasm) {

					}
				}
			}
			break;
		}
		case WM_PAINT: {
			// Paint the background and the gripper bar between disassembly and memory
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);

			RECT rectDot, rc;
			GetClientRect(hwnd, &rc);

			TRIVERTEX vert[2];
			GRADIENT_RECT gRect;
			gRect.UpperLeft  = 0;
			gRect.LowerRight = 1;

			COLORREF DarkEdge, LightEdge;

#define DARKEN_AMOUNT 0x303030

			if (bDrag) {
				DarkEdge = GetSysColor(COLOR_3DDKSHADOW) - DARKEN_AMOUNT;
				LightEdge = GetSysColor(COLOR_3DHILIGHT) - DARKEN_AMOUNT;
			} else if (bHot) {
				DarkEdge = GetSysColor(COLOR_BTNFACE) - DARKEN_AMOUNT;
				LightEdge = GetSysColor(COLOR_BTNFACE);
			} else {
				DarkEdge = GetSysColor(COLOR_BTNSHADOW);
				LightEdge = GetSysColor(COLOR_BTNFACE);
			}

			vert[0].x = 0;
			// 40%
			vert[0].y = cyDisasm + cyGripper * 4 / 10;
			vert[0].Red = GetRValue(LightEdge) << 8;
			vert[0].Green = GetGValue(LightEdge) << 8;
			vert[0].Blue = GetBValue(LightEdge) << 8;

			vert[1].x = rc.right - REG_PANE_WIDTH;
			vert[1].y = cyDisasm + cyGripper;
			vert[1].Red = GetRValue(DarkEdge) << 8;
			vert[1].Green = GetGValue(DarkEdge) << 8;
			vert[1].Blue = GetBValue(DarkEdge) << 8;

			GradientFill(hdc,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);

			#define DOT_WIDTH	4

			HBRUSH hbr = CreateSolidBrush(DarkEdge); //GetSysColorBrush(COLOR_BTNSHADOW);

			rectDot.left = (rc.right-REG_PANE_WIDTH)/2 - 12;
			rectDot.right = rectDot.left + DOT_WIDTH;

			rectDot.top = cyDisasm + cyGripper/2 - DOT_WIDTH/2;
			rectDot.bottom = rectDot.top + DOT_WIDTH;
			SelectObject(hdc, hbr);
			SelectObject(hdc, GetStockObject(DC_PEN));
			SetDCPenColor(hdc, DarkEdge - DARKEN_AMOUNT);

			if (bDrag)
				OffsetRect(&rectDot, 1, 1);

			int i;
			for (i = 0; i < 3; i ++, OffsetRect(&rectDot, DOT_WIDTH*2, 0)) {
				Ellipse(hdc, rectDot.left, rectDot.top, rectDot.right, rectDot.bottom);
			}
			RECT r;
			r.left = 0; r.right = rc.right - REG_PANE_WIDTH;
			r.top = cyDisasm;
			r.bottom = r.top + cyGripper;

			if (bDrag) {
				DrawEdge(hdc, &r, EDGE_SUNKEN, BF_TOP|BF_SOFT);
			} else {
				DrawEdge(hdc, &r, EDGE_RAISED, BF_TOP|BF_BOTTOM|BF_SOFT);
			}

			SelectObject(hdc, GetStockObject(DC_PEN));
			SetDCPenColor(hdc, GetSysColor(COLOR_BTNSHADOW)); //(147, 176, 194));
			MoveToEx(hdc, rc.right - 102 - REG_PANE_WIDTH, cyDisasm+cyGripper, NULL);
			LineTo(hdc, rc.right - 102 - REG_PANE_WIDTH, rc.bottom);

			DeleteObject(hbr);

			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_USER:
			switch (wParam) {
				case DB_UPDATE:
					EnumChildWindows(hwnd, EnumDebugUpdate, 0);
					break;
			}
			return 0;
		case WM_DESTROY:
			CPU_step((&calcs[gslot].cpu));
			calcs[gslot].running = TRUE;
			GetWindowRect(hwnd, &db_rect);

			GetExpandPaneState(&expand_pane_state);
			return 0;
	}
	return DefWindowProc(hwnd, Message, wParam, lParam);
}

LRESULT CALLBACK ProfileDialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch(Message)
	{
		/*case WM_INITDIALOG:

			break;*/
		case WM_COMMAND: {
			switch(LOWORD(wParam)) {
				case IDC_PROFILE_OK: {
					char string[9];
					int output;
					HWND hWndEdit;
					hWndEdit = GetDlgItem(hwnd, IDC_LOW_EDT);
					SendMessage(hWndEdit, WM_GETTEXT, 8, (LPARAM)&string);
					xtoi((const char*)&string, &output);
					calcs[gslot].profiler.lowAddress = output;
					hWndEdit = GetDlgItem(hwnd, IDC_HIGH_EDT);
					SendMessage(hWndEdit, WM_GETTEXT, 8, (LPARAM)&string);
					xtoi((const char*)&string, &output);
					calcs[gslot].profiler.highAddress = output;
					hWndEdit = GetDlgItem(hwnd, IDC_BLOCK_EDT);
					SendMessage(hWndEdit, WM_GETTEXT, 8, (LPARAM)&string);
					output = atoi((const char*) &string);
					calcs[gslot].profiler.blockSize = output;
					EndDialog(hwnd, IDOK);
					break;
				}
				case IDC_PROFILE_CANCEL:
					EndDialog(hwnd, IDCANCEL);
					break;
			}
			break;
		}
	}
	return DefWindowProc(hwnd, Message, wParam, lParam);
}

// Converts a hexadecimal string to integer
int xtoi(const char* xs, int* result) {
	size_t szlen = strlen(xs);
	int i, xv, fact;
	if (szlen <= 0)
		// Nothing to convert
		return 1;
	// Converting more than 32bit hexadecimal value?
	if (szlen>8) return 2; // exit
	// Begin conversion here
	*result = 0;
	fact = 1;
	// Run until no more character to convert
	for(i=szlen-1; i>=0 ;i--) {
		if (isxdigit(*(xs+i))) {
			if (*(xs+i)>=97) {
				xv = ( *(xs+i) - 97) + 10;
			}
			else if ( *(xs+i) >= 65) {
				xv = (*(xs+i) - 65) + 10;
			} else {
				xv = *(xs+i) - 48;
			}
			*result += (xv * fact);
			fact *= 16;
		} else {
		// Conversion was abnormally terminated
		// by non hexadecimal digit, hence
		// returning only the converted with
		// an error value 4 (illegal hex character)
			return 4;
		}
	}
	return 0;
}


