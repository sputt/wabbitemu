#ifndef __OPCODES_H
#define __OPCODES_H

#include <stdio.h>
#include <stdbool.h>

typedef struct _instr {
	char *args;
	unsigned char instr_data[8];
	int instr_size;
	int min_exectime;
	int max_exectime;
	bool has_end_data;
	unsigned char end_data;
	int size;
} instr;

typedef struct _opcode {
	char *name;
	instr *instrs;
	int num_instrs;
	int use_count;
	struct _opcode *next;
	bool is_added;
} opcode;

extern opcode *all_opcodes;

#endif
