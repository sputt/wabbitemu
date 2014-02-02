#ifndef FILEUTILITIES_H
#define FILEUTILITIES_H

int BrowseFile(TCHAR *lpstrFile, const TCHAR *lpstrFilter, const TCHAR *lpstrTitle, const TCHAR *lpstrDefExt, unsigned int flags, unsigned int filterIndex);
int SaveFile(TCHAR *lpstrFile, const TCHAR *lpstrFilter, const TCHAR *lpstrTitle, const TCHAR *lpstrDefExt, unsigned int flags, unsigned int filterIndex);
BOOL ValidPath(TCHAR *lpstrFile);
void GetAppDataString(TCHAR *buffer, int len);

#endif	//FILEUTILITIES_H
