#ifndef LABEL_H
#define LABEL_H

#include "coretypes.h"

typedef struct {
    char *name;
    BOOL IsRAM;
    uint8_t page;
    uint16_t addr;
} label_struct;

char* FindAddressLabel( int slot, BOOL IsRAM, uint8_t page, uint16_t addr);
//void ImportBcalls(char* fn);
char* FindBcall(int address);
void FindFlags(int flag,int bit, char **flagstring, char **bitstring);
void VoidLabels(int slot);
label_struct *lookup_label(char *label);
int labels_app_load(int slot, char* fn);
#endif

