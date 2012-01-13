#ifndef FILEUTILITIES_H
#define FILEUTILITIES_H

int BrowseFile(TCHAR *lpstrFile, const TCHAR *lpstrFilter, const TCHAR *lpstrTitle, const TCHAR *lpstrDefExt, unsigned int flags = 0);
int SaveFile(TCHAR *lpstrFile, const TCHAR *lpstrFilter, const TCHAR *lpstrTitle, const TCHAR *lpstrDefExt, unsigned int flags = 0, unsigned int filterIndex = 0);
BOOL ValidPath(TCHAR *lpstrFile);
void GetAppDataString(TCHAR *buffer, int len);

#endif	//FILEUTILITIES_H
