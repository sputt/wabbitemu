#ifndef LABEL_H
#define LABEL_H

#include "coretypes.h"

typedef struct {
    TCHAR *name;
    BOOL IsRAM;
    uint8_t page;
    uint16_t addr;
} label_struct;

TCHAR* FindAddressLabel( int slot, BOOL IsRAM, uint8_t page, uint16_t addr);
//void ImportBcalls(char* fn);
TCHAR* FindBcall(int address);
void FindFlags(int flag,int bit, TCHAR **flagstring, TCHAR **bitstring);
void VoidLabels(int slot);
label_struct *lookup_label(TCHAR *label);
int labels_app_load(int slot, TCHAR* fn);
#endif

