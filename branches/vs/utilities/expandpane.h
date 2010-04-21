#ifndef EXPANDPANE_H
#define EXPANDPANE_H

#include <windows.h>

LRESULT CALLBACK ExpandPaneProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
HWND CreateExpandPane(HWND hwndParent, char *name, HWND contents);
//int GetExpandedPanesHeight(void);

typedef struct {
	int total;
	BOOL state[32];
} ep_state;

#define g_szExpandPane	"wabexppane"

INT_PTR QueryDebugKey(char*);
void SaveDebugKey(char*, DWORD*);
void GetExpandPaneState(ep_state *);
void SetExpandPaneState(const ep_state *);
void ArrangeExpandPanes(void);
int GetExpandPanesHeight(void);

#define WM_SHIFT (WM_USER) //wParam = position to move to

#endif /*EXPANDPANE_H_*/
