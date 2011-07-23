#ifdef WITH_REVERSE
#include "stdafx.h"

#include "core.h"
#include "indexcb_reverse.h"
//-----------------------------------------
// CB OPCODES


//------------------
// Bit num,reg
void bit_ind(CPU_t *cpu, char offset) {
	int result, reg;
	unsigned short address;
	int test_mask = (1 << ((cpu->bus >> 3) & 0x07));
	tc_add(cpu->timer_c,20);
	if (cpu->prefix == IX_PREFIX) {
		address = cpu->ix + offset;
	} else {
		address = cpu->iy + offset;
	}
	reg = CPU_mem_read(cpu,address);
	result = reg & test_mask;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk16(address) + HC_MASK +
		 x3chk16(address)+ parity(result) + unaffect(CARRY_MASK);
}

//------------------
// RES num,reg
void res_ind(CPU_t *cpu, char offset) {
	int reg;
	int save = (cpu->bus & 0x07);
	unsigned char bit = ~(1 << ((cpu->bus >> 3)& 0x07));
	
	tc_add(cpu->timer_c, 23);
	if (cpu->prefix == IX_PREFIX) {
		reg = CPU_mem_read(cpu, cpu->ix + offset);
		CPU_mem_write(cpu, cpu->ix + offset, reg & bit);
	} else {
		reg = CPU_mem_read(cpu, cpu->iy + offset);
		CPU_mem_write(cpu, cpu->iy + offset, reg & bit);
	}
	
	reg &= bit;
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
void set_ind(CPU_t *cpu, char offset) {
	int reg;
	unsigned char bit = (1 << ((cpu->bus >> 3)& 0x07));
	int save = (cpu->bus & 0x07);
	tc_add(cpu->timer_c, 23);
	if (cpu->prefix == IX_PREFIX) {
		reg = CPU_mem_read(cpu, cpu->ix + offset);
		CPU_mem_write(cpu, cpu->ix + offset, reg | bit);
	} else {
		reg = CPU_mem_read(cpu, cpu->iy + offset);
		CPU_mem_write(cpu, cpu->iy + offset, reg | bit);
	}
	
	reg |= bit;
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


void rl_ind(CPU_t *cpu, char offset) {
	int result;
	int carry;
	int save = (cpu->bus & 0x07);
	tc_add(cpu->timer_c,23);
	if (cpu->prefix == IX_PREFIX) {
		result = CPU_mem_read(cpu, cpu->ix + offset);
		carry = (result>>7)&1;
		result = (result<<1)+(cpu->f&1);
		CPU_mem_write(cpu, cpu->ix + offset, result);
	} else {
		result = CPU_mem_read(cpu, cpu->iy + offset);
		carry = (result >> 7) & 1;
		result = (result << 1) + (cpu->f & 1);
		CPU_mem_write(cpu, cpu->iy + offset, result);
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+
		 parity(result) + carry;
		 

	switch(save) {
		case 0:
			cpu->b = result;
			break;
		case 1:
			cpu->c = result;
			break;
		case 2:
			cpu->d = result;
			break;
		case 3:
			cpu->e = result;
			break;
		case 4:
			cpu->h = result;
			break;
		case 5:
			cpu->l = result;
			break;
		case 7:
			cpu->a = result;
			break;
	}
		 
}

void rlc_ind(CPU_t *cpu, char offset) {
	int result;
	int carry;
	int save = (cpu->bus & 0x07);
	tc_add(cpu->timer_c, 23);
	if (cpu->prefix == IX_PREFIX) {
		result = CPU_mem_read(cpu, cpu->ix + offset);
		carry = (result>>7)&1;
		result = (result << 1) + carry;
		CPU_mem_write(cpu, cpu->ix + offset, result);
	} else {
		result = CPU_mem_read(cpu, cpu->iy + offset);
		carry = (result>>7)&1;
		result = (result<<1) + carry;
		CPU_mem_write(cpu, cpu->iy + offset, result);
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+
		 parity(result) + carry;
		 
	switch(save) {
		case 0:
			cpu->b = result;
			break;
		case 1:
			cpu->c = result;
			break;
		case 2:
			cpu->d = result;
			break;
		case 3:
			cpu->e = result;
			break;
		case 4:
			cpu->h = result;
			break;
		case 5:
			cpu->l = result;
			break;
		case 7:
			cpu->a = result;
			break;
	}
}

void rr_ind(CPU_t *cpu, char offset) {
	int result;
	int carry;
	int save = (cpu->bus & 0x07);
	tc_add(cpu->timer_c,23);
	if (cpu->prefix == IX_PREFIX) {
		result = CPU_mem_read(cpu, cpu->ix + offset);
		carry = result & 1;
		result = (result>>1) + ((cpu->f & 1)<<7);
		CPU_mem_write(cpu, cpu->ix + offset, result);
	} else {
		result = CPU_mem_read(cpu, cpu->iy + offset);
		carry = result & 1;
		result = (result>>1) + ((cpu->f & 1)<<7);
		CPU_mem_write(cpu, cpu->iy + offset, result);
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+
		 parity(result) + carry;

	switch(save) {
		case 0:
			cpu->b = result;
			break;
		case 1:
			cpu->c = result;
			break;
		case 2:
			cpu->d = result;
			break;
		case 3:
			cpu->e = result;
			break;
		case 4:
			cpu->h = result;
			break;
		case 5:
			cpu->l = result;
			break;
		case 7:
			cpu->a = result;
			break;
	}
}

void rrc_ind(CPU_t *cpu, char offset) {
	int result;
	int carry;
	int save = (cpu->bus & 0x07);
	tc_add(cpu->timer_c,23);
	if (cpu->prefix == IX_PREFIX) {
		result = CPU_mem_read(cpu, cpu->ix + offset);
		carry = result & 1;
		result = (result>>1) + (carry << 7);
		CPU_mem_write(cpu, cpu->ix + offset, result);
	} else {
		result = CPU_mem_read(cpu, cpu->iy + offset);
		carry = result & 1;
		result = (result>>1) + (carry << 7);
		CPU_mem_write(cpu, cpu->iy + offset, result);
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+
		 parity(result) + carry;
		 
	switch(save) {
		case 0:
			cpu->b = result;
			break;
		case 1:
			cpu->c = result;
			break;
		case 2:
			cpu->d = result;
			break;
		case 3:
			cpu->e = result;
			break;
		case 4:
			cpu->h = result;
			break;
		case 5:
			cpu->l = result;
			break;
		case 7:
			cpu->a = result;
			break;
	}		 

}

void sll_ind(CPU_t *cpu, char offset) {
	int result;
	int carry;
	int save = (cpu->bus & 0x07);
	tc_add(cpu->timer_c, 23);
	
	if (cpu->prefix == IX_PREFIX) {
		result = CPU_mem_read(cpu, cpu->ix + offset);
		carry = (result>>7)&1;
		result = (result<<1) + 1;
		CPU_mem_write(cpu, cpu->ix + offset, result);
	} else {
		result = CPU_mem_read(cpu, cpu->iy + offset);
		carry = (result>>7)&1;
		result = (result<<1) + 1;
		CPU_mem_write(cpu, cpu->iy + offset, result);
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+
		 parity(result) + carry;
		 
	switch(save) {
		case 0:
			cpu->b = result;
			break;
		case 1:
			cpu->c = result;
			break;
		case 2:
			cpu->d = result;
			break;
		case 3:
			cpu->e = result;
			break;
		case 4:
			cpu->h = result;
			break;
		case 5:
			cpu->l = result;
			break;
		case 7:
			cpu->a = result;
			break;
	}
		 
}

void srl_ind(CPU_t *cpu, char offset) {
	int result;
	int carry;
	int save = (cpu->bus & 0x07);
	tc_add(cpu->timer_c, 23);
	if (cpu->prefix == IX_PREFIX) {
		result = CPU_mem_read(cpu, cpu->ix + offset);
		carry = result & 1;
		result = (result>>1);
		CPU_mem_write(cpu, cpu->ix + offset, result);
	} else {
		result = CPU_mem_read(cpu, cpu->iy + offset);
		carry = result & 1;
		result = (result>>1);
		CPU_mem_write(cpu, cpu->iy + offset, result);
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+
		 parity(result) + carry;
		 
	switch(save) {
		case 0:
			cpu->b = result;
			break;
		case 1:
			cpu->c = result;
			break;
		case 2:
			cpu->d = result;
			break;
		case 3:
			cpu->e = result;
			break;
		case 4:
			cpu->h = result;
			break;
		case 5:
			cpu->l = result;
			break;
		case 7:
			cpu->a = result;
			break;
	}
}

void sla_ind(CPU_t *cpu, char offset) {
	int result;
	int carry;
	int save = (cpu->bus & 0x07);
	tc_add(cpu->timer_c, 23);

	if (cpu->prefix == IX_PREFIX) {
		result = CPU_mem_read(cpu, cpu->ix + offset);
		carry = (result>>7)&1;
		result*=2;
		CPU_mem_write(cpu, cpu->ix + offset, result);
	} else {
		result = CPU_mem_read(cpu, cpu->iy + offset);
		carry = (result>>7)&1;
		result*=2;
		CPU_mem_write(cpu, cpu->iy + offset, result);
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+
		 parity(result) + carry;
		 
	switch(save) {
		case 0:
			cpu->b = result;
			break;
		case 1:
			cpu->c = result;
			break;
		case 2:
			cpu->d = result;
			break;
		case 3:
			cpu->e = result;
			break;
		case 4:
			cpu->h = result;
			break;
		case 5:
			cpu->l = result;
			break;
		case 7:
			cpu->a = result;
			break;
	}

}

void sra_ind(CPU_t *cpu, char offset) {
	int result;
	int carry;
	int save = (cpu->bus & 0x07);
	tc_add(cpu->timer_c, 23);
	if (cpu->prefix == IX_PREFIX) {
		result = CPU_mem_read(cpu, cpu->ix + offset);
		carry = result & 1;
		result = ((result>>1)+(result&0x80))&0xFF;
		CPU_mem_write(cpu, cpu->ix + offset, result);
	} else {
		result = CPU_mem_read(cpu, cpu->iy + offset);
		carry = result & 1;
		result = ((result>>1)+(result&0x80))&0xFF;
		CPU_mem_write(cpu, cpu->iy + offset, result);
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+
		 parity(result) + carry;
		 
	switch(save) {
		case 0:
			cpu->b = result;
			break;
		case 1:
			cpu->c = result;
			break;
		case 2:
			cpu->d = result;
			break;
		case 3:
			cpu->e = result;
			break;
		case 4:
			cpu->h = result;
			break;
		case 5:
			cpu->l = result;
			break;
		case 7:
			cpu->a = result;
			break;
		default:
			break;
	}

}

// END CB OPCODES
//---------------------------------------
#endif