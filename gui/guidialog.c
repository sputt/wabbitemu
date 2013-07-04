#include "stdafx.h"

#include "gui.h"
#include "guidialog.h"
#include "fileutilities.h"
#include "exportvar.h"
#include "ftp.h"
#include "guiupdate.h"

INT_PTR CALLBACK AboutDialogProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
	case WM_INITDIALOG: {
		TCHAR versionString[32];
		GetFileCurrentVersionString(versionString, sizeof(versionString));
		SetDlgItemText(hwndDlg, IDC_STC_VERSTRING, versionString);
		return FALSE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			EndDialog(hwndDlg, IDOK);
			break;
		case IDCANCEL:
			EndDialog(hwndDlg, IDCANCEL);
			break;
		}
	}
	return FALSE;
}

void SendBugReport(TCHAR *nameBuffer, TCHAR *emailBuffer, TCHAR *titleBuffer, TCHAR *stepsBuffer, TCHAR *attachFileBuffer) {
#define MAX_BUG_LEN 65536
	TCHAR *bugReport = (TCHAR *) malloc(MAX_BUG_LEN);
	*bugReport = '\0';
	if (_tcslen(nameBuffer)) {
		StringCbCopy(bugReport, MAX_BUG_LEN, _T("Name: "));
		StringCbCat(bugReport, MAX_BUG_LEN, nameBuffer);
	}
	if (_tcslen(emailBuffer)) {
		StringCbCat(bugReport, MAX_BUG_LEN, _T("Email: "));
		StringCbCat(bugReport, MAX_BUG_LEN, emailBuffer);
	}
	StringCbCat(bugReport, MAX_BUG_LEN, _T("\r\nTitle: "));
	StringCbCat(bugReport, MAX_BUG_LEN, titleBuffer);
	StringCbCat(bugReport, MAX_BUG_LEN, _T("\r\nVersion: "));
	TCHAR versionBuffer[32];
	GetFileCurrentVersionString(versionBuffer, sizeof(versionBuffer));
	StringCbCat(bugReport, MAX_BUG_LEN, versionBuffer);
	StringCbCat(bugReport, MAX_BUG_LEN, _T("\r\nRom Version: "));
	StringCbCat(bugReport, MAX_BUG_LEN, CalcModelTxt[calcs[0].model]);
	StringCbCat(bugReport, MAX_BUG_LEN, _T(" "));
#ifdef _UNICODE
	size_t size;
	mbstowcs_s(&size, versionBuffer, ARRAYSIZE(versionBuffer), calcs[0].rom_version, sizeof(versionBuffer));
	StringCbCat(bugReport, MAX_BUG_LEN, versionBuffer);
#else
	StringCbCat(bugReport, MAX_BUG_LEN, calcs[0].rom_version);
#endif
	StringCbCat(bugReport, MAX_BUG_LEN, _T("\r\nSteps:\r\n"));
	StringCbCat(bugReport, MAX_BUG_LEN, stepsBuffer);

	HINTERNET hInternet = OpenFtpConnection();

	if (_tcslen(attachFileBuffer)) {
		TCHAR *filePtr = attachFileBuffer + _tcslen(attachFileBuffer) * sizeof(TCHAR);
		for (int i = _tcslen(attachFileBuffer) - 1; i >= 0; i--) {
			if (*filePtr == '\\') {
				break;
			}
			filePtr--;

		}
		FtpPutFile(hInternet, attachFileBuffer, filePtr, FTP_TRANSFER_TYPE_BINARY, NULL);
		StringCbCat(bugReport, MAX_BUG_LEN, _T("\r\nAttached File: "));
		StringCbCat(bugReport, MAX_BUG_LEN, filePtr);
	}

	TCHAR tempFile[MAX_PATH];
	TCHAR timeStringText[MAX_PATH];
	GetAppDataString(tempFile, sizeof(tempFile));
	time_t timeUploaded;
	time(&timeUploaded);
	TCHAR timeString[256];
	_tctime_s(timeString, sizeof(timeString), &timeUploaded);
	for (int i = _tcslen(timeString); i >= 0; i--) {
		if (timeString[i] == ':') {
			timeString[i] = '_';
		}
	}
	//get rid of newline
	timeString[_tcslen((timeString)) - 1] = '\0';
	StringCbCopy(timeStringText, sizeof(timeStringText), timeString);
	if (_tcslen(nameBuffer)) {
		StringCbCat(timeStringText, sizeof(timeStringText), nameBuffer);
	}
	StringCbCat(timeStringText, sizeof(timeStringText), _T(".txt"));
	StringCbCat(tempFile, sizeof(tempFile), timeString);
	FILE *file;
	_tfopen_s(&file, tempFile, _T("wb"));
	_fputts(bugReport, file);
	fclose(file);

	FtpPutFile(hInternet, tempFile, timeStringText, FTP_TRANSFER_TYPE_ASCII, NULL);

	free(bugReport);
	_tremove(tempFile);
	return;
}

