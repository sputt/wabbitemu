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
	switch (Message) {
			case WM_INITDIALOG:
			{
				int speed = calcs[gslot].speed;
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
					calcs[gslot].speed = newPos;
					HMENU hMenu = GetSubMenu(GetMenu(calcs[gslot].hwndFrame), 2);
					switch(newPos)
					{
						case 25:
							CheckMenuRadioItem(hMenu, IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_QUARTER, MF_BYCOMMAND| MF_CHECKED);
							break;
						case 50:
							CheckMenuRadioItem(hMenu, IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_HALF, MF_BYCOMMAND| MF_CHECKED);
							break;
						case 100:
							CheckMenuRadioItem(hMenu, IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_NORMAL, MF_BYCOMMAND| MF_CHECKED);
							break;
						case 200:
							CheckMenuRadioItem(hMenu, IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_DOUBLE, MF_BYCOMMAND| MF_CHECKED);
							break;
						case 400:
							CheckMenuRadioItem(hMenu, IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_QUADRUPLE, MF_BYCOMMAND| MF_CHECKED);
							break;
						default:
							CheckMenuRadioItem(hMenu, IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_SET, MF_BYCOMMAND| MF_CHECKED);
							break;
					}
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
