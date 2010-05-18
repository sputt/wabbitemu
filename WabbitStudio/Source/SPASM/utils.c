#define _GNU_SOURCE

//max depth of #defines
#define RECURSION_LIMIT 20

#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include <stdlib.h>

#include <stdbool.h>
#include "storage.h"
#include "parser.h"
#include "utils.h"
#include "expand_buf.h"
#include "console.h"
#include "spasm.h"

/*
 * Returns true if ptr has reached what is the end of an assembly
 * line of code.
 * For example: comment, line break (\), or new line
 */
bool is_end_of_code_line (char *ptr) {
	if (ptr == NULL)
		return NULL;

	return (*ptr == '\0' || *ptr == '\n' || *ptr == '\r' || *ptr == ';' || *ptr == '\\');
}

/*
 * Skips to the next physical line in the file
 */
char *skip_to_next_line (char *ptr) {
	if (ptr == NULL)
		return NULL;

	while (*ptr && *ptr != '\n' && *ptr != '\r')
		ptr++;

	if (*ptr == '\r')
		ptr++;
	if (*ptr == '\n')
		ptr++;

	return ptr;
}

/*
 * Skips to the end of a value spasm2 label name
 */
char *skip_to_name_end (char *ptr) {
	const char ext_label_set[] = "_[]!?.";
	
	if (ptr == NULL)
		return NULL;

	while (*ptr != '\0' && (isalnum((unsigned char) *ptr) || strchr(ext_label_set, *ptr) != NULL))
		ptr++;

	return ptr;
}

char *skip_to_code_line_end (char *ptr) {
	bool in_escape = false, in_quotes = false;

	if (ptr == NULL)
		return NULL;

	while (*ptr && *ptr != '\n' && *ptr != '\r' && (*ptr != ';' || in_quotes)) {
		if (*ptr == '\\') in_escape = true;
		else if (*ptr == '\"' && !in_escape) in_quotes = !in_quotes;
		else in_escape = false;

		ptr++;
	}

	return ptr;
}

char *skip_whitespace (char *ptr) {
	if (ptr == NULL)
		return NULL;

	while (isspace ((unsigned char) *ptr) && *ptr != '\n' && *ptr != '\r')
		ptr++;

	return ptr;
}

bool is_arg (char test) {
	return (test == '*' || test == '&' || test == '%' ||
	        test == '@' || test == '^');
}


char *next_expr (char* ptr, const char *delims) {
    bool in_string = false, // "xxx xxx xx"
	     in_escape = false;	// \n
	int in_quote = 0;		// 'x'

	/* Is there a word left to get? */
	// Skip whitespace at the start
	ptr = skip_whitespace (ptr);
	if (is_end_of_code_line (ptr) || *ptr == ')') 
		return ptr;

    while (*ptr != '\0' && !((strpbrk (ptr, delims) == ptr || is_end_of_code_line (ptr))
				&& !in_escape && !in_string && in_quote == 0)) {

		if (!in_escape) {
			switch( *ptr ) {
				case '"': 	
					if (in_quote == 0) in_string = !in_string; 
					break;
				case '\'':
					if (!in_string && in_quote == 0) in_quote = 3; 
					break;		
	        	case '\\': 	
					in_escape = true;
					break;
				default:
					/* 	If this char wasn't ', redo the inside */
					if (in_quote == 1) {
						ptr -= 2;
					}
			}	
			if (in_quote) in_quote--;
		} else {
			in_escape = false;
		}
		ptr++;
    }

	return ptr;
}

char *next_code_line(char *ptr) {
	
	ptr = next_expr(ptr, "\\;\r\n");
	
	if (*ptr == ';')
		return skip_to_next_line(ptr);
	
	if (*ptr == '\\')
		ptr++;
	if (*ptr == '\r')
		ptr++;
	if (*ptr == '\n')
		ptr++;
	
	return ptr;
}


/*
 * Parses an expression and returns
 * the result as a string if possible,
 * otherwise expands the expression
 */

char *eval (char *expr) {
	char result[256], *expr_value;
	bool suppress_backup = suppress_errors;
	int value;

	suppress_errors = true;
	if (parse_num (expr, &value)) {
		sprintf (result, "$%0X", value);
		expr_value = strdup (result);
	} else {
		expr_value = expand_expr (expr);
	}
	suppress_errors = suppress_backup;
	return expr_value;
}


