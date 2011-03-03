#include "stdafx.h"

#include "guiopenfile.h"
#include "guifilepreview.h"
#include "calc.h"
#include "resource.h"
#include "sendfiles.h"

#define OFN_PREVIEW_WIDTH ((96*2) + (96/6))

static HWND hwndPreview;
static WNDPROC wndProcOFN;

extern HINSTANCE g_hInst;

static LRESULT CALLBACK OFNSubclassProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static int heightDiff = 0;
	
	switch (Message) {
		case WM_SIZE: 
		{
			RECT r;
			GetWindowRect(hwnd, &r);

			// During the first resize, keep track of how much of the standard
			// openfile dialog the panel takes up
			if (heightDiff == 0) {
				RECT rc;
				GetWindowRect(hwndPreview, &rc);
				
				heightDiff = (r.bottom - r.top) - (rc.bottom - rc.top);
			}
			
			SendMessage(hwndPreview, WM_SIZE, 
					SIZE_RESTORED, 
					MAKELPARAM(OFN_PREVIEW_WIDTH, r.bottom - r.top - heightDiff));
			
			break;
		}
	}

	return CallWindowProc(wndProcOFN, hwnd, Message, wParam, lParam);
}

static UINT_PTR CALLBACK OFNHookProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam) {
	static OFNHookOptions *HookOptions;
	
	switch (Message) {
		case WM_INITDIALOG: {
			HookOptions = (OFNHookOptions*) ((OPENFILENAME*) lParam)->lCustData;
			
			// Subclass it to intercept WM_SIZE messages
			wndProcOFN = (WNDPROC) SetWindowLongPtr(GetParent(hwndDlg), GWLP_WNDPROC, (LONG_PTR) OFNSubclassProc);	

			HWND hwndContainer = GetParent(hwndDlg);
			// Grab the list box
			HWND hwndLst = GetDlgItem(GetParent(hwndDlg), lst1);
			
			// Get list box's window rect
			RECT r;
			GetWindowRect(hwndLst, &r);
			
			// Get full window size
			RECT rc;
			GetWindowRect(hwndContainer, &rc);
			
			POINT p = {r.left, r.top};
			ScreenToClient(hwndContainer, &p);
			
			// Get distance of listbox to right edge
			// this will be the same as distance between
			// right edge of listbox and preview pane
			int controlGap = rc.right - r.right;
			
			// New list width
			int lstWidth = (r.right - r.left) - controlGap - OFN_PREVIEW_WIDTH;
			int lstHeight = (r.bottom - r.top);

			hwndPreview = 
				CreateFilePreviewPane(
					hwndContainer, 
					p.x + lstWidth + controlGap, 
					p.y, 
					OFN_PREVIEW_WIDTH, 
					lstHeight,
					HookOptions
				);
			if (hwndPreview == NULL) return FALSE;
			
			// Resize list to make room for new controls
			SetWindowPos(hwndLst, NULL, 0, 0, lstWidth, lstHeight, SWP_NOZORDER | SWP_NOMOVE);
			
			// Resize main window to 25% larger
			//GetWindowRect(hwndContainer, &r);
			//SendMessage(hwndContainer, WM_SIZE, SIZE_RESTORED, MAKELPARAM((r.right-r.left)*5/4, r.bottom-r.top));
			return TRUE;
		}
		case WM_NOTIFY: {
			switch (((OFNOTIFY*) lParam)->hdr.code) {
				case CDN_SELCHANGE: {
					TCHAR szPath[MAX_PATH];
					int result = (int) SendMessage(GetParent(hwndDlg), CDM_GETFILEPATH,
									sizeof(szPath), (LPARAM) szPath);
					if (result < 0) break;
					
					ShowFilePreview(hwndPreview, szPath);
					break;
				}
			}
			break;
		}
	}
	return 0;	
}


void GetOpenSendFileName(HWND hwnd) {
	OPENFILENAME ofn;
	int result;
	const TCHAR lpstrFilter[] 	= _T("Known File Types\0*.73p;*.82*;*.83p*;*.8xp*;*.8xk;*.73k;*.sav;*.rom;*.lab;*.8xu\0\
										Calculator Program Files  (*.73p;*.82*;*.83p*;*.8xp*)\0*.73p;*.82*;*.83p*;*.8xp*\0\
										Calculator Applications  (*.8xk, *.73k)\0*.8xk;*.73k\0\
										Calculator OSes (*.8xu)\0*.8xu\0\
										Save States  (*.sav)\0*.sav\0\
										ROMS  (*.rom)\0*.rom\0\
										Label Files (*.lab)\0*.lab\0\
										All Files (*.*)\0*.*\0\0");
	const TCHAR lpstrTitle[] = _T("Wabbitemu Open File");
	TCHAR filepath[MAX_PATH+256];
	TCHAR filestr[MAX_PATH+256];
	TCHAR *FileNames = NULL;
	TCHAR *filename;
	TCHAR *filestroffset;
	
	static OFNHookOptions HookOptions;
	
	ZeroMemory(filepath, sizeof(filepath));
	ZeroMemory(filestr, sizeof(filepath));

	ofn.lStructSize			= sizeof(OPENFILENAME);
	ofn.hwndOwner			= hwnd;
	ofn.hInstance			= g_hInst;
	ofn.lpstrFilter			= (LPCTSTR) lpstrFilter;
	ofn.lpstrCustomFilter	= NULL;
	ofn.nMaxCustFilter		= 0;
	ofn.nFilterIndex		= 0;
	ofn.lpstrFile			= filepath;
	ofn.nMaxFile			= ARRAYSIZE(filepath);
	ofn.lpstrFileTitle		= NULL;
	ofn.nMaxFileTitle		= 0;
	ofn.lpstrInitialDir		= NULL;
	ofn.lpstrFileTitle		= NULL;
	ofn.nMaxFileTitle		= 0;
	ofn.lpstrInitialDir		= NULL;
	ofn.lpstrTitle			= lpstrTitle;
	ofn.Flags				= 	OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | 
								OFN_EXPLORER | OFN_ALLOWMULTISELECT | 
								OFN_ENABLEHOOK | OFN_HIDEREADONLY |
								OFN_ENABLESIZING;
	ofn.lpstrDefExt			= NULL;
	ofn.lCustData			= (LPARAM) &HookOptions;
	ofn.lpfnHook			= OFNHookProc;
	ofn.lpTemplateName		= NULL;
	ofn.FlagsEx				= 0;

	result = GetOpenFileName(&ofn);
	
	if (!result) return;
	
	memcpy(filestr, filepath, ofn.nFileOffset * sizeof(TCHAR));
	
	for (filestroffset = filestr;filestroffset[0] != 0; filestroffset++);
	filestroffset[0] = '\\';
	filestroffset++;		/* DOUBLE CHECK THIS */
	filename = filepath + ofn.nFileOffset;

	int send_mode = SEND_CUR;
	if (!HookOptions.bFileSettings) {
		send_mode = SEND_RAM;
		if (HookOptions.bArchive) send_mode = SEND_ARC;
	}

	while(filename[0] != 0) {
		size_t len;
#ifdef WINVER
		StringCchCopy(filestroffset, _tcslen(filename) + 1, filename);
#else
		strcpy(filestroffset, filename);
#endif
		len = _tcslen(filestroffset);
		filestroffset[len] = 0;
		filename += len + 1;
		LPCALC lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		SendFileToCalc(lpCalc, filestr, TRUE);
	}
}
