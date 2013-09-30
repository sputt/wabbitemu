#include "stdafx.h"

#include "calc.h"
#include "guisavestate.h"

extern HINSTANCE g_hInst;
extern BITMAPINFO *bi;

static TCHAR save_filename[MAX_PATH];

static INT_PTR CALLBACK DlgSavestateProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static HWND edtAuthor;
	static HWND edtComment;
	static HWND edtModel, edtRom_version;
	static HWND cmbCompress;
	static HWND chkReadonly;
	static HWND imgPreview;
	static LPCALC lpCalc;
	
	switch (uMsg) {
		case WM_INITDIALOG: {
			lpCalc = (LPCALC) lParam;
			edtAuthor = GetDlgItem(hwndDlg, IDC_EDTSAVEAUTHOR);
			edtComment = GetDlgItem(hwndDlg, IDC_EDTSAVECOMMENT);
			cmbCompress = GetDlgItem(hwndDlg, IDC_CBOSAVECOMPRESS);
			chkReadonly = GetDlgItem(hwndDlg, IDC_CHKSAVEREADONLY);
			imgPreview = GetDlgItem(hwndDlg, IDC_IMGSAVEPREVIEW);
			edtModel = GetDlgItem(hwndDlg, IDC_EDTSAVEMODEL);
			edtRom_version = GetDlgItem(hwndDlg, IDC_EDTSAVEROMVER);
			
			SendMessage(cmbCompress, CB_ADDSTRING, 0, (LPARAM) _T("None"));
			SendMessage(cmbCompress, CB_ADDSTRING, 0, (LPARAM) _T("Zlib"));
			SendMessage(cmbCompress, CB_SETCURSEL, 1, (LPARAM) 0);
			
#ifdef _UNICODE
			size_t numConv;
			TCHAR romBuffer[16];
			mbstowcs_s(&numConv, romBuffer, lpCalc->rom_version, 16);
			SendMessage(edtRom_version, WM_SETTEXT, 0, (LPARAM) romBuffer);
#else
			SendMessage(edtRom_version, WM_SETTEXT, 0, (LPARAM) lpCalc->rom_version);
#endif
			SendMessage(edtModel, WM_SETTEXT, 0, (LPARAM) CalcModelTxt[lpCalc->model]);
			
			
			HBITMAP hbmPreview = CreateBitmap(96, 64, 1, 32, NULL);
			
			LCD_t *lcd = lpCalc->cpu.pio.lcd;
			
			HDC hdc = CreateCompatibleDC(NULL);
			HBITMAP hbmOld = (HBITMAP) SelectObject(hdc, hbmPreview);
			
			StretchDIBits(hdc, 0, 0, 96, 64,
				0, 0, 96, 64,
				LCD_image(lcd),
				bi,
				DIB_RGB_COLORS,
				SRCCOPY);

			SelectObject(hdc, hbmOld);
			DeleteDC(hdc);
			SendMessage(imgPreview, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hbmPreview);
			
			TCHAR lpBuffer[32];
			DWORD length = sizeof(lpBuffer);
			GetUserName(lpBuffer, (LPDWORD) &length);
			SendMessage(edtAuthor, WM_SETTEXT, 0, (LPARAM) lpBuffer);
			
			SetFocus(edtAuthor);
			SendMessage(edtAuthor, EM_SETSEL, 0, (LPARAM) -1);
			return FALSE;
		}
		case WM_COMMAND:
			switch (HIWORD(wParam)) {
			case BN_CLICKED:
				switch (LOWORD(wParam)) {
					case IDC_BTNSAVEOK:
						{
							TCHAR author[MAX_SAVESTATE_AUTHOR_LENGTH];
							TCHAR comment[MAX_SAVESTATE_COMMENT_LENGTH];
							SendMessage(edtAuthor, WM_GETTEXT, MAX_SAVESTATE_AUTHOR_LENGTH, (LPARAM) author);
							SendMessage(edtComment, WM_GETTEXT, MAX_SAVESTATE_COMMENT_LENGTH, (LPARAM) comment);
							int compression = (int) SendMessage(cmbCompress, CB_GETCURSEL, 0, 0);

							SAVESTATE_t *savestate = SaveSlot(lpCalc, author, comment);
							WriteSave(save_filename, savestate, compression);
#ifdef WINVER
							StringCbCopy(lpCalc->rom_path, sizeof(lpCalc->rom_path), save_filename);
#else
							strcpy(lpCalc->rom_path, save_filename);
#endif
							FreeSave(savestate);
						}
					case IDC_BTNSAVECANCEL:
						EndDialog(hwndDlg, wParam);
						return TRUE;
				}
			}
			return FALSE;
		case WM_CLOSE:
			EndDialog(hwndDlg, 0);
			return TRUE;
		default:
			return FALSE;
	}
}

INT_PTR gui_savestate(HWND hwndParent, TCHAR *filename, LPCALC lpCalc) {
	InitCommonControls();
#ifdef WINVER
	StringCbCopy(save_filename, sizeof(save_filename), filename);
#else
	strcpy(save_filename, filename);
#endif
	return DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_DLGSAVESTATE), hwndParent, DlgSavestateProc, (LPARAM) lpCalc);
}