/*
 * Parses the arguments for a macro,
 * returns a pointer to the end of the
 * arguments, or NULL on errors, sets
 * curr_arg_set to the node
 * holding the values of the args
 */

char *parse_args (char *ptr, define_t *define, list_t **curr_arg_set) {
	int num_args = 0;
	char word[256];

	if (curr_arg_set == NULL)
		return NULL;

	if (*ptr != '(' && define->num_args == 0)
		return ptr;

	*curr_arg_set = add_arg_set();

	//then parse each argument and store its value
	if (*ptr == '(') {
		ptr++;
		
		while (read_expr (&ptr, word, ",")) {
			if (num_args >= define->num_args) {
				show_warning ("Macro '%s' was given too many arguments, ignoring extras", define->name);
				break;
			}
			add_arg(strdup(define->args[num_args++]), eval(word), *curr_arg_set);
		}
	}

	//fill in the undefined arguments
	while (num_args < define->num_args)
		add_arg (strdup (define->args[num_args++]), NULL, *curr_arg_set);

	//make sure that the number of args came out right
/*	if (num_args < define->num_args) {
		show_error ("Macro has too few arguments");
		remove_arg_set (*curr_arg_set);
		*curr_arg_set = NULL;
		return NULL;
	}*/

	if (*ptr == ')') ptr++;
	return ptr;
}


/*
 * Returns TRUE if a
 * line contains a word,
 * ignoring whitespace at
 * the beginning
 */

bool line_has_word (char *ptr, const char *word, int word_len) {

	ptr = skip_whitespace(ptr);
	//make sure the file doesn't end before the end of the word + required whitespace,
	// then see if the word's found on the line
	if (strnlen (ptr, word_len) == word_len && !strncasecmp (ptr, word, word_len)) {
		//then make sure it isn't just the start of another word
		ptr += word_len;
		//if (isspace (*ptr) || is_end_of_code_line (ptr))
			return true;
	}
	return false;
}

/*
 * Slow function to simplify coding of macros and directives with argument lists
 * .db blank, blank, macro(blank,blank), blank
 * macro( macro( blank, blank), blank)
 *
 * Returns 
 *		ptr to delimiter if terminated by reaching delimiter
 *		NULL if input was not delimiter terminated 
 */

bool read_expr (char** ptr, char word[256], const char *delims) {
    bool in_string = false, // "xxx xxx xx"
	     in_escape = false;	// \n
	int in_quote = 0;		// 'x'
	int level = 0;
	char *word_ptr = word;

	/* Is there a word left to get? */
	// Skip whitespace at the start
	*ptr = skip_whitespace (*ptr);
	if (is_end_of_code_line (*ptr) || **ptr == ')') {
		if (word)
			*word = '\0';
		return false;
	}

    while (**ptr != '\0' && !((strpbrk (*ptr, delims) == *ptr || is_end_of_code_line (*ptr))
				&& !in_escape && !in_string && in_quote == 0 && level == 0)) {

		if (!in_escape) {
			switch( **ptr ) {
				case '"': 	
					if (in_quote == 0) in_string = !in_string; 
					break;
				case '\'':
					if (!in_string && in_quote == 0) in_quote = 3; 
					break;		
	        	case '\\': 	
					in_escape = true;
					break;
				case '(':
					if (!in_string && in_quote == 0) level++;
					break;
				case ')':
					if (!in_string && in_quote == 0) level--;
					if (level < 0) {
						(*ptr)--;
						goto finish_read_expr;
					}
					break;
				default:
					/* 	If this char wasn't ', redo the inside */
					if (in_quote == 1) {
						*ptr -= 2;
						word_ptr -= 2;
					}
			}	
			if (in_quote) in_quote--;
		} else {
			in_escape = false;
		}
		if (word_ptr - word >= 254) {
			show_fatal_error ("Expression is too long - must be 255 chars or less");
			if (word)
				strcpy (word, "0");
			return true;
		}
		if (word)
			*word_ptr++ = **ptr;
		(*ptr)++;
    }
    
finish_read_expr:
    // Remove whitespace at the end
	if (word) {
	    while (word_ptr > word && isspace((unsigned char) word_ptr[-1])) 
	    	*(--word_ptr) = '\0';
		*word_ptr = '\0';
	}

    /* input is either the delimiter or null */
    if (is_end_of_code_line (*ptr)) return true;
    (*ptr)++;
	return true;
}
 
       
/*
 * Removes surrounding quotation marks (if necessary)
 * and reduces control characters
 */

