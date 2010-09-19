#ifndef FILEUTILITIES_H
#define FILEUTILITIES_H

int BrowseFile(char* lpstrFile, char *lpstrFilter, char *lpstrTitle, char *lpstrDefExt, unsigned int Flags = 0);
int SaveFile(char* lpstrFile, char *lpstrFilter, char *lpstrTitle, char *lpstrDefExt, unsigned int Flags = 0);
BOOL ValidPath(char *lpstrFile);

#endif	//FILEUTILITIES_H