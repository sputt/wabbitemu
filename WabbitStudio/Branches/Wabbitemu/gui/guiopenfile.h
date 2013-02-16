#ifndef GUIOPENFILE_H_
#define GUIOPENFILE_H_

typedef struct tag_OFNHookOptions {
	BOOL bArchive;
	BOOL bFileSettings;
	int model;
} OFNHookOptions;

void GetOpenSendFileName(HWND);

#endif /*GUIOPENFILE_H_*/
