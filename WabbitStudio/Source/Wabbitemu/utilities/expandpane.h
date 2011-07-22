#ifndef EXPANDPANE_H
#define EXPANDPANE_H

LRESULT CALLBACK ExpandPaneProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
HWND CreateExpandPane(HWND hwndParent, TCHAR *name, HWND contents);
//int GetExpandedPanesHeight(void);

typedef struct {
	int total;
	BOOL state[32];
} ep_state;

#define g_szExpandPane	_T("wabexppane")

void GetExpandPaneState(ep_state *);
void SetExpandPaneState(const ep_state *);
void ArrangeExpandPanes(void);
int GetExpandPanesHeight(void);
void DrawExpandPanes(void);

#define WM_SHIFT (WM_USER) //wParam = position to move to

#endif /*EXPANDPANE_H_*/
