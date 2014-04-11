#ifndef GUITEACHERVIEW_H
#define GUITEACHERVIEW_H

#include "calc.h"
#include "keys.h"

LRESULT CALLBACK TeacherViewProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void HandleTeacherViewKey(HWND hwndTeacherView, LPCALC lpCalc, int group, int bit);

#define TEACHER_VIEW_SCALE 4
#define TEACHER_VIEW_CAPTION_SIZE 40
#define TEACHER_VIEW_ROWS 2
#define TEACHER_VIEW_COLS 2

typedef enum {
	YEQU_SCREEN = 0,
	STATPLOT_SCREEN,
	WINDOW_SCREEN,
	TABLESET_SCREEN,
	GRAPH_SCREEN,
	TABLE_SCREEN,
	LIST_SCREEN
} TeacherViewScreen_t;

#endif