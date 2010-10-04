#include "stdafx.h"

#ifdef WINVER
#include "sound.h"
#endif

#include "calc.h"
#include "label.h"
#include "state.h"
#include "var.h"
#include "link.h"
#include "sendfiles.h"




static LPCALC lpCalc = NULL;
static HWND hwndMain = NULL;

typedef struct SENDFILES {
	TCHAR* FileNames;
	int ram;
//	HANDLE hdlSend;
} SENDFILES_t;


int SizeofFileList(TCHAR* FileNames) {
	int i;
	if (FileNames == NULL) return 0;
	for(i = 0; FileNames[i]!=0 || FileNames[i+1]!=0; i++);
	return i+2;
}

TCHAR* AppendName(TCHAR* FileNames, TCHAR* fn) {
	size_t length;
	TCHAR* pnt;
	int i;
	length = _tcslen(fn);
	if (FileNames == NULL) {
		FileNames = (TCHAR *) malloc(sizeof(TCHAR) * (length+2));
		memset(FileNames, 0, sizeof(TCHAR) * (length+2));
		pnt = FileNames;
	} else {
		for(i = 0; FileNames[i] != 0 || FileNames[i+1] != 0; i++);
		i++;
		FileNames = (TCHAR *) realloc(FileNames, sizeof(TCHAR) * (i + length + 2));
		pnt = FileNames+i;
		memset(pnt, 0, length + 2);
	}
#ifdef WINVER
	StringCchCopy(pnt, length + 1, fn);
#else
	strcpy(pnt,fn);
#endif
	return FileNames;
}

BOOL SendFile(HWND hwndParent, const LPCALC lpCalc, LPCTSTR lpszFileName, SEND_FLAG Destination)
{
	BOOL is_link_connected = link_connected(lpCalc->slot);
	TIFILE_t *var = newimportvar(lpszFileName);//importvar(lpszFileName, 0, Destination);

	LINK_ERR result;
	if (var != NULL) {
		switch(var->type) {
			case BACKUP_TYPE:
			case VAR_TYPE:
			case FLASH_TYPE:
				lpCalc->SendSize = var->length;
				lpCalc->cpu.pio.link->vlink_send = 0;
				result = link_send_var(&lpCalc->cpu, var, (SEND_FLAG) Destination);
#ifdef WINVER
				switch (result)
				{
				case LERR_MEM:
					switch (Destination)
					{
					case SEND_RAM:
						MessageBox(hwndParent, _T("Not enough free RAM on calculator"), _T("Error"),MB_OK);
						break;
					case SEND_ARC:
						MessageBox(hwndParent, _T("Not enough free Archive on calculator"), _T("Error"),MB_OK);
						break;
					default:
						MessageBox(hwndParent, _T("Not enough free memory on calculator"), _T("Error"),MB_OK);
						break;
					}
					break;
				case LERR_TIMEOUT:
					MessageBox(hwndParent, _T("Link timed out"), _T("Error"),MB_OK);
					break;
				case LERR_FORCELOAD:
					MessageBox(hwndParent, _T("Error force loading an application"), _T("Error"), MB_OK);
					break;
				case LERR_CHKSUM:
					MessageBox(hwndParent, _T("Link checksum was not correct"), _T("Error"), MB_OK);
					break;
				case LERR_MODEL:
					MessageBox(hwndParent, _T("Calculator model not correct for this type of file"), _T("Error"), MB_OK);
					break;
				case LERR_NOTINIT:
					MessageBox(hwndParent, _T("Virtual link was not initialized"), _T("Error"), MB_OK);
					break;
				case LERR_LINK:
					MessageBox(hwndParent, _T("Virtual link error"), _T("Error"), MB_OK);
					break;
				case LERR_FILE:
					MessageBox(hwndParent, _T("The file was unable to be sent because it is corrupt"), _T("Error"), MB_OK);
					break;
				}

#endif
				if (var->type == FLASH_TYPE) {
					// Rebuild the applist
					state_build_applist(&lpCalc->cpu, &lpCalc->applist);

					u_int i;
					for (i = 0; i < lpCalc->applist.count; i++) {
						if (_tcsncmp((TCHAR *) var->flash->name, lpCalc->applist.apps[i].name, 8) == 0) {
							lpCalc->last_transferred_app = &lpCalc->applist.apps[i];
							break;
						}
					}
				}
				break;
			case ROM_TYPE:
			case SAV_TYPE:
				FreeTiFile(var);
				var = NULL;
				rom_load(lpCalc, lpszFileName);
				SendMessage(lpCalc->hwndFrame, WM_USER, 0, 0);
				break;
			case LABEL_TYPE:
				{
					StringCbCopy(lpCalc->labelfn, sizeof(lpCalc->labelfn), lpszFileName);
					_tprintf_s(_T("loading label file for slot %d: %s\n"), lpCalc->slot, lpszFileName);
					VoidLabels(lpCalc);
					labels_app_load(lpCalc, lpCalc->labelfn);

					break;
				}
			case SKIP_TYPE:
			case BREAKPOINT_TYPE:
				break;
			default:
#ifdef WINVER
				MessageBox(NULL, _T("The file was an invalid or unspecified type"), _T("Error"), MB_OK);
#endif
				break;
		}
		if (var)
			FreeTiFile(var);
		if (is_link_connected)
			link_connect(&calcs[0].cpu, &calcs[1].cpu);

		return TRUE;
	} else {
#ifdef WINVER
		MessageBox(NULL, _T("Invalid file format"), _T("Error"), MB_OK);
#endif
		return FALSE;
	}
}

