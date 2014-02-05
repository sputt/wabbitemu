#include "stdafx.h"

#include "sendfileswindows.h"

#include "sound.h"
#include "calc.h"
#include "label.h"
#include "state.h"
#include "var.h"
#include "link.h"
#include "fileutilities.h"

typedef class SENDINFO
{
public:
	HANDLE hFileListMutex;
	HANDLE hThread;
	HWND hwndDlg;
	std::vector<std::tstring> *FileList;
	std::vector<SEND_FLAG> *DestinationList;
	
	// Download progress
	u_int iCurrentFile;
	ULONG ulBytesSent;
	ULONG ulFileSize;
	
	// Overall progress
	LINK_ERR Error;
	~SENDINFO();
} *LPSENDINFO;

SENDINFO::~SENDINFO() {
	if (this->FileList) {
		delete this->FileList;
		this->FileList = NULL;
	}
	if (this->DestinationList) {
		delete this->DestinationList;
		this->DestinationList = NULL;
	}
}

LPCTSTR g_szLinkErrorDescriptions[] =
{
	_T("No error"),
	_T("Virtual link error"),
	_T("Link timed out"),
	_T("Error force loading application"),
	_T("Invalid checksum on a packet"),
	_T("The virtual link was not initialized"),
	_T("Not enough free space on the calculator"),
	_T("Not the correct model for this calculator"),
	_T("Invalid file argument"),
	_T("A problem with Wabbitemu prevented the file from getting sent"),
};

static LRESULT CALLBACK SendProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
static LINK_ERR SendFile(HWND hwndParent, const LPCALC lpCalc, LPCTSTR lpszFileName, SEND_FLAG Destination);

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

static DWORD CALLBACK SendFileToCalcThread(LPVOID lpParam) {
	LPCALC lpCalc = (LPCALC) lpParam;
	LPSENDINFO lpsi = g_SendInfo[lpCalc];
	BOOL fRunningBackup = lpCalc->running;
	BOOL fSoundBackup = FALSE;
	if (lpCalc->audio)
		fSoundBackup = lpCalc->audio->enabled;

	lpCalc->running = FALSE;
	if (fSoundBackup)
		pausesound(lpCalc->audio);

	lpsi->Error = LERR_SUCCESS;
	for (lpsi->iCurrentFile = 0; lpsi->iCurrentFile < lpsi->FileList->size(); lpsi->iCurrentFile++)	{
		SendMessage(lpsi->hwndDlg, WM_USER, 0, NULL);
		lpsi->Error = SendFile(NULL, lpCalc, lpsi->FileList->at(lpsi->iCurrentFile).c_str(), lpsi->DestinationList->at(lpsi->iCurrentFile));
		if (lpsi->Error != LERR_SUCCESS) {
			if (MessageBox(lpsi->hwndDlg, g_szLinkErrorDescriptions[lpsi->Error], _T("Wabbitemu"), MB_OKCANCEL | MB_ICONERROR) == IDCANCEL) {
				break;
			}
		}
	}
	
	if (WaitForSingleObject(lpsi->hFileListMutex, INFINITE) == WAIT_OBJECT_0) {
		lpsi->FileList->clear();
		ReleaseMutex(lpsi->hFileListMutex);
	}

	if (lpsi->hwndDlg != NULL) {
		HWND hwndDlg = lpsi->hwndDlg;

		lpsi->hwndDlg = NULL;
		PostMessage(hwndDlg, WM_CLOSE, 0, NULL);
	}
	lpCalc->running = fRunningBackup;
	if (fSoundBackup == TRUE)
		playsound(lpCalc->audio);
	current_file_sending = NULL;
	lpsi->FileList->clear();
	lpsi->DestinationList->clear();
	lpsi->iCurrentFile = 0;
	return 0;
}

