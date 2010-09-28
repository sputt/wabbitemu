#include "stdafx.h"

#define LABEL_C
#include "label.h"
#include "core.h"
#include "calc.h"

#include "bcalls.h"
#include "flags.h"

label_struct *lookup_label(TCHAR *label_name) {
	int i;
	for (i = 0; calcs[gslot].labels[i].name != NULL; i++) {
		if (_tcsicmp(calcs[gslot].labels[i].name, label_name) == 0)
			return &calcs[gslot].labels[i];
	}
	return NULL;
}	
	

void VoidLabels(int slot) {
	int i;
	
	for (i = 0; calcs[slot].labels[i].name != NULL; i++) {
		free(calcs[slot].labels[i].name);
		calcs[slot].labels[i].name = NULL;
	}
}

void ClearLabels(int slot) {
	VoidLabels(slot);
}

TCHAR* FindAddressLabel(int slot, BOOL IsRAM, uint8_t page, uint16_t addr) {
	int i;
	
	for (i = 0; calcs[slot].labels[i].name != NULL; i++) {
		label_struct *label = &calcs[slot].labels[i];
		if (label->IsRAM == IsRAM && label->page == page && label->addr == addr)
			return label->name;
	}
	return NULL;
}
	
//-------------------------------------------
// True means label is found and is the same
//
BOOL label_search_tios(TCHAR *label,int equate) {
	int i,b;
	
	if (!label) return FALSE;

	for(i=0;bcalls[i].address != -1; i++ ) {
		if (_tcscmp(label, bcalls[i].name) == 0) {
			if (bcalls[i].address == (equate&0xFFFF) ) {
				return TRUE;
			}
		}
	}
	
	for(i=0; flags83p[i].flag != -1; i++ ) {
		if (_tcscmp(label, flags83p[i].name) == 0) {
			if (flags83p[i].flag == (equate&0xFFFF)) {
				return TRUE;
			}
		}
		for(b=0;b<8;b++) {
			if (_tcscmp(label, flags83p[i].bits[b].name) == 0) {
				if (flags83p[i].bits[b].bit == (equate&0xFFFF)) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
	

int labels_app_load(int slot, TCHAR* fn) {
	FILE *labelFile;
	int i,length;
	TCHAR buffer[256];
	TCHAR name[256];
	unsigned int equate;
	label_struct *label = &calcs[slot].labels[0];	

#ifdef WINVER
	_tfopen_s(&labelFile, fn, _T("r"));
	if (!labelFile) {
#else
    if (!(labelFile = fopen(fn,"r"))) {
#endif
        _putts(_T("Error opening label files."));
        return 1;
    }
    
    // Clear out the old labels
    VoidLabels(slot);

    while (!feof(labelFile)) {
		_fgetts(buffer, 256, labelFile);
		i = 0;
		if (buffer[0] != ';')
#ifdef WINVER
			i = _tscanf_s(buffer, _T("%s = $%X"), name, &equate);
			//i = sscanf_s(buffer,"%s = $%X", name, &equate);
#else
			i = sscanf(buffer,"%s = $%X", name, &equate);
#endif
		if (i == 2) {
			length = (int) _tcslen(name);
			if (!label_search_tios(name, equate)) {
				
				label->name = (TCHAR *) malloc(length + 1);
#ifdef WINVER
				StringCbCopy(label->name, length + 1, name);
#else
				strcpy(label->name, name);
#endif
				label->addr = equate & 0xFFFF;

				if ( (equate & 0x0000FFFF) >= 0x4000 && (equate & 0x0000FFFF) < 0x8000) {
					int page_offset = (equate >> 16) & 0xFF;
					
					label->IsRAM = FALSE;
					if (calcs[slot].last_transferred_app == NULL)
						label->page = calcs[slot].mem_c.flash_pages - page_offset;
					else
						label->page = calcs[slot].last_transferred_app->page - page_offset;

				} else {
					label->IsRAM = TRUE;
					label->page = 1;
				}
				label++;
			}
		}
    }
    fclose(labelFile);
    return 0;
}
    
/*
void ImportBcalls(char* fn) {
	int i,address;
	char string[256],tmp[32];
	FILE* infile;
	
	infile = fopen(fn,"r");
	
	if (!infile) {
		puts("COuld not open bcall file");
		return;
	}
	for(address=0;address<65536;address++) {
		for(i=0;i<32;i++) bcalls[address][i] = 0;
	}		
	while( !feof(infile) ) {
		fgets(string,256,infile);
		i = sscanf(string,"%s = $%04X",tmp,&address);
		if (i == 2) {
			strcpy(bcalls[address],tmp);
		}
	}
	fclose(infile);
}
*/
TCHAR* FindBcall(int address) {
	int i;
	for(i=0;bcalls[i].address != -1; i++ ) {
		if (bcalls[i].address == address) {
			return bcalls[i].name;
		}
	}
	return NULL;
}


void FindFlags(int flag,int bit, TCHAR **flagstring, TCHAR **bitstring) {
	int i,b;
	for(i=0; flags83p[i].flag != -1; i++ ) {
		if (flags83p[i].flag == flag) {
			for(b=0;b<8;b++) {
				if (flags83p[i].bits[b].bit == bit) {
					*flagstring = flags83p[i].name;
					*bitstring  = flags83p[i].bits[b].name;
					return;
				}
			}
		}
	}
	*flagstring = NULL;
	*bitstring  = NULL;
}
	
	
	
	
	

