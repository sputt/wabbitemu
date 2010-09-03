#include "stdafx.h"

#include "dbsubbar.h"
#include "calc.h"

#define COLUMN_X_OFFSET 7

extern HFONT hfontSegoe;
extern HINSTANCE g_hInst;

void GetBankString(int i, char buffer[16]) {
	bank_t *bank = &calcs[DebuggerSlot].mem_c.banks[i];
	
	char *base;
	if (bank->ram)
		base = "RAM";
	else
		base = "ROM";
	
	sprintf(buffer, "%s %d", base, bank->page);
}


LRESULT CALLBACK SubBarProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND btnBanks[4];
	static HBRUSH hbrButton;
	static BOOL hasRoom = FALSE;
	switch (Message) {
		case WM_CREATE:
		{
			BUTTON_SPLITINFO bsi = {0};
			bsi.mask = BCSIF_STYLE;
			bsi.uSplitStyle = BCSS_NOSPLIT;
			
			int i;
			for (i = 0; i < 4; i++) {
				char szName[32];
				sprintf(szName, "ROM%d", i);
				btnBanks[i] = CreateWindow(
						"BUTTON", 
						szName,
						BS_SPLITBUTTON | WS_CHILD | WS_VISIBLE,
						0, 0, 1, 1,
		                hwnd, (HMENU) 1000, g_hInst, NULL); 
			
				//SendMessage(btnBanks[i], BCM_SETSPLITINFO, 0, (LPARAM) &bsi);
				SendMessage(btnBanks[i], WM_SETFONT, (WPARAM) hfontSegoe, (LPARAM) TRUE);
			}
			
			break;
		}
		case WM_SIZE:
		{
			RECT rc;
			GetClientRect(hwnd, &rc);
			DWORD dwBaseUnits = GetDialogBaseUnits();
			DWORD dwWidth = (rc.right - rc.left - LOWORD(dwBaseUnits)/2)/4;
			
			HDWP hdwp = BeginDeferWindowPos(4);
			UINT uFlags;
			int btnWidth = (30* LOWORD(dwBaseUnits))/4;
			if (dwWidth >  btnWidth + btnWidth/2) {
				uFlags = SWP_NOZORDER | SWP_SHOWWINDOW;
				hasRoom = TRUE;
			} else {
				uFlags = SWP_HIDEWINDOW | SWP_NOZORDER;
				hasRoom = FALSE;
			}
			int i;
			for (i = 0; i < 4; i++) {
				DeferWindowPos(hdwp, btnBanks[i], NULL, 
						LOWORD(dwBaseUnits)/4 + (dwWidth - (32* LOWORD(dwBaseUnits)) / 4) + dwWidth * i,
						(3 * HIWORD(dwBaseUnits)) / 8, 
						(32* LOWORD(dwBaseUnits)) / 4, 
						(11 * HIWORD(dwBaseUnits)) / 8,
						uFlags);
			}
			
			hbrButton = CreateSolidBrush(RGB(187, 217, 240));
			
			EndDeferWindowPos(hdwp);
			
			return 0;
		}
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORSTATIC:
		{
			return (LRESULT) hbrButton;
			
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			
			hdc = BeginPaint(hwnd, &ps);
			
			TRIVERTEX vert[2];
			GRADIENT_RECT gRect;
			gRect.UpperLeft  = 0;
			gRect.LowerRight = 1;
			
			RECT rc;
			GetClientRect(hwnd, &rc);
			vert[0].x = 0;
			vert[0].y = 1;
			vert[0].Red = 243 << 8;
			vert[0].Green = 251 << 8;
			vert[0].Blue = 254 << 8;
			
			vert[1].x = rc.right;
			vert[1].y = rc.bottom;
			vert[1].Red = 187 << 8;
			vert[1].Green = 217 << 8;
			vert[1].Blue = 240 << 8;
			
			GradientFill(hdc,vert,2,&gRect,1,GRADIENT_FILL_RECT_H);
			
			// Draw the horizontal dividing line
			SelectObject(hdc, GetStockObject(DC_PEN));
			SetDCPenColor(hdc, RGB(147, 176, 194));
			MoveToEx(hdc, 0, 0, NULL);
			LineTo(hdc, rc.right, 0);

			
			// Set up the alpha function for the bitmap with alpha values
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = AC_SRC_ALPHA;		
			
			// Create the header for the bitmap with alpha values
			BITMAPINFO bmi;
			ZeroMemory(&bmi, sizeof(BITMAPINFO));
			
			BITMAPINFOHEADER *bi = &bmi.bmiHeader;
			bi->biSize = sizeof(BITMAPINFOHEADER);
			bi->biWidth = 1;
			bi->biHeight = 6;
			bi->biPlanes = 1;
			bi->biBitCount = 32;
			bi->biCompression = BI_RGB;
			
			int width = bi->biWidth;
			int height = bi->biHeight;
			
			HDC hdcGrad = CreateCompatibleDC(hdc);
			// Create a solid brush of the gradient color
			HBRUSH hbrGrad = CreateSolidBrush(RGB(147, 176, 194));
			SelectObject(hdcGrad, hbrGrad);
			
			SelectObject(hdcGrad, GetStockObject(DC_PEN));
			SetDCPenColor(hdcGrad, RGB(147, 176, 194));
			
			BYTE *pBits;
			HBITMAP hbmGrad = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**) &pBits, NULL, 0);
			
			SelectObject(hdcGrad, hbmGrad);

			// Fill it with green
			Rectangle(hdcGrad, 0, 0, width, height);
			
			DeleteObject(hbrGrad);
			
			int y;
			BYTE * pPixel = pBits;
			for (y = 0; y < height; y++, pPixel+=4) {
				pPixel[3] = 200*(y+1)/height;
				
				pPixel[0] = pPixel[0] * pPixel[3] / 0xFF;
				pPixel[1] = pPixel[1] * pPixel[3] / 0xFF;
				pPixel[2] = pPixel[2] * pPixel[3] / 0xFF;
			}
			
			AlphaBlend(	hdc, 0, 1, rc.right, 6,
						hdcGrad, 0, 0, width, height,
						bf);
			
			DeleteObject(hbmGrad);
			DeleteDC(hdcGrad);
			
			
			SelectObject(hdc, hfontSegoe);
			SetBkMode(hdc, TRANSPARENT);
			
			DWORD dwBaseUnits = GetDialogBaseUnits();
			DWORD dwWidth = (rc.right - rc.left - LOWORD(dwBaseUnits)/2)/4;
			
			if (hasRoom == TRUE) {
				int i;
				for (i = 0; i < 4; i++) {
					char szName[32];
					sprintf(szName, "Bank %d", i);
					
					RECT dr = {LOWORD(dwBaseUnits)/4 + (dwWidth * i), 
							(3 * HIWORD(dwBaseUnits)) / 8, 
							LOWORD(dwBaseUnits)/4 + (dwWidth * i)+(dwWidth - (34* LOWORD(dwBaseUnits)) / 4), 
							(14 * HIWORD(dwBaseUnits)) / 8
					};
					DrawText(hdc, szName, -1, &dr, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
				}
			} else {
				RECT dr = {LOWORD(dwBaseUnits) / 2, (3 * HIWORD(dwBaseUnits)) / 8,
						rc.right-LOWORD(dwBaseUnits) / 2, (14 * HIWORD(dwBaseUnits)) / 8};
				DrawText(hdc, "Enlarge the window to view memory bank settings.", -1, &dr, 
						DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
			}
			
			
			EndPaint(hwnd, &ps);
			
			break;
		}
		case WM_USER: {
			int i;
			for (i = 0; i < 4; i++) {
				char szName[16];
				GetBankString(i, szName);
				SetWindowTextA(btnBanks[i], szName);				
			}
			InvalidateRect(hwnd, NULL, FALSE);
			break;
		}
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	
	
	return 0;
}