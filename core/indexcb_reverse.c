#ifdef WITH_REVERSE
#include "stdafx.h"

#include "core.h"
#include "indexcb_reverse.h"
//-----------------------------------------
// CB OPCODES


//------------------
// Bit num,reg
void bit_ind_reverse(CPU_t *cpu, char offset) {
	tc_sub(cpu->timer_c, 20);
	cpu->f = cpu->prev_instruction->flag;
}

//------------------
// RES num,reg
void res_ind_reverse(CPU_t *cpu, char offset) {
	int reg;
	int save = (cpu->bus & 0x07);
	unsigned char bit = ~(1 << ((cpu->bus >> 3)& 0x07));
	
	tc_sub(cpu->timer_c, 23);
	if (cpu->prefix == IX_PREFIX) {
		CPU_mem_write(cpu, cpu->ix + offset, cpu->prev_instruction->lower_data2);
	} else {
		CPU_mem_write(cpu, cpu->iy + offset, cpu->prev_instruction->lower_data2);
	}
	
	reg = cpu->prev_instruction->lower_data1;
	switch(save) {
		case 0:
			cpu->b = reg;
			break;
		case 1:
			cpu->c = reg;
			break;
		case 2:
			cpu->d = reg;
			break;
		case 3:
			cpu->e = reg;
			break;
		case 4:
			cpu->h = reg;
			break;
		case 5:
			cpu->l = reg;
			break;
		case 7:
			cpu->a = reg;
			break;
	}
}

//------------------
// SET num,reg
void set_ind_reverse(CPU_t *cpu, char offset) {
	int reg;
	unsigned char bit = 1 << ((cpu->bus >> 3) & 0x07);
	int save = cpu->bus & 0x07;
	tc_sub(cpu->timer_c, 23);
	if (cpu->prefix == IX_PREFIX) {
		reg = CPU_mem_read(cpu, cpu->ix + offset);
		CPU_mem_write(cpu, cpu->ix + offset, cpu->prev_instruction->lower_data2);
	} else {
		reg = CPU_mem_read(cpu, cpu->iy + offset);
		CPU_mem_write(cpu, cpu->iy + offset, cpu->prev_instruction->lower_data2);
	}
	
	reg = cpu->prev_instruction->lower_data1;
	switch(save) {
		case 0:
			cpu->b = reg;
			break;
		case 1:
			cpu->c = reg;
			break;
		case 2:
			cpu->d = reg;
			break;
		case 3:
			cpu->e = reg;
			break;
		case 4:
			cpu->h = reg;
			break;
		case 5:
			cpu->l = reg;
			break;
		case 7:
			cpu->a = reg;
			break;
	}

}


void rl_ind_reverse(CPU_t *cpu, char offset) {
	int save = cpu->bus & 0x07;
	tc_sub(cpu->timer_c, 23);
	cpu->f = cpu->prev_instruction->flag;

	if (cpu->prefix == IX_PREFIX) {
		CPU_mem_write(cpu, cpu->ix + offset, cpu->prev_instruction->lower_data1);
	} else {;
		CPU_mem_write(cpu, cpu->iy + offset, cpu->prev_instruction->lower_data1);
	}
		 

	switch(save) {
		case 0:
			cpu->b = cpu->prev_instruction->lower_data2;
			break;
		case 1:
			cpu->c = cpu->prev_instruction->lower_data2;
			break;
		case 2:
			cpu->d = cpu->prev_instruction->lower_data2;
			break;
		case 3:
			cpu->e = cpu->prev_instruction->lower_data2;
			break;
		case 4:
			cpu->h = cpu->prev_instruction->lower_data2;
			break;
		case 5:
			cpu->l = cpu->prev_instruction->lower_data2;
			break;
		case 7:
			cpu->a = cpu->prev_instruction->lower_data2;
			break;
	}
}

