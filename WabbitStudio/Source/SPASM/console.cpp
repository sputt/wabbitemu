#include "stdafx.h"

#include "console.h"
#include "spasm.h"

#ifdef WIN32
//saved console attributes, to be restored on exit
WORD user_attributes;
#endif

void restore_console_attributes_at_exit () {
	if (!use_colors) return;
#ifdef WIN32
	SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE), user_attributes);
#elif !defined(MACVER)
	printf ("\x1b[0m");
#endif
}

void restore_console_attributes (WORD orig_attributes) {
	if (!use_colors) return;
#ifdef WIN32
	SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE), orig_attributes);
#elif !defined(MACVER)
	printf ("\x1b[0m");
#endif
}

WORD save_console_attributes () {
#ifdef WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbiScreenBufferInfo;
    GetConsoleScreenBufferInfo (GetStdHandle (STD_OUTPUT_HANDLE), &csbiScreenBufferInfo);
    return csbiScreenBufferInfo.wAttributes;
#endif
}

bool set_console_attributes (unsigned short attr) {
	if (!use_colors) return true;
#ifdef WIN32
    return SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE), (WORD)attr);
#elif !defined(MACVER)
	printf ("\x1b[1;%d;40m", attr);
    return true;
#endif
}

