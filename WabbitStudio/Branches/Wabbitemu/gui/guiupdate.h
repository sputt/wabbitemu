#ifndef GUIUPDATE_H
#define GUIUPDATE_H

TCHAR *GetWhatsNewText();
void ShowWhatsNew(BOOL);
DWORD WINAPI CheckForUpdates(LPVOID);

#endif