#ifndef GUIRESOURCE_H
#define GUIRESOURCE_H

DWORD ExtractResource(TCHAR *path, HRSRC resource);
DWORD ExtractResourceText(char *buffer, size_t bufferSize, HRSRC resource);

#endif