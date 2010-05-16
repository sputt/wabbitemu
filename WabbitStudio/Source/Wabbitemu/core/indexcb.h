#include "core.h"
#include "alu.h"
#include "control.h"

void bit_ind(CPU_t*, char);
void res_ind(CPU_t*, char);
void set_ind(CPU_t*, char);

void rr_ind(CPU_t*, char);
void rrc_ind(CPU_t*, char);
void rl_ind(CPU_t*, char);
void rlc_ind(CPU_t*, char);

void srl_ind(CPU_t*, char);
void sll_ind(CPU_t*, char);

void sra_ind(CPU_t*, char);
void sla_ind(CPU_t*, char);
