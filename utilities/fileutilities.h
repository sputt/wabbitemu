#ifndef FILEUTILITIES_H
#define FILEUTILITIES_H

int BrowseFile(TCHAR *lpstrFile, const TCHAR *lpstrFilter, const TCHAR *lpstrTitle, const TCHAR *lpstrDefExt, unsigned int Flags = 0);
int SaveFile(TCHAR *lpstrFile, const TCHAR *lpstrFilter, const TCHAR *lpstrTitle, const TCHAR *lpstrDefExt, unsigned int Flags = 0);
BOOL ValidPath(TCHAR *lpstrFile);

#endif	//FILEUTILITIES_H
