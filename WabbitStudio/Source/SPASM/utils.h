#ifndef __UTILS_H
#define __UTILS_H

#include <stdlib.h>
#include <stdbool.h>
#include "storage.h"

#ifdef WINVER
#include <windows.h>
#endif

char *eval (char *expr);
bool is_end_of_code_line (char *ptr);
char *skip_to_next_line (char *ptr);
char *skip_to_name_end (char *ptr);
char *skip_to_code_line_end (char *ptr);
char *skip_whitespace (char *ptr);
bool is_arg (char test);
char *next_code_line (char *);
char *next_expr (char* ptr, const char *delims);
bool read_expr (char **input, char word[256], const char *delims);
char *parse_args (char *ptr, define_t *define, list_t **arg_local_labels);
bool line_has_word (char *ptr, const char *word, int word_len);
char *reduce_string (char* input);
char *fix_filename (char *filename);
bool is_abs_path(const char *filename);
char *strup (const char *input);
char *get_file_contents (const char *filename);
char *change_extension (const char *filename, const char *new_ext);
bool define_with_value (const char *name, const int value);
#if defined(MACVER) || defined(WINVER)
char *strndup (const char *str, int len);
int strnlen (const char *str, int maxlen);
#ifdef WINVER
#define strdup _strdup
#endif
#endif

char *expand_expr (char *expr);
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

