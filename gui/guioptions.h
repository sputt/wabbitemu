#ifndef GUIOPTIONS_H
#define GUIOPTIONS_H

INT_PTR CALLBACK DisplayOptionsProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK GIFOptionsProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ROMOptionsProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK SkinOptionsProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK KeysOptionsProc(HWND, UINT, WPARAM, LPARAM);
void ChangeMenuCommands(HWND);
char* GetFriendlyMenuText(HMENU, int, UINT);
void RecurseAddItems(HMENU, char*);
BOOL IsValidCmdRange(WORD);
void DoPropertySheet(HWND);
void ChangeCommand(HWND);
char* NameFromAccel(ACCEL);
void AddNormalKeys(char *);
int SetGifName(BOOL);

#define TBRTICS 4
#define TBRSTEP 5

#endif
