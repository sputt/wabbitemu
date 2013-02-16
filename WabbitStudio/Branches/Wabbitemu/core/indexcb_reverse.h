#ifndef INDEXCB_REVERSE_H
#define INDEXCB_REVERSE_H
#include "core.h"
#include "alu.h"
#include "control.h"

void bit_ind_reverse(CPU_t*, char);
void res_ind_reverse(CPU_t*, char);
void set_ind_reverse(CPU_t*, char);

void rr_ind_reverse(CPU_t*, char);
void rrc_ind_reverse(CPU_t*, char);
void rl_ind_reverse(CPU_t*, char);
void rlc_ind_reverse(CPU_t*, char);

void srl_ind_reverse(CPU_t*, char);
void sll_ind_reverse(CPU_t*, char);

void sra_ind_reverse(CPU_t*, char);
void sla_ind_reverse(CPU_t*, char);

#endif	//INDEXCB_REVERSE_H