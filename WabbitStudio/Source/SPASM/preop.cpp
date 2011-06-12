#include "stdafx.h"

#include "preop.h"
#include "spasm.h"
#include "utils.h"
#include "storage.h"
#include "pass_one.h"
#include "parser.h"
#include "expand_buf.h"
#include "errors.h"
#include "bitmap.h"

char *do_if (char *ptr, int condition);
char *do_elif (char *ptr, int condition);
char *handle_preop_define (const char *ptr);
static char *handle_preop_include (char *ptr);
char *handle_preop_import (char *ptr);
char *handle_preop_if (char *ptr);
char *handle_preop_elif (char *ptr);
static char *skip_until (char *ptr, int *line, int argc, ...);

extern bool case_sensitive;
static define_t *last_define = NULL;
static int nIfLevel = 0;
static bool fInMacro = false;

/*
 * Handles a preop, returns the new
 * location in the file
 */

char *handle_preop (char *ptr) {
	const char *preops[] = {"define", "include", "if", "ifdef", "ifndef", "else", "elif", "endif",
		"undef", "undefine", "comment", "endcomment", "macro", "endmacro", "import", "defcont", "region", "endregion", NULL};
	char *name_end, *name;
	int preop;

	//first get the name
	name_end = ptr;
	while (isalpha (*name_end))
		name_end++;

	name = strndup (ptr, name_end - ptr);

	//then try to match it against the list of preops
	preop = 0;
	while (preops[preop]) {
		if (!strcasecmp (preops[preop], name))
			break;
		preop++;
	}

	ptr = skip_whitespace (name_end);

	// Error if it doesn't match any
	if (!preops[preop])
	{
		SetLastSPASMError(SPASM_ERR_UNKNOWN_PREOP, name);
		free(name);
		return ptr;
	}

	free (name);

	//otherwise, decide what to do depending on what the preop is
	switch (preop) {
		case 0:	//DEFINE
		{
			ptr = handle_preop_define (ptr);
			break;
		}
		case 1: //INCLUDE
		{
			ptr = handle_preop_include (ptr);
			break;
		}
		case 2: //IF
		{
			nIfLevel++;
			ptr = handle_preop_if (ptr);
			break;
		}
		case 6: // ELIF
			{
				if (nIfLevel == 0)
				{
					SetLastSPASMError(SPASM_ERR_ELIF_WITHOUT_IF);
				}
				int session = StartSPASMErrorSession();
				ptr = skip_until(ptr, &line_num, 1, "#endif");
				EndSPASMErrorSession(session);
				break;
			}
		case 3: //IFDEF
		{
			char *name_end, *name;
			define_t *define;
			bool condition;

			//get the name of the define to test
			if (is_end_of_code_line (ptr)) {
				SetLastSPASMError(SPASM_ERR_EXPRESSION_EXPECTED);
				return ptr;
			}

			nIfLevel++;
			name_end = skip_to_name_end (ptr);
			name = strndup (ptr, name_end - ptr);
			//if it's defined, do all the normal #if stuff
			define = search_defines (name);
			condition = (define != NULL) && (define->contents!= NULL);
			ptr = do_if (name_end, condition);
			free (name);
			break;
		}
		case 4: //IFNDEF
		{
			char *name_end, *name;
			define_t *define;
			bool condition;

			//get the name of the define to test
			if (is_end_of_code_line (ptr))
			{
				SetLastSPASMError(SPASM_ERR_EXPRESSION_EXPECTED);
				return ptr;
			}

			nIfLevel++;
			name_end = skip_to_name_end (ptr);
			name = strndup (ptr, name_end - ptr);

			//same as #ifdef, but reversed
			define = search_defines (name);
			condition = (define == NULL) || (define && (define->contents == NULL));
			ptr = do_if (name_end, condition);
			free (name);
			break;
		}
		case 5: //ELSE
		{
			if (nIfLevel == 0)
			{
				SetLastSPASMError(SPASM_ERR_STRAY_PREOP, preops[preop]);
			}
			else
			{
				int session = StartSPASMErrorSession();
				ptr = skip_until (ptr, &line_num, 3, "#else", "#elif", "#endif");
				EndSPASMErrorSession(session);
			}
			break;
		}
		case 7: //ENDIF
		{
			if (nIfLevel > 0)
			{
				nIfLevel--;
			}
			else
			{
				SetLastSPASMError(SPASM_ERR_STRAY_PREOP, _T("ENDIF"));
			}
			break;
		}
		case 8: //UNDEF
		case 9: //UNDEFINE
		{
			//get the name of the define to remove
			if (is_end_of_code_line (ptr)) {
				SetLastSPASMError(SPASM_ERR_NAME_EXPECTED);
				return ptr;
			}

			name_end = skip_to_name_end (ptr);
			name = strndup (ptr, name_end - ptr);
			remove_define (name);
			free (name);
			ptr = name_end;
			break;
		}
		case 10: //COMMENT
		{
			int session = StartSPASMErrorSession();
			ptr = skip_until (ptr, &line_num, 1, "#endcomment");
			EndSPASMErrorSession(session);
			break;
		}
		case 12: //MACRO
		{
			char *name_end, *macro_end;
			define_t *macro;

			if (is_end_of_code_line (ptr)) {
				SetLastSPASMError(SPASM_ERR_NAME_EXPECTED);
				return ptr;
			}

			//get the name
			name_end = skip_to_name_end (ptr);
			char *name = strndup(ptr, name_end - ptr);
			
			macro = add_define (name, NULL);

			name_end = skip_whitespace (name_end);
			if (*name_end == '(') {
				//it has arguments, so parse them
				ptr = parse_arg_defs (++name_end, macro);
				if (ptr == NULL)
					return NULL;
			} else
				ptr = name_end;

			ptr = skip_to_next_line(ptr);
			line_num++;

			macro->line_num++;
			
			fInMacro = true;
			//now find the end of the macro (at the end of the file or an #endmacro directive)
			//ptr = skip_to_next_line (ptr);
			macro_end = skip_until (ptr, &line_num, 1, "#endmacro");

			//...and copy everything up to the end into the contents
			set_define (macro, ptr, macro_end - ptr, false);
			fInMacro = false;
			macro_end = skip_to_line_end(macro_end);
			ptr = macro_end;
			break;
		}
		case 13: //ENDMACRO
			{
				break;
			}
		case 14: //IMPORT
		{
			ptr = handle_preop_import (ptr);
			break;
		}
		case 15: //DEFCONT
		{
			if (last_define == NULL) {
				SetLastSPASMError(SPASM_ERR_NO_PREVIOUS_DEFINE);
				break;
			}
			char *defcont_start = ptr;
			ptr = skip_to_next_line(ptr);
			int defcont_len = strlen(last_define->name) + (ptr - defcont_start) + 1;
			char *new_contents = (char *) malloc(defcont_len + 1);
			strcpy(new_contents, last_define->name);
			strcat(new_contents, " ");
			strncat(new_contents, defcont_start, ptr - defcont_start);
			new_contents[defcont_len] = '\0';
			set_define(last_define, new_contents, -1, true);
			ptr--;
		}
	}

	return ptr;
}


