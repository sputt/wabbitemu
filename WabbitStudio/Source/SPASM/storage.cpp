#include "stdafx.h"

#define MAX_REUSABLES 512

#include "spasm.h"
#include "storage.h"
#include "utils.h"
#include "hash.h"
#include "list.h"
#include "preop.h"
#include "parser.h"
#include "directive.h"
#include "opcodes.h"
#include "console.h"
#include "errors.h"

extern char *curr_input_file;
extern int line_num;
extern bool error_occurred;

#ifdef USE_REUSABLES
extern unsigned int program_counter;
unsigned int reusables[MAX_REUSABLES];
extern int total_reusables, curr_reusable;
#endif

#ifdef WIN32
#define strcasecmp _stricmp
#endif

bool case_sensitive;
hash_t *label_table, *define_table;
list_t *arg_list = NULL;
const char *separators = "+-*<>|&/%^()\\, \t\r";

void write_labels_callback(label_t *label, list_t *label_list);
void write_defines_callback(define_t *, list_t *label_list);


/*
 * Writes a label file
 */
#define NUM_BUILTIN_DEFINES 7
void write_labels (char *filename) {
	FILE *symtable;
	label_t hdr_node;
	list_t label_list;
	list_t *node;

	symtable = fopen (filename, "w");
	if (!symtable) {
		printf ("Couldn't open output file %s\n", filename);
		return;
	}

	// Create a header node with an impossible label name
	hdr_node.name = "#header";
	
	label_list.data = &hdr_node;
	label_list.next = NULL;
	
	int session = StartSPASMErrorSession();
	hash_enum (label_table, (HASH_ENUM_CALLBACK) write_labels_callback, &label_list);
	hash_enum (define_table, (HASH_ENUM_CALLBACK) write_defines_callback, &label_list);
	EndSPASMErrorSession(session);
	
	node = label_list.next;
	int index = 0;
	while (node != NULL) {
		label_t *label = (label_t *) node->data;
		fprintf (symtable, "%s = $%.4X\n", label->name, label->value);
		index++;
		node = node->next;
	}

	fclose (symtable);
	
	list_free (label_list.next, true, NULL);
}

/*
 * Inserts a label into label_list alphabetically
 */

void write_labels_callback(label_t *label, list_t *label_list) {
	label_t *labelToAdd;

	list_t * node = label_list->next, *prev = label_list;
	while (node != NULL && strcasecmp (label->name, ((label_t *) node->data)->name) > 0) {
		prev = node;
		node = node->next;
	}
	//this is necessary since we have to free the define created labels
	labelToAdd = (label_t *) malloc(sizeof(label_t));
	labelToAdd->name = label->name;
	labelToAdd->value = label->value;
	list_insert (prev, labelToAdd);
}

/*
 * Inserts a define into a label list alphabetically (will parse if possible)
 */
void write_defines_callback(define_t *define, list_t *label_list) {
	int value;
	label_t *label;
	list_t *node, *prev;

	if (define->num_args > 0 || define->contents == NULL)
		return;
	
	if (parse_num(define->contents, &value) == false)
		return;
	
	node = label_list->next, prev = label_list;
	while (node != NULL && strcasecmp (define->name, ((label_t *) node->data)->name) > 0) {
		prev = node;
		node = node->next;
	}
	
	label = (label_t *) malloc(sizeof(label_t));
	label->name = define->name;
	label->value = value;
	list_insert(prev, label);
}

void dump_defines_callback(define_t *define, void *reserved) {
#ifdef WIN32
	OutputDebugString(define->name);
	OutputDebugString("---------\n");
	OutputDebugString(define->contents);
	OutputDebugString("\n---------\n\n");
#else

#endif
}

void dump_defines() {

	hash_enum(define_table, (HASH_ENUM_CALLBACK) dump_defines_callback, NULL);

}

/*
 * Destroy functions for
 * values in hash tables
 */

static void destroy_define_value (define_t *define) {
	int curr_arg;

	if (define->contents)
		free (define->contents);
	if (define->name)
		free (define->name);
	if (define->input_file)
		free(define->input_file);

	for (curr_arg = 0; curr_arg < MAX_ARGS; curr_arg++) {
		if (define->args[curr_arg] != NULL) {
			free (define->args[curr_arg]);
		}
	}
	free (define);
}

static void destroy_label_value (label_t *label) {
	if (label->name)
		free (label->name);
	if (label->input_file)
		free(label->input_file);
	free (label);
}


