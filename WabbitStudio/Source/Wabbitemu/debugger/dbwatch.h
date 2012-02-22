#ifndef DBWATCH_H
#define DBWATCH_H

#include "calc.h"
#include "dbcommon.h"

LRESULT CALLBACK WatchProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

#define MAX_WATCHPOINTS 20

typedef struct watchpoint {
	TCHAR label[64];
	waddr_t waddr;
	waddr_t waddr_is_valid;		//when we start the boxes are blank. This is tells us whether we should display the data
	//HWND hComboBox;				//we have to handle the combo box ourselves, this is its handle
	u_char size;					//number of elements to display
	u_char width, height;			//widthxheight for bitmaps
	BOOL is_bitmap;				//how we should display size
	BOOL size_is_valid;			//same as other is_valid
	VALUE_FORMAT val;			//how do we display these elements
} watchpoint_t;

#endif //DBWATCH_H