BOOL SendFileToCalc(const LPCALC lpCalc, LPCTSTR lpszFileName, BOOL fAsync, SEND_FLAG destination) {
	if ((lpCalc == NULL) || (lpszFileName == NULL)) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	if (fAsync == TRUE) {
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
			lpsi->DestinationList = new std::vector<SEND_FLAG>;
			g_SendInfo[lpCalc] = lpsi;

			lpsi->hFileListMutex = CreateMutex(NULL, FALSE, NULL);
			lpsi->iCurrentFile = -1;
		}
		else
		{
			lpsi = g_SendInfo[lpCalc];
		}

		if (lpsi->hwndDlg == NULL) {
			lpsi->hwndDlg = CreateSendFileProgress(lpCalc->hwndLCD, lpCalc);
		}

		// Add the file to the transfer queue
		if (WaitForSingleObject(lpsi->hFileListMutex, INFINITE) == WAIT_OBJECT_0) {
			lpsi->FileList->push_back(lpszFileName);
			lpsi->DestinationList->push_back(destination);
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
		SendFile(lpCalc->hwndLCD, lpCalc, lpszFileName, destination);
		return TRUE;
	}
}

static LINK_ERR SendFile(HWND hwndParent, const LPCALC lpCalc, LPCTSTR lpszFileName, SEND_FLAG Destination)
{
	TIFILE_t *var = importvar(lpszFileName, FALSE);

	BOOL exec_vio_backup = break_on_exe_violation;
	break_on_exe_violation = false;
	LINK_ERR result;
	if (var != NULL) {
		switch(var->type)
		{
		case GROUP_TYPE:
		case VAR_TYPE:
		case FLASH_TYPE:
			{
				if (lpCalc->cpu.pio.link == NULL) {
					result = LERR_MODEL;
					//81 for now
					break;
				}
				lpCalc->cpu.pio.link->vlink_size = var->length;
				lpCalc->cpu.pio.link->vlink_send = 0;

				result = link_send_var(&lpCalc->cpu, var, (SEND_FLAG) Destination);
				if (var->type == FLASH_TYPE)
				{
					if (var->flash->type == FLASH_TYPE_OS) {
						calc_reset(lpCalc);
						//calc_turn_on(lpCalc);
					} else {
						// Rebuild the app list
						state_build_applist(&lpCalc->cpu, &lpCalc->applist);

						u_int i;
						for (i = 0; i < lpCalc->applist.count; i++) {
							if (_tcsncmp((TCHAR *) var->flash->name, lpCalc->applist.apps[i].name, 8) == 0) {
								lpCalc->last_transferred_app = &lpCalc->applist.apps[i];
								break;
							}
						}
					}
				}
				break;
			}
		case BACKUP_TYPE:
			lpCalc->cpu.pio.link->vlink_size = var->length;
			lpCalc->cpu.pio.link->vlink_send = 0;
			result = link_send_backup(&lpCalc->cpu, var, (SEND_FLAG) Destination);
			break;
		case ZIP_TYPE: {
			WIN32_FIND_DATA FindFileData;
			HANDLE hFind;
			TCHAR path[MAX_PATH];
			TCHAR search[MAX_PATH];
			GetAppDataString(path, sizeof(path));
			StringCbCat(path, sizeof(path), _T("Zip\\"));
			StringCbCopy(search, sizeof(search), path);
			StringCbCat(search, sizeof(search), _T("*"));
			hFind = FindFirstFile(search, &FindFileData);
			if (hFind == INVALID_HANDLE_VALUE) {
				result = LERR_FILE;
				break;
			} else if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				TCHAR filename[MAX_PATH];
				StringCbCopy(filename, sizeof(filename), path);
				StringCbCat(filename, sizeof(filename), FindFileData.cFileName);
				SendFileToCalc(lpCalc, filename, FALSE, Destination);
				SendFileToCalc(lpCalc, filename, FALSE, Destination);
			}
			while (FindNextFile(hFind, &FindFileData)) {
				if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
					TCHAR filename[MAX_PATH];
					StringCbCopy(filename, sizeof(filename), path);
					StringCbCat(filename, sizeof(filename), FindFileData.cFileName);
					SendFileToCalc(lpCalc, filename, FALSE, Destination);
					DeleteFile(filename);
				}
			}
			FindClose(hFind);
			DeleteFile(path);
			result = LERR_SUCCESS;
			break;
		}
		case ROM_TYPE:
		case SAV_TYPE: {
			if (rom_load(lpCalc, lpszFileName) == TRUE) {
				result = LERR_SUCCESS;
			} else {
				result = LERR_LINK;
			}

			if (var->type == ROM_TYPE || var->type == SAV_TYPE) {
				//calc_turn_on(lpCalc);
				SendMessage(lpCalc->hwndFrame, WM_USER, 0, 0);
			}
			FreeTiFile(var);
			var = NULL;
			break;
		}
		case LABEL_TYPE: {
				StringCbCopy(lpCalc->labelfn, sizeof(lpCalc->labelfn), lpszFileName);
				_tprintf_s(_T("loading label file for slot %d: %s\n"), lpCalc->slot, lpszFileName);
				VoidLabels(lpCalc);
				labels_app_load(lpCalc, lpCalc->labelfn);
				result = LERR_SUCCESS;
				break;
			}
		case BREAKPOINT_TYPE:
			break;
		}
		if (var) {
			FreeTiFile(var);
		}
		break_on_exe_violation = exec_vio_backup;
		return result;
	}
	else
	{
		break_on_exe_violation = exec_vio_backup;
		return LERR_FILE;
	}
}