INT_PTR CALLBACK BugReportDialogProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam) {
	TCHAR attachFileNameBuffer[MAX_PATH];
	switch (Message) {
	case WM_INITDIALOG: {
		ZeroMemory(attachFileNameBuffer, sizeof(attachFileNameBuffer));
		return FALSE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			case IDC_BTN_BUGATTACH: {
				if (!BrowseFile(attachFileNameBuffer, _T("All files (*.*)\0*.*"), _T("Open file to attach..."), _T(".txt"))) {
					Edit_SetText(GetDlgItem(hwndDlg, IDC_EDT_BUGATTACH), attachFileNameBuffer);
				}
				break;
			}
			case IDOK: {
				TCHAR nameBuffer[256], titleBuffer[256], emailBuffer[256], stepsBuffer[4096];
				ZeroMemory(nameBuffer, sizeof(nameBuffer));
				ZeroMemory(titleBuffer, sizeof(titleBuffer));
				ZeroMemory(stepsBuffer, sizeof(stepsBuffer));
				ZeroMemory(emailBuffer, sizeof(emailBuffer));
				GetDlgItemText(hwndDlg, IDC_EDT_BUGNAME, nameBuffer, ARRAYSIZE(nameBuffer));
				GetDlgItemText(hwndDlg, IDC_EDT_BUGEMAIL, emailBuffer, ARRAYSIZE(emailBuffer));
				GetDlgItemText(hwndDlg, IDC_EDT_BUGTITLE, titleBuffer, ARRAYSIZE(titleBuffer));
				GetDlgItemText(hwndDlg, IDC_EDT_BUGSTEPS, stepsBuffer, ARRAYSIZE(stepsBuffer));
				GetDlgItemText(hwndDlg, IDC_EDT_BUGATTACH, attachFileNameBuffer, ARRAYSIZE(attachFileNameBuffer));
				if (_tcslen(titleBuffer) == 0) {
					MessageBox(hwndDlg, _T("Title cannot be empty"), _T("Error"), MB_OK);
				} else if (_tcslen(stepsBuffer) == 0) {
					MessageBox(hwndDlg, _T("Steps description cannot be empty"), _T("Error"), MB_OK);
				} else {
					SendBugReport(nameBuffer, emailBuffer, titleBuffer, stepsBuffer, attachFileNameBuffer);
					EndDialog(hwndDlg, IDOK);
				}
				return FALSE;
			}
			case IDCANCEL:
				EndDialog(hwndDlg, IDCANCEL);
				return FALSE;
		}
		case WM_SIZE: {
			RECT rc;
			GetClientRect(hwndDlg, &rc);
			MoveWindow(GetDlgItem(hwndDlg, IDC_EDT_BUGNAME), 99, 11, rc.right - 99 - 11, 23, TRUE);
			MoveWindow(GetDlgItem(hwndDlg, IDC_EDT_BUGEMAIL), 99, 36, rc.right - 99 - 11, 23, TRUE);
			MoveWindow(GetDlgItem(hwndDlg, IDC_EDT_BUGTITLE), 99, 62, rc.right - 99 - 11, 23, TRUE);
			MoveWindow(GetDlgItem(hwndDlg, IDC_EDT_BUGSTEPS), 11, 107, rc.right - 21, rc.bottom - 107 - 88, TRUE);
			MoveWindow(GetDlgItem(hwndDlg, IDC_STATIC_ATTACH), 11, rc.bottom - 83, 111, 12, TRUE);
			MoveWindow(GetDlgItem(hwndDlg, IDC_EDT_BUGATTACH), 11, rc.bottom - 66, rc.right - 21 - 75 - 7, 22, TRUE);
			MoveWindow(GetDlgItem(hwndDlg, IDC_BTN_BUGATTACH), rc.right - 11 - 75, rc.bottom - 67, 75, 23, TRUE);
			MoveWindow(GetDlgItem(hwndDlg, IDOK), rc.right - 18 - 75 - 75, rc.bottom - 33, 75, 23, TRUE);
			MoveWindow(GetDlgItem(hwndDlg, IDCANCEL), rc.right - 11 - 75, rc.bottom - 33, 75, 23, TRUE);
			return FALSE;
		}
	}
	
	return FALSE;
}

