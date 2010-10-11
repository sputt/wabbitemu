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

typedef struct tagSENDINFO
{
	HANDLE hFileListMutex;
	HANDLE hThread;
	HWND hwndDlg;
	std::vector<std::tstring> *FileList;
	
	// Download progress
	int iCurrentFile;
	ULONG ulBytesSent;
	ULONG ulFileSize;
	
	// Overall progress
	LINK_ERR Error;
} SENDINFO, *LPSENDINFO;

static LRESULT CALLBACK SendProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

static HANDLE hSendInfoMutex = NULL;
static std::map<LPCALC, LPSENDINFO> g_SendInfo;
static const TCHAR *current_file_sending;

static HWND CreateSendFileProgress(HWND hwndParent, const LPCALC lpCalc)
{
	static ATOM Atom = 0;
	if (Atom == 0)
	{
		WNDCLASSEX wcx = {0};
		wcx.cbSize = sizeof(wcx);
		wcx.lpszClassName = _T("WabbitSendClass");
		wcx.lpfnWndProc = SendProc;
		wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcx.hbrBackground = (HBRUSH) (COLOR_BTNFACE+1);
		Atom = RegisterClassEx(&wcx);
		if (Atom == 0)
		{
			return NULL;
		}
	}

	HWND hwnd = CreateWindowEx(
		0,
		(LPCTSTR) Atom,
		NULL,
		WS_SIZEBOX | WS_POPUP,
		0, 0, 0, 0,
		hwndParent, NULL, GetModuleHandle(NULL),
		(LPVOID) lpCalc);

	SendMessage(lpCalc->hwndFrame, WM_MOVE, 0, NULL);

	return hwnd;
}

static DWORD CALLBACK SendFileToCalcThread(LPVOID lpParam)
{
	LPCALC lpCalc = (LPCALC) lpParam;
	LPSENDINFO lpsi = g_SendInfo[lpCalc];
	BOOL fRunningBackup = lpCalc->running;
	BOOL fSoundBackup = lpCalc->audio->enabled;

	lpCalc->running = FALSE;
	if (fSoundBackup)
	{
		pausesound();
	}

	for (lpsi->iCurrentFile = 0; lpsi->iCurrentFile < lpsi->FileList->size(); lpsi->iCurrentFile++)
	{
		SendMessage(lpsi->hwndDlg, WM_USER, 0, NULL);	
		SendFile(NULL, lpCalc, lpsi->FileList->at(lpsi->iCurrentFile).c_str(), SEND_CUR);
	}

	if (WaitForSingleObject(lpsi->hFileListMutex, INFINITE) == WAIT_OBJECT_0)
	{
		lpsi->FileList->clear();
		ReleaseMutex(lpsi->hFileListMutex);
	}

	if (lpsi->hwndDlg != NULL)
	{
		HWND hwndDlg = lpsi->hwndDlg;
		lpsi->hwndDlg = NULL;
		PostMessage(hwndDlg, WM_CLOSE, 0, NULL);
	}
	lpCalc->running = fRunningBackup;
	if (fSoundBackup == TRUE)
	{
		playsound();
	}
	current_file_sending = NULL;
	return 0;
}

