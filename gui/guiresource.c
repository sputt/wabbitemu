#include "stdafx.h"

#include "guiresource.h"

DWORD ExtractResource(TCHAR *path, HRSRC resource) {
	HMODULE hModule = GetModuleHandle(NULL);
	if (!resource) {
		return 1;
	}

	HGLOBAL hGlobal = LoadResource(hModule, resource);
	DWORD size = SizeofResource(hModule, resource);
	void *data = LockResource(hGlobal);
	HANDLE hFile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile) {
		return 1;
	}

	DWORD writtenBytes;
	BOOL error = WriteFile(hFile, data, size, &writtenBytes, NULL);
	if (error == 0) {
		return GetLastError();
	}

	CloseHandle(hFile);
	return 0;
}