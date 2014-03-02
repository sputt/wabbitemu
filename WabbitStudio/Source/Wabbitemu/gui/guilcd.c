#include "stdafx.h"

#include "gui.h"
#include "guilcd.h"
#include "guibuttons.h"
#include "calc.h"
#include "sound.h"
#include "gif.h"
#include "screenshothandle.h"
#include "guioptions.h"
#include "link.h"
#include "lcd.h"
#include "colorlcd.h"
#include "savestate.h"
#include "SendFilesWindows.h"
#include "guifilepreview.h"
#include "guicontext.h"
#include "guiopenfile.h"
#include "fileutilities.h"
#include "pngexport.h"

#include "DropSource.h"
#include "DropTarget.h"
#include "DataObject.h"

extern POINT drop_pt;
extern RECT db_rect;
extern HINSTANCE g_hInst;
extern HDC hdcSkin;
extern BOOL is_exiting;

static int alphablendfail = 0;

BITMAPINFO *bi = NULL; 
BITMAPINFO *colorbi = NULL;

HDC DrawDragPanes(HWND hwnd, HDC hdcDest, LPMAINWINDOW lpMainWindow) {
	if (!lpMainWindow->is_calc_file) {
		return NULL;
	}

	RECT rl, rr, clientRect;
	GetClientRect(hwnd, &clientRect);
	SIZE TxtSize;
	POINT TxtPt;

	CopyRect(&rl, &clientRect);
	CopyRect(&rr, &clientRect);

	LPCALC lpCalc = lpMainWindow->lpCalc;

	if (lpCalc->model >= TI_83P && !lpMainWindow->is_archive_only) {
		rl.right = rr.left = rr.right / 2;
	}

	// Create the device context that holds the overlay
	HDC hdc = CreateCompatibleDC(hdcDest);
	HBITMAP bmpDragPane =
	CreateCompatibleBitmap(hdcDest, clientRect.right, clientRect.bottom);

	SelectObject(hdc, bmpDragPane);

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(0xFF, 0xFF, 0xFF));
	HBRUSH hbrRAM = CreateSolidBrush(RAM_COLOR);
	HBRUSH hbrArchive = CreateSolidBrush(ARCHIVE_COLOR);

	if (!hbrRAM || !hbrArchive) {
		_tprintf(_T("Brush creation failed\n"));
		return hdc;
	}

	POINT pt = drop_pt;
	ScreenToClient(hwnd, &pt);

	TRIVERTEX vert[4];
	GRADIENT_RECT gRect[2];

	if (lpCalc->model >= TI_83P) {
		if (lpMainWindow->is_archive_only)
			rr = rl;

		FillRect(hdc, &rr, hbrArchive);

		if (PtInRect(&rr, pt)) {
			gRect[0].UpperLeft  = 0;
			gRect[0].LowerRight = 1;
			gRect[1].UpperLeft  = 2;
			gRect[1].LowerRight = 3;
			vert[0].x      = rr.left;
			vert[0].y      = rr.top;
			vert[0].Red    = 0x7000;
			vert[0].Green  = 0x2000;
			vert[0].Blue   = 0x3000;
			vert[1].x      = rr.left + rl.right/3;
			vert[1].y      = rr.bottom;
			vert[1].Red    = 0xC000;
			vert[1].Green  = 0x4000;
			vert[1].Blue   = 0x6000;

			GradientFill(hdc, vert, 2, gRect, 1, GRADIENT_FILL_RECT_H);

			//0x60, 0xC0, 0x40
			vert[2].x      = rr.left + 2*rl.right/3;
			vert[2].y      = rr.bottom;
			vert[2].Red    = 0xC000;
			vert[2].Green  = 0x4000;
			vert[2].Blue   = 0x6000;
			vert[3].x      = rr.right;
			vert[3].y      = rr.top;
			vert[3].Red    = 0x7000;
			vert[3].Green  = 0x2000;
			vert[3].Blue   = 0x3000;
			GradientFill(hdc, &vert[2], 2, &gRect[0], 1, GRADIENT_FILL_RECT_H);
		}

		const TCHAR txtArch[] = _T("Archive");
		if ( GetTextExtentPoint32(hdc,txtArch, (int) _tcslen(txtArch), &TxtSize)) {
			TxtPt.x = ((rr.right - rr.left)-TxtSize.cx)/2;
			TxtPt.y = ((rr.bottom - rr.top)-TxtSize.cy)/2;
			if ( TxtPt.x < 0 ) TxtPt.x = 0;
			if ( TxtPt.y < 0 ) TxtPt.y = 0;
		} else {
			TxtPt.x = rr.left+5;
			TxtPt.y = rr.top+52;
		}
		TextOut(hdc, TxtPt.x+rr.left, TxtPt.y, txtArch, (int) _tcslen(txtArch));
	}

	if (!lpMainWindow->is_archive_only) {
		FillRect(hdc, &rl, hbrRAM);

		if (PtInRect(&rl, pt)) {
			gRect[0].UpperLeft  = 0;
			gRect[0].LowerRight = 1;
			gRect[1].UpperLeft  = 2;
			gRect[1].LowerRight = 3;
			vert[0].x      = rl.left;
			vert[0].y      = rl.top;
			vert[0].Red    = 0x3000;
			vert[0].Green  = 0x7000;
			vert[0].Blue   = 0x2000;
			vert[1].x      = rl.right / 3;
			vert[1].y      = rl.bottom;
			vert[1].Red    = 0x6000;
			vert[1].Green  = 0xC000;
			vert[1].Blue   = 0x4000;

			GradientFill(hdc, vert, 2, gRect, 1, GRADIENT_FILL_RECT_H);
			//0x60, 0xC0, 0x40
			vert[2].x      = 2*rl.right / 3;
			vert[2].y      = rl.bottom;
			vert[2].Red    = 0x6000;
			vert[2].Green  = 0xC000;
			vert[2].Blue   = 0x4000;
			vert[3].x      = rl.right;
			vert[3].y      = rl.top;
			vert[3].Red    = 0x3000;
			vert[3].Green  = 0x7000;
			vert[3].Blue   = 0x2000;

			GradientFill(hdc, &vert[2], 2, &gRect[0], 1, GRADIENT_FILL_RECT_H);
		}

		const TCHAR txtRam[] = _T("RAM");
		if ( GetTextExtentPoint32(hdc, txtRam, (int) _tcslen(txtRam), &TxtSize) ) {
			TxtPt.x = ((rl.right - rl.left)-TxtSize.cx)/2;
			TxtPt.y = ((rl.bottom - rl.top)-TxtSize.cy)/2;
			if ( TxtPt.x < 0 ) TxtPt.x =0;
			if ( TxtPt.y < 0 ) TxtPt.y =0;
		} else {
			TxtPt.x = rl.left+5;
			TxtPt.y = rl.top+52;
		}
		TextOut(hdc, TxtPt.x, TxtPt.y, txtRam, (int) _tcslen(txtRam));
	}

	DeleteObject(hbrRAM);
	DeleteObject(hbrArchive);

	return hdc;
}

