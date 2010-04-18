#ifndef CONTROL_H
#define CONTROL_H

#include "core.h"

void rst(CPU_t *);
void ret(CPU_t *);
void out(CPU_t *);
void in(CPU_t *);
void call(CPU_t *);
void call_condition(CPU_t *);
void push_reg16(CPU_t *);
void pop_reg16(CPU_t *);
void ld_sp_hl(CPU_t *);
void ld_mem16_hlf(CPU_t *);
void ld_hlf_mem16(CPU_t *);
void ld_hl_num16(CPU_t *);
void ld_de_num16(CPU_t *);
void ld_bc_num16(CPU_t *);
void ld_sp_num16(CPU_t *);
void ld_a_mem16(CPU_t *);
void ld_a_bc(CPU_t *);
void ld_a_de(CPU_t *);
void ld_mem16_a(CPU_t *);
void ld_bc_a(CPU_t *);
void ld_de_a(CPU_t *);
void ld_r_num8(CPU_t *);
void ld_r_r(CPU_t *);
void halt(CPU_t *);
void ex_sp_hl(CPU_t *);
void ex_de_hl(CPU_t *);
void exx(CPU_t *);
void ex_af_afp(CPU_t *);
void jp_hl(CPU_t *);
void jp(CPU_t *);
void jr(CPU_t *);
void jp_condition(CPU_t *);
void jr_condition(CPU_t *);
void djnz(CPU_t *);
void ret_condition(CPU_t *);
void ccf(CPU_t *);
void scf(CPU_t *);


void IM0(CPU_t *);
void IM1(CPU_t *);
void IM2(CPU_t *);
void ei(CPU_t *);
void di(CPU_t *);
void ldd(CPU_t *);
void lddr(CPU_t *);
void ldi(CPU_t *);
void ldir(CPU_t *);
void neg(CPU_t *);
void nop(CPU_t *);
void ld_mem16_reg16(CPU_t *);
void ld_reg16_mem16(CPU_t *);
void in_reg_c(CPU_t *);
void ind(CPU_t *);
void indr(CPU_t *);
void ini(CPU_t *);
void inir(CPU_t *);
void ld_i_a(CPU_t *);
void ld_r_a(CPU_t *);
void ld_a_i(CPU_t *);
void ld_a_r(CPU_t *);
void out_reg(CPU_t *);
void outd(CPU_t *);
void otdr(CPU_t *);
void outi(CPU_t *);
void otir(CPU_t *);
void reti(CPU_t *);
void retn(CPU_t *);
void ednop(CPU_t *);


#endif
