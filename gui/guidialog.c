#include "stdafx.h"

#include "gui.h"
#include "guidialog.h"
#include "fileutilities.h"
#include "resource.h"
#include "exportvar.h"
#include "ftp.h"

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

void SendBugReport(TCHAR *nameBuffer, TCHAR *titleBuffer, TCHAR *stepsBuffer) {
#define MAX_BUG_LEN 65536
	TCHAR *bugReport = (TCHAR *) malloc(MAX_BUG_LEN);
	*bugReport = '\0';
	if (strlen(nameBuffer)) {
		StringCbCopy(bugReport, MAX_BUG_LEN, _T("Name: "));
		StringCbCat(bugReport, MAX_BUG_LEN, nameBuffer);
	}
	StringCbCat(bugReport, MAX_BUG_LEN, _T("\r\nTitle: "));
	StringCbCat(bugReport, MAX_BUG_LEN, titleBuffer);
	StringCbCat(bugReport, MAX_BUG_LEN, _T("\r\nVersion: "));
	TCHAR version[32];
	GetFileCurrentVersionString(version, sizeof(version));
	StringCbCat(bugReport, MAX_BUG_LEN, version);
	StringCbCat(bugReport, MAX_BUG_LEN, _T("\r\nSteps:\r\n"));
	StringCbCat(bugReport, MAX_BUG_LEN, stepsBuffer);

	HINTERNET hInternet = OpenFtpConnection();

	TCHAR tempFile[MAX_PATH];
	TCHAR timeStringText[MAX_PATH];
	GetAppDataString(tempFile, sizeof(tempFile));
	time_t timeUploaded;
	time(&timeUploaded);
	TCHAR *timeString = _tctime(&timeUploaded);
	for (int i = strlen(timeString); i >= 0; i--) {
		if (timeString[i] == ':') {
			timeString[i] = '_';
		}
	}
	//get rid of newline
	timeString[strlen((timeString)) - 1] = '\0';
	StringCbCopy(timeStringText, sizeof(timeStringText), timeString);
	if (strlen(nameBuffer)) {
		StringCbCat(timeStringText, sizeof(timeStringText), nameBuffer);
	}
	StringCbCat(timeStringText, sizeof(timeStringText), _T(".txt"));
	StringCbCat(tempFile, sizeof(tempFile), timeString);
	FILE *file;
	fopen_s(&file, tempFile, _T("wb"));
	fputs(bugReport, file);
	fclose(file);

	FtpPutFile(hInternet, tempFile, timeStringText, FTP_TRANSFER_TYPE_ASCII, NULL);

	free(bugReport);
	_tremove(tempFile);
	return;
}

INT_PTR CALLBACK BugReportDialogProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
	case WM_INITDIALOG: {
		return FALSE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			case IDOK: {
				TCHAR nameBuffer[256], titleBuffer[256], stepsBuffer[4096];
				GetDlgItemText(hwndDlg, IDC_EDT_BUGNAME, nameBuffer, ARRAYSIZE(nameBuffer));
				GetDlgItemText(hwndDlg, IDC_EDT_BUGTITLE, titleBuffer, ARRAYSIZE(titleBuffer));
				GetDlgItemText(hwndDlg, IDC_EDT_BUGSTEPS, stepsBuffer, ARRAYSIZE(stepsBuffer));
				if (strlen(titleBuffer) == 0) {
					MessageBox(hwndDlg, _T("Title cannot be empty"), _T("Error"), MB_OK);
				} else if (strlen(stepsBuffer) == 0) {
					MessageBox(hwndDlg, _T("Steps description cannot be empty"), _T("Error"), MB_OK);
				} else {
					SendBugReport(nameBuffer, titleBuffer, stepsBuffer);
					EndDialog(hwndDlg, IDOK);
				}
				return FALSE;
			}
			case IDCANCEL:
				EndDialog(hwndDlg, IDCANCEL);
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
		int totalPages = lpCalc->cpu.mem_c->flash_pages;
		for (int i = 0; i < totalPages; i++) {
			LVITEM item;
			item.mask = LVIF_TEXT;		
			StringCbPrintf(temp, sizeof(temp), _T("%02X"), i);
			item.pszText = temp;
			item.iItem = i;
			item.iSubItem = 0;
			ListView_InsertItem(hListPagesToExport, &item);
			upages_t pages;
			state_userpages(&lpCalc->cpu, &pages);
			if (i < pages.end - 1 || (i > pages.start && (i & 0xF) != 0xE && (i & 0xF) != 0xF))
			{
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