char* reduce_string (char* input) {
    char *output = input;
    int i = 0;
    if (input[0] == '"') i = 1;
    
    for (; input[i] != '\0' && input[i] != '"'; i++, output++) {
        if (input[i] == '\\') {
			int value;
            char pbuf[8] = "'\\ '";
            pbuf[2] = input[++i];
			parse_num (pbuf, &value);
            *output = value;
        } else {
            *output = input[i];
        }
    }
    *output = '\0';
    return input;
}


/*
 * Replaces backslashes in filename
 * with forward slashes to work across
 * different platforms
 */

char *fix_filename (char *filename) {
	char *sl = strchr (filename, WRONG_PATH_SEPARATOR);
	while (sl != NULL) {
		sl[0] = PATH_SEPARATOR;
		sl = strchr (sl + 1, WRONG_PATH_SEPARATOR);
	}

	return filename;
}


/*
 * Checks to see if the given path is a full path or relative path
 */
bool is_abs_path(const char *filename) {
	
	if (filename == NULL)
		return false;

	char *fn = skip_whitespace((char *) filename);
	
	if (fn[0] == '/' || fn[0] == '\\')
		return true;
	
	if (strlen(fn) < 2)
		return false;
	
	if (fn[1] == ':' && isalpha(fn[0]))
		return true;
	
	return false;
}

/*
 * Returns an allocated copy
 * of the string converted to
 * uppercase
 */

char *strup (const char *input) {	
	char *new_string = (char *) malloc_chk (strlen (input) + 1);
	int i;
	for (i = 0; input[i] != '\0'; i++)
		new_string[i] = toupper (input[i]);

	new_string[i] = '\0';
	return new_string;
}


/*
 * Gets file contents,
 * sets size and
 * returns address of
 * allocated contents
 * or NULL on error
 */

char *get_file_contents (const char *filename) {
	FILE *file;
	char *buffer, *p;
	size_t size, read_size;

	// first try to open it
	file = fopen (filename, "rb");

	if (!file)
		return NULL;

	// find file size (Spencer says it's not a hack)
	fseek (file, 0, SEEK_END);
	size = ftell (file);
	rewind (file);


	// now allocate the memory and read in the contents
	buffer = (char *) malloc_chk (size + 1);

	const unsigned char utf8_endian_mark[] = {0xEF, 0xBB, 0xBF};
	fread(buffer, 1, sizeof(utf8_endian_mark), file);
	if (memcmp(buffer, utf8_endian_mark, sizeof(utf8_endian_mark)) != 0) {
		size -= sizeof(utf8_endian_mark);
		p = buffer + sizeof(utf8_endian_mark);
	} else {
		p = buffer;
	}

	read_size = fread (p, 1, size, file);
	fclose (file);

	if (read_size != size) {
		free (buffer);
		return NULL;
	}

	p[size] = '\0';
	return buffer;
}


/* 
 * check defined named "name" to see if
 * it has the value "value".  true
 * if it does
 */
bool define_with_value (const char *name, const int value) {
	define_t *define = search_defines (name);
	if (define == NULL) return false;
	
	return (parse_f (define->contents) == value);
}


/*
 * Changes the filename extension of a file to the given ext.  If 
 * the file does not have an extension, a new one is added on
 */

char *change_extension (const char *filename, const char *ext) {
	char *dot;
	char *new_fn = (char *) malloc_chk(strlen(filename) + strlen(ext) + 2);
	strcpy(new_fn, filename);
	
	dot = strrchr(new_fn, '.');
	if (dot != NULL) *dot = '\0';
		
	strcat(new_fn, ".");
	return strcat(new_fn, ext);
}

#if defined(WINVER) || defined(MACVER)
char *strndup (const char *str, int len) {
	char *dupstr;

	dupstr = (char *) malloc_chk(len + 1);
	if (dupstr) {
		strncpy (dupstr, str, len);
		dupstr[len] = '\0';
	}
	return dupstr;
}

int strnlen (const char *str, int maxlen) {
	int len = 0;

	while (*(str + len) && len < maxlen)
		len++;

	return len;
}
#endif

/*
 * Fully expands the text of an expression by
 * replacing all macros and labels with their
 * text replacements
 */

