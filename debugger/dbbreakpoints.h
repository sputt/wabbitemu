#ifndef DBBREAKPOINTS_H
#define DBBREAKPOINTS_H

LRESULT CALLBACK BreakpointsDialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
void add_breakpoint(CPU_t *cpu, BREAK_TYPE type, waddr_t waddr);

#endif			//DBBREAKPOINTS_H