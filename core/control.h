#ifndef CONTROL_H
#define CONTROL_H

#include "core.h"

int rst(CPU_t *);
int ret(CPU_t *);
int out(CPU_t *);
int in(CPU_t *);
int call(CPU_t *);
int call_condition(CPU_t *);
int push_reg16(CPU_t *);
int pop_reg16(CPU_t *);
int ld_sp_hl(CPU_t *);
int ld_mem16_hlf(CPU_t *);
int ld_hlf_mem16(CPU_t *);
int ld_hl_num16(CPU_t *);
int ld_de_num16(CPU_t *);
int ld_bc_num16(CPU_t *);
int ld_sp_num16(CPU_t *);
int ld_a_mem16(CPU_t *);
int ld_a_bc(CPU_t *);
int ld_a_de(CPU_t *);
int ld_mem16_a(CPU_t *);
int ld_bc_a(CPU_t *);
int ld_de_a(CPU_t *);
int ld_r_num8(CPU_t *);
int ld_r_r(CPU_t *);
int halt(CPU_t *);
int ex_sp_hl(CPU_t *);
int ex_de_hl(CPU_t *);
int exx(CPU_t *);
int ex_af_afp(CPU_t *);
int jp_hl(CPU_t *);
int jp(CPU_t *);
int jr(CPU_t *);
int jp_condition(CPU_t *);
int jr_condition(CPU_t *);
int djnz(CPU_t *);
int ret_condition(CPU_t *);
int ccf(CPU_t *);
int scf(CPU_t *);


int IM0(CPU_t *);
int IM1(CPU_t *);
int IM2(CPU_t *);
int ei(CPU_t *);
int di(CPU_t *);
int ldd(CPU_t *);
int lddr(CPU_t *);
int ldi(CPU_t *);
int ldir(CPU_t *);
int neg(CPU_t *);
int nop(CPU_t *);
int ld_mem16_reg16(CPU_t *);
int ld_reg16_mem16(CPU_t *);
int in_reg_c(CPU_t *);
int ind(CPU_t *);
int indr(CPU_t *);
int ini(CPU_t *);
int inir(CPU_t *);
int ld_i_a(CPU_t *);
int ld_r_a(CPU_t *);
int ld_a_i(CPU_t *);
int ld_a_r(CPU_t *);
int out_reg(CPU_t *);
int outd(CPU_t *);
int otdr(CPU_t *);
int outi(CPU_t *);
int otir(CPU_t *);
int reti(CPU_t *);
int retn(CPU_t *);
int ednop(CPU_t *);


#endif
