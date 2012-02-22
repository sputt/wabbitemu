#ifndef DBTOOLBAR_H_
#define DBTOOLBAR_H_

#include "gui.h"
#include "core.h"
#include "disassemble.h"
#include "guidebug.h"

typedef enum {
	MOUSE_UP,
	MOUSE_DOWN,
	MOUSE_DOWN_SPLIT
} TOOL_BUTTON_STATE;

typedef struct TBBTN_tag{
	HWND hwnd;
	int bHotLit;
	BOOL bFading;
	TOOL_BUTTON_STATE MouseState;
	BOOL bSplitButton;
	struct TBBTN_tag *prev, *next;
	int trans_state;
	UINT_PTR uIDTimer;
	HBITMAP hbmIcon;
	HMENU hMenu;
	LPDEBUGWINDOWINFO lpDebugInfo;
} TBBTN;

void ChangeRunButtonIconAndText(LPCALC lpCalc, TBBTN *tbb);

LRESULT CALLBACK ToolBarProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

#endif /*DBTOOLBAR_H_*/