HANDLE DDBToDIB(HBITMAP bitmap, DWORD dwCompression) 
{
	BITMAP			bm;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER 	lpbi;
	DWORD			dwLen;
	HANDLE			hDIB;
	HANDLE			handle;
	HDC 			hDC;


	// The function has no arg for bitfields
	if(dwCompression == BI_BITFIELDS)
		return NULL;

	// Get bitmap information
	GetObject(bitmap, sizeof(bm), (LPSTR) &bm);

	// Initialize the bitmapinfoheader
	bi.biSize		= sizeof(BITMAPINFOHEADER);
	bi.biWidth		= bm.bmWidth;
	bi.biHeight 		= bm.bmHeight;
	bi.biPlanes 		= 1;
	bi.biBitCount		= bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression	= dwCompression;
	bi.biSizeImage		= 0;
	bi.biXPelsPerMeter	= 0;
	bi.biYPelsPerMeter	= 0;
	bi.biClrUsed		= 0;
	bi.biClrImportant	= 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if( nColors > 256 ) 
		nColors = 0;
	dwLen  = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = GetDC(NULL);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED, dwLen);

	if (!hDIB){
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER) hDIB;

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver 
	// will calculate the biSizeImage field 
	GetDIBits(hDC, bitmap, 0L, (DWORD) bi.biHeight,
			(LPBYTE) NULL, (LPBITMAPINFO) lpbi, (DWORD) DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0) {
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) 
						* bi.biHeight;

		// If a compression scheme is used the result may in fact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;
	else{
		GlobalFree(hDIB);
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits( hDC, bitmap,
				0L,				// Start scan line
				(DWORD)bi.biHeight,		// # of scan lines
				/*(LPBYTE)lpbi 			// address for bitmap bits
				+ (bi.biSize + nColors * sizeof(RGBQUAD))*/ (LPBYTE) hDIB,
				(LPBITMAPINFO)lpbi,		// address of bitmapinfo
				(DWORD)DIB_RGB_COLORS);		// Use RGB for color table

	if( !bGotBits )
	{
		GlobalFree(hDIB);
		
		ReleaseDC(NULL,hDC);
		return NULL;
	}

	ReleaseDC(NULL, hDC);
	return hDIB;
}

