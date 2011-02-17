#include "stdafx.h"

#if _WINDLL
#include "spasm.h"
#include "utils.h"
#include "storage.h"
#include "list.h"

list_t *default_defines;
typedef struct {
	char name[64];
	char value[64];
} default_define_pair_t;

#ifdef LOG
#include <stdio.h>
FILE *logfile;
#endif
extern "C" 
{
	__declspec (dllexport) BOOL __stdcall DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
	{
		switch (ul_reason_for_call)
		{
		case DLL_PROCESS_ATTACH:
	#ifdef LOG
		logfile = fopen("logfile.txt", "a");
		fprintf(logfile, "Log opened\n");
		fflush(logfile);  
	#endif
		/*
		AllocConsole();    
		SetConsoleTitle("SPASM");   
		// The following is a really disgusting hack to make stdin and stdout attach   
		// to the newly created console using the MSVC++ libraries. I hope other   
		// operating systems don't need this kind of kludge.. :)    
		stdout->_file = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);    
		stdin->_file  = _open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT);  
		*/
			output_contents = (unsigned char *) malloc_chk (OUTPUT_BUF_SIZE);
			setvbuf(stdout, output_text, _IOFBF, 65536);
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
	#ifdef LOG
		fprintf(logfile, "Log closed\n");
		fflush(logfile);
		fclose(logfile);
	#endif
		//stdout->_file = -1;    
		//stdin->_file  = -1;    
		//FreeConsole();
			free(output_contents);
#ifdef _DEBUG
			_CrtDumpMemoryLeaks();
#endif
			break;
		}

    
		return TRUE;
	}

	__declspec (dllexport) int __stdcall 
		SetInputFile(const char *lpFilename) {
		curr_input_file = strdup(lpFilename); 
	#ifdef LOG
		fprintf(logfile, "SetInputFile: %s\n", curr_input_file);
		fflush(logfile);
	#endif
		return EXIT_NORMAL;
	}

	__declspec (dllexport) int __stdcall 
	SetOutputFile(const char *lpFilename) {
		output_filename = strdup(lpFilename);
	#ifdef LOG
		fprintf(logfile, "SetInputFile: %s\n", output_filename);
		fflush(logfile);
	#endif
		return EXIT_NORMAL;
	}

	__declspec (dllexport) int __stdcall 
	ClearDefines() {
		list_free(default_defines, true);
		default_defines = NULL;
	#ifdef LOG
		fprintf(logfile, "ClearDefines");
		fflush(logfile);
	#endif
		return 0;
	}

	__declspec (dllexport) int __stdcall
	AddDefine(const char *lpName, const char *lpValue) {
		default_define_pair_t *ddp = (default_define_pair_t*)malloc_chk (sizeof(*ddp));
		strncpy(ddp->name, lpName, sizeof(ddp->name));
		if (lpValue == NULL) {
			strcpy(ddp->value, "1");
		} else {
			strncpy(ddp->value, lpValue, sizeof(ddp->value));
		}
		default_defines = list_append(default_defines, ddp);
	#ifdef LOG
		fprintf(logfile, "AddDefine: %s %d\n", ddp->name, ddp->value);
		fflush(logfile);
	#endif
		return 0;
	}

	__declspec (dllexport) int __stdcall
	ClearIncludes() {
		list_free(include_dirs, true);
		include_dirs = NULL;
	#ifdef LOG
		fprintf(logfile, "ClearIncludes\n");
		fflush(logfile);
	#endif
		return 0;
	}

	__declspec (dllexport) int __stdcall
	AddInclude(const char *lpDirectory) {
		include_dirs = list_append(include_dirs, strdup (lpDirectory));
	#ifdef LOG
		fprintf(logfile, "AddInclude: %s\n", lpDirectory);
		fflush(logfile);
	#endif
		return 0;
	}

	__declspec (dllexport) int __stdcall
	RunAssembly() {
		list_t *list = default_defines;
		int result;

		mode = MODE_NORMAL;
		init_storage();
	
		while (list) {
			char *name = ((default_define_pair_t *) list->data)->name;
			char *value = ((default_define_pair_t *) list->data)->value;
			add_define (strdup (name), NULL)->contents = strdup (value);
			list = list->next;
		}
	
		printf("Input file: %s\n", curr_input_file);
		printf("Output file: %s\n", output_filename);
	
		list = include_dirs;
		while (list) {
			printf("Include dir: %s\n", (char *) list->data);
			list = list->next;
		}
	
		if (curr_input_file == NULL || output_filename == NULL) {
	#ifdef LOG
			fprintf(logfile, "Unable to assemble file\n");
			fflush(logfile);
	#endif
			return EXIT_FATAL_ERROR;
		}

		result = run_assembly();
		fflush(stdout);
	#ifdef LOG
		fprintf(logfile, "File assembled with return value: %d\n", result);
		fflush(logfile);
	#endif
		free_storage();
		return result;
	}

	__declspec (dllexport) 
	char* __stdcall GetStdOut()
	{
		return output_text;
	}

	__declspec (dllexport) 
	int __stdcall RunAssemblyWithArguments(char *szCommand, BYTE *lpResult, int cbResult) {

		output_filename = NULL;
		mode = MODE_NORMAL | MODE_COMMANDLINE;
		input_contents = (char *) malloc_chk (strlen(szCommand) + 1 + 2);
		
		strcpy(input_contents, " ");
		strcat(input_contents, szCommand);
		strcat(input_contents, "\n");
	
		curr_input_file = NULL;
		int result = run_assembly();

		if (lpResult != NULL) {
			memcpy(lpResult, output_contents, min(out_ptr - output_contents, cbResult));
		}
		return min(out_ptr - output_contents, cbResult);
	}

	__declspec (dllexport) int __stdcall
	ShowMessage() {
		MessageBox(NULL, "Hello", "Hello", MB_OK);
		return 0;
	}
}

#endif