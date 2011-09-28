#include "stdafx.h"

#include "spasm.h"
#include "utils.h"
#include "storage.h"
#include "parser.h"
#include "pass_two.h"
#include "opcodes.h"
#include "directive.h"
#include "preop.h"
#include "list.h"
#include "errors.h"

#ifdef USE_BUILTIN_FCREATE
#include "expand_buf.h"
#include "directive.h"
#include "pass_one.h"
#define MAX_BUFFERS 10
static expand_buf_t* fcreate_bufs[MAX_BUFFERS];
#endif

char *run_first_pass_line (char *ptr);
char *run_first_pass_line_sec (char *ptr);
char *handle_opcode_or_macro (char *ptr);
char *match_opcode_args (char *ptr, char **arg_ptrs, char **arg_end_ptrs, opcode *curr_opcode, instr **curr_instr);
void write_instruction_data (instr *curr_instr, char **arg_ptrs, char **arg_end_ptrs);

int listing_width;


/*
 * Writes val to the output
 */

int write_out (int val) {

	//write hex data if listing mode is on
	if (mode & MODE_LIST && listing_on) {
		char text[4];

		if (pass_one && listing_width >= 4) {
			listing_width = 0;
			listing_offset = eb_insert (listing_buf, listing_offset, NEWLINE "              ", -1);
		} else if (!pass_one) {
			/* If it's on the second pass, a word might
			   span two lines, so check if we need to skip
			   to the beginning of the next line */
			char buf_char = eb_get_char (listing_buf, listing_offset);
			if (buf_char == '\n' || buf_char == '\r') {
				while (isspace ((unsigned char) eb_get_char (listing_buf, listing_offset)))
					listing_offset++;
			}
		}

		sprintf (text, "%02X ", val & 0xFF);
		if (pass_one)
			listing_offset = eb_insert (listing_buf, listing_offset, text, 3);
		else {
			eb_overwrite (listing_buf, listing_offset, text, 3);
			listing_offset += 3;
		}
		listing_width++;
	}

	//if (mode & MODE_NORMAL)
	*(out_ptr++) = val;

	return val;
}


/*
 * Writes listing for a single line, not
 * including hex data (handled by write_out)
 * or line number / program counter (handled
 * in run_first_pass)
 */

void do_listing_for_line (char *ptr) {
	char *curr_line, text[32];

	//make sure empty bytes are filled
	while (listing_width < 4) {
		listing_offset = eb_insert (listing_buf, listing_offset, "-  ", 3);
		listing_width++;
	}

	//and finally, write the contents of the line
	//TODO: clean up?
	curr_line = skip_to_next_line (line_start);
	while (curr_line < ptr) {
		listing_offset = eb_insert (listing_buf, listing_offset, line_start, curr_line - line_start);
		line_start = curr_line;
		curr_line = skip_to_next_line (curr_line);

		old_line_num++;
		sprintf (text, "%5d %02X:%04X             ", old_line_num, (program_counter >> 16), program_counter & 0xFFFF);
		listing_offset = eb_insert (listing_buf, listing_offset, text, strlen (text));
	}
	listing_offset = eb_insert (listing_buf, listing_offset, line_start, ptr - line_start);
	//make sure there's a newline char at the end
	if (*(ptr - 1) != '\n') {
		listing_offset = eb_insert (listing_buf, listing_offset, NEWLINE, -1);
	}
}


/*
 * Goes through the first
 * pass on an mmapped file
 */

void run_first_pass (char *ptr) {
	line_num = 1;
	pass_one = true;

	while (!error_occurred && *ptr != '\0') {

		if (mode & MODE_LIST) {
			//init some listing stuff
			listing_width = 0;
			line_start = ptr;
			old_line_num = line_num;
			listing_for_line_done = false;

			if (listing_on) {
				char text[32];

				//add the line number and program counter at the beginning of the line
				sprintf (text, "%5d %02X:%04X ", line_num, (program_counter >> 16), program_counter & 0xFFFF);
				listing_offset = eb_insert (listing_buf, listing_offset, text, strlen (text));
			}
		}

		ptr = run_first_pass_line (ptr);
		if (ptr != NULL) {
			//evaluate the line
			ptr = skip_to_next_line (ptr);
	
			//do listing stuff for this line
			if (mode & MODE_LIST && listing_on && !listing_for_line_done)
				do_listing_for_line (ptr);
	
			line_num++;
		}
	}
}