#ifdef WINVER
extern HINSTANCE g_hInst;
HWND hwndSend;
static TCHAR *current_file_sending;
#endif

void SendFiles(TCHAR *FileNames, int ram ) {
	int i;
	int modelsave;
	_TUCHAR *fn = (_TUCHAR *)  FileNames;
	lpCalc->send = TRUE;
	lpCalc->running = FALSE;
	lpCalc->CurrentFile = 0;
	lpCalc->FileCnt = 0;
	i = 0;
	while(FileNames[i] != 0) {
		for(;FileNames[i] != 0; i++);
		i++;
		lpCalc->FileCnt++;
	}

	while (fn[0] != 0) {
		modelsave = lpCalc->model;
		lpCalc->CurrentFile++;
		current_file_sending = (TCHAR *) fn;
		SendFile(NULL, lpCalc, (TCHAR *) fn, (SEND_FLAG) ram);
#ifdef WINVER
		SendMessage(hwndSend, WM_USER, 0, 0);
#endif
		for(;fn[0] != 0; fn++);
		fn++;
	}
	//if (calcs[SlotSave].model == TI_82 && modelsave == calcs[SlotSave].model) end82send(SlotSave);
	lpCalc->running = TRUE;
	free(FileNames);
	lpCalc->send = FALSE;
	current_file_sending = NULL;
}

#ifdef WINVER
static int CALLBACK EnumFontFamExProc(
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


static LRESULT CALLBACK SendProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HFONT hfontSegoe;
	static HWND hwndProgress;
	static TEXTMETRIC tm;
	switch (Message)  {
		case WM_CREATE: {
				LOGFONT lfSegoe;
				memset(&lfSegoe, 0, sizeof(LOGFONT));
				StringCbCopy(lfSegoe.lfFaceName, sizeof(lfSegoe.lfFaceName), _T("Segoe UI"));

				if (EnumFontFamiliesEx(GetDC(NULL), &lfSegoe, (FONTENUMPROC) EnumFontFamExProc, (LPARAM) &hfontSegoe, 0) != 0) {
					StringCbCopy(lfSegoe.lfFaceName, sizeof(lfSegoe.lfFaceName), _T("Tahoma"));
					EnumFontFamiliesEx(GetDC(NULL), &lfSegoe, (FONTENUMPROC) EnumFontFamExProc, (LPARAM) &hfontSegoe, 0);
				}

				SelectObject(GetDC(hwnd), hfontSegoe);
				GetTextMetrics(GetDC(hwnd), &tm);

				hwndProgress = CreateWindowEx(
						0,
						PROGRESS_CLASS,
						NULL,
						WS_CHILD | WS_VISIBLE,
						tm.tmAveCharWidth*2, tm.tmHeight * 4, 1, 1,
						hwnd, (HMENU) 0, g_hInst, NULL
				);

				return 0;
			}
		case WM_GETMINMAXINFO: {
				MINMAXINFO *info = (MINMAXINFO *) lParam;
				RECT rc;
				GetWindowRect(lpCalc->hwndLCD, &rc);

				DWORD SendWidth = (rc.right - rc.left) * 9 / 10;
				DWORD SendHeight = 110;
				AdjustWindowRect(&rc, WS_SIZEBOX | WS_POPUP, FALSE);
				info->ptMinTrackSize.x = SendWidth;
				info->ptMinTrackSize.y = SendHeight;
				info->ptMaxTrackSize.x = SendWidth;
				info->ptMaxTrackSize.y = SendHeight;
				return 0;
			}
		case WM_SIZE: {
				RECT rc;
				GetClientRect(hwnd, &rc);
				SetWindowPos(hwndProgress, NULL, 0, 0, rc.right - rc.left - tm.tmAveCharWidth*4, tm.tmHeight*3/2, SWP_NOMOVE | SWP_NOZORDER);

				rc.top = 0;
				rc.bottom = tm.tmHeight * 6;
				AdjustWindowRect(&rc, WS_SIZEBOX | WS_POPUP, FALSE);
				SetWindowPos(hwnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);

				return 0;
			}
		case WM_PAINT: {
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);

				SetBkMode(hdc, TRANSPARENT);
				SelectObject(hdc, hfontSegoe);

				TCHAR szFile[256];
				StringCbPrintf(szFile, sizeof(szFile), _T("Sending file %d of %d"), lpCalc->CurrentFile, lpCalc->FileCnt);

				RECT r;
				GetClientRect(hwnd, &r);
				r.bottom = tm.tmHeight*5/2 + tm.tmHeight;
				FillRect(hdc, &r, GetStockBrush(WHITE_BRUSH));

				SelectObject(hdc, GetStockObject(DC_PEN));
				SetDCPenColor(hdc, RGB(223,223,233));
				MoveToEx(hdc, 0, r.bottom-1, NULL);
				LineTo(hdc, r.right, r.bottom-1);

				r.left = tm.tmAveCharWidth*2;
				r.top = tm.tmHeight*3/4;
				r.right -= tm.tmAveCharWidth*2;
				SetTextColor(hdc, RGB(0, 0, 0));
				DrawText(hdc, szFile, -1, &r, DT_SINGLELINE);


				OffsetRect(&r, 0, tm.tmHeight);
				r.left += tm.tmAveCharWidth;
				SetTextColor(hdc, RGB(90, 90, 90));

				DrawText(hdc, current_file_sending, -1, &r, DT_SINGLELINE | DT_PATH_ELLIPSIS);

				EndPaint(hwnd, &ps);
				return 0;
			}
		case WM_USER: {
				// Update the progress bar
				SendMessage(hwndProgress, PBM_SETSTEP, 1, 0);
				SendMessage(hwndProgress, PBM_SETRANGE, 0, MAKELPARAM(0, lpCalc->cpu.pio.link->vlink_size/4));
				SendMessage(hwndProgress, PBM_SETPOS, lpCalc->cpu.pio.link->vlink_send/4, 0);

				InvalidateRect(hwnd, NULL, FALSE);
				UpdateWindow(hwnd);
				return 0;
			}
		case WM_CLOSE: {
				DestroyWindow(hwnd);
				return 0;
			}
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
}

