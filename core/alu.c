#include "stdafx.h"

#include "core.h"
#include "alu.h"

#pragma warning(push)
// byte conversion
#pragma warning(disable : 4244)
// uninit var
#pragma warning(disable : 4701)

//---------------------------------------------
// ED OPCODES

int neg(CPU_t *cpu) {
	int result = -cpu->a;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + hcsubchk(0,cpu->a,0) + 
		 x3chk(result)+ vchksub(0,cpu->a,result) + 
		 SUB_INSTR +  carrychk(result);
	cpu->a = result;

	return 8;
}

int adc_hl_reg16(CPU_t *cpu) {
	int result;
	int reg;

	switch (((cpu->bus)>>4)&0x03) {
		case 0x00:
			reg = cpu->bc;
			break;
		case 0x01:
			reg = cpu->de;
			break;
		case 0x02:
			reg = cpu->hl;
			break;
		case 0x03:
			reg = cpu->sp;
			break;
	}
	result = cpu->hl+reg+ (cpu->f&CARRY_MASK);
	cpu->f = signchk16(result) + zerochk16(result) + 
		 x5chk16(result) + hcaddchk16(cpu->hl,reg,cpu->f&CARRY_MASK) + 
		 x3chk16(result)+ vchkadd16(cpu->hl,reg,result) + 
		 ADD_INSTR +  carrychk16(result);
	cpu->hl = result;

	return 15;
}

int sbc_hl_reg16(CPU_t *cpu) {
	int result;
	int reg;

	switch (((cpu->bus)>>4)&0x03) {
		case 0x00:
			reg = cpu->bc;
			break;
		case 0x01:
			reg = cpu->de;
			break;
		case 0x02:
			reg = cpu->hl;
			break;
		case 0x03:
			reg = cpu->sp;
			break;
	}
	result = cpu->hl - reg - (cpu->f&CARRY_MASK);

	cpu->f = signchk16(result) + zerochk16(result) + 
		 x5chk16(result) + hcsubchk16(cpu->hl,reg,cpu->f&CARRY_MASK) + 
		 x3chk16(result)+ vchksub16(cpu->hl, reg, result) +                //DOUBLE CHECK!!!!
		 SUB_INSTR +  carrychk16(result);
	cpu->hl = result;

	return 15;
}



int cpd(CPU_t *cpu) {
	int result;
	int reg;

	reg = CPU_mem_read(cpu,cpu->hl);
	result = cpu->a - reg;
	cpu->bc--;
	cpu->hl--;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(reg-((cpu->f&HC_MASK)>>4)) + hcsubchk(cpu->a,reg,0) + 
		 x3chk(reg-((cpu->f&HC_MASK)>>4))+ doparity(cpu->bc!=0) + 
		 SUB_INSTR +  unaffect(CARRY_MASK);

	return 16;
}

int cpdr(CPU_t *cpu) {
	int result;
	int reg;

	reg = CPU_mem_read(cpu,cpu->hl);
	result = cpu->a - reg;
	cpu->bc--;
	cpu->hl--;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(reg-((cpu->f&HC_MASK)>>4)) + hcsubchk(cpu->a,reg,0) + 
		 x3chk(reg-((cpu->f&HC_MASK)>>4))+ doparity(cpu->bc!=0) + 
		 SUB_INSTR +  unaffect(CARRY_MASK);
	if ((cpu->f&PV_MASK)!=0 && (cpu->f&ZERO_MASK)==0 ) {
		cpu->pc-=2;
		return 21;
	}
	
	return 16;
}

int cpi(CPU_t *cpu) {
	int result;
	int reg;
	reg = CPU_mem_read(cpu,cpu->hl);
	result = cpu->a - reg;
	cpu->bc--;
	cpu->hl++;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(reg-((cpu->f&HC_MASK)>>4)) + hcsubchk(cpu->a,reg,0) + 
		 x3chk(reg-((cpu->f&HC_MASK)>>4))+ doparity(cpu->bc!=0) + 
		 SUB_INSTR +  unaffect(CARRY_MASK);
	return 16;
}

int cpir(CPU_t *cpu) {
	int result;
	int reg = CPU_mem_read(cpu,cpu->hl);
	result = cpu->a - reg;
	cpu->bc--;
	cpu->hl++;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(reg-((cpu->f&HC_MASK)>>4)) + hcsubchk(cpu->a,reg,0) + 
		 x3chk(reg-((cpu->f&HC_MASK)>>4))+ doparity(cpu->bc!=0) + 
		 SUB_INSTR +  unaffect(CARRY_MASK);
	if ((cpu->f&PV_MASK)!=0 && (cpu->f&ZERO_MASK)==0 ) {
		cpu->pc-=2;
		return 21;
	}

	return 16;
}


int rld(CPU_t *cpu) {
	int result = (CPU_mem_read(cpu,cpu->hl)<<4)+(cpu->a&0x0f);
	CPU_mem_write(cpu,cpu->hl,result&0xff);
	cpu->a = (cpu->a&0xF0)+((result>>8)&0x0F);
	cpu->f = signchk(cpu->a) + zerochk(cpu->a) +
		 x5chk(cpu->a) + x3chk(cpu->a)+ 
		 parity(cpu->a) + unaffect(CARRY_MASK);

	return 18;
}
int rrd(CPU_t *cpu) {
	int result,tmp;

	result = (CPU_mem_read(cpu,cpu->hl)>>4)+((cpu->a&0x0f)<<4);
	tmp = cpu->bus;
	CPU_mem_write(cpu,cpu->hl,result&0xff);
	cpu->a = (cpu->a&0xF0)+(tmp&0x0F);
	cpu->f = signchk(cpu->a) + zerochk(cpu->a) +
		 x5chk(cpu->a) + x3chk(cpu->a)+ 
		 parity(cpu->a) + unaffect(CARRY_MASK);

	return 18;
}


