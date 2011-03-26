#ifndef __OPCODES_H
#define __OPCODES_H

typedef struct _instr {
	const char *args;
	unsigned char instr_data[8];
	int instr_size;
	int min_exectime;
	int max_exectime;
	bool has_end_data;
	unsigned char end_data;
	int size;
} instr;

typedef struct _opcode {
	const char *name;
	instr *instrs;
	int num_instrs;
	int use_count;
	struct _opcode *next;
	bool is_added;
} opcode;

extern opcode *all_opcodes;
extern opcode opcode_list[];

#endif
