#include "stdafx.h"

#ifndef DBFINDDIALOG_H
#define DBFINDDIALOG_H

#include "calc.h"
#include "dbcommon.h"

typedef struct {
	LPCALC lpCalc;
	ViewType type;
	HWND hwndParent;
	int start_addr;
} find_dialog_params_t;

INT_PTR CALLBACK FindDialogProc(HWND, UINT, WPARAM, LPARAM);

#endif