/*
 * Inits label, define, and macro
 * storage tables
 */
void init_storage() {
	define_t *define;
	char* test ;

	define_table = hash_init (MAX_DEFINES, (HASH_REMOVE_CALLBACK) destroy_define_value);
	label_table = hash_init (MAX_LABELS, (HASH_REMOVE_CALLBACK) destroy_label_value);

	add_define (strdup ("SPASM"), NULL)->contents = strdup ("1");
	add_define (strdup ("SPASMVER"), NULL)->contents = strdup ("2");
	add_define (strdup ("__LINE"), NULL)->contents = strdup ("0");
	add_define (strdup ("__FILE"), NULL)->contents = strdup ("Init");
	add_define (strdup ("TRUE"), NULL)->contents = strdup ("1");
	add_define (strdup ("FALSE"), NULL)->contents = strdup ("0");
	add_define (strdup ("__BM_SHD"), NULL)->contents = strdup ("2");
	add_define (strdup ("__BM_MSK_RGB"), NULL)->contents = strdup ("RGB(0, 255, 0)");

	(define = add_define (strdup ("RGB"), NULL))->contents = strdup ("((__R) << 16)|((__G) << 8)|(__B)");
	test = strdup ("__R,__G,__B)");
	parse_arg_defs (test, define);
	free(test);
	
	(define = add_define (strdup ("GETC"), NULL))->contents = strdup ("(0)");
	test = strdup ("__A, __N)"); 
	parse_arg_defs (test, define);
	free(test);
}


/*
 * Frees all storage for labels and defines
 */
EXPORT void free_storage() {
	opcode *next_opcode = NULL, *last_opcode = NULL, *curr_opcode = all_opcodes;
	all_opcodes = opcode_list;

	list_free(input_files, true, NULL);

	hash_free(label_table);
	label_table = NULL;

	hash_free(define_table);
	define_table = NULL;

	while (curr_opcode) {
		next_opcode = curr_opcode->next;
		if (curr_opcode->is_added) {
			free((void *) curr_opcode->instrs->args);
			free(curr_opcode->instrs);
			free((void *) curr_opcode->name);
			free(curr_opcode);
			if (last_opcode)
				last_opcode->next = next_opcode;
		} else {
			last_opcode = curr_opcode;
		}
		curr_opcode = next_opcode;
	}
	last_opcode->next = NULL;
}

void set_case_sensitive(bool sensitive) {
	case_sensitive = sensitive;
}

bool get_case_sensitive() {
	return case_sensitive;
}


/*
 * Adds define with allocated name,
 * returns a pointer to it and sets
 * a bool if it's being redefined
 * 
 * Call with redefined = NULL if you don't care
 */

define_t *add_define (char *name, bool *redefined, bool search_local) {
	define_t *define;
	label_t *conflict_label;

	if (!case_sensitive) {
		char *new_name = strup (name);
		free (name);
		name = new_name;
	}
	
	if ((conflict_label = search_labels(name))) {
		show_error ("conflicting definition of '%s'", name);
		//if (suppress_errors == false) {
			show_error_prefix (conflict_label->input_file, conflict_label->line_num);
			WORD attr = save_console_attributes();
			set_console_attributes (COLOR_RED);
			printf ("previous definition of '%s' was here\n", name);
			restore_console_attributes(attr);
		//}
		return NULL;
	}
	
	// handle redefinitions
	if ((define = search_defines (name, search_local))) {
		int curr_arg;

		free (name);
		//define->line_num = line_num;
		//define->input_file = curr_input_file;
		/* Don't clear the contents of the #define, because
		   if it's being redefined references to itself may
		   need to be expanded, which will require the original
		   contents - all handled by set_define */

		for (curr_arg = 0; curr_arg < define->num_args; curr_arg++) {
			if (define->args[curr_arg] != NULL) {
				free(define->args[curr_arg]);
				define->args[curr_arg] = NULL;
			}
		}
		define->num_args = 0;

		if (redefined != NULL)
			*redefined = true;

		return define;
	} 
	
	if (redefined != NULL)
		*redefined = false;

	define = (define_t *) malloc (sizeof (define_t));
	if (define != NULL) {
		int curr_arg;

		define->name = name;
		define->line_num = line_num;
		define->input_file = strdup(curr_input_file);
		define->contents = NULL;
		define->num_args = 0;

		for (curr_arg = 0; curr_arg < MAX_ARGS; curr_arg++)
			define->args[curr_arg] = NULL;

		hash_insert (define_table, define);
	}
	return define;
}


