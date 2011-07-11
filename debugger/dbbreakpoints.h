#ifndef DBBREAKPOINTS_H
#define DBBREAKPOINTS_H

LRESULT CALLBACK BreakpointsDialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
void add_breakpoint(memc *mem, BREAK_TYPE type, waddr_t waddr);
void rem_breakpoint(memc *mem, BREAK_TYPE type, waddr_t waddr);
BOOL check_break_callback(memc *mem, BREAK_TYPE type, waddr_t waddr);

#endif			//DBBREAKPOINTS_H