#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include "core.h"

typedef enum
{
	CONDITION_HIT_COUNT,
	CONDITION_REGISTER,
	CONDITION_MEMORY,
	CONDITION_TIMER,
	CONDITION_INTERRUPT,
} CONDITIONAL_BREAKPOINT_TYPE;

typedef struct breakpoint_condition
{
	CONDITIONAL_BREAKPOINT_TYPE type;
	void *data;

} breakpoint_condition_t;

typedef enum {
	TRIGGER_EQUAL,
	TRIGGER_GREATER_EQUAL,
	TRIGGER_MULTIPLE_OF,
} TRIGGER_CONDITION;

typedef struct
{
	int hit_count;
	TRIGGER_CONDITION condition;
	int trigger_value;
} condition_hitcount_t;

typedef struct
{
	void *reg;
	BOOL is_word;
	TRIGGER_CONDITION condition;
	int trigger_value;
} condition_register_t;

typedef struct
{
	waddr_t waddr;
	BOOL is_word;
	TRIGGER_CONDITION condition;
	int trigger_value;
} condition_memory_t;

typedef struct breakpoint
{
	TCHAR label[32];
	BREAK_TYPE type;
	waddr_t waddr;
	uint16_t end_addr;			//end of block memory 
	BOOL active;
	breakpoint_condition_t conditions[10];
	int num_conditions;
} breakpoint_t, *LPBREAKPOINT;

void add_breakpoint(memc *mem, BREAK_TYPE type, waddr_t waddr);
void rem_breakpoint(memc *mem, BREAK_TYPE type, waddr_t waddr);
BOOL check_break_callback(memc *mem, BREAK_TYPE type, waddr_t waddr);

#endif // BREAKPOINT_H