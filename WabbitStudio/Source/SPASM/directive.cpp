#include "stdafx.h"

#include "spasm.h"
#include "directive.h"
#include "utils.h"
#include "pass_one.h"
#include "pass_two.h"
#include "parser.h"
#include "opcodes.h"
#include "console.h"
#include "expand_buf.h"
#include "errors.h"


/*
 * Handles a directive, returns the
 * new location in the file
 */

char *handle_directive (const char *ptr) {
	static const char *dirs[] = {"db", "dw", "end", "org", "byte", "word", "fill", "block", "addinstr",
		"echo", "error", "list", "nolist", "equ", "show", "option", "seek", NULL};
	const char *name_end;
	char *name;
	int dir;

	//same deal as handle_preop, just with directives instead
	name_end = ptr;
	while (isalpha (*name_end))
		name_end++;

	dir = 0;
	while (dirs[dir]) {
		if (!strncasecmp (dirs[dir], ptr, name_end - ptr))
			break;
		dir++;
	}

	if (!dirs[dir])
		return handle_opcode_or_macro ((char *) ptr - 1);

	ptr = skip_whitespace (name_end);

	switch (dir) {
		case 0: //DB
		case 4: //BYTE
		{
			ptr = parse_emit_string (ptr, ES_BYTE, NULL);
			break;
		}
		case 1: //DW
		case 5: //WORD
		{
			ptr = parse_emit_string (ptr, ES_WORD, NULL);
			break;
		}
		case 3: //ORG
		{
			int value;
			char value_str[256] = "";
			bool fResult = read_expr(&ptr, value_str, "");
			if (fResult == true)
			{
				if (parse_num(value_str, &value) == true)
				{
					if (value < 0)
					{
						SetLastSPASMError(SPASM_ERR_INVALID_ADDRESS, skip_whitespace(value_str));
					}
					else
					{
						program_counter = value;
					}
				}
			}
			if ((fResult == false) || (strlen(skip_whitespace(value_str)) == 0))
			{
				SetLastSPASMError(SPASM_ERR_VALUE_EXPECTED);
			}
			break;
		}
		case 6: //FILL
		case 7: //BLOCK
		{
			int size, fill_value;
			char szSize[256], szFill[256];
			bool old_listing_on = listing_on;
			//listing_on = false;

			read_expr (&ptr, szSize, ",");
			parse_num (szSize, &size);
			ptr = skip_whitespace (ptr);
			if (*ptr == ',')
				ptr++;

			if (read_expr (&ptr, szFill, "")) {
				//if there's a value to fill the block with, then handle that
				parse_num (szFill, &fill_value);
			} else {
				//otherwise use 0
				fill_value = 0;
			}

			if (size < 0) {
				SetLastSPASMError(SPASM_ERR_SIZE_MUST_BE_POSITIVE, szSize);
				listing_on = old_listing_on;
				break;
			}

			if (fill_value < -128 || fill_value > 255)
			{
				SetLastSPASMWarning(SPASM_WARN_TRUNCATING_8);
			}

			program_counter += size;
			stats_datasize += size;

			while (size-- > 0)
				write_out (fill_value & 0xFF);

			//listing_on = old_listing_on;
			break;
		}
		case 8: //ADDINSTR
		{
			instr *instr = (struct _instr *) malloc (sizeof (struct _instr));
			char word[256];
			int result;
			char *mnemonic;
			size_t i, base = 0, size_left;
			int j;
			opcode *last_opcode = NULL, *curr_opcode = all_opcodes, *new_opcode;

			memset (instr, 0, sizeof (struct _instr));

			// Mnemonic
			if (!read_expr (&ptr, word, " \t")) goto addinstr_fail;
			mnemonic = strdup (word);

			// Args
			if (!read_expr (&ptr, word, " \t")) goto addinstr_fail;
			reduce_string (word);
			instr->args = strdup (word);

			// Instruction data
			if (!read_expr (&ptr, word, " \t")) goto addinstr_fail;
	        conv_hex (word, word + strlen (word), &result);
	        instr->instr_size = strlen (word) / 2;

	        for (j = instr->instr_size - 1; j >= 0; j--)
	        	instr->instr_data[instr->instr_size - j - 1] = (result >> (j * 8)) & 0xFF;


			// Size
			if (!read_expr (&ptr, word, " \t")) goto addinstr_fail;
			if (!parse_num (word, &instr->size)) goto addinstr_fail;

			// Class
			read_expr (&ptr, word, " \t");

			// Extended
			read_expr (&ptr, word, " \t");

			// End data ...
			if (read_expr (&ptr, word, " \t")) {
				int output;
				conv_hex (word, word + strlen (word), &output);
				instr->end_data = (unsigned char) output;
				instr->has_end_data = true;
			}

			size_left = instr->size - instr->instr_size;
			while ((i = strcspn (&instr->args[base], "*")) + base != strlen (instr->args)) {
				switch (size_left - instr->has_end_data) {
					case 2:	((char *) instr->args)[base+i] = '*'; break;
					case 1: ((char *) instr->args)[base+i] = '&'; break;
					default:
						((char *) instr->args)[base+i] = '&'; break;
						//show_error ("Invalid wildcard type in ADDRINSTR");
						//goto addinstr_fail;
						break;
				}
				size_left -= 2;
				base += i + 1;
			}

			new_opcode = (opcode *)  malloc (sizeof (opcode));
			new_opcode->name = mnemonic;
			new_opcode->num_instrs = 1;
			new_opcode->use_count = 0;
			new_opcode->instrs = instr;
			new_opcode->is_added = true;

			while (curr_opcode) {
				if (strcasecmp (mnemonic, curr_opcode->name) == 0)
					break;
				last_opcode = curr_opcode;
				curr_opcode = curr_opcode->next;
			}

			if (curr_opcode == NULL) {
				last_opcode->next = new_opcode;
				new_opcode->next = NULL;
			} else {
				new_opcode->next = curr_opcode;
				if (last_opcode)
					last_opcode->next = new_opcode;
				else
					all_opcodes = new_opcode;
			}

			// You can ignore class, etc
			ptr = skip_to_line_end (ptr);
			break;
addinstr_fail:
			SetLastSPASMError(SPASM_ERR_INVALID_ADDINSTR);
			if (instr && instr->args) free ((void *) instr->args);
			if (instr) free (instr);
			ptr = NULL;
			break;
		}
		case 9: //ECHO
		{
			if (ptr[0] == '>')
			{
				char target_format[2] = "w";
				FILE *echo_target;
				char filename[MAX_PATH];
				char temp_filename[MAX_PATH];
				define_t *define;

				if ((++ptr)[0] == '>')
					target_format[0] = 'a';

				ptr = skip_whitespace (ptr + 1);
				if (is_end_of_code_line (ptr)) {
					SetLastSPASMError(SPASM_ERR_FILENAME_EXPECTED);
					return NULL;
				}

				read_expr (&ptr, filename, " \t");

				// Is the filename given a macro?
				if ((define = search_defines (filename)))
					strncpy (filename, skip_whitespace(define->contents), sizeof (filename));
					
				reduce_string(filename);

				if (is_abs_path(filename)) {
					strncpy(temp_filename, skip_whitespace (filename), sizeof (temp_filename));
				} else {
					strncpy(temp_filename, temp_path, sizeof (temp_filename));
					strncat(temp_filename, "/", sizeof (temp_filename));
					strncat(temp_filename, skip_whitespace (filename), sizeof (temp_filename));
				}
				echo_target = fopen (fix_filename (temp_filename), target_format);
				if (echo_target == NULL) {
					SetLastSPASMError(SPASM_ERR_NO_ACCESS, filename);
					return NULL;
				}

				//if the output's redirected to a file, process it now
				WORD orig_attributes = save_console_attributes();
				set_console_attributes (COLOR_GREEN);
				ptr = parse_emit_string (ptr, ES_ECHO, echo_target);
				restore_console_attributes(orig_attributes);
			} else {
				char expr[256];
				read_expr (&ptr, expr, "");

				// If it craps out for some reason, save it for the next pass
				int session = StartSPASMErrorSession();
				parse_emit_string (expr, ES_ECHO, NULL);

				if (IsSPASMErrorSessionFatal(session) == true)
				{
					add_pass_two_output (expr, OUTPUT_ECHO);
				}
				else if (GetSPASMErrorSessionErrorCount(session) > 0)
				{
					WORD orig_attributes = save_console_attributes();
					set_console_attributes(COLOR_GREEN);
					int internal_session = StartSPASMErrorSession();
					parse_emit_string (expr, ES_ECHO, stdout);
					restore_console_attributes(orig_attributes);
					EndSPASMErrorSession(internal_session);

					ReplaySPASMErrorSession(session);
				}
				else
				{
					expand_buf_t *echo = eb_init(256);
					parse_emit_string (expr, ES_FCREATE, echo);

					char *echo_string = eb_extract(echo);
					eb_free(echo);

					char *expanded_string = escape_string(echo_string);
					free(echo_string);

					add_pass_two_output (expanded_string, OUTPUT_ECHO);
					free(expanded_string);
				}
				EndSPASMErrorSession(session);
			}
			break;
		}
		case 10: //ERROR
		{
			expand_buf_t *eb = eb_init(64);
			ptr = parse_emit_string(ptr, ES_FCREATE, eb);

			char *error_str = eb_extract(eb);
			eb_free(eb);

			SetLastSPASMError(SPASM_ERR_CUSTOM, error_str);
			free(error_str);
			break;
		}
		case 11: //LIST
		{
			//if listing was off already, then the listing
			// for the start of this line wouldn't have been
			// written, so make sure the end doesn't get
			// written either
			if (!listing_on)
				listing_for_line_done = true;
			listing_on = true;
			break;
		}
		case 12: //NOLIST
		{
			//if listing is on, then it would've written
			// the starting stuff for this line already,
			// so take that out
			if ((mode & MODE_LIST) && listing_on)
				listing_offset -= 14;
			listing_on = false;
			break;
		}
		case 13: //EQU
		{
			// Finally, a proper .equ!
			int value;
			char value_str[256];

			read_expr (&ptr, value_str, "");
			if (!parse_num (value_str, &value) && parser_forward_ref_err) {
			} else {
				if (last_label == NULL)
					SetLastSPASMError(SPASM_ERR_EQUATE_MISSING_LABEL);
				else
					last_label->value = value;
			}
			break;
		}
		case 14: //SHOW
		{
			char name[256];
			define_t *define;

			//get the name
			read_expr (&ptr, name, "");
			define = search_defines (name);
			if (define == NULL) {
				//if it doesn't exist yet, save it for the second pass
				add_pass_two_output (name, OUTPUT_SHOW);
			} else {
				//otherwise, show it now
				show_define (define);
			}
			break;
		}
		case 15: //OPTION
		{
			char *word = NULL;
			arg_context_t context = ARG_CONTEXT_INITIALIZER;
			while ((word = extract_arg_string(&ptr, &context)) != NULL)
			{
				char name[256], *expr = word;
				char *define_name;
				define_t *define;

				read_expr(&expr, name, "=");
				if (*expr == '=')
				{
					expr++;
				}
				
				if (!(isalpha(name[0]))) {
					SetLastSPASMError(SPASM_ERR_INVALID_OPTION, name);
					return (char *) ptr;
				}
				
				if (is_end_of_code_line (skip_whitespace (expr)))
					expr = strdup ("1");
				else {
					//if (!parse_num (expr, NULL))
					//	return NULL;
					expr = strdup (expr);
				}

				if (strlen (name) == 0) {
					show_error ("Invalid option statement");
					return NULL;
				}

				define_name = (char *) malloc (strlen (name) + 3);
				strcat (strcpy (define_name, "__"), name);

				define = add_define (define_name, NULL);
				set_define (define, expr, -1, false);
				free(expr);
			}
			break;
		}
		case 16: //SEEK
		{
			int value;
			char value_str[256];

			read_expr (&ptr, value_str, "");
			parse_num (value_str, (int *) &value);

			//printf("value_str: %s\npc: %d\n", value_str, program_counter);

			if (value > program_counter && (value - program_counter > output_buf_size - (out_ptr - output_contents)))
				show_fatal_error ("Seek location %d out of bounds", value);
			else if (value < program_counter && (value - (int) program_counter + (out_ptr - output_contents) < 0))
				show_fatal_error ("Seek value %d too small", value);
		
			out_ptr += value - ((int) program_counter);
			//printf("base: %p; ptr: %p\n", output_contents, out_ptr);
			program_counter = value;
			break;
		}
	}

	return (char *) ptr;
}


