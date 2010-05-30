#pragma once

#include <windows.h>

int __stdcall RunAssembly();
int __stdcall RunAssemblyWithArguments(char *szCommand, BYTE *lpResult, int cbResult);
int __stdcall ClearDefines();
int __stdcall AddDefine(const char *lpName, const char *lpValue);
int __stdcall ClearIncludes();
int __stdcall AddInclude(const char *lpDirectory);
int __stdcall SetInputFile(const char *lpFilename);
int __stdcall SetOutputFile(const char *lpFilename);