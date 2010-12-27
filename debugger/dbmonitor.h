#ifndef DBMONITOR_H
#define DBMONITOR_H

LRESULT CALLBACK PortMonitorDialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
HWND DoCreateHeader(HWND hwndParent);
int DoInsertItem(HWND hwndHeader, int iInsertAfter, int nWidth, LPSTR lpsz);

#endif			//DBMONITOR_H