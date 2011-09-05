#include "stdafx.h"

#include "spasm.h"
#include "pass_one.h"
#include "pass_two.h"
#include "utils.h"
#include "parser.h"
#include "directive.h"
#include "console.h"
#include "errors.h"

void write_arg (int value, arg_type type, int or_value);

expr_t *expr_list = NULL, *expr_list_tail = NULL;
output_t *output_list = NULL, *output_list_tail = NULL;

/*
 * Tries to evaluate an expression,
 * if it succeeds writes the data
 * to the output, otherwise writes
 * 0s to fill the space and adds
 * the expression to the list to be
 * parsed in pass two
 */

void add_pass_two_expr (char *expr, arg_type type, int or_value) {
	int value;

	//if we're in code counter or stats mode, where we don't need actual expressions, then just skip this crap
	/*if (mode & MODE_CODE_COUNTER)
		return;*/

	if (type == ARG_IX_IY_OFFSET) {
		//if it's an IX or IY offset, it's allowed to have a + in front of it, so skip that
		// so the parser doesn't try to interpret it as a local label
		if (*expr == '+') {
			expr++;
		} else if (*expr == '\0') {
			//IX/IY offsets are also allowed to be blank - a 0 is assumed then
			write_arg (0, ARG_IX_IY_OFFSET, 0);
			return;
		}
	}

	int session = StartSPASMErrorSession();
	bool fResult = parse_num (expr, &value);
	if ((fResult == false) && (IsSPASMErrorSessionFatal(session) == false))
	{
		expr_t *new_expr;

		//if that didn't work, then add it to the pass two expression list
		new_expr = (expr_t *)malloc (sizeof (expr_t));
		new_expr->program_counter = program_counter;
		new_expr->line_num = line_num;
#ifdef USE_REUSABLES
		new_expr->curr_reusable = get_curr_reusable();
#endif
		new_expr->out_ptr = out_ptr;
		if (mode & MODE_LIST)
			new_expr->listing_offset = listing_offset;
		new_expr->type = type;
		new_expr->input_file = curr_input_file;
		new_expr->listing_on = listing_on;
		new_expr->or_value = or_value;
		new_expr->next = NULL;

		if (expr_list_tail)
			expr_list_tail->next = new_expr;
		expr_list_tail = new_expr;
		if (!expr_list)
			expr_list = new_expr;

		/* Important stuff here: to prevent forward references
		   combined with changing #defines (either through
		   redefinition, or with macro arguments, for example),
		   all #defines need to be fully expanded in the saved
		   expression for the second pass */

		//store the contents of the expanded expression for the second pass
		new_expr->expr = expand_expr (expr);

		//and write a blank value in its place
		if (type == ARG_ADDR_OFFSET) {
			type = ARG_NUM_8;
		}
		write_arg (0, type, 0);

	}
	else if (fResult == false)
	{
		ReplaySPASMErrorSession(session);
	}
	else
	{
		if (type == ARG_RST) {
			switch (value) {
				case 0x00:
				case 0x08:
				case 0x10:
				case 0x18:
				case 0x20:
				case 0x28:
				case 0x30:
				case 0x38:
					write_arg(value + 0xC7, type, or_value);
					break;
				default:
					SetLastSPASMError(SPASM_ERR_INVALID_RST_OPERANDS);
					break;
			}
		} else {
		//write the value now
			write_arg (value, type, or_value);
		}
		ReplaySPASMErrorSession(session);
	}

	EndSPASMErrorSession(session);
}


/*
 * Adds an expression to be
 * echo'ed on the second pass
 *
 * Allocates a copy
 */

void add_pass_two_output (char *expr, output_type type) {
	output_t *new_output;

	new_output = (output_t *)malloc (sizeof (output_t));
	new_output->program_counter = program_counter;
	new_output->line_num = line_num;
#ifdef USE_REUSABLES
	new_output->curr_reusable = get_curr_reusable();
#endif
	new_output->type = type;
	new_output->input_file = curr_input_file;
	new_output->next = NULL;

	if (type == OUTPUT_SHOW)
		new_output->expr = strdup (expr);
	else
		new_output->expr = expand_expr (expr);

	if (output_list_tail)
		output_list_tail->next = new_output;
	output_list_tail = new_output;
	if (!output_list)
		output_list = new_output;
}


/*
 * Goes through the list of
 * expressions that couldn't
 * be parsed before and evaluates
 * them and writes the values
 * to the output file
 */

extern unsigned char *output_contents;

