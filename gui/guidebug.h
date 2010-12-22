#ifndef GUIDEBUG_H
#define GUIDEBUG_H

LRESULT CALLBACK DebugProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ProfileDialogProc(HWND, UINT, WPARAM, LPARAM);
int xtoi(const TCHAR *, int*);
int BrowseTxtFile(TCHAR *);
#define REG_PANE_WIDTH 200


#endif
