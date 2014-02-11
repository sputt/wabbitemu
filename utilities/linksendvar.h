#ifndef LINKSENDVAR_H
#define LINKSENDVAR_H

#include "core.h"
#include "link.h"
#include "var.h"

/* Send a Request To Send packet
* On error: Throws Packet Exception */
void link_RTS(CPU_t *cpu, TIVAR_t *var, int dest);
LINK_ERR link_send_var(CPU_t *, TIFILE_t *, SEND_FLAG);
LINK_ERR link_send_backup(CPU_t *, TIFILE_t *, SEND_FLAG);
LINK_ERR forceload_os(CPU_t *, TIFILE_t *);
void writeboot(FILE*, memory_context_t *, int page);

#endif