//-----------------------------------------
// CB OPCODES


//------------------
// Bit num,reg
int bit(CPU_t *cpu) {
	int result;
	int reg;
	int dbus = cpu->bus;
	int time = 8;

	switch ((cpu->bus)&0x07) {
		case 0x00:
			reg = cpu->b;
			break;
		case 0x01:
			reg = cpu->c;
			break;
		case 0x02:
			reg = cpu->d;
			break;
		case 0x03:
			reg = cpu->e;
			break;
		case 0x04:
			reg = cpu->h;
			break;
		case 0x05:
			reg = cpu->l;
			break;
		case 0x06:
			reg = CPU_mem_read(cpu,cpu->hl);
			time += 4;
			break;
		case 0x07:
			reg = cpu->a;
			break;
	}
	result = reg & (1<<((dbus>>3)&0x07));
	int xchk;
	if ( (dbus&0x07)!=0x06 ) xchk = result;
	else xchk = cpu->h;
		xchk = result;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(xchk) + HC_MASK + 
		 x3chk(xchk)+ parity(result) + unaffect(CARRY_MASK);

	return time;
}
//------------------
// RES num,reg
int res(CPU_t *cpu) {
	int reg;
	int bit =	(~(1<<((cpu->bus>>3)&0x07)))&0xFF;
	int time = 8;
	switch ((cpu->bus)&0x07) {
		case 0x00:
			cpu->b &= bit;
			break;
		case 0x01:
			cpu->c &= bit;
			break;
		case 0x02:
			cpu->d &= bit;
			break;
		case 0x03:
			cpu->e &= bit;
			break;
		case 0x04:
			cpu->h &= bit;
			break;
		case 0x05:
			cpu->l &= bit;
			break;
		case 0x06:
			reg = CPU_mem_read(cpu,cpu->hl);
			time += 7;
			CPU_mem_write(cpu,cpu->hl,reg&bit);
			break;
		case 0x07:
			cpu->a &= bit;
			break;
	}

	return time;
}
//------------------
// SET num,reg
int set(CPU_t *cpu) {
	int reg;
	int bit = (1<<((cpu->bus>>3)&0x7))&0xFF;
	int time = 8;

	switch ((cpu->bus)&0x07) {
		case 0x00:
			cpu->b |= bit;
			break;
		case 0x01:
			cpu->c |= bit;
			break;
		case 0x02:
			cpu->d |= bit;
			break;
		case 0x03:
			cpu->e |= bit;
			break;
		case 0x04:
			cpu->h |= bit;
			break;
		case 0x05:
			cpu->l |= bit;
			break;
		case 0x06:
			reg = CPU_mem_read(cpu,cpu->hl);
			time += 7;
			CPU_mem_write(cpu,cpu->hl,reg|bit);
			break;
		case 0x07:
			cpu->a |= bit;
			break;
	}

	return time;
}