/*
 * Parses a single line,
 * returns a pointer to the
 * end of the line
 */

char *run_first_pass_line (char *ptr) {
	do {
		if (*ptr == '\\')
			ptr++;
		
		//char *code_end = skip_to_line_end(ptr);
		//printf("line %d: %s\n", line_num, strndup(ptr, code_end - ptr));
		//parse what's on the line
		ptr = run_first_pass_line_sec (ptr);
		if (ptr == NULL)
			return NULL;
		ptr = skip_whitespace (ptr);

		//when that's done, skip any whitespace and see if more is stuck on the end with a backslash -
		// if so, keep looping until this line's done
	} while (!error_occurred && *ptr == '\\');

	return ptr;
}


/*
 * Parses a single command/section
 * on a line, returns a pointer to
 * the end of the usable part
 */

char *run_first_pass_line_sec (char *ptr) {

	if (is_end_of_code_line (ptr)) {
		//if the line's blank or a comment, don't do anything with it
		return ptr;
	} else if (isalpha ((unsigned char) *ptr) || *ptr == '_') {
		//if the first char on the line is a number, letter, or underscore, it must be a label
		char *name, *label_end;

		//find the end of the label
		label_end = skip_to_name_end (ptr);
		if (*label_end == '(')
			return handle_opcode_or_macro (ptr);

		name = strndup (ptr, label_end - ptr);

		ptr = label_end;
		
		if (*ptr == ':') ptr++;
		
#ifdef USE_REUSABLES
		if (strcmp (name, "_") == 0) {
			add_reusable();
			free(name);
#else
		if (0) {
#endif
		} else {
			//if there's a ':', either the line ends or there's an instruction after it
			last_label = add_label (name, program_counter);
		}

		return run_first_pass_line_sec (ptr);
	} else if (isspace ((unsigned char) *ptr)) {
		//if it starts with whitespace, skip that
		ptr = skip_whitespace (ptr);

		if (isalpha (*ptr) || *ptr == '_')
			//otherwise, it might be an instruction or macro
			return handle_opcode_or_macro (ptr);
		
		return run_first_pass_line_sec (ptr);		
	} else if (*ptr == '.') {
		//handle it if it's a directive
		return handle_directive (++ptr);
	} else if (*ptr == '#') {
		//or pre-op
		return handle_preop (++ptr);
	} else if (*ptr == '=') {
		//handle 'x = 1234' type lines
		int value;
		char value_str[256];

		if (last_label == NULL)
		{
			SetLastSPASMError(SPASM_ERR_EQUATE_MISSING_LABEL);
		}
		ptr++;
		read_expr (&ptr, value_str, "");

		if (parse_num (value_str, &value) == true)
		{
			last_label->value = value;
		}
		return ptr;

	} else {
		SetLastSPASMError(SPASM_ERR_SYNTAX);
		return ptr;
	}
}


static define_t *search_defines_helper(const char *name_start, size_t len)
{
	char *name = strndup(name_start, len);
	define_t *result = search_defines(name);
	free(name);
	return result;
}

/*
 * Handles an opcode or macro,
 * returns the new location in
 * the file
 */

char *handle_opcode_or_macro (char *ptr) {
	char *name_end;
	char *name_start = ptr;
	opcode *curr_opcode;

	//first get the name
	name_end = skip_to_name_end (ptr);
	ptr = name_end;

	//try to match it against opcodes first
	curr_opcode = all_opcodes;
	while (curr_opcode && curr_opcode->name) {
		if (!strncasecmp (name_start, curr_opcode->name, max(strlen(curr_opcode->name), name_end - name_start)))
			break;
		curr_opcode = curr_opcode->next;
	}

	

	//if it was found, then find the right instruction
	if (curr_opcode && curr_opcode->name) {
		char *arg_ptrs[3], *arg_end_ptrs[3];
		instr *curr_instr = NULL;

		//go to the start of the arguments
		ptr = skip_whitespace (ptr);

		//try to match them to one of the opcode's sets of arguments
		ptr = match_opcode_args (ptr, arg_ptrs, arg_end_ptrs, curr_opcode, &curr_instr);
		if (curr_instr != NULL)
		{
			//if that worked, write data + args
			write_instruction_data (curr_instr, arg_ptrs, arg_end_ptrs);

			//increment program counter and stats
			program_counter += curr_instr->size;
			stats_codesize += curr_instr->size;
			stats_mintime += curr_instr->min_exectime;
			stats_maxtime += curr_instr->max_exectime;
		}

	} else {
		//if this name doesn't match an opcode, it must be a macro (#macro OR #define)
		define_t *define;
#ifdef USE_BUILTIN_FCREATE
		if (!strncasecmp (name_start, "buf", name_end - name_start) && *ptr == '(') {
		    char buf[256];
			int value;
			cur_buf = read_expr(ptr, buf, _T(")"));
			if (parse_num(buf, &value)) {
				cur_buf = value;
			} else {
				SetLastSPASMError(SPASM_ERR_INVALID_OPERANDS);
			}
			ptr += 2;
		} else if (!strncasecmp (name_start, "clr", name_end - name_start) && *ptr == '(') {
			expand_buf_t *eb_fcreate = fcreate_bufs[cur_buf];
			if (eb_fcreate != NULL) {
				eb_free(eb_fcreate);
			}
			eb_fcreate = eb_init(128);
			fcreate_bufs[cur_buf] = eb_fcreate;
			ptr += 2;
		} else if (!strncasecmp (name_start, "wr", name_end - name_start) && *ptr == '(') {
			expand_buf_t *eb_fcreate = fcreate_bufs[cur_buf];
			if (eb_fcreate == NULL) {
				eb_fcreate = eb_init(128);
				fcreate_bufs[cur_buf] = eb_fcreate;
			}

			ptr = parse_emit_string(++ptr, ES_FCREATE, eb_fcreate);
			ptr++;
			eb_append(eb_fcreate, NEWLINE, strlen(NEWLINE));
		} else if (!strncasecmp (name_start, "run", name_end - name_start) && *ptr == '(') {
			expand_buf_t *eb_fcreate = fcreate_bufs[cur_buf];
			if (eb_fcreate == NULL)
			{
				SetLastSPASMError(SPASM_ERR_FCREATE_NOFILE);
			}
			else
			{
				char *fcreate_string = eb_extract(eb_fcreate);

				//make sure the listing for this line is finished up BEFORE
				// the new file is parsed and writes its listing stuff all over
				if (mode & MODE_LIST && listing_on && !listing_for_line_done)
					do_listing_for_line (skip_to_next_line (line_start));

				if (mode & MODE_LIST && listing_on) {
					char include_banner[128] = "Listing for built-in fcreate" NEWLINE;
					listing_offset = eb_insert (listing_buf, listing_offset, include_banner, strlen (include_banner));
				}

				// Thanks for this Don
				int old_line_num = line_num;
				char *old_input_file = curr_input_file;
				int old_in_macro = in_macro;
				char *old_line_start = line_start;
				int old_old_line_num = old_line_num;
				curr_input_file = "Built-in fcreate";

				int session = StartSPASMErrorSession();

				run_first_pass(fcreate_string);

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

				free(fcreate_string);
				eb_free(fcreate_bufs[cur_buf]);
				fcreate_bufs[cur_buf] = NULL;

				if (GetSPASMErrorSessionErrorCount(session) > 0)
				{
					AddSPASMErrorSessionAnnotation(session, "Error during invocation of fcreate");
					ReplaySPASMErrorSession(session);
				}
				EndSPASMErrorSession(session);
			}
			ptr += 2;
		} else
#endif

		if ((define = search_defines_helper(name_start, name_end - name_start)))
		{
			list_t *args = NULL;
			char *args_end;

			//if there are arguments, parse them
			int args_session = StartSPASMErrorSession();
			args_end = parse_args(ptr, define, &args);
			
			if (GetSPASMErrorSessionErrorCount(args_session) > 0)
			{
				ReplaySPASMErrorSession(args_session);
				EndSPASMErrorSession(args_session);
				return args_end;
			}
			else
			{
				EndSPASMErrorSession(args_session);
			}

			ptr = args_end;

			in_macro++;

			//printf("args: %s\n", ((define_t *) args->data)->name);
			
			//see if any code is left on the line
			if (!is_end_of_code_line (skip_whitespace (ptr))) {
				char *line_end = skip_to_line_end (ptr);
				char *full_line = (char *) malloc (strlen (define->contents) + line_end - ptr + 1);
				
				strcpy (full_line, define->contents);
				strncat (full_line, ptr, line_end - ptr);

				run_first_pass_line (full_line);
				free(full_line);

			} else {
				if (define->contents == NULL)
				{
					SetLastSPASMError(SPASM_ERR_ARG_USED_WITHOUT_VALUE, define->name);
				}
				else
				{
					//parse each line in the macro (prefix with space)
					//Buckeye: this malloc size is extra so that we can simply replace
					//@params and not worry about reallocating
					char *full_macro = (char *) malloc(strlen(define->contents) + 2);
					char *curr_line = full_macro;

					full_macro[0] = ' ';
					strcpy(&full_macro[1], define->contents);

					//char *replace_args_ptr = full_macro;
					//replace_args_ptr = replace_literal_args(replace_args_ptr, define, &args);

					const char *old_filename = curr_input_file;
					int old_line_num = line_num;
					curr_input_file = define->input_file;
					line_num = define->line_num;
	
					int session = StartSPASMErrorSession();
					while (curr_line != NULL && *curr_line && !error_occurred)
					{
						char *next_line = run_first_pass_line(curr_line);
						curr_line = skip_to_next_line(next_line);
						line_num++;
					}

					curr_input_file = (char *) old_filename;
					line_num = old_line_num;

					if (IsSPASMErrorSessionFatal(session))
					{
						AddSPASMErrorSessionAnnotation(session, "Error during invocation of macro '%s'", define->name);
						ReplaySPASMErrorSession(session);
					}
					EndSPASMErrorSession(session);

					free(full_macro);
				}
			}
			in_macro--;

			//clear the argument values
			remove_arg_set(args);

		} else {
			char *name = strndup(name_start, name_end - name_start);
			SetLastSPASMError(SPASM_ERR_UNKNOWN_OPCODE, name);
			free (name);
		}
	}

	return ptr;
}


/*
 * Tries to match instruction
 * arguments at ptr to one of
 * the sets of arguments for
 * curr_opcode, sets arg_ptrs
 * and arg_end_ptrs to start
 * and end of text of arguments,
 * sets curr_instr if a match
 * is found, returns new position
 * in file
 */

char *match_opcode_args (char *ptr, char **arg_ptrs, char **arg_end_ptrs, opcode *curr_opcode, instr **curr_instr) {
	char *curr_arg_file;
	int instr_num;

	for (instr_num = 0; instr_num < curr_opcode->num_instrs; instr_num++) {
		//test each possible set of arguments for this opcode
		const char *curr_arg;
		int curr_arg_num = 0;

		curr_arg_file = ptr;
		curr_arg = curr_opcode->instrs[instr_num].args;

		//check each character in the current argument
		while (*curr_arg && !(is_end_of_code_line (curr_arg_file) || *curr_arg_file == '\\')) {
			if (is_arg (*curr_arg)) {
				char trash_buf[256];

				//if this part of the argument is a number, then make sure it exists,
				//save its position in the file, and skip to the next argument
				if (is_end_of_code_line (curr_arg_file) || *curr_arg_file == '\\')
					break;

				arg_ptrs[curr_arg_num] = curr_arg_file;
				BOOL test = read_expr (&curr_arg_file, trash_buf, ",");
				if (*(curr_arg_file - 1) == ',')
					curr_arg_file--;
				arg_end_ptrs[curr_arg_num] = curr_arg_file;
				curr_arg_num++;

			} else {
				//if this part of the argument isn't a number, and they don't match, then break
				if (tolower (*curr_arg_file) != tolower (*curr_arg))
					break;

				curr_arg_file++;
			}
			curr_arg_file = skip_whitespace (curr_arg_file);
			curr_arg++;
		}

		//see if all the arguments matched
		if (!(*curr_arg) && (is_end_of_code_line (curr_arg_file) || *curr_arg_file == '\\'))
			break;
	}

	if (instr_num >= curr_opcode->num_instrs) {
		// If the next mnemonic is the same, skip to the next group
		if (curr_opcode->next != NULL && !strcasecmp (curr_opcode->name, curr_opcode->next->name))
			return match_opcode_args (ptr, arg_ptrs, arg_end_ptrs, curr_opcode->next, curr_instr);
		
		//if it doesn't match any instructions for this opcode, show an error and skip to line end
		SetLastSPASMError(SPASM_ERR_INVALID_OPERANDS, curr_opcode->name);

	} else
		*curr_instr = &(curr_opcode->instrs[instr_num]);

	curr_opcode->use_count++;
	return curr_arg_file;
}


/*
 * Writes the instruction data and arguments
 * for curr_instr, with argument text start
 * and end points in arg_ptrs and arg_end_ptrs
 */

void write_instruction_data (instr *curr_instr, char **arg_ptrs, char **arg_end_ptrs) {
	char *bit_arg_text = NULL;
	bool has_bit_arg = false;
	int i, curr_arg_num = 0;

	//write the actual instruction data first
	if (mode & MODE_NORMAL || mode & MODE_LIST) {
		int i;
		for (i = curr_instr->instr_size - 1; i >= 0; i--)
			write_out (curr_instr->instr_data[i]);
	}

	//then the arguments, if there are any
	for (i = 0; curr_instr->args[i]; i++) {
		//use the argument positions that were saved when we matched the arg strings
		if (is_arg (curr_instr->args[i])) {
			//first get the text of each argument
			char *arg_text = strndup (arg_ptrs[curr_arg_num],
			                          arg_end_ptrs[curr_arg_num] - arg_ptrs[curr_arg_num]);

			// Check for extra parentheses (confusingly looks like indirection)
			if (arg_text[0] == '(') {
				int level = 1;
				char *p = &arg_text[1];
				char *last = &arg_text[strlen(arg_text) - 1];
				
				// Clip off ending space
				while (isspace((unsigned char) *last) && last > p)
					last--;

				// Search the inner chars
				while (p < last && level > 0) {
					if (*p == '(') level++;
					else if (*p == ')') level--;
					p++;
				}
				if (p == last)
					show_warning ("Suggest remove extra parentheses around argument");
			}
			curr_arg_num++;

			switch (curr_instr->args[i]) {
				case '*': //16-bit number
					add_pass_two_expr (arg_text, ARG_NUM_16, 0);
					free (arg_text);
					break;
				case '&': //8-bit number
					add_pass_two_expr (arg_text, ARG_NUM_8, 0);
					free (arg_text);
					break;
				case '%': //8-bit address offset
					add_pass_two_expr (arg_text, ARG_ADDR_OFFSET, 0);
					free (arg_text);
					break;
				case '@': //8-bit IX/IY offset	
					add_pass_two_expr (arg_text, ARG_IX_IY_OFFSET, 0);
					free (arg_text);
					break;
				case '^': //bit number
					has_bit_arg = true;
					bit_arg_text = arg_text;
					break;
				case '#':
					add_pass_two_expr (arg_text, ARG_RST, 0);
					free(arg_text);
					break;
			}
		}
	}

	//if there's extra data at the end of the instruction, write that too
	if (curr_instr->has_end_data && (mode & MODE_NORMAL || mode & MODE_LIST)) {
		if (has_bit_arg)
			add_pass_two_expr (bit_arg_text, ARG_BIT_NUM, curr_instr->end_data);
		else
			write_out (curr_instr->end_data);
	}

	if (bit_arg_text)
		free (bit_arg_text);
}




