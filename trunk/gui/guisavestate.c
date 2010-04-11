#include "guisavestate.h"

#include <commctrl.h>
#include "rsrc.h"
#include "calc.h"

extern HINSTANCE g_hInst;
extern BITMAPINFO *bi;

static SAVESTATE_t *savestate;
static char save_filename[MAX_PATH];

static INT_PTR CALLBACK DlgSavestateProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static HWND edtAuthor;
	static HWND edtComment;
	static HWND edtModel, edtRom_version;
	static HWND cmbCompress;
	static HWND chkReadonly;
	static HWND imgPreview;
	
	switch (uMsg) {
		case WM_INITDIALOG: {
			edtAuthor = GetDlgItem(hwndDlg, IDC_EDTSAVEAUTHOR);
			edtComment = GetDlgItem(hwndDlg, IDC_EDTSAVECOMMENT);
			cmbCompress = GetDlgItem(hwndDlg, IDC_CBOSAVECOMPRESS);
			chkReadonly = GetDlgItem(hwndDlg, IDC_CHKSAVEREADONLY);
			imgPreview = GetDlgItem(hwndDlg, IDC_IMGSAVEPREVIEW);
			edtModel = GetDlgItem(hwndDlg, IDC_EDTSAVEMODEL);
			edtRom_version = GetDlgItem(hwndDlg, IDC_EDTSAVEROMVER);
			
			SendMessage(cmbCompress, CB_ADDSTRING, 0, (LPARAM) "None");
			SendMessage(cmbCompress, CB_ADDSTRING, 0, (LPARAM) "Zlib");
			SendMessage(cmbCompress, CB_SETCURSEL, 1, (LPARAM) 0);
			
			SendMessage(edtRom_version, WM_SETTEXT, 0, (LPARAM) calcs[gslot].rom_version);
			SendMessage(edtModel, WM_SETTEXT, 0, (LPARAM) CalcModelTxt[calcs[gslot].model]);
			
			
			HBITMAP hbmPreview = CreateBitmap(96, 64, 1, 32, NULL);
			
			LCD_t lcd;
			LoadLCD(savestate, &lcd);
			
			HDC hdc = CreateCompatibleDC(NULL);
			HBITMAP hbmOld = SelectObject(hdc, hbmPreview);
			
			StretchDIBits(hdc, 0, 0, 96, 64,
				0, 0, 96, 64,
				LCD_image(&lcd),
				bi,
				DIB_RGB_COLORS,
				SRCCOPY);

			SelectObject(hdc, hbmOld);
			DeleteDC(hdc);
			SendMessage(imgPreview, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hbmPreview);
			
			char lpBuffer[32];
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
						SendMessage(edtAuthor, WM_GETTEXT, 32, (LPARAM) savestate->author);
						SendMessage(edtComment, WM_GETTEXT, 64, (LPARAM) savestate->comment);
						
						int compression = SendMessage(cmbCompress, CB_GETCURSEL, 0, 0);
						
						WriteSave(save_filename, savestate, compression);
						strcpy(calcs[gslot].rom_path, save_filename);
					case IDC_BTNSAVECANCEL:
						EndDialog(hwndDlg, wParam);
						FreeSave(savestate);
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

INT_PTR gui_savestate(HWND hwndParent, SAVESTATE_t *save, char *filename) {
	InitCommonControls();
	savestate = save;
	strcpy(save_filename, filename);
	return DialogBox(
    						g_hInst, 
    						MAKEINTRESOURCE(IDD_DLGSAVESTATE), 
    						hwndParent, 
    						DlgSavestateProc);
}