static LRESULT CALLBACK FrameSubclass(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	HWND hwndTransfer = (HWND) dwRefData;
	switch (uMsg)
	{
	case WM_MOVE:
		{
			LPCALC lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			RECT wr;
			GetWindowRect(hwnd, &wr);

			DWORD dwSendWidth = (wr.right - wr.left) * 9 / 10;
			DWORD dwSendHeight = 90; //10 * HIWORD(GetDialogBaseUnits());

			RECT rcLcd;
			GetWindowRect(lpCalc->hwndLCD, &rcLcd);

			SetWindowPos(hwndTransfer, NULL, 
				wr.left + ((wr.right - wr.left) - dwSendWidth) / 2, rcLcd.top + 20,
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
					hwnd, (HMENU) 1, GetModuleHandle(NULL), NULL
			);

			SetWindowSubclass(lpCalc->hwndFrame, FrameSubclass, 0, (DWORD_PTR) hwnd);
			SetTimer(hwnd, 1, 50, NULL);
			return 0;
		}
	case WM_GETMINMAXINFO:
		{
			LPCALC lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (!lpCalc) {
				return 0;
			}

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
	case WM_SIZE:
		{
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
	case WM_PAINT:
		{
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

			if (lpsi->iCurrentFile != -1 && lpsi->FileList->size() > lpsi->iCurrentFile)
			{
				DrawText(hdc, lpsi->FileList->at(lpsi->iCurrentFile).c_str(), -1, &r, DT_SINGLELINE | DT_PATH_ELLIPSIS);
			}

			EndPaint(hwnd, &ps);
			return 0;
		}
	case WM_TIMER:
		{
			return SendMessage(hwnd, WM_USER, 0, NULL);
		}
	case WM_USER:
		{
			LPCALC lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
			HWND hwndProgress = GetDlgItem(hwnd, 1);
			// Update the progress bar
			if (lpCalc->cpu.pio.link == NULL)
			{
				SendMessage(hwndProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 1));
				SendMessage(hwndProgress, PBM_SETPOS, 1, 0);
			}
			else
			{
				SendMessage(hwndProgress, PBM_SETSTEP, 1, 0);
				SendMessage(hwndProgress, PBM_SETRANGE, 0, MAKELPARAM(0, lpCalc->cpu.pio.link->vlink_size/4));
				SendMessage(hwndProgress, PBM_SETPOS, lpCalc->cpu.pio.link->vlink_send/4, 0);
			}

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
	default:
		{
			return DefWindowProc(hwnd, Message, wParam, lParam);
		}
	}
}