int rl_reg(CPU_t *cpu) {
	int result;
	int carry;
	int time = 8;
	switch ((cpu->bus)&0x07) {
		case 0x00:
			carry = (cpu->b>>7)&0x01;
			result = cpu->b = ((cpu->b<<1)+(cpu->f&0x01))&0xFF;
			break;
		case 0x01:
			carry = (cpu->c>>7)&0x01;
			result = cpu->c = ((cpu->c<<1)+(cpu->f&0x01))&0xFF;
			break;
		case 0x02:
			carry = (cpu->d>>7)&0x01;
			result = cpu->d = ((cpu->d<<1)+(cpu->f&0x01))&0xFF;
			break;
		case 0x03:
			carry = (cpu->e>>7)&0x01;
			result = cpu->e = ((cpu->e<<1)+(cpu->f&0x01))&0xFF;
			break;
		case 0x04:
			carry = (cpu->h>>7)&0x01;
			result = cpu->h = ((cpu->h<<1)+(cpu->f&0x01))&0xFF;
			break;
		case 0x05:
			carry = (cpu->l>>7)&0x01;
			result = cpu->l = ((cpu->l<<1)+(cpu->f&0x01))&0xFF;
			break;
		case 0x06:
			result = CPU_mem_read(cpu,cpu->hl);
			carry = (result>>7)&0x01;
			result = ((result<<1)+(cpu->f&0x01))&0xFF;
			time += 7;
			CPU_mem_write(cpu,cpu->hl,result);
			break;
		case 0x07:
			carry = (cpu->a>>7)&0x01;
			result = cpu->a = ((cpu->a<<1)+(cpu->f&0x01))&0xFF;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+ 
		 parity(result) + carry;

	return time;
}

int rlc_reg(CPU_t *cpu) {
	int result;
	int carry;
	int time = 8;
	switch ((cpu->bus)&0x07) {
		case 0x00:
			carry = (cpu->b>>7)&1;
			result = cpu->b = ((cpu->b<<1)+carry)&0xFF;
			break;
		case 0x01:
			carry = (cpu->c>>7)&1;
			result = cpu->c = ((cpu->c<<1)+carry)&0xFF;
			break;
		case 0x02:
			carry = (cpu->d>>7)&1;
			result = cpu->d = ((cpu->d<<1)+carry)&0xFF;
			break;
		case 0x03:
			carry = (cpu->e>>7)&1;
			result = cpu->e = ((cpu->e<<1)+carry)&0xFF;
			break;
		case 0x04:
			carry = (cpu->h>>7)&1;
			result = cpu->h = ((cpu->h<<1)+carry)&0xFF;
			break;
		case 0x05:
			carry = (cpu->l>>7)&1;
			result = cpu->l = ((cpu->l<<1)+carry)&0xFF;
			break;
		case 0x06:
			result = CPU_mem_read(cpu,cpu->hl);
			carry = (result>>7)&1;
			result = ((result<<1)+carry)&0xFF;
			time += 7;
			CPU_mem_write(cpu,cpu->hl,result);
			break;
		case 0x07:
			carry = (cpu->a>>7)&1;
			result = cpu->a = ((cpu->a<<1)+carry)&0xFF;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+ 
		 parity(result) + carry;
	return time;
}

int rr_reg(CPU_t *cpu) {
	int result;
	int carry;
	int time = 8;
	switch ((cpu->bus)&7) {
		case 0x00:
			carry = cpu->b&1;
			result = cpu->b = ((cpu->b>>1)+((cpu->f&1)<<7))&0xFF;
			break;
		case 0x01:
			carry = cpu->c&1;
			result = cpu->c = ((cpu->c>>1)+((cpu->f&1)<<7))&0xFF;
			break;
		case 0x02:
			carry = cpu->d&1;
			result = cpu->d = ((cpu->d>>1)+((cpu->f&1)<<7))&0xFF;
			break;
		case 0x03:
			carry = cpu->e&1;
			result = cpu->e = ((cpu->e>>1)+((cpu->f&1)<<7))&0xFF;
			break;
		case 0x04:
			carry = cpu->h&1;
			result = cpu->h = ((cpu->h>>1)+((cpu->f&1)<<7))&0xFF;
			break;
		case 0x05:
			carry = cpu->l&1;
			result = cpu->l = ((cpu->l>>1)+((cpu->f&1)<<7))&0xFF;
			break;
		case 0x06:
			result = CPU_mem_read(cpu,cpu->hl);
			carry = result&1;
			result = ((result>>1)+((cpu->f&1)<<7))&0xFF;
			time += 7;
			CPU_mem_write(cpu,cpu->hl,result);
			break;
		case 0x07:
			carry = cpu->a&1;
			result = cpu->a = ((cpu->a>>1)+((cpu->f&1)<<7))&0xFF;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+ 
		 parity(result) + carry;
	return time;
}
int rrc_reg(CPU_t *cpu) {
	int result;
	int carry;
	int time = 8;
	switch ((cpu->bus)&7) {
		case 0x00:
			carry = cpu->b&1;
			result = cpu->b = ((cpu->b>>1)+(carry<<7))&0xFF;
			break;
		case 0x01:
			carry = cpu->c&1;
			result = cpu->c = ((cpu->c>>1)+(carry<<7))&0xFF;
			break;
		case 0x02:
			carry = cpu->d&1;
			result = cpu->d = ((cpu->d>>1)+(carry<<7))&0xFF;
			break;
		case 0x03:
			carry = cpu->e&1;
			result = cpu->e = ((cpu->e>>1)+(carry<<7))&0xFF;
			break;
		case 0x04:
			carry = cpu->h&1;
			result = cpu->h = ((cpu->h>>1)+(carry<<7))&0xFF;
			break;
		case 0x05:
			carry = cpu->l&1;
			result = cpu->l = ((cpu->l>>1)+(carry<<7))&0xFF;
			break;
		case 0x06:
			result = CPU_mem_read(cpu,cpu->hl);
			carry = result&1;
			result = ((result>>1)+(carry<<7))&0xFF;
			time += 7;
			CPU_mem_write(cpu,cpu->hl,result);
			break;
		case 0x07:
			carry = cpu->a&1;
			result = cpu->a = ((cpu->a>>1)+(carry<<7))&0xFF;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+ 
		 parity(result) + carry;
	return time;
}

int sll_reg(CPU_t *cpu) {
	int result;
	int carry;
	int time = 8;
	switch ((cpu->bus)&7) {
		case 0x00:
			carry = (cpu->b>>7)&1;
			result = cpu->b = ((cpu->b<<1)+1)&0xFF;
			break;
		case 0x01:
			carry = (cpu->c>>7)&1;
			result = cpu->c = ((cpu->c<<1)+1)&0xFF;
			break;
		case 0x02:
			carry = (cpu->d>>7)&1;
			result = cpu->d = ((cpu->d<<1)+1)&0xFF;
			break;
		case 0x03:
			carry = (cpu->e>>7)&1;
			result = cpu->e = ((cpu->e<<1)+1)&0xFF;
			break;
		case 0x04:
			carry = (cpu->h>>7)&1;
			result = cpu->h = ((cpu->h<<1)+1)&0xFF;
			break;
		case 0x05:
			carry = (cpu->l>>7)&1;
			result = cpu->l = ((cpu->l<<1)+1)&0xFF;
			break;
		case 0x06:
			result = CPU_mem_read(cpu,cpu->hl);
			carry = (result>>7)&1;
			result = ((result<<1)+1)&0xFF;
			time += 7;
			CPU_mem_write(cpu,cpu->hl,result);
			break;
		case 0x07:
			carry = (cpu->a>>7)&1;
			result = cpu->a = ((cpu->a<<1)+1)&0xFF;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+ 
		 parity(result) + carry;
	return time;
}
int sla_reg(CPU_t *cpu) {
	int result;
	int carry;
	int time = 8;
	switch ((cpu->bus)&7) {
		case 0x00:
			carry = (cpu->b>>7)&1;
			result = cpu->b = ((cpu->b<<1))&0xFF;
			break;
		case 0x01:
			carry = (cpu->c>>7)&1;
			result = cpu->c = ((cpu->c<<1))&0xFF;
			break;
		case 0x02:
			carry = (cpu->d>>7)&1;
			result = cpu->d = ((cpu->d<<1))&0xFF;
			break;
		case 0x03:
			carry = (cpu->e>>7)&1;
			result = cpu->e = ((cpu->e<<1))&0xFF;
			break;
		case 0x04:
			carry = (cpu->h>>7)&1;
			result = cpu->h = ((cpu->h<<1))&0xFF;
			break;
		case 0x05:
			carry = (cpu->l>>7)&1;
			result = cpu->l = ((cpu->l<<1))&0xFF;
			break;
		case 0x06:
			result = CPU_mem_read(cpu,cpu->hl);
			carry = (result>>7)&1;
			result = ((result<<1))&0xFF;
			time += 7;
			CPU_mem_write(cpu,cpu->hl,result);
			break;
		case 0x07:
			carry = (cpu->a>>7)&1;
			result = cpu->a = ((cpu->a<<1))&0xFF;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+ 
		 parity(result) + carry;

	return time;
}

int sra_reg(CPU_t *cpu) {
	int result;
	int carry;
	int time = 8;
	switch ((cpu->bus)&7) {
		case 0x00:
			carry = cpu->b&1;
			result = cpu->b = ((cpu->b>>1)+(cpu->b&128))&0xFF;
			break;
		case 0x01:
			carry = (cpu->c)&1;
			result = cpu->c = ((cpu->c>>1)+(cpu->c&128))&0xFF;
			break;
		case 0x02:
			carry = (cpu->d)&1;
			result = cpu->d = ((cpu->d>>1)+(cpu->d&128))&0xFF;
			break;
		case 0x03:
			carry = (cpu->e)&1;
			result = cpu->e = ((cpu->e>>1)+(cpu->e&128))&0xFF;
			break;
		case 0x04:
			carry = (cpu->h)&1;
			result = cpu->h = ((cpu->h>>1)+(cpu->h&128))&0xFF;
			break;
		case 0x05:
			carry = (cpu->l)&1;
			result = cpu->l = ((cpu->l>>1)+(cpu->l&128))&0xFF;
			break;
		case 0x06:
			result = CPU_mem_read(cpu,cpu->hl);
			carry = (result)&1;
			result = ((result>>1)+(result&128))&0xFF;
			time += 7;
			CPU_mem_write(cpu,cpu->hl,result);
			break;
		case 0x07:
			carry = (cpu->a)&1;
			result = cpu->a = ((cpu->a>>1)+(cpu->a&128))&0xFF;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+ 
		 parity(result) + carry;

	return time;
}
int srl_reg(CPU_t *cpu) {
	int result;
	int carry;
	int time = 8;
	switch ((cpu->bus)&7) {
		case 0x00:
			carry = cpu->b&1;
			result = cpu->b = ((cpu->b>>1))&0xFF;
			break;
		case 0x01:
			carry = (cpu->c)&1;
			result = cpu->c = ((cpu->c>>1))&0xFF;
			break;
		case 0x02:
			carry = (cpu->d)&1;
			result = cpu->d = ((cpu->d>>1))&0xFF;
			break;
		case 0x03:
			carry = (cpu->e)&1;
			result = cpu->e = ((cpu->e>>1))&0xFF;
			break;
		case 0x04:
			carry = (cpu->h)&1;
			result = cpu->h = ((cpu->h>>1))&0xFF;
			break;
		case 0x05:
			carry = (cpu->l)&1;
			result = cpu->l = ((cpu->l>>1))&0xFF;
			break;
		case 0x06:
			result = CPU_mem_read(cpu,cpu->hl);
			carry = (result)&1;
			result = ((result>>1))&0xFF;
			time += 7;
			CPU_mem_write(cpu,cpu->hl,result);
			break;
		case 0x07:
			carry = (cpu->a)&1;
			result = cpu->a = ((cpu->a>>1))&0xFF;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+ 
		 parity(result) + carry;
	return time;
}
// END CB OPCODES
//---------------------------------------


//-----------------
// and reg8
int and_reg8(CPU_t *cpu) {
	int result;
	int reg;

	int time = 4;
	switch ((cpu->bus)&7) {
		case 0x00:
			reg = cpu->b;
			break;
		case 0x01:
			reg = cpu->c;
			break;
		case 0x02:
			reg = cpu->d;
			break;
		case 0x03:
			reg = cpu->e;
			break;
		case 0x04:
			index_ext (
				reg = cpu->h;,
				reg = cpu->ixh; time += 4;,
				reg = cpu->iyh; time += 4;
			)
			break;
		case 0x05:
			index_ext (
				reg = cpu->l;,
				reg = cpu->ixl; time += 4;,
				reg = cpu->iyl; time += 4;
			)
			break;
		case 0x06:
			if (!cpu->prefix) {
				reg = CPU_mem_read(cpu,cpu->hl);
				time += 3;
			} else {
				char offset = CPU_mem_read(cpu, cpu->pc++);
				if (cpu->prefix == 0xDD) {
					reg = CPU_mem_read(cpu, cpu->ix + offset);
				} else {
					reg = CPU_mem_read(cpu, cpu->iy + offset);
				}
				
				time += 15;
			}
			break;
		case 0x07:
			reg = cpu->a;
			break;
	}
	result = cpu->a & reg;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + HC_MASK + 
		 x3chk(result)+ parity(result);
	cpu->a = result;
	return time;
}



//-----------------
// and num8
int and_num8(CPU_t *cpu) {
	int result;
	int reg;
	
	reg = CPU_mem_read(cpu,cpu->pc++);
	result = cpu->a&reg;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + HC_MASK + 
		 x3chk(result)+ parity(result);
	cpu->a = result;
	return 7;
}



//-----------------
// or reg8
int or_reg8(CPU_t *cpu) {
	int result;
	int reg;
	int time = 4;
	switch ((cpu->bus)&7) {
		case 0x00:
			reg = cpu->b;
			break;
		case 0x01:
			reg = cpu->c;
			break;
		case 0x02:
			reg = cpu->d;
			break;
		case 0x03:
			reg = cpu->e;
			break;
		case 0x04:
			index_ext (
				reg = cpu->h;,
				reg = cpu->ixh; time += 4;,
				reg = cpu->iyh; time += 4;
			)
			break;
		case 0x05:
			index_ext (
				reg = cpu->l;,
				reg = cpu->ixl; time += 4;,
				reg = cpu->iyl; time += 4;
			)
			break;
		case 0x06:
			if (!cpu->prefix) {
				reg = CPU_mem_read(cpu,cpu->hl);
				time += 3;
			} else {
				char offset = CPU_mem_read(cpu, cpu->pc++);
				if (cpu->prefix == 0xDD) {
					reg = CPU_mem_read(cpu, cpu->ix + offset);
				} else {
					reg = CPU_mem_read(cpu, cpu->iy + offset);
				}
				time += 15;
			}
			break;
		case 0x07:
			reg = cpu->a;
			break;
	}
	result = cpu->a | reg;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + 
		 x3chk(result)+ parity(result);
	cpu->a = result;
	return time;
}

//-----------------
// or num8
int or_num8(CPU_t *cpu) {
	int result;
	int reg;
	
	reg = CPU_mem_read(cpu,cpu->pc++);
	result = cpu->a|reg;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + 
		 x3chk(result)+ parity(result);
	cpu->a = result;

	return 7;
}

//-----------------
// xor reg8
int xor_reg8(CPU_t *cpu) {
	int result;
	int reg;
	int time = 4;
	switch ((cpu->bus)&7) {
		case 0x00:
			reg = cpu->b;
			break;
		case 0x01:
			reg = cpu->c;
			break;
		case 0x02:
			reg = cpu->d;
			break;
		case 0x03:
			reg = cpu->e;
			break;
		case 0x04:
			index_ext (
				reg = cpu->h;,
				reg = cpu->ixh; time += 4;,
				reg = cpu->iyh; time += 4;
			)
			break;
		case 0x05:
			index_ext (
				reg = cpu->l;,
				reg = cpu->ixl; time += 4;,
				reg = cpu->iyl; time += 4;
			)
			break;
		case 0x06:
			if (!cpu->prefix) {
				reg = CPU_mem_read(cpu,cpu->hl);
				time += 3;
			} else {
				char offset = CPU_mem_read(cpu, cpu->pc++);
				if (cpu->prefix == 0xDD) {
					reg = CPU_mem_read(cpu, cpu->ix + offset);
				} else {
					reg = CPU_mem_read(cpu, cpu->iy + offset);
				}
				time += 15;
			}
			break;
		case 0x07:
			reg = cpu->a;
			break;
	}
	result = cpu->a^reg;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + 
		 x3chk(result)+ parity(result);
	cpu->a = result;
	return time;
}


//-----------------
// xor num8
int xor_num8(CPU_t *cpu) {
	int result;
	int reg;

	reg = CPU_mem_read(cpu,cpu->pc++);
	result = cpu->a^reg;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + 
		 x3chk(result)+ parity(result);
	cpu->a = result;
	return 7;
}

int cp_reg8(CPU_t *cpu) {
	int result;
	int reg;
	int time = 4;
	switch ((cpu->bus)&7) {
		case 0x00:
			reg = cpu->b;
			break;
		case 0x01:
			reg = cpu->c;
			break;
		case 0x02:
			reg = cpu->d;
			break;
		case 0x03:
			reg = cpu->e;
			break;
		case 0x04:
			index_ext (
				reg = cpu->h;,
				reg = cpu->ixh; time += 4;,
				reg = cpu->iyh; time += 4;
			)
			break;
		case 0x05:
			index_ext (
				reg = cpu->l;,
				reg = cpu->ixl; time += 4;,
				reg = cpu->iyl; time += 4;
			)
			break;
		case 0x06:
			if (!cpu->prefix) {
				reg = CPU_mem_read(cpu,cpu->hl);
				time += 3;
			} else {
				char offset = CPU_mem_read(cpu, cpu->pc++);
				if (cpu->prefix == 0xDD) {
					reg = CPU_mem_read(cpu, cpu->ix + offset);
				} else {
					reg = CPU_mem_read(cpu, cpu->iy + offset);
				}
				time += 15;
			}
			break;
		case 0x07:
			reg = cpu->a;
			break;
	}
	result = cpu->a - reg;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(reg) + hcsubchk(cpu->a,reg,0) + 
		 x3chk(reg)+ vchksub(cpu->a,reg,result) + 
		 SUB_INSTR +  carrychk(result);
	return time;
}

int cp_num8(CPU_t *cpu) {
	int result;
	int reg;

	reg = CPU_mem_read(cpu,cpu->pc++);
	result = cpu->a - reg;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(reg) + hcsubchk(cpu->a,reg,0) + 
		 x3chk(reg)+ vchksub(cpu->a,reg,result) + 
		 SUB_INSTR +  carrychk(result);

	return 7;
}

int cpl(CPU_t *cpu) {
	int result = (~cpu->a)&255;
	cpu->f =unaffect( SIGN_MASK+ZERO_MASK+PV_MASK+CARRY_MASK) +
		 x5chk(result) + HC_MASK + x3chk(result)+ N_MASK;
	cpu->a = result;

	return 4;
}

int daa(CPU_t *cpu) {
	int result = cpu->a;
	
	if ( (cpu->f&N_MASK)!=0 ) {
		if ( (cpu->f&HC_MASK)!=0 || (cpu->a&0x0f)>9 ) result -= 0x06;
		if ( (cpu->f&CARRY_MASK)!=0 || (cpu->a > 0x99) ) result -= 0x60;
	} else {
		if ( (cpu->f&HC_MASK)!=0 || (cpu->a&0x0f)>9 ) result += 0x06;
		if ( (cpu->f&CARRY_MASK)!=0 || (cpu->a > 0x99) ) result += 0x60;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + (cpu->a&0x10 ^ result&0x10) + 
		 x3chk(result)+ parity(result) + 
		 unaffect( N_MASK ) + ((cpu->f & CARRY_MASK) | ((cpu->a>0x99)?CARRY_MASK:0));
	cpu->a = result;
	return 4;
}




int dec_reg8(CPU_t *cpu) {
	int result;
	int reg;
	int time = 4;
	switch ((cpu->bus>>3)&7) {
		case 0x00:
			reg = cpu->b;
			cpu->b--;
			result = cpu->b;
			break;
		case 0x01:
			reg = cpu->c;
			cpu->c--;
			result = cpu->c;
			break;
		case 0x02:
			reg = cpu->d;
			cpu->d--;
			result = cpu->d;
			break;
		case 0x03:
			reg = cpu->e;
			cpu->e--;
			result = cpu->e;
			break;
		case 0x04:
			if (!cpu->prefix) {
				reg = cpu->h;
				cpu->h--;
				result = cpu->h;
			} else if (cpu->prefix == 0xDD) {
				reg = cpu->ixh;
				cpu->ixh--;
				result = cpu->ixh;
				time += 4;
			} else {
				reg = cpu->iyh;
				cpu->iyh--;
				result = cpu->iyh;
				time += 4;
			}
			break;
		case 0x05:
			if (!cpu->prefix) {
				reg = cpu->l;
				cpu->l--;
				result = cpu->l;
			} else if (cpu->prefix == 0xDD) {
				reg = cpu->ixl;
				cpu->ixl--;
				result = cpu->ixl;
				time += 4;
			} else {
				reg = cpu->iyl;
				cpu->iyl--;
				result = cpu->iyl;
				time += 4;
			}
			break;
		case 0x06:
			if (!cpu->prefix) {
				reg = CPU_mem_read(cpu,cpu->hl);
				result = reg;
				result--;
				CPU_mem_write(cpu,cpu->hl,result);
				time += 7;
			} else {
				char offset = CPU_mem_read(cpu, cpu->pc++);
				if (cpu->prefix == 0xDD) {
					reg = CPU_mem_read(cpu,cpu->ix+offset);
					result = reg;
					result--;
					CPU_mem_write(cpu,cpu->ix+offset,result);
					time += 19 - 4;
				} else {
					reg = CPU_mem_read(cpu,cpu->iy+offset);
					result = reg;
					result--;
					CPU_mem_write(cpu,cpu->iy+offset,result);
					time += 19 - 4;
				}
			}
			break;
		case 0x07:
			reg = cpu->a;
			cpu->a--;
			result = cpu->a;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + hcsubchk(reg,1,0) + 
		 x3chk(result)+ vchksub(reg,1,result) + 
		 SUB_INSTR +  unaffect(CARRY_MASK);

	return time;
}


int inc_reg8(CPU_t *cpu) {
	int result;
	int reg;
	int time = 4;
	switch ((cpu->bus>>3)&7) {
		case 0x00:
			reg = cpu->b;
			cpu->b++;
			result = cpu->b;
			break;
		case 0x01:
			reg = cpu->c;
			cpu->c++;
			result = cpu->c;
			break;
		case 0x02:
			reg = cpu->d;
			cpu->d++;
			result = cpu->d;
			break;
		case 0x03:
			reg = cpu->e;
			cpu->e++;
			result = cpu->e;
			break;
		case 0x04:
			if (!cpu->prefix) {
				reg = cpu->h;
				cpu->h++;
				result = cpu->h;
			} else if (cpu->prefix == 0xDD) {
				reg = cpu->ixh;
				cpu->ixh++;
				result = cpu->ixh;
				time += 4;
			} else {
				reg = cpu->iyh;
				cpu->iyh++;
				result = cpu->iyh;
				time += 4;
			}
			break;
		case 0x05:
			if (!cpu->prefix) {
				reg = cpu->l;
				cpu->l++;
				result = cpu->l;
			} else if (cpu->prefix == 0xDD) {
				reg = cpu->ixl;
				cpu->ixl++;
				result = cpu->ixl;
				time += 4;
			} else {
				reg = cpu->iyl;
				cpu->iyl++;
				result = cpu->iyl;
				time += 4;
			}
			break;
		case 0x06:
			if (!cpu->prefix) {
				reg = CPU_mem_read(cpu,cpu->hl);
				result = reg;
				result++;
				CPU_mem_write(cpu,cpu->hl,result);
				time += 7;
			} else {
				char offset = CPU_mem_read(cpu, cpu->pc++);
				if (cpu->prefix == 0xDD) {
					reg = CPU_mem_read(cpu,cpu->ix+offset);
					result = reg;
					result++;
					CPU_mem_write(cpu,cpu->ix+offset,result);
					time += 19 - 4;
				} else {
					reg = CPU_mem_read(cpu,cpu->iy+offset);
					result = reg;
					result++;
					CPU_mem_write(cpu,cpu->iy+offset,result);
					time += 19 - 4;
				}
			}
			break;
		case 0x07:
			reg = cpu->a;
			cpu->a++;
			result = cpu->a;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + hcaddchk(reg,1,0) + 
		 x3chk(result)+vchkadd(reg,1,result) + 
		 ADD_INSTR +  unaffect(CARRY_MASK);
	return time;
}
//-----------------
// add a,reg8
// add a,(hl)
// adc a,reg8
// adc a,(hl)
int add_a_reg(CPU_t *cpu, int carry) {
	int result;
	int reg;
	int time = 4;
	switch ((cpu->bus)&7) {
		case 0x00:
			reg = cpu->b;
			break;
		case 0x01:
			reg = cpu->c;
			break;
		case 0x02:
			reg = cpu->d;
			break;
		case 0x03:
			reg = cpu->e;
			break;
		case 0x04:
			index_ext (
				reg = cpu->h;,
				reg = cpu->ixh; time += 4;,
				reg = cpu->iyh; time += 4;
			)
			break;
		case 0x05:
			index_ext (
				reg = cpu->l;,
				reg = cpu->ixl; time += 4;,
				reg = cpu->iyl; time += 4;
			)
			break;
		case 0x06:
			if (!cpu->prefix) {
				reg = CPU_mem_read(cpu,cpu->hl);
				time += 3;
			} else {
				char offset = CPU_mem_read(cpu, cpu->pc++);
				if (cpu->prefix == 0xDD) {
					reg = CPU_mem_read(cpu, cpu->ix + offset);
				} else {
					reg = CPU_mem_read(cpu, cpu->iy + offset);
				}
				time += 15;
			}
			break;
		case 0x07:
			reg = cpu->a;
			break;
	}
	result = cpu->a+reg+carry;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + hcaddchk(cpu->a,reg,carry) + 
		 x3chk(result)+ vchkadd(cpu->a,reg,result) + 
		 ADD_INSTR +  carrychk(result);
	cpu->a = result;
	return time;
}
int adc_a_reg8(CPU_t *cpu) {
	return add_a_reg(cpu, cpu->f & 1);
}
int add_a_reg8(CPU_t *cpu) {
	return add_a_reg(cpu, 0);
}


//-----------------
// add a,num8
// adc a,num8
int add_a_num(CPU_t *cpu, int carry) {
	int result;
	int reg;
	reg = CPU_mem_read(cpu,cpu->pc++);	//THIS IS NOT AN OPCODE READ
	result = cpu->a + reg + carry;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + hcaddchk(cpu->a,reg,carry) + 
		 x3chk(result)+ vchkadd(cpu->a,reg,result) + 
		 ADD_INSTR +  carrychk(result);
	cpu->a = result;
	return 7;
}
int adc_a_num8(CPU_t *cpu) {
	return add_a_num(cpu, cpu->f & 1);
}
int add_a_num8(CPU_t *cpu) {
	return add_a_num(cpu,0);
}


//-----------------
// sub reg8
// sub (hl)
// sbc a,reg8
// sbc a,(hl)
int sub_a_reg(CPU_t *cpu, int carry) {
	int result;
	int reg;
	int time = 4;
	switch ((cpu->bus)&7) {
		case 0x00:
			reg = cpu->b;
			break;
		case 0x01:
			reg = cpu->c;
			break;
		case 0x02:
			reg = cpu->d;
			break;
		case 0x03:
			reg = cpu->e;
			break;
		case 0x04:
			index_ext (
				reg = cpu->h;,
				reg = cpu->ixh; time += 4;,
				reg = cpu->iyh; time += 4;
			)
			break;
		case 0x05:
			index_ext (
				reg = cpu->l;,
				reg = cpu->ixl; time += 4;,
				reg = cpu->iyl; time += 4;
			)
			break;
		case 0x06:
			if (!cpu->prefix) {
				reg = CPU_mem_read(cpu,cpu->hl);
				time += 3;
			} else {
				char offset = CPU_mem_read(cpu, cpu->pc++);
				if (cpu->prefix == 0xDD) {
					reg = CPU_mem_read(cpu, cpu->ix + offset);
				} else {
					reg = CPU_mem_read(cpu, cpu->iy + offset);
				}
				time += 19 - 4;
			}
			break;
		case 0x07:
		default:
			reg = cpu->a;
			break;
	}
	result = cpu->a - reg - carry;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + hcsubchk(cpu->a,reg,carry) + 
		 x3chk(result)+ vchksub(cpu->a,reg,result) + 
		 SUB_INSTR +  carrychk(result);
	cpu->a = result;
	return time;
}
int sbc_a_reg8(CPU_t *cpu) {
	return sub_a_reg(cpu,(cpu->f)&1);
}
int sub_a_reg8(CPU_t *cpu) {
	return sub_a_reg(cpu,0);
}

//-----------------
// sub num8
// sbc a,num8
int sub_a_num(CPU_t *cpu,int carry) {
	int result;
	int reg;
	
	reg = CPU_mem_read(cpu,cpu->pc++);
	result = cpu->a - reg - carry;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + hcsubchk(cpu->a,reg,carry) + 
		 x3chk(result)+ vchksub(cpu->a,reg,result) + 
		 SUB_INSTR +  carrychk(result);
	cpu->a = result;

	return 7;
}
int sbc_a_num8(CPU_t *cpu) {
	return sub_a_num(cpu, cpu->f & 1);
}
int sub_a_num8(CPU_t *cpu) {
	return sub_a_num(cpu, 0);
}


int dec_reg16(CPU_t *cpu) {
	int time = 6;
	switch (((cpu->bus)>>4)&3) {
		case 0x00:
			cpu->bc--;
			break;
		case 0x01:
			cpu->de--;
			break;
		case 0x02:
			index_ext(
				cpu->hl--;,
				cpu->ix--; time+=4;,
				cpu->iy--; time+=4;
			)
			break;
		case 0x03:
			cpu->sp--;
			break;
	}
	return time;
}

int inc_reg16(CPU_t *cpu) {
	int time = 6;
	switch (((cpu->bus)>>4)&3) {
		case 0x00:
			cpu->bc++;
			break;
		case 0x01:
			cpu->de++;
			break;
		case 0x02:
			index_ext(
				cpu->hl++;,
				cpu->ix++; time+=4;,
				cpu->iy++; time+=4;
			)
			break;
		case 0x03:
			cpu->sp++;
			break;
	}
	
	return time;
}

int add_hl_reg16(CPU_t *cpu) {
	int result;
	int base, reg;

	int time = 11; 
	switch (((cpu->bus)>>4)&3) {
		case 0x00:
			reg = cpu->bc;
			break;
		case 0x01:
			reg = cpu->de;
			break;
		case 0x02:
			if (!cpu->prefix) {
				reg = cpu->hl;
			} else if (cpu->prefix == 0xDD) {
				reg = cpu->ix;
				time+=4;
			} else {
				reg = cpu->iy;
				time+=4;
			}
			break;
		case 0x03:
			reg = cpu->sp;
			break;
	}

	if (!cpu->prefix) {
		base = cpu->hl;
		cpu->hl = result = base + reg;
	} else if (cpu->prefix == 0xDD) {
		base = cpu->ix;
		cpu->ix = result = base + reg;
	} else {
		base = cpu->iy;
		cpu->iy = result = base + reg;
	}

	cpu->f = unaffect(SIGN_MASK +ZERO_MASK+PV_MASK) + 
		 x5chk16(result) + hcaddchk16(base, reg, 0) + 
		 x3chk16(result)+ 
		 ADD_INSTR +  carrychk16( base + reg);
	return time;
}


int rla(CPU_t *cpu) {
	int result = ((cpu->a<<1)+(cpu->f&1))&255;
	cpu->f = unaffect(SIGN_MASK +ZERO_MASK+PV_MASK) + 
		 x5chk(result) + x3chk(result)+ (((cpu->a)>>7)&1);
	cpu->a =result;

	return 4;
	
}
int rlca(CPU_t *cpu) {
	int result = ((cpu->a<<1)+(((cpu->a)>>7)&1))&255;
	cpu->f = unaffect(SIGN_MASK +ZERO_MASK+PV_MASK) + 
		 x5chk(result) + x3chk(result)+ (((cpu->a)>>7)&1);
	cpu->a =result;

	return 4;
}
int rra(CPU_t *cpu) {
	int result = ((cpu->a>>1)+((cpu->f&1)<<7))&255;
	cpu->f = unaffect(SIGN_MASK +ZERO_MASK+PV_MASK) + 
		 x5chk(result) + x3chk(result)+ ((cpu->a)&1);
	cpu->a =result;

	return 4;
	
}
int rrca(CPU_t *cpu) {
	int result = ((cpu->a>>1)+(((cpu->a)<<7)&128))&255;
	cpu->f = unaffect(SIGN_MASK +ZERO_MASK+PV_MASK) + 
		 x5chk(result) + x3chk(result)+ ((cpu->a)&1);
	cpu->a =result;

	return 4;
}

#pragma warning(pop)