define_t *search_local_defines (const char *name) {	
	define_t *result = NULL;
	char *search_name;

	//make name uppercase if needed for case-insensitivity
	if (!case_sensitive)
		search_name = strup (name);
	else
		search_name = (char *)name;

	//first search all the sets of arguments in order
	list_t *curr_arg_set = arg_list;
	while (curr_arg_set) {
		result = (define_t *)hash_lookup ((hash_t *)(curr_arg_set->data), search_name);
		if (result)
			break;
		curr_arg_set = curr_arg_set->next;
	}

	//first search all the sets of arguments in order
	while (curr_arg_set) {
		result = (define_t *)hash_lookup ((hash_t *)(curr_arg_set->data), search_name);
		if (result)
			break;
		curr_arg_set = curr_arg_set->next;
	}

	if (!case_sensitive)
		free (search_name);

	return result;
}
/*
 * Finds define by name,
 * returns pointer to
 * define data if found
 * or NULL if not found
 */

define_t *search_defines (const char *name, bool search_local) {	
	define_t *result = NULL;
	char *search_name;
	list_t *curr_arg_set = arg_list;
	size_t curr_hash;

	//make name uppercase if needed for case-insensitivity
	if (!case_sensitive)
		search_name = strup (name);
	else
		search_name = (char *)name;

	//first search all the sets of arguments in order
	if (search_local)
	{
		while (curr_arg_set) {
			result = (define_t *)hash_lookup ((hash_t *)(curr_arg_set->data), search_name);
			if (result)
				break;
			curr_arg_set = curr_arg_set->next;
		}
	}

	//if that doesn't work, look in the global define table
	if (!result)
		result = (define_t *)hash_lookup (define_table, search_name);
	
#define MHASH(Z) (murmur_hash(Z, strlen(Z)))

	curr_hash = murmur_hash (search_name, strlen (search_name));
	// Search all SPASM predefined values
	if (curr_hash == MHASH("__LINE")) {
		
		char line_buf[32];
		sprintf (line_buf, "%d", line_num);
		if (result)
			set_define (result, line_buf, -1, false);
		
	} else if (curr_hash == MHASH("__FILE")) {
		
		char fn_buf[MAX_PATH + 2];
		sprintf (fn_buf, "\"%s\"", curr_input_file);
		if (result)
			set_define (result, fn_buf, -1, false);
		
	}
	//printf("fail: %s %08x\n", search_name, murmur_hash(search_name, strlen(search_name)));
	
	if (!case_sensitive)
		free (search_name);

	//make sure any empty arguments get returned as undefined
	//if (result && result->contents == NULL) result = NULL;
	return result;
}


/*
 * Removes a define by name
 */

void remove_define (char *name) {
	if (!case_sensitive) {
		char *new_name = strup (name);
		name = new_name;
	}
			
	hash_remove (define_table, name);

	if (!case_sensitive)
		free(name);
}


/*
 * Sets #define contents from str
 * with length (-1 to find internally),
 * if it's been redefined, processes
 * it and replaces instances of its
 * own name with the expanded contents
 * to avoid endless recursion
 */

void set_define (define_t *define, const char *str, int len, bool redefined) {
	//if there's any possibility that the define is being redefined, use
	// this function to avoid memory leaks and infinite recursion,
	// otherwise writing directly to define->contents is fine

	if (!redefined || !define->contents) {
		//just set the contents
		if (define->contents)
			free (define->contents);

		define->contents = (len == -1 ? strdup (str) : strndup (str, len));

	} else {
		char *result, *temp;
		hash_t *old_list = define_table;
		list_t *old_arg_list = arg_list;
		//arg_list = NULL;

		define_table = hash_init (1, (HASH_REMOVE_CALLBACK) destroy_define_value);
		
		add_define (strdup (define->name), NULL, false)->contents = strdup (define->contents);
		temp = len == -1 ? strdup (str) : strndup (str, len);
		result = expand_expr (temp, false);
		free(temp);

		if (define->contents != NULL)
			free(define->contents);
		define->contents = result;

		hash_free(define_table);
		define_table = old_list;
		arg_list = old_arg_list;
	}
}

#ifdef USE_REUSABLES
/* 
 * write a new reusable label
 */
