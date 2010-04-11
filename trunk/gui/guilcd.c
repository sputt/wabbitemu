#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <objbase.h>
#include "guilcd.h"
#include "calc.h"
#include "sendfiles.h"
#include "sound.h"
#include "gif.h"
#include "guioptions.h"
#include "link.h"
#include "rsrc.h"
#include "savestate.h"
#include <shlobj.h>
#include "wbded.h"
#include "guifilepreview.h"
#include "guicontext.h"
#include "guiopenfile.h"
#ifdef USE_DIRECTX
#include <D3D9.h>
#endif

extern POINT drop_pt;
extern RECT db_rect;
extern HINSTANCE g_hInst;
extern HDC hdcSkin;

static int alphablendfail = 0;

BITMAPINFO *bi;


HDC DrawSending(HWND hwnd, HDC hdcDest) {
	TCHAR Sendstr1[] = "Sending File(s)";
	TCHAR Filecntstr[64];

	static HBITMAP bmpSend = NULL;
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);

	// Create the device context that holds the overlay
	HDC hdc = CreateCompatibleDC(hdcDest);

	if (bmpSend != NULL) DeleteObject(bmpSend);
	bmpSend =
	CreateCompatibleBitmap(hdcDest, clientRect.right, clientRect.bottom);

	SelectObject(hdc, bmpSend);


	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(0x00,0x00,0x00));


	HBRUSH hbrSend = CreateSolidBrush(SEND_COLOR);
	FillRect(hdc, &clientRect, hbrSend);

	DeleteObject(hbrSend);

	return hdc;
}


HDC DrawDragPanes(HWND hwnd, HDC hdcDest, int mode) {
	RECT rl, rr, clientRect;
	GetClientRect(hwnd, &clientRect);
	SIZE TxtSize;
	POINT TxtPt;

	CopyRect(&rl, &clientRect);
	CopyRect(&rr, &clientRect);

	if (calcs[gslot].model >= TI_83P) {
		rl.right = rr.left = rr.right/2;
	}

	// Create the device context that holds the overlay
	HDC hdc = CreateCompatibleDC(hdcDest);
	HBITMAP bmpDragPane =
	CreateCompatibleBitmap(hdcDest, clientRect.right, clientRect.bottom);

	SelectObject(hdc, bmpDragPane);

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(0xFF,0xFF,0xFF));
	HBRUSH hbrRAM = CreateSolidBrush(RAM_COLOR);
	HBRUSH hbrArchive = CreateSolidBrush(ARCHIVE_COLOR);

	if (!hbrRAM || !hbrArchive) {
		printf("Brush creation failed\n");
		return hdc;
	}

	POINT pt = drop_pt;
	ScreenToClient(hwnd, &pt);

	TRIVERTEX vert[4];
	GRADIENT_RECT gRect[2];

	if (calcs[gslot].model >= TI_83P) {

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

			GradientFill(hdc,vert,2,gRect,1,GRADIENT_FILL_RECT_H);

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
			GradientFill(hdc,&vert[2],2,&gRect[0],1,GRADIENT_FILL_RECT_H);
		}

		TCHAR txtArch[]="Archive";
		if ( GetTextExtentPoint32(hdc,txtArch,strlen(txtArch),&TxtSize) ) {
			TxtPt.x = ((rr.right - rr.left)-TxtSize.cx)/2;
			TxtPt.y = ((rr.bottom - rr.top)-TxtSize.cy)/2;
			if ( TxtPt.x < 0 ) TxtPt.x =0;
			if ( TxtPt.y < 0 ) TxtPt.y =0;
		} else {
			TxtPt.x = rr.left+5;
			TxtPt.y = rr.top+52;
		}
		TextOut(hdc, TxtPt.x+rr.left, TxtPt.y, txtArch, strlen(txtArch));
	}

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
		vert[1].x      = rl.right/3;
		vert[1].y      = rl.bottom;
		vert[1].Red    = 0x6000;
		vert[1].Green  = 0xC000;
		vert[1].Blue   = 0x4000;

		GradientFill(hdc,vert,2,gRect,1,GRADIENT_FILL_RECT_H);
		//0x60, 0xC0, 0x40
		vert[2].x      = 2*rl.right/3;
		vert[2].y      = rl.bottom;
		vert[2].Red    = 0x6000;
		vert[2].Green  = 0xC000;
		vert[2].Blue   = 0x4000;
		vert[3].x      = rl.right;
		vert[3].y      = rl.top;
		vert[3].Red    = 0x3000;
		vert[3].Green  = 0x7000;
		vert[3].Blue   = 0x2000;

		GradientFill(hdc,&vert[2],2,&gRect[0],1,GRADIENT_FILL_RECT_H);
	}

	TCHAR txtRam[] ="RAM";
	if ( GetTextExtentPoint32(hdc,txtRam,strlen(txtRam),&TxtSize) ) {
		TxtPt.x = ((rl.right - rl.left)-TxtSize.cx)/2;
		TxtPt.y = ((rl.bottom - rl.top)-TxtSize.cy)/2;
		if ( TxtPt.x < 0 ) TxtPt.x =0;
		if ( TxtPt.y < 0 ) TxtPt.y =0;
	} else {
		TxtPt.x = rl.left+5;
		TxtPt.y = rl.top+52;
	}
	TextOut(hdc, TxtPt.x, TxtPt.y, txtRam, strlen(txtRam));

	DeleteObject(hbrRAM);
	DeleteObject(hbrArchive);

	return hdc;
}