DWORD WINAPI ThreadSendStart( LPVOID lpParam ) {
	SENDFILES_t* sf = (SENDFILES_t *) lpParam;
	int save = 0;

	if (lpCalc->audio->enabled) {
		save = 1;
		pausesound();
	}


	SendFiles(sf->FileNames, sf->ram);
	if (save==1) playsound();
	free(sf);
	lpCalc = NULL;

	PostMessage(hwndSend, WM_CLOSE, 0, 0);

    return 0;
}


void ThreadSend(TCHAR *FileNames, int ram, LPCALC calc) {
	static HANDLE hdlSend = NULL;
	SENDFILES_t* sf;

	if (FileNames == NULL) return;

	if (lpCalc == NULL || lpCalc->send == FALSE) {
		lpCalc = calc;
	} else {
		MessageBox(NULL, _T("Currently sending files please wait..."), _T("Error"), MB_OK);
		return;
	}

	if (hdlSend != NULL) {
		CloseHandle(hdlSend);
		hdlSend = NULL;
	}

	sf = (SENDFILES_t *) malloc(sizeof(SENDFILES_t));
	if (sf != NULL) {
		sf->FileNames	= FileNames;
		sf->ram			= ram;
	}
//	sf->hdlSend		= NULL;

	WNDCLASSEX wcx;
	ZeroMemory(&wcx, sizeof(wcx));

	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_DBLCLKS;
	wcx.lpszClassName = _T("WabbitSendClass");
	wcx.lpfnWndProc = SendProc;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = (HBRUSH) (COLOR_BTNFACE+1);
	RegisterClassEx(&wcx);


	RECT r;
	GetWindowRect(lpCalc->hwndLCD, &r);

	DWORD SendWidth = (r.right - r.left) * 9 / 10;
	DWORD SendHeight = 90; //10 * HIWORD(GetDialogBaseUnits());

	hwndSend = CreateWindowEx(
			0,
			_T("WabbitSendClass"),
			NULL,
			WS_SIZEBOX | WS_POPUP | WS_VISIBLE,
			r.left+(r.right - r.left - SendWidth)/2, r.top+(r.bottom - r.top - SendHeight)/2, SendWidth, SendHeight,
			lpCalc->hwndLCD, NULL, g_hInst, 0
	);
	
    hdlSend = CreateThread(NULL,0,ThreadSendStart, sf, 0, NULL);

    if ( hdlSend  == NULL) {
		lpCalc = NULL;
		free(sf);
		free(FileNames);
		MessageBox(NULL, _T("Could not create thread to send"), _T("Error"), MB_OK);
		return;
	}

	return;
}
#endif

void NoThreadSend(const TCHAR* FileNames, int ram, LPCALC calc) {
	if (lpCalc == NULL) {
		lpCalc = calc;
	} else {
		// error;
	}

	SendFile(NULL, lpCalc, (TCHAR *) FileNames, (SEND_FLAG) ram);
	lpCalc = NULL;
}