/*
 * Handles #DEFINE statement,
 * returns pointer to new location
 * in file
 */

char *handle_preop_define (const char *ptr) {
	char *name_end, *value_end;
	define_t *define;
	bool redefined;

	if (is_end_of_code_line (ptr)) {
		SetLastSPASMError(SPASM_ERR_NAME_EXPECTED);
		return (char *) ptr;
	}

	//get the name
	name_end = skip_to_name_end (ptr);
	define = add_define (strndup (ptr, name_end - ptr), &redefined);
	if (define == NULL)
		return skip_to_line_end (ptr);

	last_define = define;
	
	if (*name_end == '(') {
		//it's a simple macro, so get all the arguments
		ptr = parse_arg_defs (++name_end, define);
		if (ptr == NULL)
			return (char *) ptr;
		
		//then find the function it's defining
		if (is_end_of_code_line (ptr)) {
			show_error ("#DEFINE macro is missing function");
			return (char *) ptr;
		}

		value_end = skip_to_line_end (ptr);
		set_define (define, ptr, value_end - ptr, redefined);
		ptr = value_end;
	} else {
		//it's a normal define without arguments
		
		char word[256];
		const char *eval_ptr = ptr = name_end;

		if (is_end_of_code_line (skip_whitespace (ptr))) {
			//if there's no value specified, then set it to 1
			set_define (define, "1", 1, false);
			return (char *) ptr;
		}

		//check for certain special functions
		read_expr (&eval_ptr, word, "(");
		//handle EVAL, evaluate the contents
		if (!strcasecmp (word, "eval")) {
			char expr[256], *new_value;
			if (*eval_ptr == '(')
				eval_ptr++;
			read_expr (&eval_ptr, expr, ")");
			new_value = eval (expr);
			set_define (define, new_value, -1, redefined);
			free (new_value);
			if (*eval_ptr == ')')
				eval_ptr++;
			ptr = eval_ptr;

		//handle CONCAT, concatenate contents
		} else if (!strcasecmp (word, "concat")) {
			
			expand_buf *buffer;
			int value;

			buffer = eb_init(-1);
			if (*eval_ptr == '(')
				eval_ptr++;

			char *arg = NULL;
			arg_context_t context = ARG_CONTEXT_INITIALIZER;
			while ((arg = extract_arg_string(&eval_ptr, &context)) != NULL)
			{
				if (arg[0] == '\"')
				{
					reduce_string (arg);
					eb_insert (buffer, -1, arg, -1);
				}
				else
				{
					define_t *define;

					if ((define = search_defines (arg)) != NULL)
					{
						if (define->contents == NULL)
						{
							show_error("'%s' is not yet fully defined", arg);
						
						}
						else
						{
							char *define_buffer = strdup(define->contents);
							reduce_string(define_buffer);
							eb_insert (buffer, -1, define_buffer, -1);
							free(define_buffer);
						}
					}
					else if (parse_num(arg, &value) == true)
					{
						char num_buf[256];
						sprintf (num_buf, "%d", value);
						eb_insert (buffer, -1, num_buf, -1);
					}
				}
			}
			set_define (define, eb_extract (buffer), -1, redefined);
			eb_free(buffer);
			ptr = eval_ptr;
			
		} else {
			value_end = skip_to_line_end (ptr);
			set_define (define, ptr, value_end - ptr, redefined);
			ptr = value_end;
		}
	}

	return (char *) ptr;
}

