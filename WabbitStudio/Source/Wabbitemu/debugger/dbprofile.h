#ifndef DBPROFILE_H
#define DBPROFILE_H

#include "core.h"
#include "calc.h"

LRESULT CALLBACK ProfileDialogProc(HWND, UINT, WPARAM, LPARAM);
BOOL GetProfileOptions(HWND hwnd, profiler_t *profiler, int flash_pages, int ram_pages);
void ExportProfile(LPCALC lpCalc, profiler_t *profiler);

#endif		//DBPROFILE_H