void rlc_ind_reverse(CPU_t *cpu, char offset) {
	int save = cpu->bus & 0x07;
	tc_sub(cpu->timer_c, 23);
	if (cpu->prefix == IX_PREFIX) {
		CPU_mem_write(cpu, cpu->ix + offset, cpu->prev_instruction->lower_data1);
	} else {
		CPU_mem_write(cpu, cpu->iy + offset, cpu->prev_instruction->lower_data1);
	}
	cpu->f = cpu->prev_instruction->flag;
		 
	switch (save) {
		case 0:
			cpu->b = cpu->prev_instruction->lower_data2;
			break;
		case 1:
			cpu->c = cpu->prev_instruction->lower_data2;
			break;
		case 2:
			cpu->d = cpu->prev_instruction->lower_data2;
			break;
		case 3:
			cpu->e = cpu->prev_instruction->lower_data2;
			break;
		case 4:
			cpu->h = cpu->prev_instruction->lower_data2;
			break;
		case 5:
			cpu->l = cpu->prev_instruction->lower_data2;
			break;
		case 7:
			cpu->a = cpu->prev_instruction->lower_data2;
			break;
	}
}

void rr_ind_reverse(CPU_t *cpu, char offset) {
	int save = cpu->bus & 0x07;
	tc_sub(cpu->timer_c, 23);
	if (cpu->prefix == IX_PREFIX) {
		CPU_mem_write(cpu, cpu->ix + offset, cpu->prev_instruction->lower_data1);
	} else {
		CPU_mem_write(cpu, cpu->iy + offset, cpu->prev_instruction->lower_data1);
	}
	cpu->f = cpu->prev_instruction->flag;

	switch(save) {
		case 0:
			cpu->b = cpu->prev_instruction->lower_data2;
			break;
		case 1:
			cpu->c = cpu->prev_instruction->lower_data2;
			break;
		case 2:
			cpu->d = cpu->prev_instruction->lower_data2;
			break;
		case 3:
			cpu->e = cpu->prev_instruction->lower_data2;
			break;
		case 4:
			cpu->h = cpu->prev_instruction->lower_data2;
			break;
		case 5:
			cpu->l = cpu->prev_instruction->lower_data2;
			break;
		case 7:
			cpu->a = cpu->prev_instruction->lower_data2;
			break;
	}
}

void rrc_ind_reverse(CPU_t *cpu, char offset) {
	int save = (cpu->bus & 0x07);
	tc_sub(cpu->timer_c,23);
	if (cpu->prefix == IX_PREFIX) {
		CPU_mem_write(cpu, cpu->ix + offset, cpu->prev_instruction->lower_data1);
	} else {
		CPU_mem_write(cpu, cpu->iy + offset, cpu->prev_instruction->lower_data1);
	}
	cpu->f = cpu->prev_instruction->flag;
		 
	switch(save) {
		case 0:
			cpu->b = cpu->prev_instruction->lower_data2;
			break;
		case 1:
			cpu->c = cpu->prev_instruction->lower_data2;
			break;
		case 2:
			cpu->d = cpu->prev_instruction->lower_data2;
			break;
		case 3:
			cpu->e = cpu->prev_instruction->lower_data2;
			break;
		case 4:
			cpu->h = cpu->prev_instruction->lower_data2;
			break;
		case 5:
			cpu->l = cpu->prev_instruction->lower_data2;
			break;
		case 7:
			cpu->a = cpu->prev_instruction->lower_data2;
			break;
	}		 

}

void sll_ind_reverse(CPU_t *cpu, char offset) {
	int save = cpu->bus & 0x07;
	tc_sub(cpu->timer_c, 23);
	
	if (cpu->prefix == IX_PREFIX) {
		CPU_mem_write(cpu, cpu->ix + offset, cpu->prev_instruction->lower_data1);
	} else {
		CPU_mem_write(cpu, cpu->iy + offset, cpu->prev_instruction->lower_data1);
	}
	cpu->f = cpu->prev_instruction->flag;
		 
	switch(save) {
		case 0:
			cpu->b = cpu->prev_instruction->lower_data2;
			break;
		case 1:
			cpu->c = cpu->prev_instruction->lower_data2;
			break;
		case 2:
			cpu->d = cpu->prev_instruction->lower_data2;
			break;
		case 3:
			cpu->e = cpu->prev_instruction->lower_data2;
			break;
		case 4:
			cpu->h = cpu->prev_instruction->lower_data2;
			break;
		case 5:
			cpu->l = cpu->prev_instruction->lower_data2;
			break;
		case 7:
			cpu->a = cpu->prev_instruction->lower_data2;
			break;
	}
		 
}

