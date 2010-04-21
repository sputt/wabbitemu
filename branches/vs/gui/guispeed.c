/*
 * guispeed.c
 *
 *  Created on: Apr 4, 2010
 *      Author: Chris
 */
#include "guispeed.h"
#include "resource.h"
#include "calc.h"
#define MIN_SPEED 5

float originalSpeed;

LRESULT CALLBACK SetSpeedProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
			case WM_INITDIALOG:
			{
				originalSpeed = calcs[gslot].speed;
				int speed = calcs[gslot].speed * 100;
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

			}
			case WM_HSCROLL: {
				HWND hTrackbar = GetDlgItem(hwnd, IDC_TRB1);
				if (hTrackbar == (HWND)lParam)
				{
					int newPos = SendMessage(hTrackbar, TBM_GETPOS, 0, 0);
					newPos = newPos / 50;
					newPos = newPos * 50;
					if (newPos == 0)
						newPos = MIN_SPEED;
					SendMessage(hTrackbar, TBM_SETPOS, TRUE, newPos);
					calcs[gslot].speed = (float)newPos / 100.0;
				}
				break;
			}
			case WM_COMMAND: {
				switch (LOWORD(wParam)) {
					case IDC_SPEED_OK: {
						EndDialog(hwnd, IDOK);
						return TRUE;
					}
					case IDC_SPEED_CANCEL: {
						calcs[gslot].speed = originalSpeed;
						EndDialog(hwnd, IDCANCEL);
						return FALSE;
					}
				}
			}
	}
	return DefWindowProc(hwnd, Message, wParam, lParam);
}
