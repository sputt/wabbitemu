#include "stdafx.h"

#include "breakpoint.h"
#include "core.h"
#include "calc.h"

void add_breakpoint(memc *mem, BREAK_TYPE type, waddr_t waddr)
{
	LPBREAKPOINT new_break = (LPBREAKPOINT)malloc(sizeof(breakpoint_t));
	new_break->active = TRUE;
	new_break->end_addr = waddr.addr % PAGE_SIZE;
	new_break->type = type;
	new_break->waddr = waddr;
	new_break->waddr.addr %= PAGE_SIZE;
	new_break->num_conditions = 0;
	StringCbPrintf(new_break->label, sizeof(new_break->label), _T("%04X"), waddr.addr);
	LPCALC lpCalc = calc_from_memc(mem);
	if (lpCalc != NULL) {
		lpCalc->cond_breakpoints[waddr.is_ram][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] = new_break;
	}
}

void rem_breakpoint(memc *mem, BREAK_TYPE type, waddr_t waddr)
{
	LPCALC lpCalc = calc_from_memc(mem);
	if (lpCalc == NULL) {
		return;
	}

	LPBREAKPOINT lpBreak = lpCalc->cond_breakpoints[waddr.is_ram][PAGE_SIZE * waddr.page + mc_base(waddr.addr)];
	if (lpBreak == NULL) {
		return;
	}

	free(lpBreak);
	lpCalc->cond_breakpoints[waddr.is_ram][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] = NULL;
}

// returns true if it should break, false otherwise
BOOL check_break_callback(memc *mem, BREAK_TYPE type, waddr_t waddr) {
	LPCALC lpCalc = calc_from_memc(mem);
	breakpoint_t *lpBreak = lpCalc->cond_breakpoints[waddr.is_ram][PAGE_SIZE * waddr.page + mc_base(waddr.addr)];
	if (lpBreak == NULL)
		return FALSE;

	//necessary because of page handling
	waddr.addr %= PAGE_SIZE;

	if (!lpBreak->active)
		return FALSE;
	int result = TRUE;
	for (int i = 0; i < lpBreak->num_conditions; i++) {
		switch (lpBreak->conditions[i].type) {
		case CONDITION_HIT_COUNT: {
			condition_hitcount_t *cond = (condition_hitcount_t *)lpBreak->conditions[i].data;
			waddr_t pcWaddr = addr_to_waddr(lpCalc->cpu.mem_c, lpCalc->cpu.pc);
			if (pcWaddr.addr % PAGE_SIZE == waddr.addr && pcWaddr.page == waddr.page && pcWaddr.is_ram == waddr.is_ram) {
				cond->hit_count++;
			}
			switch (cond->condition) {
			case TRIGGER_EQUAL:
				result = cond->hit_count == cond->trigger_value;
				break;
			case TRIGGER_GREATER_EQUAL:
				result = cond->hit_count >= cond->trigger_value;
				break;
			case TRIGGER_MULTIPLE_OF:
				result = !(cond->hit_count % cond->trigger_value);
				break;
			}
			break;
		}
		case CONDITION_REGISTER: {
			condition_register_t *cond = (condition_register_t *)lpBreak->conditions[i].data;
			int value = cond->is_word ? *((uint16_t *)cond->reg) : *((uint8_t *)cond->reg);
			switch (cond->condition) {
			case TRIGGER_EQUAL:
				result = value == cond->trigger_value;
				break;
			case TRIGGER_GREATER_EQUAL:
				result = value >= cond->trigger_value;
				break;
			case TRIGGER_MULTIPLE_OF:
				result = !(value % cond->trigger_value);
				break;
			}
			break;
		}
		case CONDITION_MEMORY: {
			condition_memory_t *cond = (condition_memory_t *)lpBreak->conditions[i].data;
			int value = cond->is_word ? wmem_read16(lpCalc->cpu.mem_c, cond->waddr) :
				wmem_read(lpCalc->cpu.mem_c, cond->waddr);
			switch (cond->condition) {
			case TRIGGER_EQUAL:
				result = value == cond->trigger_value;
				break;
			case TRIGGER_GREATER_EQUAL:
				result = value >= cond->trigger_value;
				break;
			case TRIGGER_MULTIPLE_OF:
				result = !(value % cond->trigger_value);
				break;
			}
			break;
		}
		}
	}
	return TRUE;
}