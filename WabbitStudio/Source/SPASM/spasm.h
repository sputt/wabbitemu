#ifndef __SPASM_H
#define __SPASM_H

//#include <stdbool.h>
#include "storage.h"
#include "list.h"
#include "expand_buf.h"

typedef enum {
	MODE_NORMAL = 1,
	MODE_CODE_COUNTER = 2,
	MODE_SYMTABLE = 4,
	MODE_STATS = 8,
	MODE_LIST = 16,
	MODE_COMMANDLINE = 32,
} ASM_MODE;

typedef enum {
	EXIT_NORMAL = 0,
	EXIT_WARNINGS = 1,
	EXIT_ERRORS = 2,
	EXIT_FATAL_ERROR = 3
} EXIT_STATUS;

#ifdef WIN32
#include <windows.h>
#define NEWLINE "\r\n"
#define PATH_SEPARATOR '\\'
#define WRONG_PATH_SEPARATOR '/'
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define snprintf sprintf_s
#else
#define NEWLINE "\n"
#define PATH_SEPARATOR '/'
#define WRONG_PATH_SEPARATOR '\\'
#endif

#ifdef __MAIN_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

//#define OUTPUT_BUF_SIZE 8000000
const unsigned int output_buf_size = 8000000;	//size of output buffer for assembled code

//make sure that MAX_PATH is max path length on *nix and Windows
#if !defined(MAX_PATH) || defined(UNIX_VER)
	#include <limits.h>
	#define MAX_PATH PATH_MAX
#endif

GLOBAL unsigned int mode;
GLOBAL list_t *include_dirs, *input_files;
GLOBAL unsigned int program_counter;
GLOBAL label_t *last_label;
GLOBAL unsigned int stats_codesize, stats_datasize, stats_mintime, stats_maxtime;
GLOBAL int line_num;
GLOBAL char temp_path[MAX_PATH];
GLOBAL char *curr_input_file, *output_filename;
GLOBAL char *input_contents;
GLOBAL unsigned char *out_ptr, *output_contents;
GLOBAL bool error_occurred, suppress_errors;
GLOBAL expand_buf *listing_buf;
GLOBAL int listing_offset;
GLOBAL bool listing_on;
GLOBAL bool listing_for_line_done;	//true if listing for this line has already been done
GLOBAL char *line_start;	//saved start of current line
GLOBAL int old_line_num;	//saved line number
GLOBAL bool pass_one;	//true if pass 1, false if pass 2
GLOBAL int in_macro;	//depth in macro - 0 if not in macro, 1 if inside macro, 2 if inside macro called from macro...
GLOBAL bool use_colors;	//whether to use colors or not for messages
GLOBAL EXIT_STATUS exit_code;
#ifdef USE_REUSABLES
GLOBAL int total_reusables, curr_reusable;
#endif
#ifdef USE_BUILTIN_FCREATE
GLOBAL int cur_buf;
#endif
#ifdef _WINDLL
GLOBAL char output_text[800000];
#endif

//make sure max and min are defined for *nix
#ifndef max
	#define max(x,y) (((long) (x) > (long) (y)) ? (x) : (y))
#endif
#ifndef min
	#define min(x,y) (((long) (x) < (long) (y)) ? (x) : (y))
#endif

int run_assembly(void);

#endif