void PaintLCD(HWND hwnd, HDC hdcDest) {
	unsigned char * screen;
	LCD_t *lcd = calcs[gslot].cpu.pio.lcd;
	RECT rc;
	GetClientRect(calcs[gslot].hwndLCD, &rc);

	HDC hdcOverlay, hdc = CreateCompatibleDC(hdcDest);
	HBITMAP bmpBuf;
	if (hdc == NULL) {
		printf("Creating buffer DC failed\n");
		return;
	} else {
		bmpBuf = CreateCompatibleBitmap(hdcDest, rc.right - rc.left, rc.bottom - rc.top);
		if (bmpBuf == NULL)
			printf("Creating bitmap failed\n");
		SelectObject(hdc, bmpBuf);
	}

	if (lcd->active == FALSE) {
		BYTE lcd_data[128*64];
		memset(lcd_data, 0, sizeof(lcd_data));
		//for (i = 0; i < 128*64; i++) lcd_data[i]=0x00; //whitest pixel

		if (StretchDIBits(
			hdc,
			rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			0, 0, lcd->width, 64,
			lcd_data,
			bi,
			DIB_RGB_COLORS,
			SRCCOPY) == 0) {

			printf("error in SetDIBitsToDevice\n");
		}


		if (calcs[gslot].do_drag == TRUE) {

			hdcOverlay = DrawDragPanes(hwnd, hdcDest, 0);
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 160;
			bf.AlphaFormat = 0;
			if (AlphaBlend(	hdc, 0, 0, rc.right, rc.bottom,
						hdcOverlay, 0, 0, rc.right, rc.bottom,
						bf ) == FALSE) printf("alpha blend 1 failed\n");

			DeleteDC(hdcOverlay);

		}


		if (BitBlt(	hdcDest, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			hdc, 0, 0, SRCCOPY ) == FALSE) printf("Bit blt failed\n");

	} else {
		screen = LCD_image( calcs[gslot].cpu.pio.lcd ) ;
		//screen = GIFGREYLCD();

		if (StretchDIBits(
			hdc,
			rc.left, rc.top, rc.right - rc.left,  rc.bottom - rc.top,
			0, 0, lcd->width, 64,
			screen,
			bi,
			DIB_RGB_COLORS,
			SRCCOPY) == 0) {

			printf("error in SetDIBitsToDevice\n");
		}

		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 160;
		bf.AlphaFormat = 0;

		if (calcs[gslot].do_drag == TRUE) {

			hdcOverlay = DrawDragPanes(hwnd, hdcDest, 0);

			if (AlphaBlend(	hdc, 0, 0, rc.right, rc.bottom,
						hdcOverlay, 0, 0, rc.right, rc.bottom,
						bf ) == FALSE) printf("alpha blend 1 failed\n");

			DeleteDC(hdcOverlay);

		}


		if (calcs[gslot].send == TRUE) {
			bf.SourceConstantAlpha = 192;
			hdcOverlay = DrawSending(hwnd, hdcDest);

			if (AlphaBlend(	hdc, 0, 0, rc.right, rc.bottom,
						hdcOverlay, 0, 0, rc.right, rc.bottom,
						bf ) == FALSE) printf("alpha blend send failed\n");

			DeleteDC(hdcOverlay);
		}

		bf.SourceConstantAlpha = 108;

		POINT pt;
		pt.x = rc.left;
		pt.y = rc.top;
		ClientToScreen(hwnd, &pt);
		ScreenToClient(GetParent(hwnd), &pt);

		if (alphablendfail<100) {
			if (AlphaBlend(	hdc, rc.left, rc.top, rc.right,  rc.bottom,
				calcs[gslot].hdcSkin, calcs[gslot].rectLCD.left, calcs[gslot].rectLCD.top, 192, 128,
					bf )  == FALSE){
				//printf("alpha blend 2 failed\n");
				alphablendfail++;
			}
		}

		if (BitBlt(	hdcDest, rc.left, rc.top, rc.right - rc.left,  rc.bottom - rc.top,
			hdc, 0, 0, SRCCOPY ) == FALSE) printf("Bit blt failed\n");


	}
	DeleteObject(bmpBuf);
	DeleteDC(hdc);
}