void srl_ind_reverse(CPU_t *cpu, char offset) {
	int save = (cpu->bus & 0x07);
	tc_sub(cpu->timer_c, 23);
	if (cpu->prefix == IX_PREFIX) {
		CPU_mem_write(cpu, cpu->ix + offset, cpu->prev_instruction->lower_data1);
	} else {
		CPU_mem_write(cpu, cpu->iy + offset, cpu->prev_instruction->lower_data1);
	}
	cpu->f = cpu->prev_instruction->flag;
		 
	switch(save) {
		case 0:
			cpu->b = cpu->prev_instruction->lower_data2;
			break;
		case 1:
			cpu->c = cpu->prev_instruction->lower_data2;
			break;
		case 2:
			cpu->d = cpu->prev_instruction->lower_data2;
			break;
		case 3:
			cpu->e = cpu->prev_instruction->lower_data2;
			break;
		case 4:
			cpu->h = cpu->prev_instruction->lower_data2;
			break;
		case 5:
			cpu->l = cpu->prev_instruction->lower_data2;
			break;
		case 7:
			cpu->a = cpu->prev_instruction->lower_data2;
			break;
	}
}

void sla_ind_reverse(CPU_t *cpu, char offset) {
	int save = cpu->bus & 0x07;
	tc_sub(cpu->timer_c, 23);

	if (cpu->prefix == IX_PREFIX) {
		CPU_mem_write(cpu, cpu->ix + offset, cpu->prev_instruction->lower_data1);
	} else {
		CPU_mem_write(cpu, cpu->iy + offset, cpu->prev_instruction->lower_data1);
	}
	cpu->f = cpu->prev_instruction->flag;
		 
	switch(save) {
		case 0:
			cpu->b = cpu->prev_instruction->lower_data2;
			break;
		case 1:
			cpu->c = cpu->prev_instruction->lower_data2;
			break;
		case 2:
			cpu->d = cpu->prev_instruction->lower_data2;
			break;
		case 3:
			cpu->e = cpu->prev_instruction->lower_data2;
			break;
		case 4:
			cpu->h = cpu->prev_instruction->lower_data2;
			break;
		case 5:
			cpu->l = cpu->prev_instruction->lower_data2;
			break;
		case 7:
			cpu->a = cpu->prev_instruction->lower_data2;
			break;
	}

}

void sra_ind_reverse(CPU_t *cpu, char offset) {
	int save = (cpu->bus & 0x07);
	tc_sub(cpu->timer_c, 23);
	if (cpu->prefix == IX_PREFIX) {
		CPU_mem_write(cpu, cpu->ix + offset, cpu->prev_instruction->lower_data1);
	} else {
		CPU_mem_write(cpu, cpu->iy + offset, cpu->prev_instruction->lower_data1);
	}
	cpu->f = cpu->prev_instruction->flag;
		 
	switch(save) {
		case 0:
			cpu->b = cpu->prev_instruction->lower_data2;
			break;
		case 1:
			cpu->c = cpu->prev_instruction->lower_data2;
			break;
		case 2:
			cpu->d = cpu->prev_instruction->lower_data2;
			break;
		case 3:
			cpu->e = cpu->prev_instruction->lower_data2;
			break;
		case 4:
			cpu->h = cpu->prev_instruction->lower_data2;
			break;
		case 5:
			cpu->l = cpu->prev_instruction->lower_data2;
			break;
		case 7:
			cpu->a = cpu->prev_instruction->lower_data2;
			break;
		default:
			break;
	}

}

// END CB OPCODES
//---------------------------------------
#endif