/*
 * Given a filename (which may be surrounded in quotes), return
 * an allocated full path of that filename
 * return NULL if there's no matching path
 */

char *full_path (const char *filename) {
	list_t *dir;
	char *full_path;
#ifdef WIN32
	if (is_abs_path(filename) && (GetFileAttributes(filename) != 0xFFFFFFFF))
#else
	if (is_abs_path(filename) && (access (filename, R_OK) == 0))
#endif
		return strdup (filename);
	
	dir = include_dirs;
	full_path = NULL;
	do if (dir) {
		expand_buf_t *eb = eb_init (-1);
		
		eb_append (eb, (char *) dir->data, -1);
		eb_append (eb, "/", 1);
		eb_append (eb, filename, -1);
		free (full_path);
		full_path = eb_extract (eb);
		fix_filename (full_path);
		eb_free (eb);
		dir = dir->next;
#ifdef WIN32
	} while (GetFileAttributes(full_path) == 0xFFFFFFFF && dir);
#else
	} while (access (full_path, R_OK) && dir);
#endif

#ifdef WIN32
	if (GetFileAttributes(full_path) != 0xFFFFFFFF)
#else
	if (access (full_path, R_OK) == 0)
#endif
		return full_path;
	
	free (full_path);
	return NULL;
}

/*
 * Handles #INCLUDE statement, returns pointer to new location
 * in file
 */