#include "guisavestate.h"

void SaveStateDialog(HWND hwnd){
	OPENFILENAME ofn;
	char FileName[MAX_PATH];
	char lpstrFilter[] 	= "\
Known File types ( *.sav; *.rom; *.bin) \0*.sav;*.rom;*.bin\0\
Save States  (*.sav)\0*.sav\0\
ROMS  (*.rom; .bin)\0*.rom;*.bin\0\
All Files (*.*)\0*.*\0\0";

	ZeroMemory(&ofn, sizeof(ofn));
	ZeroMemory(FileName, MAX_PATH);

	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= calcs[gslot].hwndLCD;
	ofn.lpstrFilter		= (LPCTSTR) lpstrFilter;
	ofn.lpstrFile		= FileName;
	ofn.nMaxFile		= MAX_PATH;
	ofn.lpstrTitle		= "Wabbitemu Save State";
	ofn.Flags			= OFN_PATHMUSTEXIST | OFN_EXPLORER |
						  OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	ofn.lpstrDefExt			= "sav";

	if (!GetSaveFileName(&ofn)) return;

	SAVESTATE_t* save = SaveSlot(gslot);

	gui_savestate(hwnd, save, FileName);

}

void LoadStateDialog(HWND hwnd){
	OPENFILENAME ofn;
	char FileName[MAX_PATH];
	char lpstrFilter[] 	= "\
Known File types ( *.sav; *.rom; *.bin) \0*.sav;*.rom;*.bin\0\
Save States  (*.sav)\0*.sav\0\
ROMS  (*.rom; .bin)\0*.rom;*.bin\0\
All Files (*.*)\0*.*\0\0";

	ZeroMemory(&ofn, sizeof(ofn));
	ZeroMemory(FileName, MAX_PATH);

	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= calcs[gslot].hwndLCD;
	ofn.lpstrFilter		= (LPCTSTR) lpstrFilter;
	ofn.lpstrFile		= FileName;
	ofn.nMaxFile		= MAX_PATH;
	ofn.lpstrTitle		= "Wabbitemu Load State";
	ofn.Flags			= OFN_PATHMUSTEXIST | OFN_EXPLORER |
						  OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	ofn.lpstrDefExt			= "sav";
	if (!GetOpenFileName(&ofn)) return;
	printf("%s \n",FileName);
	BOOL Running_Backup = calcs[gslot].running;
	calcs[gslot].running = FALSE;
	rom_load(gslot,FileName);
	calcs[gslot].running =  Running_Backup;

}

HRESULT CreateDropSource(WB_IDropSource **ppDropSource);
HRESULT CreateDataObject(FORMATETC *fmtetc, STGMEDIUM *stgmeds, UINT count, WB_IDataObject **ppDataObject);
void SetDropSourceDataObject(WB_IDropSource *pDropSource, WB_IDataObject *pDataObject);

