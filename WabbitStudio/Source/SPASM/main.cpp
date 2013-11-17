#include "stdafx.h"

#define __MAIN_C

#include "pass_one.h"
#include "pass_two.h"
#include "storage.h"
#include "spasm.h"
#include "utils.h"
#ifndef USE_GMP
#include "big.h"
#endif
#include "console.h"
#include "errors.h"
#include "Module.h"

#define LISTING_BUF_SIZE 65536	//initial size of buffer for output listing

void write_file (const unsigned char *, int, const char *);

extern expr_t *expr_list, *expr_list_tail;
extern output_t *output_list, *output_list_tail;

#ifdef _WIN32
CSPASMModule _AtlModule;
#endif

/*
 * Must have mode set
 * Must have input file name
 * Must have output file name
 * Must have storage initialized
 * 
 * After those conditions are met, go for it!
 */

int run_assembly()
{
#ifdef _WIN32
	struct _timeb time_start, time_end;
	_ftime(&time_start);
#endif
	exit_code = EXIT_NORMAL;
	
	/*extern int generic_map[256];
	ZeroMemory(generic_map, 256);*/
	program_counter = 0x0000;
	stats_codesize = stats_datasize = stats_mintime = stats_maxtime = 0;
	last_label = NULL;
	error_occurred = false;
	listing_offset = 0;
	listing_for_line_done = false;
	line_start = NULL;
	old_line_num = 0;
	in_macro = 0;
	line_num = 0;
#ifdef USE_BUILTIN_FCREATE
	cur_buf = 0;
#endif
#ifdef USE_REUSABLES
	curr_reusable = 0;
	total_reusables = 0;
#endif
	
	expr_list = NULL;
	expr_list_tail = NULL;
	output_list = NULL;
	output_list_tail = NULL;

	assert(curr_input_file != NULL);

	//read in the input file
	if (!(mode & MODE_COMMANDLINE))
		input_contents = (char *) get_file_contents (curr_input_file);

	if (!input_contents) {
		puts ("Couldn't open input file");
		return EXIT_FATAL_ERROR;
	}
	
	out_ptr = output_contents;

	//along with the listing buffer, if required
	if ((mode & MODE_LIST)) {
		listing_buf = eb_init (LISTING_BUF_SIZE);
		listing_offset = 0;
		listing_on = true;
	}
	
	//find the path of the input file
	if (is_abs_path(curr_input_file)) {
		int i;

		strcpy(temp_path, curr_input_file);
	
		for (i = strlen(temp_path) - 1; 
			temp_path[i] != '\\' && temp_path[i] != '/' && i > 0; i--);
		if (i >= 0)
			temp_path[i] = '\0';
		else
			strcpy(temp_path, ".");
	} else {
#ifdef WIN32
		_getcwd(temp_path, sizeof (temp_path));
#else
		getcwd(temp_path, sizeof (temp_path));
#endif
	}

	//add the the input file's path to the include directories
	include_dirs = list_prepend (include_dirs, strdup (temp_path));

	printf ("Pass one... \n");

	run_first_pass ((char *) input_contents);

	//free include dirs when done
	if ((mode & MODE_COMMANDLINE) == 0)
	{
		release_file_contents(input_contents);
		input_contents = NULL;
	}
	
	list_free (include_dirs, true, NULL);
	include_dirs = NULL;

	//...and if there's output, run the second pass and write it to the output file
	if (mode & MODE_NORMAL || mode & MODE_LIST)
	{
		printf ("Pass two... \n");

		int session = StartSPASMErrorSession();
		run_second_pass ();
		ReplaySPASMErrorSession(session);
		EndSPASMErrorSession(session);
		printf ("Done\n");
		
		//run the output through the appropriate program export and write it to a file
		if (mode & MODE_NORMAL && output_filename != NULL)
		{
			write_file (output_contents, out_ptr - output_contents, output_filename);
		}

		//write the listing file if necessary
		if ((mode & MODE_LIST)) {
			FILE *file;
			char *name;

			//get file name
			name = change_extension (output_filename, "lst");
			file = fopen (name, "wb");
			if (!file) {
				printf ("Couldn't open listing file '%s'", name);
				free (name);
				return EXIT_FATAL_ERROR;
			}
			free (name);
			
			if (fwrite (listing_buf->start, 1, listing_offset, file) != listing_offset) {
				puts ("Error writing to listing file");
				fclose (file);
				return EXIT_FATAL_ERROR;
			}

			fclose (file);
			eb_free(listing_buf);
			listing_buf = NULL;
		}
		
		//free the output buffer and all the names of input files
		list_free(input_files, true, NULL);
		input_files = NULL;
	}

	//if there's info to be dumped, do that
	if (mode & MODE_CODE_COUNTER) {
		fprintf (stderr, "Size: %u\nMin. execution time: %u\nMax. execution time: %u\n",
		         stats_codesize, stats_mintime, stats_maxtime);
	}
	
	if (mode & MODE_SYMTABLE) {
		char* fileName = change_extension(output_filename, "lab");
		write_labels (fileName);
		free(fileName);
	}

	if (mode & MODE_STATS) {
		fprintf (stderr, "Number of labels: %u\nNumber of defines: %u\nCode size: %u\nData size: %u\nTotal size: %u\n",
		         get_num_labels (), get_num_defines (), stats_codesize, stats_datasize, stats_codesize + stats_datasize);
	}
	
#ifdef _WIN32
	_ftime(&time_end);
	int s_diff = (int) (time_end.time - time_start.time);
	int ms_diff = time_end.millitm - time_start.millitm;
	if (ms_diff < 0) {
		ms_diff += 1000;
		s_diff -= 1;
	} else if (ms_diff > 1000) {
		ms_diff -= 1000;
		s_diff += 1;
	}
	printf("Assembly time: %0.3f seconds\n", (float) s_diff + ((float) ms_diff / 1000.0f));
#endif
	return exit_code;
}