void PaintLCD(HWND hwnd, HDC hdcDest, LPMAINWINDOW lpMainWindow) {
	if (lpMainWindow == NULL) {
		return;
	}

	unsigned char * screen;
	LPCALC lpCalc = lpMainWindow->lpCalc;
	if (lpCalc == NULL) {
		return;
	}

	LCDBase_t *lcd = lpCalc->cpu.pio.lcd;
	if (lcd == NULL) {
		_tprintf_s(_T("Invalid LCD pointer"));
		return;
	}
	RECT rc;
	GetClientRect(hwnd, &rc);

	HDC hdcOverlay, hdc = CreateCompatibleDC(hdcDest);
	HBITMAP bmpBuf;
	if (hdc == NULL) {
		_tprintf_s(_T("Creating buffer DC failed\n"));
		return;
	} else {
		bmpBuf = CreateCompatibleBitmap(hdcDest, rc.right - rc.left, rc.bottom - rc.top);
		if (bmpBuf == NULL)
			_tprintf_s(_T("Creating bitmap failed\n"));
		SelectObject(hdc, bmpBuf);
	}

	BITMAPINFO *info = lpCalc->model >= TI_84PCSE ? colorbi : bi;
	
	BOOL lcd_scaled = (rc.right - rc.left) % lcd->display_width;
	screen = lcd->image(lcd);

	if (lcd_scaled) {
		LONG clientWidth = rc.right - rc.left;
		LONG clientHeight = rc.bottom - rc.top;
		LONG clientScaleWidth = max(1, clientWidth / lcd->display_width) * lcd->display_width;
		LONG clientScaleHeight = max(1, clientHeight / lcd->height) * lcd->height;

		Graphics graphics(hdc);
		Bitmap lcdBitmap(info, screen);
		Bitmap scaledBitmap((INT)clientScaleWidth, (INT)clientScaleHeight, &graphics);
		Bitmap *bitmap = &lcdBitmap;
		if (clientScaleWidth != lcd->display_width && clientScaleHeight != lcd->height) {
			Graphics scaledGraphics(&scaledBitmap);
			scaledGraphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
			// +1 because otherwise there is a black line at max width and max height
			Rect scaleRect(rc.left, rc.top, clientScaleWidth + 1, clientScaleHeight + 1);
			scaledGraphics.DrawImage(&lcdBitmap, scaleRect, 0, 0, lcd->display_width, lcd->height, UnitPixel);
			bitmap = &scaledBitmap;
		}

		graphics.SetInterpolationMode(InterpolationModeLowQuality);
		Rect rect(rc.left, rc.top, clientWidth + 1, clientHeight + 1);
		graphics.DrawImage(bitmap, rect, 0, 0, clientScaleWidth, clientScaleHeight, UnitPixel);
		rect.Width--;
		rect.Height--;
		graphics.SetClip(rect);
	} else {
		SetStretchBltMode(hdc, BLACKONWHITE);
		if (StretchDIBits(hdc,
						rc.left, rc.top, rc.right - rc.left,  rc.bottom - rc.top,
						0, 0, lcd->display_width, lcd->height,
						screen,
						info,
						DIB_RGB_COLORS,
						SRCCOPY) == 0)
		{
			_tprintf_s(_T("error in SetDIBitsToDevice\n"));
		}
	}

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 160;
	bf.AlphaFormat = 0;

	if (lpMainWindow->bDoDrag == TRUE) {
		hdcOverlay = DrawDragPanes(hwnd, hdcDest, lpMainWindow);

		if (AlphaBlend(	hdc, 0, 0, rc.right, rc.bottom,
					hdcOverlay, 0, 0, rc.right, rc.bottom,
					bf ) == FALSE) 
		{
			_tprintf_s(_T("alpha blend 1 failed\n"));
		}

		DeleteDC(hdcOverlay);
	}

	bf.SourceConstantAlpha = 108;

	POINT pt;
	pt.x = rc.left;
	pt.y = rc.top;
	ClientToScreen(hwnd, &pt);
	ScreenToClient(GetParent(hwnd), &pt);

	if (alphablendfail < 100 && lpMainWindow->bAlphaBlendLCD && lcd->active != FALSE &&
		!lcd_scaled && lpCalc->model < TI_84PCSE) 
	{
		if (AlphaBlend(	hdc, rc.left, rc.top, rc.right,  rc.bottom,
			lpMainWindow->hdcSkin, lpMainWindow->rectLCD.left, lpMainWindow->rectLCD.top,
			lpMainWindow->rectLCD.right - lpMainWindow->rectLCD.left,
			lpMainWindow->rectLCD.bottom - lpMainWindow->rectLCD.top, bf) == FALSE) {
			//printf("alpha blend 2 failed\n");
			alphablendfail++;
		}
	}

#ifdef WITH_AVI
#include "avi_utils.h"
	HBITMAP hbm;
	if (is_recording) {
		BYTE *pBitsDest;
		hbm = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
		HDC newhdc = CreateCompatibleDC(hdc);
		SelectObject(newhdc, hbm);
		BitBlt(newhdc, rc.left, rc.top, rc.right - rc.left,  rc.bottom - rc.top, hdc, 0, 0, SRCCOPY);
		HANDLE ptr = DDBToDIB(hbm, BI_RGB);
		BITMAPINFO *nbi = (BITMAPINFO *) malloc(sizeof(BITMAPINFOHEADER));
		nbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		nbi->bmiHeader.biWidth = rc.right - rc.left;
		nbi->bmiHeader.biHeight = rc.bottom - rc.top;
		nbi->bmiHeader.biPlanes = 1;
		nbi->bmiHeader.biBitCount = 32;
		nbi->bmiHeader.biCompression = BI_RGB;
		nbi->bmiHeader.biSizeImage = 0;
		nbi->bmiHeader.biXPelsPerMeter = 0;
		nbi->bmiHeader.biYPelsPerMeter = 0;
		nbi->bmiHeader.biClrUsed = MAX_SHADES+1;
		nbi->bmiHeader.biClrImportant = MAX_SHADES+1;
		DWORD dwBmpSize = ((nbi->bmiHeader.biWidth * nbi->bmiHeader.biBitCount + 31) / 32) * 4 * nbi->bmiHeader.biHeight;
		HBITMAP newhbm = CreateDIBSection(newhdc, nbi, DIB_RGB_COLORS, (void **) &pBitsDest, NULL, NULL);
		memcpy(pBitsDest, ptr, dwBmpSize);
		currentAvi->AppendNewFrame(newhbm);
		DeleteObject(hbm);
		DeleteObject(newhbm);
		free(nbi);
	}
#endif
	if (BitBlt(	hdcDest, rc.left, rc.top, rc.right - rc.left,  rc.bottom - rc.top,
		hdc, 0, 0, SRCCOPY ) == FALSE) _tprintf_s(_T("Bit blit failed\n"));

	free(screen);
	DeleteObject(bmpBuf);
	DeleteDC(hdc);
}