void add_reusable() {
	reusables[curr_reusable++] = program_counter;
	total_reusables = curr_reusable;
}


/*
 * Returns the index to the current reusable
 */
int get_curr_reusable() {
	return curr_reusable - 1;
}

/*
 * sets the index of the current reusable
 */
int set_curr_reusable(int index) {
	return curr_reusable = index + 1;
}

/*
 * Search reusables
 */
unsigned int search_reusables(int index) {
	if (index == -1 || index >= total_reusables) {
		show_error("Referenced undefined reusable label");
		return 0;
	} else {
		return reusables[index];
	}
}

/*
 * Get the number of reusable labels
 */
int get_num_reusables() {
	return total_reusables;
}

#endif

/*
 * Adds a label with allocated name
 */

label_t *add_label (char *name, int value) {
	label_t *new_label;
	define_t *conflict_define;

	if (!case_sensitive) {
		char *new_name = strup (name);
		free (name);
		name = new_name;
	}

	if ((conflict_define = search_defines(name))) {
		show_error ("conflicting definition of '%s'", name);
		//if (suppress_errors == false) {
			show_error_prefix (conflict_define->input_file, conflict_define->line_num);
			WORD attr = save_console_attributes();
			set_console_attributes (COLOR_RED);
			printf ("previous definition of '%s' was here\n", name);
			restore_console_attributes(attr);
		//}
		return NULL;
	}
	
	if ((new_label = search_labels (name))) {
		if (value != new_label->value) {
			new_label->value = value;
			show_warning ("redefinition of '%s'", name);
			show_warning_prefix (new_label->input_file, new_label->line_num);
			WORD attr = save_console_attributes();
			set_console_attributes (COLOR_YELLOW);
			printf ("previous definition of '%s' was here\n", name);
			restore_console_attributes(attr);
		}
	} else {
		new_label = (label_t *)malloc (sizeof (label_t));
		
		if (new_label != NULL) {
			new_label->name = name;
			new_label->line_num = line_num;
			new_label->input_file = strdup(curr_input_file);
			new_label->value = value;
			
			hash_insert (label_table, new_label);
		}
		
	}
	return new_label;
}


/*
 * Adds a set of arguments
 * returns a pointer to it
 */

list_t *add_arg_set(void) {
	arg_list = list_prepend(arg_list, hash_init(MAX_ARGS, (HASH_REMOVE_CALLBACK) destroy_define_value));
	return arg_list;
}


/*
 * Adds an argument defined within a macro,
 * with allocated contents and name
 */

void add_arg(char *name, char *value, list_t *arg_set) {
	define_t *new_arg;

	//label_t *label = search_labels(name);
	//if (label != NULL) {
	//	SetLastSPASMError(SPASM_ERR_LABEL_CONFLICT, name, label->input_file, label->line_num);
	//}

	if (!case_sensitive) {
		char *new_name = strup(name);
		free(name);
		name = new_name;
	}

	new_arg = (define_t *) malloc(sizeof(define_t));
	memset(new_arg, 0, sizeof(define_t));

	new_arg->name = name;
	new_arg->contents = value;
	new_arg->num_args = 0;
	new_arg->line_num = line_num;
	new_arg->input_file = strdup(curr_input_file);

	hash_insert((hash_t *) arg_set->data, new_arg);
}


/*
 * Removes a set of arguments
 * (used when a macro ends)
 */

void remove_arg_set(list_t *arg_set) {
	//define_t *new_arg;
	if (arg_set == NULL)
		return;
	/*new_arg = (define_t*)arg_list->data;
	free(new_arg->name);
	free(new_arg);*/

	hash_free((hash_t *) arg_set->data);
	arg_list = list_remove (arg_list, arg_set);
	list_free_node(arg_set);
}


/*
 * Searches for a label by name,
 * returns pointer to data if
 * found or NULL if not
 */

label_t *search_labels (const char *name) {
	label_t *result;

	if (!case_sensitive) {
		char *new_name = strup (name);
		result = (label_t *)hash_lookup (label_table, new_name);
		free (new_name);
		return result;
	} else {
		return (label_t *)hash_lookup (label_table, name);
	}
}


/*
 * Returns the number of defines
 */

int get_num_defines () {
	return hash_count (define_table);
}


/*
 * Returns the number of labels
 */

int get_num_labels () {
	return hash_count (label_table);
}
