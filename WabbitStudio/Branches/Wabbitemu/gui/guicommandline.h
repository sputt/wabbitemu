#ifndef GUICOMMANDLINE_H
#define GUICOMMANDLINE_H

#include "link.h"

#define MAX_FILES 255
struct ParsedCmdArgs
{
	LPTSTR rom_files[MAX_FILES];
	LPTSTR utility_files[MAX_FILES];
	LPTSTR archive_files[MAX_FILES];
	LPTSTR ram_files[MAX_FILES];
	LPTSTR cur_files[MAX_FILES];
	int num_rom_files;
	int num_utility_files;
	int num_archive_files;
	int num_ram_files;
	int num_cur_files;
	BOOL silent_mode;
	BOOL force_new_instance;
	BOOL force_focus;
};

void ParseCommandLineArgs(ParsedCmdArgs *);
void LoadAlreadyExistingWabbit(LPARAM, LPTSTR, SEND_FLAG);
void LoadToLPCALC(LPARAM, LPTSTR, SEND_FLAG);
void LoadCommandlineFiles(ParsedCmdArgs *, LPARAM,  void (*)(LPARAM, LPTSTR, SEND_FLAG));

#endif //GUICOMMANDLINE_H