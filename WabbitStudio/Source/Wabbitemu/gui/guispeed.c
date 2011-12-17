/*
 * guispeed.c
 *
 *  Created on: Apr 4, 2010
 *      Author: Chris
 */
#include "stdafx.h"

#include "calc.h"
#include "guispeed.h"
#include "resource.h"

#define MIN_SPEED 1

int originalSpeed;

LRESULT CALLBACK SetSpeedProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static LPCALC lpCalc;
	switch (Message) {
		case WM_INITDIALOG:
		{
			lpCalc = (LPCALC) lParam;
			int speed = lpCalc->speed;
			originalSpeed = speed;
			HWND hTrackbar = GetDlgItem(hwnd, IDC_TRB1);
			SendMessage(hTrackbar, TBM_SETRANGE,
				    (WPARAM) TRUE,					// redraw flag
				    (LPARAM) MAKELONG(0, 1600));  	// min. & max. positions
			SendMessage(hTrackbar, TBM_SETTICFREQ, 50, 0);
			SendMessage(hTrackbar, TBM_SETPAGESIZE,
				    0, (LPARAM) 50);					// new page size
			SendMessage(hTrackbar, TBM_SETPOS,
				    (WPARAM) TRUE,                  // redraw flag
				    (LPARAM) speed);
			return TRUE;
		}
		case WM_HSCROLL: {
			HWND hTrackbar = GetDlgItem(hwnd, IDC_TRB1);
			if (hTrackbar == (HWND)lParam)
			{
				int newPos = (int) SendMessage(hTrackbar, TBM_GETPOS, 0, 0);
				newPos = newPos / 50;
				newPos = newPos * 50;
				if (newPos == 0)
					newPos = MIN_SPEED;
				SendMessage(hTrackbar, TBM_SETPOS, TRUE, newPos);
				lpCalc->speed = newPos;
			}
			return TRUE;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDC_SPEED_OK: {
					EndDialog(hwnd, IDOK);
					return TRUE;
				}
				case IDC_SPEED_CANCEL: {
					lpCalc->speed = originalSpeed;
					EndDialog(hwnd, IDCANCEL);
					return TRUE;
				}
				case 2:
					EndDialog(hwnd, IDCANCEL);
					return TRUE;
			}
			break;
		}
	}
	return FALSE;
}