void run_second_pass () {
	int value;
	expr_t *old_expr;
	output_t *old_output;
	unsigned char *saved_out_ptr = out_ptr;
	int saved_listing_offset = listing_offset;
	char* old_input_file = curr_input_file;

	pass_one = false;

	//FILE *file = fopen ("passtwoexprs.txt", "w");

	//printf("running through the list %p\n", expr_list);
	while (expr_list)
	{
		//go through each expression and evaluate it
		program_counter = expr_list->program_counter;
		line_num = expr_list->line_num;
		curr_input_file = expr_list->input_file;
#ifdef USE_REUSABLES
		set_curr_reusable(expr_list->curr_reusable);
#endif

		//fprintf(file, "%s:%d:offset(%d): %s\n", curr_input_file, line_num, expr_list->out_ptr - output_contents, expr_list->expr);

		//printf("passtwoexpr: '%s'\n", expr_list->expr);
		if (parse_num (expr_list->expr, &value))
		{
			//if that was successful, then write it to the file
			if (mode & MODE_LIST)
				listing_offset = expr_list->listing_offset;
			if (expr_list->type == ARG_RST) {
				switch (value) {
					case 0x00:
					case 0x08:
					case 0x10:
					case 0x18:
					case 0x20:
					case 0x28:
					case 0x30:
					case 0x38:
						write_arg(value + 0xC7, expr_list->type, expr_list->or_value);
						break;
					default:
						SetLastSPASMError(SPASM_ERR_INVALID_RST_OPERANDS);
						break;
				}
			} else {
				out_ptr = expr_list->out_ptr;
				listing_on = expr_list->listing_on;
				write_arg (value, expr_list->type, expr_list->or_value);
			}
		}
		free (expr_list->expr);
		old_expr = expr_list;
		expr_list = expr_list->next;
		free (old_expr);
	}

	//fclose(file);

	out_ptr = saved_out_ptr;
	if (mode & MODE_LIST)
		listing_offset = saved_listing_offset;

	while (output_list) {
		//show each saved echo
		program_counter = output_list->program_counter;
		line_num = output_list->line_num;
		curr_input_file = output_list->input_file;
#ifdef USE_REUSABLES
		set_curr_reusable(output_list->curr_reusable);
#endif

		switch (output_list->type) {
			case OUTPUT_ECHO:
				{
					WORD orig_attributes = save_console_attributes();
					set_console_attributes (COLOR_GREEN);
					int session = StartSPASMErrorSession();
					parse_emit_string (output_list->expr, ES_ECHO, stdout);
					ReplaySPASMErrorSession(session);
					EndSPASMErrorSession(session);
					restore_console_attributes(orig_attributes);
					break;
				}
			case OUTPUT_SHOW:
			{
				define_t *define;
				if (!(define = search_defines (output_list->expr)))
				{
					SetLastSPASMError(SPASM_ERR_LABEL_NOT_FOUND, output_list->expr);
					break;
				}
				show_define (define);
				break;
			}
		}
#if defined(DUMP_DEFINES) && defined(_DEBUG)
		dump_defines();
#endif

		free (output_list->expr);
		old_output = output_list;
		output_list = output_list->next;
		free (old_output);
	}
	curr_input_file = old_input_file;
}


/*
 * Writes an argument
 * directly to the file,
 * OR'ing it with a value
 * for bit numbers
 */

void write_arg (int value, arg_type type, int or_value) {

	switch (type) {
		case ARG_NUM_8:
			if (value < -128 || value > 255)
			{
				SetLastSPASMWarning(SPASM_WARN_TRUNCATING_8);
			}
			write_out (value & 0xFF);
			break;
		case ARG_NUM_16:
			//no range checking for 16 bits, as higher bits of labels are used to store page info
			write_out (value & 0xFF);
			write_out ((value >> 8) & 0xFF);
			break;
		case ARG_ADDR_OFFSET:
			value &= 0xFFFF;
			value -= ((program_counter & 0xFFFF)+ 2);
			
			if (value < -128 || value > 127)
			{
				SetLastSPASMError(SPASM_ERR_JUMP_EXCEEDED, value);
				value = 0;
			}
			write_out (value & 0xFF);
			break;
		case ARG_IX_IY_OFFSET:
			if (value > 127 || value < -128)
			{
				SetLastSPASMError(SPASM_ERR_INDEX_OFFSET_EXCEEDED, value);
				value = 0;
			}
			write_out (value & 0xFF);
			break;
		case ARG_BIT_NUM:
			if (value < 0 || value > 7) {
				show_error ("Bit number can only range from 0 to 7");
				value = 0;
			}
			write_out (((value & 0x07) << 3) | or_value);
			break;
	}
}