static char *handle_preop_include (char *ptr)
{
	char name[MAX_PATH], *file_path;
	FILE *file;
	char *qs, *alloc_path, *input_contents, *old_input_file, *old_line_start;
	
	int old_line_num, old_in_macro, old_old_line_num;

	if (is_end_of_code_line (ptr)) {
		show_error ("#INCLUDE is missing file name");
		return ptr;
	}

	//get the name of the file to include	
	read_expr (&ptr, name, "");
	fix_filename (name);
	
	qs = skip_whitespace (name);
	if (*qs == '"') {
		int i;
		qs++;
		for (i = 0; qs[i] != '"' && qs[i] != '\0'; i++);
		qs[i] = '\0';
	}

	//now see where it is, using include directories
	file_path = full_path (qs);

	//finally, now that we've got the full path, determine file type
	if (!file_path || !(file = fopen (file_path, "rb")))
	{
		SetLastSPASMError(SPASM_ERR_FILE_NOT_FOUND, name);
		show_error ("%s: No such file or directory", name);
		if (file_path) free (file_path);
		return ptr;
	}
	
	if (IsFileBitmap(file))
	{
		handle_bitmap(file);
		fclose (file);
	}
	else
	{
		fclose (file);
		
		input_contents = get_file_contents (file_path);
		if (!input_contents) {
			show_error ("Couldn't open #included file %s", file_path);
			free (file_path);
			return ptr;
		}

		//add it to the list of input files
		alloc_path = strdup (file_path);
		input_files = list_prepend (input_files, alloc_path);
		free (file_path);

		//make sure the listing for this line is finished up BEFORE
		// the new file is parsed and writes its listing stuff all over
		if (mode & MODE_LIST && listing_on && !listing_for_line_done)
			do_listing_for_line (skip_to_next_line (line_start));

		if (mode & MODE_LIST && listing_on) {
			char include_banner[MAX_PATH + 64];
			snprintf(include_banner, sizeof (include_banner), "Listing for file \"%s\"" NEWLINE, fix_filename (alloc_path));
			listing_offset = eb_insert (listing_buf, listing_offset, include_banner, strlen (include_banner));
		}
		
		//swap out the old curr_X values, and swap in the new ones
		old_input_file = curr_input_file;
		old_line_num = line_num;
		old_in_macro = in_macro;
		old_line_start = line_start;
		old_old_line_num = old_line_num;
		curr_input_file = alloc_path;

		//now parse the file
		run_first_pass (input_contents);

		//when done, swap the old curr_X values back in
		curr_input_file = old_input_file;
		line_num = old_line_num;
		in_macro = old_in_macro;
		line_start = old_line_start;
		old_line_num = old_old_line_num;

		if (mode & MODE_LIST && listing_on && !listing_for_line_done)
			listing_for_line_done = true;

		if (mode & MODE_LIST && listing_on) {
			char include_banner[MAX_PATH + 64];
			snprintf(include_banner, sizeof (include_banner), "Listing for file \"%s\"" NEWLINE, curr_input_file);
			listing_offset = eb_insert (listing_buf, listing_offset, include_banner, strlen (include_banner));
		}
		
		//and free up stuff
		//free (input_contents);
		release_file_contents(input_contents);
	}
	return ptr;
}


/*
 * Handles #IMPORT statement,
 * returns pointer to new location
 * in file
 */

char *handle_preop_import (char *ptr) {
	FILE *import_file;
	int c;
	char name[256];

	if (is_end_of_code_line (ptr)) {
		show_error ("#IMPORT is missing file name");
		return ptr;
	}

	//get the name of the file to include	
	read_expr (&ptr, name, "");
	fix_filename (name);
	
	char *qs = skip_whitespace(name);
	if (*qs == '"') {
		int i;
		qs++;
		for (i = 0; qs[i] != '"' && qs[i] != '\0'; i++);
		qs[i] = '\0';
	}

	//now see where it is, using include directories
	char *file_path = full_path(qs);

	//...and read it in
	import_file = fopen(file_path, "rb");
	if (!import_file)
	{
		SetLastSPASMError(SPASM_ERR_FILE_NOT_FOUND, file_path);
		free(file_path);
		return ptr;
	}

	free(file_path);

	//write the contents to the output
	while ((c = fgetc(import_file)) != EOF)
	{
		write_out (c);
		program_counter++;
	}

	fclose (import_file);
	return ptr;
}


/*
 * Handles #IF statement,
 * returns pointer to new
 * location in file
 */

