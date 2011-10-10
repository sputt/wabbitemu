#include "stdafx.h"

#include "fileutilities.h"

int BrowseFile(TCHAR* lpstrFile, const TCHAR *lpstrFilter, const TCHAR *lpstrTitle, const TCHAR *lpstrDefExt, unsigned int Flags) {
	lpstrFile[0] = '\0';
	OPENFILENAME ofn;
	ofn.lStructSize			= sizeof(OPENFILENAME);
	ofn.hwndOwner			= GetForegroundWindow();
	ofn.hInstance			= NULL;
	ofn.lpstrFilter			= (LPCTSTR) lpstrFilter;
	ofn.lpstrCustomFilter	= NULL;
	ofn.nMaxCustFilter		= 0;
	ofn.nFilterIndex		= 0;
	ofn.lpstrFile			= (LPTSTR) lpstrFile;
	ofn.nMaxFile			= MAX_PATH;
	ofn.lpstrFileTitle		= NULL;
	ofn.nMaxFileTitle		= 0;
	ofn.lpstrInitialDir		= NULL;
	ofn.lpstrTitle			= lpstrTitle;
	ofn.Flags				= Flags | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_LONGNAMES;
	ofn.lpstrDefExt			= lpstrDefExt;
	ofn.lCustData			= 0;
	ofn.lpfnHook			= NULL;
	ofn.lpTemplateName		= NULL;
	ofn.pvReserved			= NULL;
	ofn.dwReserved			= 0;
	ofn.FlagsEx				= 0;
	if (!GetOpenFileName(&ofn)) {
		return 1;
	}
	return 0;
}

int SaveFile(TCHAR *lpstrFile, const TCHAR *lpstrFilter, const TCHAR *lpstrTitle, const TCHAR *lpstrDefExt, unsigned int flags, unsigned int filterIndex) {
	lpstrFile[0] = '\0';
	OPENFILENAME ofn;
	ofn.lStructSize			= sizeof(OPENFILENAME);
	ofn.hwndOwner			= GetForegroundWindow();
	ofn.hInstance			= NULL;
	ofn.lpstrFilter			= (LPCTSTR) lpstrFilter;
	ofn.lpstrCustomFilter	= NULL;
	ofn.nMaxCustFilter		= 0;
	ofn.nFilterIndex		= filterIndex;
	ofn.lpstrFile			= (LPTSTR) lpstrFile;
	ofn.nMaxFile			= MAX_PATH;
	ofn.lpstrFileTitle		= NULL;
	ofn.nMaxFileTitle		= 0;
	ofn.lpstrInitialDir		= NULL;
	ofn.lpstrTitle			= lpstrTitle;
	ofn.Flags				= flags | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_LONGNAMES | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt			= lpstrDefExt;
	ofn.lCustData			= 0;
	ofn.lpfnHook			= NULL;
	ofn.lpTemplateName		= NULL;
	ofn.pvReserved			= NULL;
	ofn.dwReserved			= 0;
	ofn.FlagsEx				= 0;
	if (!GetSaveFileName(&ofn)) {
		return 1;
	}
	return 0;
}

BOOL ValidPath(TCHAR *lpstrFile) {
	FILE *file;
#ifdef WINVER
	errno_t error = _tfopen_s(&file, lpstrFile, _T("r"));
	if (file)
		fclose(file);
	return error == 0;
#else
	file = fopen(lpstrFile, "r");
	BOOL error = file == NULL;
	fclose(file);
	return error;
#endif
}
