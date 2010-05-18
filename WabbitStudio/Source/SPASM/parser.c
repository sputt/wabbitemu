#define __PARSER_C

//max depth of #defines
#define RECURSION_LIMIT 20

#define _GNU_SOURCE
#include "spasm.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "parser.h"
#include "storage.h"
#include "utils.h"
#include "preop.h"

#ifdef WINVER
#define strcasecmp _stricmp
#endif

extern char *curr_input_file;
extern int line_num;
extern bool suppress_errors;
extern bool error_occurred;
extern bool pass_one;
extern unsigned int program_counter;
extern bool suppress_errors;

static char *parse_single_num (char *expr, int *value);
static char *parse_num_full (char *expr, int *value, int depth);
static int conv_dec (const char* str, const char *end);
static int conv_bin (const char* str, const char *end);

//set when the parser hits an undefined #define or label
bool parser_forward_ref_err;
//current recursive depth in expression
int parse_depth;


/*
 * Parses an expression, sets
 * the result in value and
 * returns TRUE if successful,
 * otherwise sets value to 0
 * and returns FALSE
 */

bool parse_num (char *expr, int *value) {
	int fake_value;
	char *result;

	if (value == NULL)
		value = &fake_value;

	parse_depth = 0;
	result = parse_num_full (expr, value, 0);

	if (!result)
		*value = 0;

	return (result != NULL);
}

/*
 * Parses expression,
 * ignoring errors
 */

int parse_f (char *expr) {
	int result;
	suppress_errors = true;
	if (!(parse_num (expr, &result)))
		result = -1;

	suppress_errors = false;
	return result;
}


/*
 * Parses the contents of a define,
 * handling preops, and returns
 * the first line with a value,
 * allocated
 */

char *parse_define (define_t *define) {
	char *ptr = define->contents;

	//go through each line of the macro
	while (ptr) {
		ptr = skip_whitespace (ptr);
		//if it's a blank line, then skip it
		if (is_end_of_code_line (ptr)) {
			ptr = skip_to_next_line (ptr);
			continue;
		} else if (*ptr == '#') {
			//if it's a preop, then handle that
			ptr = skip_to_next_line (handle_preop (++ptr));
		} else {
			//otherwise, it must be a value, so return that
			return strndup (ptr, skip_to_code_line_end (ptr) - ptr);
		}
	}

	return NULL;
}


/*
 * Parses a single number, sets
 * the value, and returns the
 * end of the number
 */