BOOL SendFileToCalc(const LPCALC lpCalc, LPCTSTR lpszFileName, BOOL fAsync)
{
	if ((lpCalc == NULL) || (lpszFileName == NULL))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	if (fAsync == TRUE)
	{
		if (hSendInfoMutex == NULL) {
			hSendInfoMutex = CreateMutex(NULL, TRUE, NULL);
		} else {
			if (WaitForSingleObject(hSendInfoMutex, INFINITE) != WAIT_OBJECT_0) {
				return FALSE;
			}
		}

		LPSENDINFO lpsi;
		if (g_SendInfo.find(lpCalc) == g_SendInfo.end()) {
			lpsi = new SENDINFO;
			ZeroMemory(lpsi, sizeof(SENDINFO));
			lpsi->FileList = new std::vector<std::tstring>;
			g_SendInfo[lpCalc] = lpsi;

			lpsi->hFileListMutex = CreateMutex(NULL, FALSE, NULL);
			lpsi->iCurrentFile = -1;
		}
		else
		{
			lpsi = g_SendInfo[lpCalc];
		}

		if (lpsi->hwndDlg == NULL) {
			lpsi->hwndDlg = CreateSendFileProgress(lpCalc->hwndFrame, lpCalc);
		}

		// Add the file to the transfer queue
		if (WaitForSingleObject(lpsi->hFileListMutex, INFINITE) == WAIT_OBJECT_0) {
			lpsi->FileList->push_back(lpszFileName);
			ReleaseMutex(lpsi->hFileListMutex);
		} else {
			ReleaseMutex(hSendInfoMutex);
			return FALSE;
		}

		// Make sure the download thread is started
		if ((lpsi->hThread == NULL) || (WaitForSingleObject(lpsi->hThread, 0) == WAIT_OBJECT_0)) {
			if (lpsi->hThread != NULL) {
				CloseHandle(lpsi->hThread);
			}
			lpsi->hThread = CreateThread(NULL, 0, SendFileToCalcThread, lpCalc, 0, NULL);
		}

		ReleaseMutex(hSendInfoMutex);
		return TRUE;
	} else {
		return FALSE;
	}
}

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
	for(i = 0; FileNames[i] != 0 || FileNames[i+1] != 0; i++);
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
	//BOOL is_link_connected = link_connected(lpCalc->slot);
	TIFILE_t *var = newimportvar(lpszFileName);//importvar(lpszFileName, 0, Destination);

	LINK_ERR result;
	if (var != NULL) {
		switch(var->type) {
			case GROUP_TYPE:
			case BACKUP_TYPE:
			case VAR_TYPE:
			case FLASH_TYPE:
				lpCalc->cpu.pio.link->vlink_size = var->length;
				lpCalc->cpu.pio.link->vlink_send = 0;
				result = link_send_var(&lpCalc->cpu, var, (SEND_FLAG) Destination);
#ifdef WINVER
				switch (result)
				{
				case LERR_MEM:
					switch (Destination)
					{
					case SEND_RAM:
						MessageBox(hwndParent, _T("Not enough free RAM on calculator"), _T("Wabbitemu"),MB_OK);
						break;
					case SEND_ARC:
						MessageBox(hwndParent, _T("Not enough free Archive on calculator"), _T("Wabbitemu"),MB_OK);
						break;
					default:
						MessageBox(hwndParent, _T("Not enough free memory on calculator"), _T("Wabbitemu"),MB_OK);
						break;
					}
					break;
				case LERR_TIMEOUT:
					MessageBox(hwndParent, _T("Link timed out"), _T("Wabbitemu"),MB_OK);
					break;
				case LERR_FORCELOAD:
					MessageBox(hwndParent, _T("Error force loading an application"), _T("Wabbitemu"), MB_OK);
					break;
				case LERR_CHKSUM:
					MessageBox(hwndParent, _T("Link checksum was not correct"), _T("Wabbitemu"), MB_OK);
					break;
				case LERR_MODEL:
					MessageBox(hwndParent, _T("Calculator model not correct for this type of file"), _T("Wabbitemu"), MB_OK);
					break;
				case LERR_NOTINIT:
					MessageBox(hwndParent, _T("Virtual link was not initialized"), _T("Wabbitemu"), MB_OK);
					break;
				case LERR_LINK:
					MessageBox(hwndParent, _T("Virtual link error"), _T("Wabbitemu"), MB_OK);
					break;
				case LERR_FILE:
					MessageBox(hwndParent, _T("The file was unable to be sent because it is corrupt"), _T("Wabbitemu"), MB_OK);
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
					if (var->flash->type == FLASH_TYPE_OS) {
						calc_reset(lpCalc);
						calc_turn_on(lpCalc);
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
			case LABEL_TYPE: {
					StringCbCopy(lpCalc->labelfn, sizeof(lpCalc->labelfn), lpszFileName);
					_tprintf_s(_T("loading label file for slot %d: %s\n"), lpCalc->slot, lpszFileName);
					VoidLabels(lpCalc);
					labels_app_load(lpCalc, lpCalc->labelfn);
					break;
				}
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
#endif

void SendFiles(TCHAR *FileNames, int ram ) {
	int i;
	int modelsave;
	_TUCHAR *fn = (_TUCHAR *)  FileNames;
	lpCalc->running = FALSE;
	i = 0;
	while(FileNames[i] != 0) {
		for(;FileNames[i] != 0; i++);
		i++;
	}

	while (fn[0] != 0) {
		modelsave = lpCalc->model;
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
	current_file_sending = NULL;
}

#ifdef WINVER

static LRESULT CALLBACK FrameSubclass(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	HWND hwndTransfer = (HWND) dwRefData;
	switch (uMsg)
	{
	case WM_MOVE:
		{
			RECT wr;
			GetWindowRect(hwnd, &wr);

			DWORD dwSendWidth = (wr.right - wr.left) * 9 / 10;
			DWORD dwSendHeight = 90; //10 * HIWORD(GetDialogBaseUnits());

			SetWindowPos(hwndTransfer, NULL, 
				wr.left + ((wr.right - wr.left) - dwSendWidth) / 2, wr.top + ((wr.bottom - wr.top) - dwSendHeight) / 2,
				dwSendWidth, dwSendHeight,
				SWP_NOSIZE | SWP_NOZORDER);
			break;
		}
	}
	return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

static LRESULT CALLBACK SendProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static HFONT hfontSegoe = NULL;
	static TEXTMETRIC tm;
	switch (Message)
	{
	case WM_CREATE:
		{
			LPCALC lpCalc = (LPCALC) ((LPCREATESTRUCT) lParam)->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) lpCalc);

			if (hfontSegoe == NULL) {
				NONCLIENTMETRICS ncm = {0};
				ncm.cbSize = sizeof(ncm);
				SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);

				hfontSegoe = CreateFontIndirect(&ncm.lfMessageFont);
			}

			HDC hdc = GetDC(hwnd);

			SelectObject(hdc, hfontSegoe);
			GetTextMetrics(hdc, &tm);

			ReleaseDC(hwnd, hdc);

			CreateWindowEx(
					0,
					PROGRESS_CLASS,
					NULL,
					WS_CHILD | WS_VISIBLE,
					tm.tmAveCharWidth*2, tm.tmHeight * 4, 1, 1,
					hwnd, (HMENU) 1, g_hInst, NULL
			);

			SetWindowSubclass(lpCalc->hwndFrame, FrameSubclass, 0, (DWORD_PTR) hwnd);
			SetTimer(hwnd, 1, 50, NULL);
			return 0;
		}
		case WM_GETMINMAXINFO: {
			LPCALC lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (!lpCalc)
				return 0;
			MINMAXINFO *info = (MINMAXINFO *) lParam;
			RECT rc;
			GetWindowRect(lpCalc->hwndFrame, &rc);

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
			HWND hwndProgress = GetDlgItem(hwnd, 1);
			SetWindowPos(hwndProgress, NULL, 0, 0, rc.right - rc.left - tm.tmAveCharWidth*4, tm.tmHeight*3/2, SWP_NOMOVE | SWP_NOZORDER);

			rc.top = 0;
			rc.bottom = tm.tmHeight * 6;
			AdjustWindowRect(&rc, WS_SIZEBOX | WS_POPUP, FALSE);
			SetWindowPos(hwnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);

			return 0;
		}
		case WM_PAINT: {
			PAINTSTRUCT ps;
			LPCALC lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			LPSENDINFO lpsi = g_SendInfo[lpCalc];

			HDC hdc = BeginPaint(hwnd, &ps);

			SetBkMode(hdc, TRANSPARENT);
			SelectObject(hdc, hfontSegoe);

			TCHAR szFile[256] = _T("");
			StringCbPrintf(szFile, sizeof(szFile), _T("Sending file %d of %d"), lpsi->iCurrentFile + 1, lpsi->FileList->size());

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

			if (lpsi->iCurrentFile != -1)
			{
				DrawText(hdc, lpsi->FileList->at(lpsi->iCurrentFile).c_str(), -1, &r, DT_SINGLELINE | DT_PATH_ELLIPSIS);
			}

			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_TIMER: {
			return SendMessage(hwnd, WM_USER, 0, NULL);
		}
		case WM_USER: {
			LPCALC lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			HWND hwndProgress = GetDlgItem(hwnd, 1);
			// Update the progress bar
			SendMessage(hwndProgress, PBM_SETSTEP, 1, 0);
			SendMessage(hwndProgress, PBM_SETRANGE, 0, MAKELPARAM(0, lpCalc->cpu.pio.link->vlink_size/4));
			SendMessage(hwndProgress, PBM_SETPOS, lpCalc->cpu.pio.link->vlink_send/4, 0);

			ShowWindow(hwnd, SW_SHOW);
			InvalidateRect(hwnd, NULL, FALSE);
			UpdateWindow(hwnd);
			return 0;
		}
	case WM_CLOSE:
		{
			LPCALC lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			RemoveWindowSubclass(lpCalc->hwndFrame, FrameSubclass, 0);
			DestroyWindow(hwnd);
			return 0;
		}
		default: {
			return DefWindowProc(hwnd, Message, wParam, lParam);
		}
	}
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
