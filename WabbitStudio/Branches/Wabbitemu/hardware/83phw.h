#ifndef TI83PHW_H
#define TI83PHW_H
#include "core.h"
#include "ti_stdint.h"

#ifndef LINK_READ
#define LINK_READ
#define LinkRead (((cpu->pio.link->host & 0x03) | (cpu->pio.link->client[0] & 0x03)) ^ 3)
#endif
#define NumElm(array) (sizeof (array) / sizeof ((array)[0]))

int device_init_83p(CPU_t*);
int memory_init_83p(memc *);

#endif 
