#ifndef EXPANDPANE_H
#define EXPANDPANE_H

#include "guidebug.h"

LRESULT CALLBACK ExpandPaneProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
HWND CreateExpandPane(HWND hwndParent, LPDEBUGWINDOWINFO lpDebugInfo, TCHAR *name, HWND contents);
//int GetExpandedPanesHeight(void);

#define g_szExpandPane	_T("wabexppane")

void GetExpandPaneState(LPDEBUGWINDOWINFO lpDebugInfo, ep_state *);
void SetExpandPaneState(LPDEBUGWINDOWINFO lpDebugInfo, const ep_state *);
void ArrangeExpandPanes(LPDEBUGWINDOWINFO lpDebugInfo);
int GetExpandPanesHeight(LPDEBUGWINDOWINFO lpDebugInfo);
void DrawExpandPanes(LPDEBUGWINDOWINFO lpDebugInfo);

#define WM_SHIFT (WM_USER) //wParam = position to move to

#endif /*EXPANDPANE_H_*/
