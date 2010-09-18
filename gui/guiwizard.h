#ifndef GUIWIZARD_H
#define GUIWIZARD_H

INT_PTR CALLBACK SetupStartProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK SetupTypeProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK SetupOSProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK SetupROMDumperProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK SetupMakeROMProc(HWND, UINT, WPARAM, LPARAM);
BOOL DoWizardSheet(HWND);
int BrowseOSFile(char* );
void ExtractDumperProg();
void ExtractResource(char *, HRSRC);

#endif