static char *parse_single_num (char *expr, int *value) {

	switch (*expr) {
#ifdef USE_REUSABLES
		case '+':
		//case '_':
		{
			//Local label!
			int this_reusable = get_curr_reusable() + 1;
			if (*expr == '+') expr++;
			
			while (*expr == '+') {
				expr++;
				this_reusable++;
			}

			if (*(expr++) != '_') {
				show_error ("Error in local label's syntax (had leading +/- but no _)");
				return NULL;
			} else if (this_reusable >= get_num_reusables()) {
				if (pass_one == false) show_error ("Forward reference to local label which doesn't exist");
				parser_forward_ref_err = true;
				return NULL;
			}
	
			*value = search_reusables(this_reusable);
			break;

		}
		case '-':
		{
			//Again, a local label!
			int this_reusable = get_curr_reusable();
			if (*expr == '-') expr++;
			
			while (*expr == '-' && this_reusable > 0) {
				expr++;
				this_reusable--;
			}
	
			if (*(expr++) != '_' || this_reusable < 0) {
				//show_error ("Error in local label's syntax");
				return NULL;
			}
	
			*value = search_reusables(this_reusable);
			break;

		}
#endif
		//If the number has a prefix it's easy to tell what the type is!
		case '$':
		{
			char *num_start = ++expr;
			//Could be a hex number (ld hl,$1A2F) or the current address (ld hl,$)
			if (isxdigit ((unsigned char) *expr)) {
				while (isxdigit (*expr))
					expr++;
				*value = conv_hex (num_start, expr);
			} else
				//TODO: This is questionable behavior.  It should probably check specifically for $
				*value = program_counter;
			break;
	
		}
		case '%':
		{
			char *num_start = ++expr;
			while (*expr == '1' || *expr == '0')
				expr++;
			*value = conv_bin (num_start, expr);
			break;

		}
		//Handle literal chars in single quotes
		case '\'':
		{
			*value = *(++expr);
			if (*value == '\\') {
				switch (toupper (*(++expr))) {
					case 'N': 	*value = '\n'; break;
					case '\\': 	*value = '\\'; break;
					case '0': 	*value = '\0'; break;
					case 'R': 	*value = '\r'; break;
					case 'T': 	*value = '\t'; break;
					case '\'': 	*value = '\''; break;
					case '#': 	*value = rand () & 0xFF; break;
					default: 	*value = *expr;
				}
			}

			if (*(++expr) != '\'') {
				show_warning ("Literal char is missing single quote at end");
			} else {
				expr++;
			}

			break;

		}
		//By now, it's either a label, number, or macro
		default:
			if (isalpha ((unsigned char) *expr) || *expr == '_') {
				//If it starts with a letter or underscore, it's a label or a macro that returns a value
				label_t *label;
				define_t *define;
				char *name;

				char *expr_start = expr;
				//Find the end of the name, then try it as a label first
				expr = skip_to_name_end (expr_start);
				name = strndup (expr_start, expr - expr_start);

#ifdef USE_REUSABLES
				//see if it's a local label
				if (!strcmp (name, "_")) {
					free (name);
					if (get_curr_reusable() + 1 < get_num_reusables())
						*value = search_reusables(get_curr_reusable() + 1);
					else
						return NULL;
				} else 
#endif
				if ((label = search_labels (name))) {
					*value = label->value;
					free (name);

						//or the "eval" macro
					} else if (!strcasecmp (name, "eval") && *expr == '(') {
						show_warning ("eval() has no effect except in #define");
						expr = parse_num_full (expr, value, 0);
						//then a normal label
					} else if (!strcasecmp (name, "getc") && *expr == '(') {
						char filename[256];
						FILE *temp_file;
						define_t *define;
						int char_index;
						char parse_buf[256];
						char *rfn;

						expr++;
						read_expr(&expr, filename, ",");

						// Is the filename given a macro?
						if ((define = search_defines (filename)))
							strncpy (filename, define->contents, sizeof (filename));

						read_expr (&expr, parse_buf, ",");
						char_index = parse_f (parse_buf);

						rfn = fix_filename (reduce_string (filename));
						if ((temp_file = fopen (rfn, "r"))) {
							fseek (temp_file, char_index, SEEK_SET);
							*value = fgetc (temp_file);
							fclose (temp_file);
						} else {
							show_error ("Failed to open file '%s' for GETC", filename);
						}

						if (*expr == ')') expr++;
						//If that didn't work, see if it's a macro
					} else if ((define = search_defines (name))) {
						list_t *args = NULL;
						char *contents;

						free (name);
						if (define->num_args > 0) {
							expr = parse_args (expr, define, &args);
							if (!expr)
								return NULL;
						}

						contents = parse_define (define);
						if (contents == NULL) {
							show_error ("Argument '%s' used without value", define->name);
							return NULL;
						}
						if (!parse_num_full (contents, value, 0)) {
							free (contents);
							remove_arg_set (args);
							return NULL;
						}
						free (contents);
						remove_arg_set (args);
					} else {
						show_error ("'%s' isn't a macro or label", name);
						parser_forward_ref_err = true;
						free (name);
						return NULL;
					}

			} else if ((unsigned char) isdigit (expr[0])) {
				const char *expr_start = expr;
				//Find the end of the number
				while (isalnum ((unsigned char) expr[1])) expr++;

				switch (toupper (expr[0])) {
		case 'H':	*value = conv_hex (expr_start, expr++);	break;
		case 'B':	*value = conv_bin (expr_start, expr++); break;
		case 'D':	*value = conv_dec (expr_start, expr++); break;
		default:	*value = conv_dec (expr_start, ++expr); break;
				}

				if (error_occurred) {
					//TODO: WTF???
					error_occurred = false;
					return NULL;
				}

			} else {
				show_error ("Expecting a value, found '%c' instead", *expr);
				return NULL;
			}
			break;
	}

	return expr;
}


