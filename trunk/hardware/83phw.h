#ifndef TI83PHW_H
#define TI83PHW_H
#include "core.h"
#include "ti_stdint.h"


#define NumElm(array) (sizeof (array) / sizeof ((array)[0]))



STDINT_t *INT83P_init(CPU_t*);
int device_init_83p(CPU_t*);
int memory_init_83p(memc *);

void flashwrite83p(CPU_t *, unsigned short , unsigned char );

#endif 