/*
 * Shows contents of a #define
 */

void show_define (define_t *define) {
	WORD console_attrib = save_console_attributes();
	set_console_attributes (COLOR_BLUE);
	fputs (define->name, stdout);
	if (define->num_args > 0) {
		int i;
		putchar ('(');
		for (i = 0; i < define->num_args; i++) {
			if (i != 0) fputs (", ", stdout);
			fputs (define->args[i], stdout);

		}
		putchar (')');
	}
	putchar ('\n');
	restore_console_attributes(console_attrib);

#ifdef WIN32
	if (define->contents != NULL)
	{
		OutputDebugString(define->contents);
		OutputDebugString("\n");
	}
#endif
	//printf (": %s\n", define->contents);
}


/*
 * Parses a list of values and strings,
 * either prints them to the output,
 * writes them to a file, or adds them
 * to the program output
 */

char *parse_emit_string (const char *ptr, ES_TYPE type, void *echo_target) {
	static int level = 0;
	char *word = NULL;
	int session;
	bool fWasParsed;
	char *name_end;

	level++;

	arg_context_t context = ARG_CONTEXT_INITIALIZER;
	while ((word = extract_arg_string(&ptr, &context)) != NULL)
	{
		// handle strings
		if (word[0] == '"')
		{
			char *next = next_expr (word, EXPR_DELIMS);
			if (*next != '\0') 
				goto echo_error;
			
			reduce_string (word);
			if (type == ES_ECHO)
			{
				if (echo_target != NULL)
				{
					fprintf ((FILE *) echo_target, word);
				}
			}
			else if (type == ES_FCREATE) {
				eb_append((expand_buf_t *) echo_target, word, strlen(word));
			} else {
				int i;
				for (i = 0; word[i]; i++) {
					if ((mode & MODE_NORMAL) || (mode & MODE_LIST))
						write_out (word[i]);
	                stats_datasize++;
	                program_counter++;
				}
			}

		} else {
			int value;

			session = StartSPASMErrorSession();
			fWasParsed = parse_num(word, &value);
			if (fWasParsed == true)
			{
				switch (type) 
				{
					case ES_ECHO: 
					{
						if (echo_target != NULL)
						{
							fprintf ((FILE *) echo_target, "%d", value);
						}
						break;
					}
#ifdef USE_BUILTIN_FCREATE
					case ES_FCREATE:
					{
						char buffer[256];
						sprintf_s(buffer, "%d", value);
						eb_append((expand_buf_t *) echo_target, buffer, -1);
						break;
					}
#endif
					case ES_BYTE: 
					{
						write_arg(value, ARG_NUM_8, 0);
		                stats_datasize++;
		                program_counter++;
		                break;
					}
					case ES_WORD:
					{
		                write_arg(value, ARG_NUM_16, 0);
		                stats_datasize+=2;
		                program_counter+=2;
		                break;
					}
				}
			}
			else if (IsSPASMErrorSessionFatal(session) == false && type != ES_FCREATE)
			{
				switch (type) 
				{
				case ES_ECHO:
					break;
				case ES_BYTE: 
					{
						add_pass_two_expr(word, ARG_NUM_8, 0);
		                stats_datasize++;
		                program_counter++;
		                break;
					}
				case ES_WORD:
					{
		                add_pass_two_expr(word, ARG_NUM_16, 0);
		                stats_datasize+=2;
		                program_counter+=2;
		                break;
					}
				}
			}
			else
			{
				char name[256];
				char *next;
				define_t *define;

				name_end = word;
				//printf("error occured: %d, forward: %d, value: %d\n", error_occurred, parser_forward_ref_err, value);
				read_expr (&name_end, name, "(");
				//printf("Looking up %s\n", name);
				next = name_end;
				read_expr (&next, NULL, ")");
				
				if (*next != '\0')
					goto echo_error;

				if ((define = search_defines (name))) {
					char *expr;
					list_t *args = NULL;
	
					//handle defines
					if (define->contents == NULL)
					{
						SetLastSPASMError(SPASM_ERR_ARG_USED_WITHOUT_VALUE, name);
					}
					
					if (*(name_end - 1) == '(') name_end--;
					name_end = parse_args (name_end, define, &args);
					if (!name_end)
						return (char *) ptr;
					
					expr = parse_define (define);
					if (expr != NULL)
					{
						arg_context_t nested_context = ARG_CONTEXT_INITIALIZER;

						// Is it some kind of argument list?
						const char *nested_expr = expr;
						extract_arg_string(&nested_expr, &nested_context);

						if (extract_arg_string(&nested_expr, &nested_context) != NULL)
						{
							// if it is, plug it in to the emit string
							parse_emit_string(expr, type, echo_target);
						}
						else
						{
							if (IsErrorInSPASMErrorSession(session, SPASM_ERR_EXCEEDED_RECURSION_LIMIT) == false)
							{
								
								int inner_session = StartSPASMErrorSession();
								parse_emit_string(expr, type, echo_target);
								if (IsSPASMErrorSessionFatal(inner_session))
								{
									EndSPASMErrorSession(inner_session);
									AddSPASMErrorSessionAnnotation(session, _T("Error during evaluation of macro '%s'"), define->name);

									ReplaySPASMErrorSession(session);
								}
								else
								{
									EndSPASMErrorSession(inner_session);
								}
								//ReplaySPASMErrorSession(session);
							}
							else
							{
								ReplaySPASMErrorSession(session);
							}
						}
						free (expr);
					}
					remove_arg_set (args);
				}
				else
				{
echo_error:
					
					switch (type)
					{
					case ES_ECHO:
						{
							if (echo_target != NULL)
							{
								fprintf((FILE *) echo_target, "(error)");
							}
							break;
						}
					case ES_FCREATE:
						{
							SetLastSPASMError(SPASM_ERR_LABEL_NOT_FOUND, word);
							eb_append((expand_buf_t *) echo_target, "(error)", -1);
							break;
						}
					}

					ReplaySPASMErrorSession(session);
				}
			}
			EndSPASMErrorSession(session);
		}
	}

	if (type == ES_ECHO && level == 1) {
		if (echo_target == stdout) putchar ('\n');
		else {
			if (echo_target != NULL)
			{
				fclose ((FILE *) echo_target);
			}
		}
	}
	
	level--;
	return (char *) ptr;
}
