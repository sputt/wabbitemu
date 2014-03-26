#ifndef GUIUPDATE_H
#define GUIUPDATE_H

BOOL GetWhatsNewText(TCHAR *whatsNewText, size_t sizeInChar);
void ShowWhatsNew(BOOL);
DWORD WINAPI CheckForUpdates(LPVOID);

#endif