#ifndef __CONSOLE_H
#define __CONSOLE_H

#ifdef WIN32
# define COLOR_RED			FOREGROUND_RED | FOREGROUND_INTENSITY
# define COLOR_YELLOW		FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
# define COLOR_WHITE		FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
# define COLOR_BLUE			FOREGROUND_BLUE | FOREGROUND_INTENSITY
# define COLOR_GREEN		FOREGROUND_GREEN | FOREGROUND_INTENSITY
#else
# define COLOR_RED			31
# define COLOR_YELLOW		33
# define COLOR_WHITE		37
# define COLOR_BLUE			34
# define COLOR_GREEN		32
#endif

WORD save_console_attributes ();
void restore_console_attributes_at_exit ();
void restore_console_attributes (WORD orig_attributes);
bool set_console_attributes (unsigned short);

#endif

