#ifndef GUIOPTIONS_H
#define GUIOPTIONS_H

INT_PTR CALLBACK DisplayOptionsProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK GIFOptionsProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ROMOptionsProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SkinOptionsProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam);
void DoPropertySheet(HWND hwnd);
int SetGifName(BOOL bSave);

#define TBRTICS 4
#define TBRSTEP 5

#endif