void expand_expr_full (char *expr, expand_buf *new_expr, int depth) {
	char *block_start, *name;
	static char separators[] = "=+-*<>|&/%^()\\, \t\r";
	define_t *define;
	label_t *label;
	int name_len;

	if (depth > RECURSION_LIMIT) {
		show_fatal_error ("Expression is too deep (only %d levels allowed)", RECURSION_LIMIT);
		return;
	}

	while (*expr) {
		//handle strings
		if (*expr == '\"') {
			block_start = expr++;
			while (*expr && *expr != '\"') {
				if (!(*expr == '\\' && *(++expr) == '\0'))
					expr++;
			}
			if (*expr)
				expr++;

			eb_append (new_expr, block_start, expr - block_start);
		}

		//get the next name and look it up
		name = skip_to_name_end (expr);
		name_len = name - expr;
		//skip if there's no name, or if it's a macro with arguments
		if (name_len > 0) {
			name = strndup (expr, name_len);
			
			//if it's a define, recursively expand its contents
			if ((define = search_defines (name)) && define->contents != NULL) {
				list_t *args = NULL;

				if (define->num_args > 0) {
					expr = parse_args (expr + name_len, define, &args);
					if (!expr) {
						free (name);
						return;
					}
				} else {
					expr += name_len;
				}
				expand_expr_full (define->contents, new_expr, depth + 1);
				remove_arg_set (args);
				if (error_occurred) return;
			//if it's a label, write its value
			} else if ((label = search_labels (name))) {
				char buf[10];
				snprintf (buf, sizeof(buf), "%d", label->value);
				eb_append (new_expr, buf, -1);
				expr += name_len;
			//otherwise, it might be a forward ref, so just write the name
			} else {
				eb_append (new_expr, name, name_len);
				expr += name_len;
			}
			
			free (name);
		} else if (*expr) {
			//if it can't be a name, just write the next char and move on
			eb_append (new_expr, expr, 1);
			expr++;
		}

		//skip past separators
		block_start = expr;
		while (*expr && strchr (separators, *expr))
			expr++;

		//write those
		eb_append (new_expr, block_start, expr - block_start);
	}
}


char *expand_expr (char *expr) {
	expand_buf *new_expr;
	char *new_expr_text;

	new_expr = eb_init(strlen(expr) * 2);

	expand_expr_full (expr, new_expr, 0);
	new_expr_text = eb_extract (new_expr);
	eb_free (new_expr);

	return new_expr_text;
}


/*
 * Does a checked memory allocation,
 * shows an error and exits if out
 * of memory
 */

/*void *malloc_chk (size_t size) {
	void *ret = malloc (size);
	if (ret != NULL)
		return ret;

	puts ("Out of memory, exiting");
	exit (EXIT_FATAL_ERROR);
}*/

void show_error_prefix(const char *zcif, const int zln) {
#ifdef _WIN32
	//TCHAR szPrefix[256];
	//sprintf(szPrefix, "%s:%d: error: ", zcif, zln);
	//OutputDebugString(szPrefix);
#endif
    set_console_attributes (COLOR_RED);
	printf ("%s:%d: error: ", zcif, zln);
}

void show_error(const char *text, ...) {
	if (!suppress_errors) {
		va_list args;
		if (exit_code < EXIT_ERRORS) exit_code = EXIT_ERRORS;

		show_error_prefix(curr_input_file, line_num);

		va_start(args, text);
		
		vprintf (text, args); 
		putchar ('\n');

		//char buffer[256];
		//vsprintf(buffer, text, args);
		//OutputDebugString(buffer);
		//OutputDebugString(TEXT("\n"));
	}
}

void show_fatal_error(const char *text, ...) {
	if (!suppress_errors) {
		va_list args;
		if (exit_code < EXIT_FATAL_ERROR) exit_code = EXIT_FATAL_ERROR;

		show_error_prefix(curr_input_file, line_num);
		OutputDebugString(text);
		OutputDebugString(TEXT("\n"));

		va_start(args, text);

		vprintf (text, args);
		putchar ('\n');
		error_occurred = true;
	}
}

void show_warning_prefix(const char *zcif, int zln) {
    set_console_attributes (COLOR_YELLOW);
	printf ("%s:%d: warning: ", zcif, zln);
}

void show_warning(const char *text, ...) {
	if (!suppress_errors) {
		va_list args;
		if (exit_code < EXIT_WARNINGS) exit_code = EXIT_WARNINGS;

		show_warning_prefix(curr_input_file, line_num);
		OutputDebugString(text);
		OutputDebugString(TEXT("\n"));

		va_start(args, text);

		vprintf (text, args);
		putchar ('\n');
	}
}