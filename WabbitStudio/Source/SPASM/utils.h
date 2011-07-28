#ifndef __UTILS_H
#define __UTILS_H

#include "storage.h"

#define MAX_ARG_LEN	256
#define ARG_CONTEXT_INITIALIZER {"", true, false}
typedef struct arg_context
{
	char arg[MAX_ARG_LEN];
	bool fExpectingMore;
	bool fLiteralArg;
} arg_context_t;

char *eval (const char *expr);
bool is_end_of_code_line (const char *ptr);
char *skip_to_next_line (const char *ptr);
char *skip_to_name_end (const char *ptr);
char *skip_to_line_end (const char *ptr);
const char *skip_to_code_line_end(const char *ptr);
char *skip_whitespace (const char *ptr);
bool is_arg (char test);
char *next_code_line (char *);
char *next_expr (const char *ptr, const char *delims);
#define read_expr(ptr, word, delims) (read_expr_impl((const char ** const) ptr, word, delims))
bool read_expr_impl(__inout const char ** const ptr, char word[256], const char *delims);
char *extract_arg_string(const char ** const ptr, arg_context_t *context);
char *parse_args (const char *ptr, define_t *define, list_t **arg_local_labels);
char *replace_literal_args (const char *ptr, define_t *define, list_t **curr_arg_set);
bool line_has_word (char *ptr, const char *word, int word_len);
char *escape_string(const char *input);
char *reduce_string (char* input);
char *fix_filename (char *filename);
bool is_abs_path(const char *filename);
char *strup (const char *input);
char *get_file_contents (const char *filename);
void release_file_contents(char *contents);
char *change_extension (const char *filename, const char *new_ext);
bool define_with_value (const char *name, const int value);
#if defined(MACVER) || defined(WIN32)
char *strndup (const char *str, int len);
int strnlen (const char *str, int maxlen);
#ifdef WIN32
#define strdup _strdup
#endif
#endif

char *expand_expr (const char *expr);
//void *malloc_chk (size_t);
#define malloc_chk malloc

extern char *curr_input_file;
extern int line_num;
extern bool suppress_errors;
extern bool error_occurred;

void show_error_prefix (const char *zcif, const int zln);
void show_error (const char *text, ...);
void show_fatal_error (const char *text, ...);
void show_warning_prefix (const char *zcif, const int zln);
void show_warning (const char *text, ...);

#endif

