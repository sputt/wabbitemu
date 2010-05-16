#ifndef GUIDEBUG_H
#define GUIDEBUG_H

LRESULT CALLBACK DebugProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ProfileDialogProc(HWND, UINT, WPARAM, LPARAM);
int xtoi(const char*, int*);
#define REG_PANE_WIDTH 200


#endif
