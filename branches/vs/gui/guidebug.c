#include <windows.h>
#include <windowsx.h>
#include "guidebug.h"

#include "calc.h"

#include "dbmem.h"
#include "dbdisasm.h"
#include "dbreg.h"
#include "expandpane.h"

extern HINSTANCE g_hInst;

RECT db_rect = {-1, -1, -1, -1};
HFONT hfontSegoe, hfontLucida, hfontLucidaBold;

#define CY_TOOLBAR 32
static unsigned int cyGripper = 10;
static unsigned int cyDisasm = 350, cyMem;

static ep_state expand_pane_state = {0};


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
	case ID_MEM:
		MoveWindow(hwndChild, 0, cyDisasm + cyGripper, rcParent->right - 103 - REG_PANE_WIDTH, cyMem - cyGripper, TRUE);
		break;
	case ID_STACK:
		MoveWindow(hwndChild, rcParent->right - 100 - REG_PANE_WIDTH, cyDisasm + cyGripper, 100, cyMem- cyGripper, TRUE);
		break;
	case ID_REG:
		printf("Reg: left: %d, top:% d, height: %d\n", rcParent->right - REG_PANE_WIDTH, CY_TOOLBAR, rcParent->bottom);
		SetWindowPos(hwndChild, HWND_TOP, rcParent->right - REG_PANE_WIDTH, CY_TOOLBAR, REG_PANE_WIDTH, rcParent->bottom, 0);
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
	static HWND hdisasm, hreg, hmem;
	static double ratioDisasm;
	
	static mp_settings mps[3];
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
			
			
			hreg = 
			CreateWindow(
				g_szRegName,
				"reg",
				WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
				0, 0, 100, 300,
				hwnd,
				(HMENU) ID_REG,
				g_hInst, NULL);
				
			
			mps[0].addr = 0x0000;
			mps[0].mode = MEM_BYTE;
			mps[0].sel = 0x000;
			mps[0].track = -1;
			
			hmem = CreateWindow(
				g_szMemName,
				"Memory",
				WS_VISIBLE | WS_CHILD,
				0, 0, 1, 1,
				hwnd,
				(HMENU) ID_MEM,
				g_hInst, &mps[0]);

			
			mps[1].addr = calcs[gslot].cpu.sp;
			mps[1].mode = MEM_WORD;
			mps[1].sel = mps[1].addr;
			mps[1].track = offsetof(struct CPU, sp);

			CreateWindow(
				g_szMemName,
				"Stack",
				WS_VISIBLE | WS_CHILD,
				0, 0, 1, 1,
				hwnd,
				(HMENU) ID_STACK,
				g_hInst, &mps[1]);	
			
			//*/	
			RECT rc;
			GetClientRect(hwnd, &rc);	
			ratioDisasm = (double) cyDisasm / rc.bottom;
			
			if (expand_pane_state.total != 0) {
				SetExpandPaneState(&expand_pane_state);
				SendMessage(hreg, WM_SIZE, 0, 0); // didn't help
			}
			
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
			int dy = abs((int) (y - (cyDisasm + (cyGripper/2))));
				
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
			printf("Got a command\n");
			if (GetFocus() == hdisasm)
				SendMessage(hdisasm, Message, wParam, lParam);
			else if (GetFocus() == hmem)
				SendMessage(hdisasm, Message, wParam, lParam);
			break;
			
		case WM_PAINT:
		{
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




