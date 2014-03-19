#ifndef TI81HW_H
#define TI81HW_H
#include "core.h"
#include "ti_stdint.h"

static void port10(CPU_t *cpu, device_t *dev);
int device_init_81(CPU_t*);
int memory_init_81(memc *);

#endif 
