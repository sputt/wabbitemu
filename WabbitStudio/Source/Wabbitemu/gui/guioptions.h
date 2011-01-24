#ifndef GUIOPTIONS_H
#define GUIOPTIONS_H

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
void ChangeCommand(HWND);
TCHAR* NameFromAccel(ACCEL);
int GetNumKeyEntries();
void AssignAccel(HWND);
void RemoveAccel();
void AssignEmuKey(HWND);
void RemoveEmuKey();
TCHAR* NameFromVKey(UINT nVK);

#define KEY_STRING_SIZE 56
struct key_string{
	TCHAR *text;
	int group;
	int bit;
};
void AddNormalKeys(TCHAR *, key_string keystrings[KEY_STRING_SIZE]);


#define TBRTICS 4
#define TBRSTEP 5

#endif
