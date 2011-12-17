#ifndef GUIOPTIONS_H
#define GUIOPTIONS_H

#include "calc.h"

INT_PTR CALLBACK DisplayOptionsProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK GIFOptionsProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK GeneralOptionsProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ROMOptionsProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK SkinOptionsProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK KeysOptionsProc(HWND, UINT, WPARAM, LPARAM);
void ChangeMenuCommands(HWND);
TCHAR* GetFriendlyMenuText(HMENU, int, UINT);
void RecurseAddItems(HMENU, TCHAR *);
BOOL IsValidCmdRange(WORD);
void DoPropertySheet(HWND);
void ChangeCommand();
TCHAR* NameFromAccel(ACCEL);
int GetNumKeyEntries();
void AssignAccel(HWND);
void RemoveAccel();
void AssignEmuKey(HWND);
void RemoveEmuKey();
TCHAR* NameFromVKey(UINT nVK);

void AddNormalKeys(TCHAR *, key_string keystrings[KEY_STRING_SIZE]);


#define TBRTICS 4
#define TBRSTEP 5

#endif