LRESULT CALLBACK LCDProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {

	static POINT ptOffset;

#ifdef USE_DIRECTX
	static IDirect3DSurface9 *pd3dSurface = NULL;
#endif
    switch (Message) {
		case WM_CREATE:
		{
			HDC hdc = GetDC(hwnd);

			SetBkMode(hdc, TRANSPARENT);
            RegisterDropWindow(hwnd, &calcs[gslot].pDropTarget);

            if (calc_count() == 1) {
				bi = malloc(sizeof(BITMAPINFOHEADER) + (MAX_SHADES+1)*sizeof(RGBQUAD));
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
            ReleaseDC(hwnd, hdc);

			return 0;
		}
		case WM_PAINT:
		{
			//RECT rc;
			//GetWindowRect(calcs[gslot].hwndFrame, &rc);
			//OffsetRect(&rc, LCD_X, LCD_Y);
			//SetWindowPos(hwnd, HWND_TOP, rc.left, rc.top, 0, 0, SWP_NOSIZE);

			HDC hdcDest;
			LCD_t *lcd = calcs[gslot].cpu.pio.lcd;

			PAINTSTRUCT ps;
			hdcDest = BeginPaint(hwnd, &ps);
			PaintLCD(hwnd, hdcDest);
			EndPaint(hwnd, &ps);


#ifdef USE_DIRECTX
			if (pd3dSurface == NULL) {
				pd3dDevice->lpVtbl->CreateOffscreenPlainSurface(
					pd3dDevice,
					192,
					128,
					D3DFMT_X8R8G8B8,
					D3DPOOL_DEFAULT,
					&pd3dSurface,
					NULL);
			}

			pd3dDevice->lpVtbl->BeginScene(pd3dDevice);

			HDC hdc;
			pd3dSurface->lpVtbl->GetDC(pd3dSurface, &hdc);
			PaintLCD(hwnd, hdc);
			pd3dSurface->lpVtbl->ReleaseDC(pd3dSurface, hdc);

			IDirect3DSurface9 *pd3dDest;
			pd3dDevice->lpVtbl->GetBackBuffer(pd3dDevice, 0, 0, D3DBACKBUFFER_TYPE_MONO, &pd3dDest);

			pd3dDevice->lpVtbl->StretchRect(pd3dDevice, pd3dSurface, NULL, pd3dDest, NULL, D3DTEXF_NONE );

			pd3dDest->lpVtbl->Release(pd3dDest);

			pd3dDevice->lpVtbl->EndScene(pd3dDevice);

			pd3dDevice->lpVtbl->Present( pd3dDevice, NULL, NULL, NULL, NULL );
			ValidateRect(hwnd, NULL);
#endif
			if (calcs[gslot].hwndStatusBar) {
				if (clock() > calcs[gslot].sb_refresh + CLOCKS_PER_SEC/2) {
					char sz_status[32];
					if (lcd->active)
						sprintf(sz_status,"FPS: %0.2lf",lcd->ufps);
					else
						sprintf(sz_status,"FPS: -");

					SendMessage(calcs[gslot].hwndStatusBar, SB_SETTEXT, 0, (LPARAM) sz_status);
					calcs[gslot].sb_refresh = clock();
				}
			}
			return 0;
		}
		case WM_NCCALCSIZE:
		    return 0;
		case WM_SIZE:
		{
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		}
		case WM_CONTEXTMENU:
		{
			const char names[][32] = {"File", "Edit", "Calculator", "Debug", "About"};
			HMENU hmenuMain = GetMenu(calcs[gslot].hwndFrame);

			HMENU hmenuContext = CreatePopupMenu();
			int i;
			for (i = 0; i < 4; i++)
				InsertMenu(hmenuContext, -1, MF_BYPOSITION | MF_POPUP, GetSubMenu(hmenuMain, i), (LPCTSTR) names[i]);

			if (!OnContextMenu(hwnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), hmenuContext)) {
				DefWindowProc(hwnd, Message, wParam, lParam);
			}

			//DestroyMenu(hmenuContext);
			return 0;
		}
		case WM_COMMAND:
		{
			SendMessage(calcs[gslot].hwndFrame, Message, wParam, lParam);
			return 0;
		}
		case WM_LBUTTONDOWN:
			//WriteRIFFHeader();
			break;
		case WM_LBUTTONUP:
		{
			static DWORD dwDragCountdown = 0;
			dwDragCountdown = 0;
			break;

		case WM_MOUSEMOVE:
			if (wParam != MK_LBUTTON) { // || calcs[gslot].cpu.pio.lcd->active == FALSE) {
				dwDragCountdown = 0;
			} else if (gif_write_state == GIF_IDLE) {
				if (++dwDragCountdown < GetSystemMetrics(SM_CXDRAG)) return 0;

				WB_IDataObject *pDataObject;
				WB_IDropSource *pDropSource;
				IDragSourceHelper2 *pDragSourceHelper;

				RECT rc;
				GetClientRect(hwnd, &rc);

				ptOffset.x = GET_X_LPARAM(lParam);
				ptOffset.y = GET_Y_LPARAM(lParam);

				// Create the GIF that is going to be produced by the drag
				char temp_fn[L_tmpnam];
				char fn[MAX_PATH];
				strcpy(fn, getenv("appdata"));
				strcat(fn, "\\wabbitemu.gif");

				strcpy(gif_file_name, fn);

				gif_xs = SCRXSIZE*gif_size;
				gif_ys = SCRYSIZE*gif_size;

				GIFGREYLCD();

				int i, j;
				for (i = 0; i < SCRYSIZE*gif_size; i++)
					for (j = 0; j < SCRXSIZE*gif_size; j++)
						gif_frame[i * gif_xs + j] = calcs[gslot].cpu.pio.lcd->gif[i][j];

				gif_write_state = GIF_START;
				gif_writer();

				gif_write_state = GIF_END;
				gif_writer();

				FORMATETC fmtetc[] = {
					{RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR), 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL },
					{RegisterClipboardFormat(CFSTR_FILECONTENTS), 0, DVASPECT_CONTENT, 0, TYMED_HGLOBAL },
					{RegisterClipboardFormat("WabbitShot"), 0, DVASPECT_CONTENT, 0, TYMED_NULL},
					{CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL },
				};

				STGMEDIUM stgmed[NumElm(fmtetc)];
				ZeroMemory(stgmed, sizeof(stgmed));

				// Do the file group descriptor
				stgmed[0].hGlobal = GlobalAlloc(GHND,
									sizeof(FILEGROUPDESCRIPTOR) + sizeof(FILEDESCRIPTOR));
				stgmed[0].tymed = TYMED_HGLOBAL;

				FILEGROUPDESCRIPTOR *fgd = GlobalLock(stgmed[0].hGlobal);
				fgd->cItems = 1;

				FILEDESCRIPTOR *fd = fgd->fgd;

				ZeroMemory(fd, sizeof(FILEDESCRIPTOR));

				fd->dwFlags = FD_ATTRIBUTES | FD_FILESIZE;
				fd->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
				fd->nFileSizeLow = gif_file_size;
				strcpy(fd->cFileName, "wabbitemu.gif");

				GlobalUnlock(stgmed[0].hGlobal);


				// Now do file contents
				stgmed[1].hGlobal = GlobalAlloc(GHND, gif_file_size);
				stgmed[1].tymed = TYMED_HGLOBAL;

				FILE *fgif = fopen(fn, "rb");
				if (fgif != NULL) {
					char *buf = GlobalLock(stgmed[1].hGlobal);

					fread(buf, gif_file_size, 1, fgif);
					fclose(fgif);

					GlobalUnlock(stgmed[1].hGlobal);
				} else {
					MessageBox(NULL, "Opening the GIF failed\n", "Wabbitemu", MB_OK);
				}


				// Create the CH_DROP that many apps can use
				stgmed[3].hGlobal = GlobalAlloc(GHND, sizeof(DROPFILES) + strlen(fn) + 2);
				stgmed[3].tymed = TYMED_HGLOBAL;

				DROPFILES *df = GlobalLock(stgmed[3].hGlobal);

				df[0].fWide = FALSE;
				df[0].pFiles = sizeof(DROPFILES);
				df[0].fNC = FALSE;

				memset(&df[1], 0, strlen(fn) + 2);
				strcpy((char*) &df[1], fn);

				GlobalUnlock(stgmed[3].hGlobal);

				// Create IDataObject and IDropSource COM objects
				CreateDropSource(&pDropSource);
				CreateDataObject(fmtetc, stgmed, NumElm(fmtetc), &pDataObject);

				//((IDataObject *)pDataObject)->lpVtbl->QueryInterface(pDataObject, &IID_IDataObject,
				//		(LPVOID *) &((WB_IDropSource*)pDropSource)->pDataObject);

				IID IID_IDragSourceHelper2;
				CLSIDFromString((LPOLESTR) L"{83E07D0D-0C5F-4163-BF1A-60B274051E40}", &IID_IDragSourceHelper2);

	            HRESULT result, hres;
	            result = CoCreateInstance(
            		(REFCLSID) &CLSID_DragDropHelper,
            		NULL,
            		CLSCTX_INPROC_SERVER,
            		(REFIID) &IID_IDragSourceHelper,
            		(LPVOID*) &pDragSourceHelper);

	            /*
	            if (SUCCEEDED(result)) {
	            	IDragSourceHelper2 *pDragSourceHelper2;
	            	pDragSourceHelper->lpVtbl->QueryInterface(pDragSourceHelper, &IID_IDragSourceHelper2, &pDragSourceHelper2);

	            	if (pDragSourceHelper2) {
	            		printf("Created dragsourcehelper2\n");
	            		pDragSourceHelper = pDragSourceHelper2;
	            		pDragSourceHelper->lpVtbl->SetFlags(pDragSourceHelper, DSH_ALLOWDROPDESCRIPTIONTEXT);
	            	} else {
	            		printf("Couldn't even create IDragSourceHelper2\n");
	            	}

	            } else {
	            	printf("Failed to create drop helper at all\n");
	            }
	            */
	            //hres = pDragSourceHelper->lpVtbl->InitializeFromWindow(pDragSourceHelper, hwnd, &ptOffset, (IDataObject *) pDataObject);

	            SetDropSourceDataObject(pDropSource, pDataObject);
	            DWORD dwEffect = DROPEFFECT_NONE;
				if (SUCCEEDED(hres))
					DoDragDrop((IDataObject*) pDataObject, (IDropSource*) pDropSource,  DROPEFFECT_COPY, &dwEffect);


				pDragSourceHelper->lpVtbl->Release(pDragSourceHelper);
				((IDataObject *)pDataObject)->lpVtbl->Release((IDataObject *)pDataObject);
				((IDropSource *)pDropSource)->lpVtbl->Release((IDropSource *)pDropSource);
			}
			return 0;
		}
		case WM_COPYDATA:
		{
			int size = (int)((PCOPYDATASTRUCT)lParam)->cbData;
			char* string = (char*)((PCOPYDATASTRUCT)lParam)->lpData;
			int ram = (int)((PCOPYDATASTRUCT)lParam)->dwData;

			if (size && string && size==SizeofFileList(string))	{
				char* FileNames = malloc(size);
				memset(FileNames,0,size);
				memcpy(FileNames,string,size);
				ThreadSend(FileNames,ram);
			}
			break;
		}

		case WM_DROPFILES:
		{
			char * FileNames = NULL;
			char fn[256];

			int count = DragQueryFile((HDROP) wParam, ~0, fn, 256);

			while (count--) {
				DragQueryFile((HDROP) wParam, count, fn, 256);
				FileNames = AppendName( FileNames,fn);
			}

			ThreadSend(FileNames, DropMemoryTarget(hwnd));

			InvalidateRect(calcs[gslot].hwndFrame, NULL, FALSE);
			return 0;
		}
		case WM_KEYDOWN:
		case WM_KEYUP:
			SendMessage(calcs[gslot].hwndFrame, Message, wParam, lParam);
			break;

		case WM_DESTROY:
			printf("Unregistering drop window\n");
			if (calcs[gslot].pDropTarget != NULL)
				; //UnregisterDropWindow(hwnd, (IDropTarget *) calcs[gslot].pDropTarget);
			return 0;
		default:
			if (Message == RegisterWindowMessage("ShellGetDragImage")) {
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

				HBITMAP hbmDrag = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**) &pBits, NULL, 0);

				HBITMAP hbmOld = SelectObject(hdc, hbmDrag);

				u_char *screen = LCD_image( calcs[gslot].cpu.pio.lcd ) ;
				FillRect(hdc, &rc, (HBRUSH) GetStockObject(BLACK_BRUSH));

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
				BYTE * pPixel = pBits +  16*96*4 - 4;
				BYTE * pPixelTop = pBits + 96*96*4 - 16*96*4;
				int i, j;
				for (i = 16*96*4 - 4, j = 16; j > 0; i -= 4, pPixel -= 4, pPixelTop += 4) {
					pPixel[3] = pPixelTop[3] = 42 - ((42/16)*(16 - j));

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