#if 1
int CALLBACK WinMain(HINSTANCE hInst, HINSTANCE  hPrev, LPSTR lpCommandLine, int nCmdShow)
{
	return _AtlModule.WinMain(SW_HIDE);
}
#else
int main (int argc, char **argv)
{
	int curr_arg = 1;
	bool case_sensitive = false;
	bool is_storage_initialized = false;

	use_colors = true;
	extern WORD user_attributes;
	user_attributes = save_console_attributes ();
	atexit (restore_console_attributes_at_exit);

	//if there aren't enough args, show info
	if (argc < 2) {
		puts ("SPASM Z80 Assembler by Spencer Putt and Don Straney");
#ifdef _M_X64
		puts ("64-bit Version");
#endif
		puts ("\n\nspasm [options] <input file> <output file>\n");
		puts ("Options:\n-T = Generate code listing\n-C = Code counter mode\n-L = Symbol table mode\n-S = Stats mode\n-O = Don't write to output file");
		puts ("-I [directory] = Add include directory\n-A = Labels are cAse-sensitive\n-D<name>[=value] = Create a define 'name' [with 'value']");
		puts ("-N = Don't use colors for messages");
		puts ("-V <Expression> = Pipe expression directly into assembly");

#if defined(_DEBUG) && defined(WIN32)
		if (IsDebuggerPresent())
		{
			system("PAUSE");
		}
#endif
		return EXIT_NORMAL;
	}

	//init stuff
	mode = MODE_NORMAL;
	in_macro = 0;
	
	//otherwise, get any options
	curr_input_file = strdup("Commandline");
	const char * const starting_input_file = curr_input_file;

	while (curr_arg < argc) {
		if (argv[curr_arg][0] == '-'
#ifdef _WINDOWS
			|| argv[curr_arg][0] == '/'
#endif
			)
		{
			switch (argv[curr_arg][1])
			{
			//args for different modes
			case 'O':
				mode = mode & (~MODE_NORMAL);
				break;
			case 'T':
				mode |= MODE_LIST;
				break;
			case 'C':
				mode |= MODE_CODE_COUNTER;
				break;
			case 'L':
				mode |= MODE_SYMTABLE;
				break;
			case 'S':
				mode |= MODE_STATS;
				break;
			//handle no-colors flag
			case 'N':
				use_colors = false;
				break;
			//handle include files too
			case 'I':
			{
				char *dir, *p;
				//make sure there's another argument after it for the include path
				if (strlen(argv[curr_arg]) > 2) {
					dir = strdup (&argv[curr_arg][2]);
				} else {
					if (curr_arg >= argc - 1) {
						printf ("%s used without include path\n", argv[curr_arg]);
						break;
					}
					
					dir = strdup (argv[++curr_arg]);
				}
				
				for (p = strtok (dir, ";,"); p; p = strtok (NULL, ";,")) {
					include_dirs = list_append (include_dirs, strdup(p));
				}
				free(dir);
				break;
			}
			//and the case-sensitive flag
			case 'A':
				case_sensitive = true;
				break;
			//handle adding defines
			case 'D':
			{
				char name[256];
				char *ptr;
				define_t *define;

				if (!is_storage_initialized)
				{
					init_storage();
					is_storage_initialized = true;
				}

				if (strlen (argv[curr_arg]) > 2) {
					ptr = &argv[curr_arg][2];
				} else {
					if (curr_arg >= argc - 1) {
						printf ("%s used without define name", argv[curr_arg]);
						break;
					}
					
					ptr = argv[++curr_arg];
				}

				read_expr (&ptr, name, "=");

				define = add_define (strdup (name), NULL);
				if (*skip_whitespace (++ptr) != '\0')
					define->contents = strdup (ptr);
				else
					set_define (define, "1", 1, false);
				break;
			}
			case 'V':
			{
				char *line;
				
				//check for something after -V
				if (strlen(argv[curr_arg]) > 2) {
					line = &argv[curr_arg][2];
				} else {
					//if not lets fail
					if (curr_arg >= argc - 1) {
						printf ("%s used without a line to assemble\n", argv[curr_arg]);
						return EXIT_FATAL_ERROR;
					}
					line = argv[++curr_arg];
				}
				
				mode |= MODE_COMMANDLINE;
				curr_input_file = strdup("-v");
				input_contents = (char *) malloc (strlen(line) + 1 + 2);
				output_filename = change_extension (curr_input_file, "bin");
					
				strcpy(input_contents, line);
				strcat(input_contents, "\n");
				break;
			}
			default:
				{
#ifndef _TEST
#ifdef _WINDOWS
					FreeConsole();
					return _AtlModule.WinMain(SW_HIDE);
#endif
#else
					printf ("Unrecognized option %s\n", argv[curr_arg]);
#endif
				}
				
			}

		} else {
			//if it's not a flag, then it must be a filename
			if (curr_input_file && (curr_input_file != starting_input_file) && !output_filename)
				output_filename = strdup(argv[curr_arg]);
			else if ((!curr_input_file) || (curr_input_file == starting_input_file))
				curr_input_file = strdup(argv[curr_arg]);

		}
		curr_arg++;
	}

	// Update case sensitivity settings
	set_case_sensitive (case_sensitive);
	
	//check on filenames
	if (!(mode & MODE_COMMANDLINE) && !curr_input_file) {
		puts ("No input file specified");
		return EXIT_FATAL_ERROR;
	}

	if (!output_filename) {
		if (mode & MODE_SYMTABLE)
			output_filename = change_extension (curr_input_file, "lab");
		else
			output_filename = change_extension (curr_input_file, "bin");
	}

	if (!is_storage_initialized)
	{
		init_storage();
		is_storage_initialized = true;
	}
	output_contents = (unsigned char *) malloc(output_buf_size);
	ClearSPASMErrorSessions();
	int session = StartSPASMErrorSession();
	int error = run_assembly();
	ReplaySPASMErrorSession(session);
	EndSPASMErrorSession(session);

	free(output_filename);
	output_filename = NULL;
	if (curr_input_file) {
		free(curr_input_file);
		curr_input_file = NULL;
	}
	if (include_dirs) {
		list_free(include_dirs, true, NULL);
	}

	free(output_contents);
	output_contents = NULL;
	ClearSPASMErrorSessions();
	free_storage();

#ifdef _WINDOWS
	_CrtDumpMemoryLeaks();
	if (IsDebuggerPresent())
	{
		system("PAUSE");
	}
#endif

	return error;
}
#endif