static TCHAR sz_status[32];
LRESULT CALLBACK LCDProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {

	static POINT ptOffset;
	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	LPCALC lpCalc = NULL;
	if (lpMainWindow != NULL) {
		lpCalc = lpMainWindow->lpCalc;
	}

	switch (Message) {
		case WM_CREATE:
		{
			HDC hdc = GetDC(hwnd);
			lpMainWindow = (LPMAINWINDOW) ((LPCREATESTRUCT) lParam)->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)lpMainWindow);

			SetBkMode(hdc, TRANSPARENT);

			FORMATETC fmtetc[] = {
				{CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL},
				{RegisterClipboardFormat(CFSTR_FILEDESCRIPTORW), 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL}
			};

			if (lpMainWindow->hwndLCD == NULL) {
				RegisterDropWindow(hwnd, (IDropTarget **) &lpMainWindow->pDropTarget);
				lpMainWindow->pDropTarget->AddAcceptedFormat(&fmtetc[0]);
				lpMainWindow->pDropTarget->AddAcceptedFormat(&fmtetc[1]);
			}

			if (bi == NULL) {
				bi = (BITMAPINFO *) malloc(sizeof(BITMAPINFOHEADER) + (MAX_SHADES+1)*sizeof(RGBQUAD));
				bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bi->bmiHeader.biWidth = 128;
				bi->bmiHeader.biHeight = -64;
				bi->bmiHeader.biPlanes = 1;
				bi->bmiHeader.biBitCount = 8;
				bi->bmiHeader.biCompression = BI_RGB;
				bi->bmiHeader.biSizeImage = 0;
				bi->bmiHeader.biXPelsPerMeter = 0;
				bi->bmiHeader.biYPelsPerMeter = 0;
				bi->bmiHeader.biClrUsed = MAX_SHADES+1;
				bi->bmiHeader.biClrImportant = MAX_SHADES+1;

				//#define LCD_LOW (RGB(0x9E, 0xAB, 0x88))
				int i;
#define LCD_HIGH	255
				for (i = 0; i <= MAX_SHADES; i++) {
					bi->bmiColors[i].rgbRed = (0x9E*(256-(LCD_HIGH/MAX_SHADES)*i))/255;
					bi->bmiColors[i].rgbGreen = (0xAB*(256-(LCD_HIGH/MAX_SHADES)*i))/255;
					bi->bmiColors[i].rgbBlue = (0x88*(256-(LCD_HIGH/MAX_SHADES)*i))/255;
				}
			}

			if (colorbi == NULL) {
				colorbi = (BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD));
				colorbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				colorbi->bmiHeader.biWidth = COLOR_LCD_WIDTH;
				colorbi->bmiHeader.biHeight = -COLOR_LCD_HEIGHT;
				colorbi->bmiHeader.biPlanes = 1;
				colorbi->bmiHeader.biBitCount = 24;
				colorbi->bmiHeader.biCompression = BI_RGB;
				colorbi->bmiHeader.biSizeImage = 0;
				colorbi->bmiHeader.biXPelsPerMeter = 0;
				colorbi->bmiHeader.biYPelsPerMeter = 0;
				colorbi->bmiHeader.biClrUsed = 0;
				colorbi->bmiHeader.biClrImportant = 0;
			}

			ReleaseDC(hwnd, hdc);

			return 0;
		}
		case WM_PAINT:
		{
			HDC hdcDest;
			if (lpCalc == NULL) {
				break;
			}

			LCDBase_t *lcd = lpCalc->cpu.pio.lcd;
			if (lcd == NULL) {
				break;
			}

			PAINTSTRUCT ps;
			hdcDest = BeginPaint(hwnd, &ps);
			PaintLCD(hwnd, hdcDest, lpMainWindow);
			EndPaint(hwnd, &ps);	
			
			if (lpMainWindow->hwndStatusBar) {
				if (clock() > lpMainWindow->sb_refresh + CLOCKS_PER_SEC / 2) {
					if (lcd && lcd->active)
						StringCbPrintf(sz_status, sizeof(sz_status), _T("FPS: %0.2lf"), lcd->ufps);
					else
						StringCbPrintf(sz_status, sizeof(sz_status),  _T("FPS: -"));
					SendMessage(lpMainWindow->hwndStatusBar, SB_SETTEXT, 0, (LPARAM)sz_status);
					lpMainWindow->sb_refresh = clock();
				}
			}
			return 0;
		}
		case WM_NCCALCSIZE:
			return 0;
		case WM_SIZE: {
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		}
		case WM_CONTEXTMENU:
			{
				HMENU hmenuMain = GetMenu(lpMainWindow->hwndFrame);
				if (hmenuMain == NULL) {
					return 0;
				}

				TCHAR menuStringBuffer[256];
				int i = 0;
				BOOL success = TRUE;
				HMENU hmenuContext = CreatePopupMenu();
				while (success) {
					MENUITEMINFO menuItemInfo = {0};
					menuItemInfo.cbSize = sizeof(MENUITEMINFO);
					menuItemInfo.fMask = MIIM_STRING;
					menuItemInfo.cch = 255;
					menuItemInfo.dwTypeData = menuStringBuffer;
					success = GetMenuItemInfo(hmenuMain, i, TRUE, &menuItemInfo);
					if (success) {
						InsertMenu(hmenuContext, -1, MF_BYPOSITION | MF_POPUP, (UINT_PTR) GetSubMenu(hmenuMain, i), menuItemInfo.dwTypeData);
						i++;
					}
				}

				if (!OnContextMenu(hwnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), hmenuContext)) {
					DefWindowProc(hwnd, Message, wParam, lParam);
				}

				//DestroyMenu(hmenuContext);
				return 0;
			}
		case WM_CLOSE:
		case WM_COMMAND: {
			SendMessage(lpMainWindow->hwndFrame, Message, wParam, lParam);
			return 0;
		}
		case WM_LBUTTONDOWN:
			//WriteRIFFHeader();
			break;
		case WM_LBUTTONUP: {
			static DWORD dwDragCountdown = 0;
				dwDragCountdown = 0;
			break;
		}

		case WM_MOUSEMOVE: {
				static DWORD dwDragCountdown = 0;
				if (wParam != MK_LBUTTON) {
					dwDragCountdown = 0;
				} else if (gif_write_state == GIF_IDLE) {
					// TODO: make this not ugly and work
					if (++dwDragCountdown < (u_int) GetSystemMetrics(SM_CXDRAG)) return 0;

					CDataObject *pDataObject;
					CDropSource *pDropSource;
					IDragSourceHelper2 *pDragSourceHelper;

					RECT rc;
					GetClientRect(hwnd, &rc);

					ptOffset.x = GET_X_LPARAM(lParam);
					ptOffset.y = GET_Y_LPARAM(lParam);

					// Create the GIF that is going to be produced by the drag
					TCHAR fn[MAX_PATH];
					GetStorageString(fn, sizeof(fn));
					StringCbCat(fn, sizeof(fn), _T("\\wabbitemu.png"));

					if (lpCalc == NULL) {
						break;
					}

					LCDBase_t *lcd = lpCalc->cpu.pio.lcd;
					if (lcd == NULL) {
						break;
					}
					
					export_png(lpCalc, fn);

					int file_size = 0;
					FILE *file;
					_tfopen_s(&file, fn, _T("rb"));
					if (file != NULL) {
						fseek(file, 0L, SEEK_END);
						file_size = ftell(file);
						fclose(file);
					}

					FORMATETC fmtetc[] = {
						{RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR), 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL },
						{RegisterClipboardFormat(CFSTR_FILECONTENTS), 0, DVASPECT_CONTENT, 0, TYMED_HGLOBAL },
						{RegisterClipboardFormat(_T("WabbitShot")), 0, DVASPECT_CONTENT, 0, TYMED_NULL},
						{CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL },
					};

					STGMEDIUM stgmed[NumElm(fmtetc)];
					ZeroMemory(stgmed, sizeof(stgmed));

					// Do the file group descriptor
					stgmed[0].hGlobal = GlobalAlloc(GHND, sizeof(FILEGROUPDESCRIPTOR) + sizeof(FILEDESCRIPTOR));
					stgmed[0].tymed = TYMED_HGLOBAL;

					FILEGROUPDESCRIPTOR *fgd = (FILEGROUPDESCRIPTOR *) GlobalLock(stgmed[0].hGlobal);
					fgd->cItems = 1;

					FILEDESCRIPTOR *fd = fgd->fgd;
					ZeroMemory(fd, sizeof(FILEDESCRIPTOR));

					fd->dwFlags = FD_ATTRIBUTES | FD_FILESIZE;
					fd->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
					fd->nFileSizeLow = file_size;
					StringCbCopy(fd->cFileName, sizeof(fd->cFileName), _T("wabbitemu.png"));
					GlobalUnlock(stgmed[0].hGlobal);

					// Now do file contents
					stgmed[1].hGlobal = GlobalAlloc(GHND, file_size);
					stgmed[1].tymed = TYMED_HGLOBAL;

					FILE *fgif;
					_tfopen_s(&fgif, fn, _T("rb"));
					if (fgif != NULL) {
						char *buf = (char *) GlobalLock(stgmed[1].hGlobal);
						fread(buf, file_size, 1, fgif);
						fclose(fgif);

						GlobalUnlock(stgmed[1].hGlobal);
					} else {
						MessageBox(NULL, _T("Opening the PNG failed\n"), _T("Wabbitemu"), MB_OK);
					}


					// Create the CH_DROP that many apps can use
					stgmed[3].hGlobal = GlobalAlloc(GHND, sizeof(DROPFILES) + _tcslen(fn) + 2);
					stgmed[3].tymed = TYMED_HGLOBAL;

					DROPFILES *df = (DROPFILES *) GlobalLock(stgmed[3].hGlobal);

					df[0].fWide = FALSE;
					df[0].pFiles = sizeof(DROPFILES);
					df[0].fNC = FALSE;

					memset(&df[1], 0, _tcslen(fn) + 2);
					StringCbCopy((TCHAR *) &df[1], _tcslen(fn) + 1, fn);
					GlobalUnlock(stgmed[3].hGlobal);

					// Create IDataObject and IDropSource COM objects
					HRESULT hres;
					pDropSource = new CDropSource();
					hres = CreateDataObject(fmtetc, stgmed, NumElm(fmtetc), (IDataObject **) &pDataObject);
					if (hres != S_OK) {
						MessageBox(hwnd, _T("Error in CreateDataObject"), _T("Error"), MB_OK);
						pDropSource->Release();
						return 0;
					}

					IID IID_IDragSourceHelper2;
					CLSIDFromString((LPOLESTR) L"{83E07D0D-0C5F-4163-BF1A-60B274051E40}", &IID_IDragSourceHelper2);

					hres = CoCreateInstance(
						CLSID_DragDropHelper,
						NULL,
						CLSCTX_INPROC_SERVER,
						IID_IDragSourceHelper,
						(LPVOID *) &pDragSourceHelper);
					if (hres != S_OK) {
						TCHAR buf[64];
						StringCbPrintf(buf, sizeof(buf), _T("Error in CoCreateInstance\r\nError code: %X"), hres);
						MessageBox(hwnd, buf, _T("Error"), MB_OK);
						pDropSource->Release();
						return 0;
					}

					hres = pDataObject->QueryInterface(IID_IDataObject, (LPVOID *) &pDropSource->m_pDataobject);
					if (hres != S_OK) {
						MessageBox(hwnd, _T("Error in pDataObject->QueryInterface"), _T("Error"), MB_OK);
						pDragSourceHelper->Release();
						pDataObject->Release();
						pDropSource->Release();
						return 0;
					}
					DWORD dwEffect = DROPEFFECT_NONE;
					hres = DoDragDrop((IDataObject*) pDataObject, (IDropSource*) pDropSource,  DROPEFFECT_COPY, &dwEffect);
					if (hres != S_OK && hres != DRAGDROP_S_CANCEL && hres != DRAGDROP_S_DROP) {
						MessageBox(hwnd, _T("Error in DoDragDrop"), _T("Error"), MB_OK);
					}

					pDragSourceHelper->Release();
					pDataObject->Release();
					pDropSource->Release();
				}
				return 0;
		}

		case WM_COPYDATA: {
			PCOPYDATASTRUCT copyDataStruct = (PCOPYDATASTRUCT) lParam;
			int size = (int) copyDataStruct->cbData;
			TCHAR filePath[MAX_PATH];
			StringCbCopy(filePath, sizeof(filePath), (TCHAR *) copyDataStruct->lpData);
			int ram = (int) copyDataStruct->dwData;
			filePath[size] = '\0';

			if (size && filePath)	{
				TCHAR *FileNames = (TCHAR *) malloc(size);
				ZeroMemory(FileNames, size + 1);
				StringCbCopy(FileNames, size + 1, filePath);
				SendFileToCalc(lpMainWindow->hwndFrame, lpCalc, FileNames, ram);
			}
			break;
		}

		case WM_DROPFILES: {
			TCHAR fn[256];
			int count = DragQueryFile((HDROP) wParam, ~0, fn, 256);

			while (count--) {
				DragQueryFile((HDROP) wParam, count, fn, 256);
				SendFileToCalc(lpMainWindow->hwndFrame, lpCalc, fn, TRUE, DropMemoryTarget(hwnd));
			}
			return 0;
		}

		case WM_KEYDOWN:
		case WM_KEYUP: {
			return SendMessage(lpMainWindow->hwndFrame, Message, wParam, lParam);
			}
		case WM_DESTROY: {
			if (calc_count() == 1 && is_exiting) {
				free(bi);
				free(colorbi);
			}

			if (hwnd == lpMainWindow->hwndLCD) {
				UnregisterDropWindow(hwnd, (IDropTarget *) lpMainWindow->pDropTarget);
			}
			return 0;
		}
		default:
			if (Message == RegisterWindowMessage(_T("ShellGetDragImage"))) {
				LPSHDRAGIMAGE pDragImage = (LPSHDRAGIMAGE) lParam;
				RECT rc;
				GetClientRect(hwnd, &rc);

				pDragImage->sizeDragImage.cx = 96;
				pDragImage->sizeDragImage.cy = 96;
				pDragImage->ptOffset = ptOffset;
				pDragImage->crColorKey = RGB(0, 0, 0); // <-fuck you

				HDC hdc = CreateCompatibleDC(NULL);
				BYTE *pBits;

					//CreateBitmap(pDragImage->sizeDragImage.cx, pDragImage->sizeDragImage.cy, 1, 32, NULL);
				BITMAPINFO bmi;
				ZeroMemory(&bmi, sizeof(BITMAPINFO));

				BITMAPINFOHEADER *bih = &bmi.bmiHeader;
				bih->biSize = sizeof(BITMAPINFOHEADER);
				bih->biWidth = pDragImage->sizeDragImage.cx;
				bih->biHeight = pDragImage->sizeDragImage.cy;
				bih->biPlanes = 1;
				bih->biBitCount = 32;
				bih->biCompression = BI_RGB;

				HBITMAP hbmDrag = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void **) &pBits, NULL, 0);

				HBITMAP hbmOld = (HBITMAP) SelectObject(hdc, hbmDrag);

				LPCALC lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
				u_char *screen = lpCalc->cpu.pio.lcd->image(lpCalc->cpu.pio.lcd);
				FillRect(hdc, &rc, (HBRUSH) GetStockObject(BLACK_BRUSH));

				// TODO: fix
				//screen = GIFGREYLCD();
				if (StretchDIBits(
					hdc,
					0, 16, 96,  64,
					0, 0, 96, 64,
					screen,
					bi,
					DIB_RGB_COLORS,
					SRCCOPY) == 0) {

					printf("error in SetDIBitsToDevice\n");
				}
				BYTE * pPixel = pBits +  16 * 96 * 4 - 4;
				BYTE * pPixelTop = pBits + 96 * 96 * 4 - 16 * 96 * 4;
				int i, j;
				for (i = 16*96*4 - 4, j = 16; j > 0; i -= 4, pPixel -= 4, pPixelTop += 4) {
					pPixel[3] = pPixelTop[3] = 42 - ((42 / 16) * (16 - j));

					pPixelTop[0] = pPixel[0] = pPixel[0] * pPixel[3] / 0xFF;
					pPixelTop[1] = pPixel[1] = pPixel[1] * pPixel[3] / 0xFF;
					pPixelTop[2] = pPixel[2] = pPixel[2] * pPixel[3] / 0xFF;

					if ((i % (96*4)) == 0)
						j--;
				}

				RECT r;
				SetRect(&r, 0, 16, 97, 16+64+1);

				//MoveToEx(hdc, 0, 15, NULL);
				SelectObject(hdc, GetStockObject(DC_PEN));
				SetDCPenColor(hdc, RGB(90, 90, 90));
				SelectObject(hdc, GetStockObject(NULL_BRUSH));
				//LineTo(hdc, 96, 15);

				//MoveToEx(hdc, 0, 96-16+1, NULL);
				//LineTo(hdc, 96, 96-16+1);
				Rectangle(hdc, 0, 15, 96, 16+64+1);

				SelectObject(hdc, hbmOld);
				DeleteDC(hdc);

				pDragImage->hbmpDragImage = hbmDrag;

				return 0;
			}
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}