#include "stdafx.h"

#include "gui.h"

#include "guicommandline.h"
#include "calc.h"
#include "sendfileswindows.h"

void ParseCommandLineArgs(ParsedCmdArgs *parsedArgs) {
	TCHAR tmpstring[512];
	TCHAR nextarg[512];
	bool handledargv[200];
	SEND_FLAG ram = SEND_CUR;
	int argc;
	LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	ZeroMemory(parsedArgs, sizeof(ParsedCmdArgs));
	ZeroMemory(handledargv, sizeof(handledargv));

	if (argv && argc > 1) {
#ifdef _UNICODE
		StringCbCopy(tmpstring, sizeof(tmpstring), argv[1]);
#else
		size_t numConv;
		wcstombs_s(&numConv, tmpstring, argv[1], 512);
#endif
		for(int i = 1; i < argc; i++) {
			if (handledargv[i]) {
				continue;
			}

			ZeroMemory(tmpstring, 512);
			ZeroMemory(nextarg, 512);
#ifdef _UNICODE
			StringCbCopy(tmpstring, sizeof(tmpstring), argv[i]);
			if (i + 1 < argc) {
				StringCbCopy(nextarg, sizeof(nextarg), argv[i + 1]);
			}
#else
			size_t numConv;
			wcstombs_s(&numConv, tmpstring, argv[i], 512);
			if (i + 1 < argc) {
				wcstombs_s(&numConv, nextarg, argv[i + 1], 512);
			}
#endif
			char secondChar = (char) toupper(tmpstring[1]);
			if (*tmpstring != '-' && *tmpstring != '/') {
				size_t strLen = _tcslen(tmpstring) + 1;
				TCHAR *temp = (TCHAR *) malloc(strLen * sizeof(TCHAR));
				StringCchCopy(temp, strLen, tmpstring);
				temp[strLen] = '\0';
				TCHAR extension[5] = _T("");
				const TCHAR *pext = _tcsrchr(tmpstring, _T('.'));
				if (pext != NULL) {
					StringCbCopy(extension, sizeof(extension), pext);
				}
				if (!_tcsicmp(extension, _T(".rom")) || !_tcsicmp(extension, _T(".sav")) || !_tcsicmp(extension, _T(".clc"))) {
					parsedArgs->rom_files[parsedArgs->num_rom_files++] = temp;
				}
				else if (!_tcsicmp(extension, _T(".brk")) || !_tcsicmp(extension, _T(".lab")) 
					|| !_tcsicmp(extension, _T(".zip")) || !_tcsicmp(extension, _T(".tig"))) {
						parsedArgs->utility_files[parsedArgs->num_utility_files++] = temp;
				}
				else if (ram == SEND_CUR) {
					parsedArgs->cur_files[parsedArgs->num_cur_files++] = temp;
				} else if (ram == SEND_RAM) {
					parsedArgs->ram_files[parsedArgs->num_ram_files++] = temp;
				} else {
					parsedArgs->archive_files[parsedArgs->num_archive_files++] = temp;
				}
				handledargv[i] = TRUE;
			} else if (_tcslen(tmpstring) == 2) {
				handledargv[i] = TRUE;
				if (secondChar == 'R') {
					ram = SEND_RAM;
				} else if (secondChar == 'A') {
					ram = SEND_ARC;
				} else if (secondChar == 'S') {
					parsedArgs->silent_mode = TRUE;
				} else if (secondChar == 'F') {
					parsedArgs->force_focus = TRUE;
				} else if (secondChar == 'N') {
					parsedArgs->force_new_instance = TRUE;
				} else {
					handledargv[i] = FALSE;
				}
			} else if (_tcsicmp(tmpstring + 1, _T("gdb-port")) == 0 && i + 1 < argc && *nextarg != '-' && *nextarg != '/') {
				_stscanf(nextarg, _T("%i"), &parsedArgs->gdb_port);
				handledargv[i] = TRUE;
				handledargv[i + 1] = TRUE;
			} else if (_tcsicmp(tmpstring + 1, _T("embedding")) == 0) {
				parsedArgs->no_create_calc = TRUE;
				handledargv[i] = TRUE;
			}
		}
	}
}

void LoadAlreadyExistingWabbit(LPARAM lParam, LPTSTR filePath, SEND_FLAG sendLoc)
{
	HWND hwnd = (HWND) lParam;
	COPYDATASTRUCT *cds = (COPYDATASTRUCT *) malloc(sizeof(COPYDATASTRUCT));
	cds->dwData = sendLoc;
	size_t strLen;
	cds->lpData = filePath;
	if (PathIsRelative(filePath)) {
		TCHAR tempPath[MAX_PATH];
		TCHAR *tempPath2 = (TCHAR *) malloc(MAX_PATH);
		_tgetcwd(tempPath, MAX_PATH);
		PathCombine(tempPath2, tempPath, filePath);
		cds->lpData = tempPath2;
	}
	StringCbLength(filePath, 512, &strLen);
	cds->cbData = strLen;
	SendMessage(hwnd, WM_COPYDATA, (WPARAM) NULL, (LPARAM) cds);
}

void LoadToLPCALC(LPARAM lParam, LPTSTR filePath, SEND_FLAG sendLoc)
{
	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW) lParam;
	if (lpMainWindow == NULL) {
		return;
	}

	LPCALC lpCalc = lpMainWindow->lpCalc;
	SendFileToCalc(lpMainWindow->hwndFrame, lpCalc, filePath, TRUE, sendLoc);
}

void LoadCommandlineFiles(ParsedCmdArgs *parsedArgs, LPARAM lParam,
						  void (*load_callback)(LPARAM, LPTSTR, SEND_FLAG))
{
	// first send files we don't care about the destination
	for (int i = 0; i < parsedArgs->num_cur_files; i++) {
		load_callback(lParam, parsedArgs->cur_files[i], SEND_CUR);
	}
	// then archived files
	for (int i = 0; i < parsedArgs->num_archive_files; i++) {
		load_callback(lParam, parsedArgs->archive_files[i], SEND_ARC);
	}
	// then ram
	for (int i = 0; i < parsedArgs->num_ram_files; i++) {
		load_callback(lParam, parsedArgs->ram_files[i], SEND_RAM);
	}
	// finally utility files (label, break, etc)
	for (int i = 0; i < parsedArgs->num_utility_files; i++) {
		load_callback(lParam, parsedArgs->utility_files[i], SEND_ARC);
	}
}