char *handle_preop_if (char *ptr) {
	const char *expr_end;
	char *expr;
	int condition;

	if (is_end_of_code_line (ptr)) {
		SetLastSPASMError(SPASM_ERR_EXPRESSION_EXPECTED);
		return ptr;
	}

	expr_end = skip_to_code_line_end(ptr);

	expr = strndup (ptr, expr_end - ptr);
	
	parse_num (expr, &condition);
	free(expr);
	
	return do_if ((char *) expr_end, condition);
}

/*
 * Handles #IF statement,
 * returns pointer to new
 * location in file
 */

char *handle_preop_elif (char *ptr)
{
	char *expr_end, *expr;
	int condition;

	if (is_end_of_code_line (ptr)) {
		show_fatal_error ("#ELIF is missing condition");
		return ptr;
	}

	expr_end = (char *) skip_to_code_line_end(ptr);
	while (is_end_of_code_line (expr_end))
		expr_end--;
//	expr_end = skip_to_line_end (ptr);
	
	expr = strndup (ptr, expr_end - ptr + 1);
	
	parse_num (expr, &condition);
	free(expr);
	
	//return do_elif(expr_end + 1, condition);
}




/*
 * Skips the appropriate
 * parts of #IF blocks,
 * returns pointer to location
 * in file, needs to be passed
 * whether the condition is
 * true or false
 */

char *do_if (char *ptr, int condition)
{
	// Generate an error if the matching endif isn't found
	char *endif = skip_until (ptr, NULL, 1, "#endif");
	if (condition)
	{
		return ptr;
	}
	else
	{
		char *result = skip_until (ptr, &line_num, 3, "#else", "#elif", "#endif");
		if (line_has_word(result, _T("#else"), 5))
		{
			result = next_code_line(result) - 1;
		}
		else if (line_has_word(result, _T("#elif"), 5))
		{
			result = handle_preop_if(skip_whitespace(result) + 5);
		}
		return result;
	}
}

/*
 * Parses argument definitions
 * for a macro, returns a pointer
 * to the end of the args
 */

char *parse_arg_defs (const char *ptr, define_t *define) {
	char *word;

	define->num_args = 0;
	arg_context_t context = ARG_CONTEXT_INITIALIZER;
	context.fExpectingMore = false;
	while ((word = extract_arg_string(&ptr, &context)) != NULL)
	{
		bool is_dup = false;
		int i;
		
		for (i = 0; i < define->num_args && !is_dup; i++) {
			if (case_sensitive) {
				if (strcmp(word, define->args[i]) == 0)
					is_dup = true;
			} else if (strcasecmp(word, define->args[i]) == 0)
				is_dup = true;
		}
		if (is_dup) {
			show_fatal_error("Duplicate argument name '%s'", strdup (word));
			return NULL;
		}
		define->args[define->num_args++] = strdup (word);
	}
	
	if (*ptr == ')') ptr++;
	return (char *) ptr;
}


char *skip_until (char *ptr, int *pnLine, int argc, ...)
{
	int level = 0;
	va_list argp;

	if (argc == 0) return ptr;

	int line_num_copy = line_num;

	while (*ptr && !error_occurred) {
		char *line = ptr;
		char *line_end = skip_to_next_line(ptr);
		
		do {
			// Test for nesting (includes #ifdef and #ifndef also)
			if (line_has_word (line, "#IF", 3)) {
				level++;
			} else if (level > 0 && line_has_word (line, "#ENDIF", 6)) {
				level--;
			} else if (level == 0)
			{
				int i;
				// Test all of the words that mark the end of the skipping
				va_start(argp, argc);
				i = argc;
				while (i--) {
					char *word = va_arg(argp, char *);
					if (line_has_word (line, word, strlen (word)))
					{
						if (pnLine != NULL)
						{
							*pnLine = line_num_copy;
						}	
						return line;
					}
				}
				va_end(argp);
			}
			
			line = next_code_line(line);
		} while (line < line_end && !error_occurred);
			
		ptr = line_end;
		line_num_copy++;

	}

	// Will be issued for line_num of the start
	SetLastSPASMError(SPASM_ERR_UNMATCHED_IF);
	if (pnLine != NULL)
	{
		*pnLine = line_num_copy;
	}	
	return ptr;
}
