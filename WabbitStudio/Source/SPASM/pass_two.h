#ifndef __PASS_TWO_H
#define __PASS_TWO_H

//#include <stdbool.h>

typedef enum {
	ARG_NUM_8,
	ARG_NUM_16,
	ARG_ADDR_OFFSET,
	ARG_IX_IY_OFFSET,
	ARG_BIT_NUM
} arg_type;

typedef enum {
	OUTPUT_ECHO,
	OUTPUT_SHOW
} output_type;

typedef struct tagexpr{
	int program_counter;
	int line_num;
	int curr_reusable;
	unsigned char *out_ptr;
	int listing_offset;
	char *expr;
	arg_type type;
	char *input_file;
	bool listing_on;
	int or;
	struct tagexpr *next;
} expr_t;

typedef struct output {
	int program_counter;
	int line_num;
	int curr_reusable;
	char *expr;
	output_type type;
	char *input_file;
	struct output *next;
} output_t;

void add_pass_two_expr (char *expr, arg_type type, int or_value);
void add_pass_two_output (char *expr, output_type type);
void run_second_pass ();

#endif
