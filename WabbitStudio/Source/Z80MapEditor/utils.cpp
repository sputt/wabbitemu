#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <CommCtrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <strsafe.h>
#include <math.h>

extern HWND hwndStatus;

void SetStatusText(TCHAR *szStatusText) {
	SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM) szStatusText);
}

/*
 * Returns true if ptr has reached what is the end of an assembly
 * line of code.
 * For example: comment, line break (\), or new line
 */
BOOL is_end_of_code_line (TCHAR *ptr) {
	return (*ptr == _T('\0') || *ptr == _T('\n') || *ptr == _T('\r') || *ptr == _T(';') || *ptr == _T('\\'));
}

TCHAR *skip_whitespace (TCHAR *ptr) {
	while (_istspace(*ptr) && *ptr != _T('\n') && *ptr != _T('\r'))
		ptr++;

	return ptr;
}

/*
 * Seek a file to the line after the label
 */
BOOL SeekFileLabel(FILE *file, const TCHAR *lpszLabel) {
	TCHAR szBuffer[256];

	while (_fgetts(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), file)) {
		if (lstrcmp(lpszLabel, szBuffer) == 0)
			return TRUE;
	}
	rewind(file);
	return FALSE;
}

/*
 * Slow function to simplify coding of macros and directives with argument lists
 * .db blank, blank, macro(blank,blank), blank
 * macro( macro( blank, blank), blank)
 *
 * Returns 
 *		ptr to delimiter if terminated by reaching delimiter
 *		NULL if input was not delimiter terminated 
 */

TCHAR read_expr (TCHAR** ptr, TCHAR word[256], const TCHAR *delims) {
    TCHAR in_string = FALSE, // "xxx xxx xx"
	     in_escape = FALSE;	// \n
	int in_quote = 0;		// 'x'
	int level = 0;
	TCHAR *word_ptr = word;

	/* Is there a word left to get? */
	// Skip whitespace at the start
	*ptr = skip_whitespace (*ptr);
	if (is_end_of_code_line (*ptr) || **ptr == _T(')')) {
		if (word)
			*word = _T('\0');
		return FALSE;
	}

    while (**ptr != _T('\0') && !((_tcspbrk (*ptr, delims) == *ptr || is_end_of_code_line (*ptr))
				&& !in_escape && !in_string && in_quote == 0 && level == 0)) {

		if (!in_escape) {
			switch( **ptr ) {
				case _T('"'): 	
					if (in_quote == 0) in_string = !in_string; 
					break;
				case _T('\''):
					if (!in_string && in_quote == 0) in_quote = 3; 
					break;		
	        	case _T('\\'): 	
					in_escape = TRUE;
					break;
				case _T('('):
					if (!in_string && in_quote == 0) level++;
					break;
				case _T(')'):
					if (!in_string && in_quote == 0) level--;
					if (level < 0) {
						(*ptr)--;
						goto finish_read_expr;
					}
					break;
				default:
					/* 	If this TCHAR wasn't ', redo the inside */
					if (in_quote == 1) {
						*ptr -= 2;
						word_ptr -= 2;
					}
			}	
			if (in_quote) in_quote--;
		} else {
			in_escape = FALSE;
		}
		if (word_ptr - word >= 254) {
			//show_fatal_error (_T("Expression is too long - must be 255 chars or less"));
			if (word)
				lstrcpy (word, _T("0"));
			return TRUE;
		}
		if (word)
			*word_ptr++ = **ptr;
		(*ptr)++;
    }
    
finish_read_expr:
    // Remove whitespace at the end
	if (word) {
	    while (word_ptr > word && isspace (word_ptr[-1])) 
	    	*(--word_ptr) = _T('\0');
		*word_ptr = _T('\0');
	}

    /* input is either the delimiter or null */
    if (is_end_of_code_line (*ptr)) return TRUE;
    (*ptr)++;
	return TRUE;
}


BOOL Edit_Printf(HWND hwndEdit, TCHAR *szFormat, ...) {
	TCHAR szBuffer[256];
	va_list vl;
	va_start(vl, szFormat);

	StringCbVPrintf(szBuffer, sizeof(szBuffer), szFormat, vl);
	Edit_SetText(hwndEdit, szBuffer);
	va_end(vl);
	return TRUE;
}


BOOL OnContextMenu(HWND hwnd, int x, int y, HMENU hmenu) {
	RECT rc;
	POINT pt = {x, y};
	
	GetClientRect(hwnd, &rc);
	ScreenToClient(hwnd, &pt);
	
	if (PtInRect(&rc, pt)) {
		ClientToScreen(hwnd, &pt);
		
		TrackPopupMenu(hmenu, 
            TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
            pt.x, pt.y, 0, hwnd, NULL); 
 
	    // Destroy the menu. 
		return TRUE;
	}

	return FALSE;
}

int ComboBox_SelectByItemData(HWND hwndCombo, LONG_PTR dwItemData) {
	for (int i = 0; i < ComboBox_GetCount(hwndCombo); i++) {
		LONG_PTR dwCurrItemData = ComboBox_GetItemData(hwndCombo, i);

		if (lstrcmp((LPCTSTR) dwItemData, (LPCTSTR) dwCurrItemData) == 0) {
			ComboBox_SetCurSel(hwndCombo, i);
			return i;
		}
	}
	return -1;
}

LONG_PTR ComboBox_GetSelectionItemData(HWND hwndCombo) {
	TCHAR szValue[256];
	LONG_PTR dwValue;

	ComboBox_GetText(hwndCombo, szValue, sizeof(szValue));
	dwValue = ComboBox_GetItemData(hwndCombo, ComboBox_FindString(hwndCombo, -1, szValue));

	return dwValue;
}

HCURSOR GetCursorFromEdge(int Edge) {
	LPCTSTR lpsz;

	switch (Edge)
	{
	case WMSZ_BOTTOM:
	case WMSZ_TOP:
		lpsz = IDC_SIZENS;
		break;
	case WMSZ_BOTTOMLEFT:
	case WMSZ_TOPRIGHT:
		lpsz = IDC_SIZENESW;
		break;
	case WMSZ_BOTTOMRIGHT:
	case WMSZ_TOPLEFT:
		lpsz = IDC_SIZENWSE;
		break;
	case WMSZ_LEFT:
	case WMSZ_RIGHT:
		lpsz = IDC_SIZEWE;
		break;
	default:
		lpsz = IDC_SIZEALL;
		break;
	}
	return LoadCursor(NULL, lpsz);
}

double round(double num)
{
	double remainder = num - floor(num);
	if (remainder > 0.5) {
		return ceil(num);
	} else {
		return floor(num);
	}
}

void ShowLastError(PTSTR lpszFunction) 
// Format a readable error message, display a message box, 
// and exit from the application.
{ 
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 
}