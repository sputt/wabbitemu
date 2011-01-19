#ifndef GUIOPENFILE_H_
#define GUIOPENFILE_H_

typedef struct tag_OFNHookOptions {
	BOOL bArchive;
	BOOL bFileSettings;
} OFNHookOptions;

void GetOpenSendFileName(HWND);

#endif /*GUIOPENFILE_H_*/
