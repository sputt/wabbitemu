#ifndef GUIDEBUG_H
#define GUIDEBUG_H

LRESULT CALLBACK DebugProc(HWND, UINT, WPARAM, LPARAM);
int BrowseTxtFile(TCHAR *);
#define REG_PANE_WIDTH 200

#define ID_DISASM 		0
#define ID_REG 			1
#define ID_MEMTAB		2
#define ID_MEM			3
#define ID_STACK		4
#define ID_WABBIT		5
#define ID_DISASMSIZE	6
#define ID_SIZE			6
#define ID_TOOLBAR		7
#define ID_SUBBAR		8
#define ID_DISASMTAB	9
#define ID_PANECONTAINER 10

#endif
