#ifdef WINVER
# include <windows.h>
#else
# include <stdio.h>
#endif
#include "console.h"
#include "spasm.h"

#ifdef WINVER
//saved console attributes, to be restored on exit
WORD orig_attributes;
#endif

void restore_console_attributes () {
	if (!use_colors) return;
#ifdef WINVER
	SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE), orig_attributes);
#elif !defined(MACVER)
	printf ("\x1b[0m");
#endif
}

void save_console_attributes () {
#ifdef WINVER
    CONSOLE_SCREEN_BUFFER_INFO csbiScreenBufferInfo;
    GetConsoleScreenBufferInfo (GetStdHandle (STD_OUTPUT_HANDLE), &csbiScreenBufferInfo);
    orig_attributes = csbiScreenBufferInfo.wAttributes;
#endif
}

bool set_console_attributes (unsigned short attr) {
	if (!use_colors) return true;
#ifdef WINVER
    return (SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE), (WORD)attr));
#elif !defined(MACVER)
	printf ("\x1b[1;%d;40m", attr);
    return true;
#endif
}