/*
 * Skips until the next &&, ||,
 * <, >, ==, !=, <=, or >=, and
 * returns a pointer to it
 */

char *find_next_condition (char *ptr) {
	int depth = 0;
	while (!is_end_of_code_line (ptr)) {
		if (depth == 0 &&
		    (((*ptr == '&' || *ptr == '|') && *(ptr + 1) == *ptr)
		    || *ptr == '=' || *ptr == '!' ||
		    ((*ptr == '<' || *ptr == '>') && *(ptr + 1) != *ptr)))
			break;
		switch (*ptr) {
			case '(': depth++; break;
			case ')':
				depth--;
				if (depth < 0) return ptr;
				break;
			case '\'':
				ptr++;
				if (*ptr == '\\') ptr++;
				if (*ptr) ptr++;
				break;
		}
		ptr++;
	}
	return ptr;
}


/*
 * Evaluates the expression in expr, sets value to result, returns
 * pointer to end of expression (if depth > 0) or NULL on error.
 */

static char *parse_num_full (char *expr, int *value, int depth) {
	int total = 0, last_num;
	char last_op = '\0';
	bool invert_lastnum, neg_lastnum;

	parser_forward_ref_err = false;
	if (++parse_depth > RECURSION_LIMIT) {
		show_fatal_error ("Expression is too deep (only %d levels allowed)", RECURSION_LIMIT);
		return NULL;
	}

	expr = skip_whitespace (expr);
	if (!(*expr)) {
		show_error ("Value expected");
		return NULL;
	}

	//Now loop through the whole expression
	for (;;) {

		//First look for numbers
		invert_lastnum = neg_lastnum = false;

		//Catch any invert symbols before the number
		if (*expr == '~') {
			invert_lastnum = true;
			expr++;

		//Same with - signs
		} else if (*expr == '-') {
			expr++;
			if (*expr == '-' || (*expr == '_' && skip_to_name_end (expr) == expr + 1))
				//It's a local label, so let the number parsing stuff deal with it
				expr--;
			else
				//Otherwise, it's something like "-3" or "-value"
				neg_lastnum = true;
		}

		//If it's a parenthesis, evaluate the stuff inside there
		if (*expr == '(') {
			expr = parse_num_full (++expr, &last_num, depth + 1);
			//If there was an error in that expression, then abort
			if (!expr)
				return NULL;
		//Otherwise it's just a normal value
		} else {
			expr = parse_single_num (expr, &last_num);
			if (!expr)
				return NULL;
		}

		if (invert_lastnum)
			last_num = ~last_num;
		if (neg_lastnum)
			last_num = -last_num;

		//Now check on the last operator to see what to do with this number
		switch (last_op) {
			case '\0':
				//Special case for the first number in the expression
				total = last_num; break;
			case '+': total += last_num; break;
			case '-': total -= last_num; break;
			case '*': total *= last_num; break;
			case '/': total /= last_num; break;
			case '%': total %= last_num; break;
			case '^': total ^= last_num; break;
			case '&': total &= last_num; break;
			case '|': total |= last_num; break;
			case '<': total <<= last_num; break;
			case '>': total >>= last_num; break;
		}

		//Get the next operator
	get_op:
		expr = skip_whitespace (expr);
		//If it's the end of the expression, return the total so far
		if (is_end_of_code_line (expr)) {
			if (depth > 0)
				show_warning ("Missing %d parentheses at end of expression", depth);
			*value = total;
			parse_depth--;
			return expr;
		}

		if ((*expr == '&' || *expr == '|') && *(expr + 1) == *expr ) {
			char *next_expr;
			int next_val;

			last_op = *expr;
			expr += 2;
			
			next_expr = strdup(expr);

			if (!parse_num_full (next_expr, &next_val, depth)) {
				free (next_expr);
				return NULL;
			}
			free (next_expr);

			switch (last_op) {
				case '&': total = (total && next_val); break;
				case '|': total = (total || next_val); break;
			}

			*value = total;
			parse_depth--;
			return expr + strlen (expr);
		} else if (*expr == '+' || *expr == '-' || *expr == '*' || *expr == '/' ||
		    *expr == '&' || *expr == '|' || *expr == '^' || *expr == '%')
			//Handle single-char operators
			last_op = *(expr++);
		else if ((*expr == '>' || *expr == '<') && *(expr + 1) == *expr) {
			//Two-char bit shift ops
			last_op = *expr;
			expr += 2;
		} else if (*expr == '=' || *expr == '<' || *expr == '>' || (*expr == '!' && *(expr + 1) == '=')) {
			char *expr_end, *next_expr;
			int next_val;
			// Special operators for conditions
			last_op = *expr++;
			if (*expr == '=' && *(expr - 1) == '=')
				expr++;

			expr_end = find_next_condition (expr + 1);
			if (!(is_end_of_code_line (expr_end) || *expr_end == '&' || *expr_end == '|' || *expr_end == ')')) {
				show_error ("Invalid condition test, expecting || or && '%s'", expr_end);
				return NULL;
			}

			next_expr = strndup (expr + (*expr == '='), expr_end - expr - (*expr == '='));
			if (!parse_num_full (next_expr, &next_val, 0)) {
				free (next_expr);
				return NULL;
			}
			free (next_expr);

			if (*expr == '=')
				last_op += ('A' - '<');
			
			switch (last_op) {
				case '=': total = (total == next_val); break;
				case '!' + ('A' - '<'): total = (total != next_val); break;
				case '<': total = (total < next_val); break;
				case '>': total = (total > next_val); break;
				case '<' + ('A' - '<'): total = (total <= next_val); break;
				case '>' + ('A' - '<'): total = (total >= next_val); break;
			}

			expr = expr_end;
			goto get_op;
		} else if (*expr == ')') {
			if (depth > 0) {
				// We've been parsing an expression in parentheses and reached the end,
				// so return the result and go up a level again
				*value = total;
				expr = skip_whitespace (expr + 1);
				parse_depth--;
				return expr;
			} else {
				//Otherwise it's just an extra parenthesis
				show_warning ("Extra parenthesis found, ignoring it");
				expr++;
				goto get_op;
			}

		} else {
			show_error ("Expecting an operator, found '%c' instead", *expr);
			return NULL;
		}

		//Skip any whitespace after the operator
		expr = skip_whitespace (expr);
		if (*expr == '\0') {
			show_warning ("Stray operator '%c' at end", last_op);
			if (depth > 0)
				show_warning ("Missing %d parentheses at end of expression", depth);
			*value = total;
			parse_depth--;
			return expr;
		}
	}
}