INT_PTR CALLBACK ExportOSDialogProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND hListPagesToExport;
	static LPCALC lpCalc;
	static TCHAR lpFileName[MAX_PATH];
	switch (Message) {
	case WM_INITDIALOG: {
		lpCalc = (LPCALC) GetWindowLongPtr(GetParent(hwndDlg), GWLP_USERDATA);
		StringCbCopy(lpFileName, sizeof(lpFileName), (TCHAR *) lParam);			
		hListPagesToExport = GetDlgItem(hwndDlg, IDC_LIST_EXPORTPAGES);
		SetWindowTheme(hListPagesToExport, L"Explorer", NULL);
		ListView_SetExtendedListViewStyle(hListPagesToExport, LVS_EX_CHECKBOXES);
		TCHAR temp[64];
		u_int totalPages = lpCalc->cpu.mem_c->flash_pages;
		for (u_int i = 0; i < totalPages; i++) {
			LVITEM item;
			item.mask = LVIF_TEXT;		
			StringCbPrintf(temp, sizeof(temp), _T("%02X"), i);
			item.pszText = temp;
			item.iItem = i;
			item.iSubItem = 0;
			ListView_InsertItem(hListPagesToExport, &item);
			upages_t pages;
			state_userpages(&lpCalc->cpu, &pages);
			if (i < pages.end - 1 || (i > pages.start && (i & 0xF) != 0xE && (i & 0xF) != 0xF)) {
				ListView_SetCheckState(hListPagesToExport, i, TRUE);
			}
		}
		return FALSE;
						}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			int bufferSize = 0;
			u_char (*flash)[PAGE_SIZE] = (u_char (*)[PAGE_SIZE]) lpCalc->cpu.mem_c->flash;
			unsigned char *buffer = NULL;
			unsigned char *bufferPtr = buffer;
			int currentPage = -1;
			for (int i = 0; i < lpCalc->cpu.mem_c->flash_pages; i++) {
				if (ListView_GetCheckState(hListPagesToExport, i)) {
					bufferSize += PAGE_SIZE;
					unsigned char *new_buffer = (unsigned char *) malloc(bufferSize);
					if (buffer) {
						memcpy(new_buffer, buffer, bufferSize - PAGE_SIZE);
						free(buffer);
					}
					buffer = new_buffer;
					bufferPtr = buffer + bufferSize - PAGE_SIZE;
					memcpy(bufferPtr, flash[i], PAGE_SIZE);
				}
			}
			MFILE *file = ExportOS(lpFileName, buffer, bufferSize);
			mclose(file);
			free(buffer);
			EndDialog(hwndDlg, IDOK);
			return TRUE;
				   }
		case IDCANCEL:
			EndDialog(hwndDlg, IDCANCEL);
			break;
		}
	}
	return FALSE;
}

INT_PTR CALLBACK WhatsNewDialogProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
		case WM_INITDIALOG: {
			TCHAR *newText = GetWhatsNewText();
			if (newText == NULL) {
				1 >> 8;
				EndDialog(hwndDlg, -1);
				MessageBox(NULL, _T("Failed to retrieve text"), _T("Error"), MB_OK);
				return FALSE;
			}
			HWND hwndText = GetDlgItem(hwndDlg, IDC_EDIT_WHATSNEW);
			SetWindowLongPtr(hwndText, GWL_STYLE, GetWindowLongPtr(hwndText, GWL_STYLE) | WS_VSCROLL);
			Edit_SetText(hwndText, newText);
			free(newText);
			return TRUE;
		}
		case WM_CLOSE:
		case WM_DESTROY: {
			EndDialog(hwndDlg, 0);
			return FALSE;
		}
	}
	return FALSE;
}