/*
 * Evaluates a hexadecimal string
 */

int conv_hex (const char* str, const char *end) {
    int acc = 0;

    while (str < end) {
		char hexchar = toupper (*str);
		
		if (!isxdigit (*str)) {
			show_error ("Invalid hex digit '%c'", *str);
			error_occurred = true;
			return 0;
		}
		
        acc <<= 4;
        if (hexchar > '9') acc+= hexchar - ('A' - 10);    
        else acc+= hexchar - '0';
        str++;
    }
    return acc;
}


/*
 * Evaluates a decimal string
 */

static int conv_dec (const char* str, const char *end) {
    int acc = 0;
    
    while (str < end) {
        acc *= 10;
        
        if (!isdigit ((unsigned char) *str)) {
			show_error ("Invalid decimal digit '%c'", *str);
			error_occurred = true;
			return 0;
        }
        
        acc += *str-'0';
        str++;
    }
    return acc;
}


/*
 * Evaluates a binary string
 */

static int conv_bin (const char* str, const char *end) {
    int acc = 0;
    
    while (str < end) {
        acc <<= 1;
        
        if (!(*str == '0' || *str == '1')) {
        	show_error ("Invalid binary digit '%c'", *str);
        	error_occurred = true;
        	return 0;
        }
        
        if (*str == '1') acc++;
        str++;
    }
    return acc;
}

