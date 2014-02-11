#include "stdafx.h"

#define LABEL_C
#include "label.h"
#include "core.h"
#include "calc.h"

#include "bcalls.h"
#include "flags.h"

label_struct *lookup_label(LPCALC lpCalc, TCHAR *label_name) {
	int i;
	for (i = 0; lpCalc->labels[i].name != NULL; i++) {
		if (_tcsicmp(lpCalc->labels[i].name, label_name) == 0)
			return &lpCalc->labels[i];
	}
	return NULL;
}	
	

void VoidLabels(LPCALC lpCalc) {
	int i;
	
	for (i = 0; lpCalc->labels[i].name != NULL; i++) {
		free(lpCalc->labels[i].name);
		lpCalc->labels[i].name = NULL;
	}
}

TCHAR* FindAddressLabel(LPCALC lpCalc, waddr_t waddr) {
	
	for (int i = 0; lpCalc->labels[i].name != NULL; i++) {
		label_struct *label = &lpCalc->labels[i];
		if (label->IsRAM == waddr.is_ram && label->page == waddr.page && label->addr == waddr.addr)
			return label->name;
	}
	return NULL;
}
	
//-------------------------------------------
// True means label is found and is the same
//
BOOL label_search_tios(TCHAR *label, int equate) {
	if (!label) {
		return FALSE;
	}

	for(int i = 0; bcalls[i].address != -1; i++ ) {
		if (_tcscmp(label, bcalls[i].name) == 0) {
			if (bcalls[i].address == (equate & 0xFFFF) ) {
				return TRUE;
			}
		}
	}
	
	for(int i = 0; flags83p[i].flag != -1; i++ ) {
		if (_tcscmp(label, flags83p[i].name) == 0) {
			if (flags83p[i].flag == (equate & 0xFFFF)) {
				return TRUE;
			}
		}
		for(int b = 0; b < 8; b++) {
			if (_tcscmp(label, flags83p[i].bits[b].name) == 0) {
				if (flags83p[i].bits[b].bit == (equate & 0xFFFF)) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
	

int labels_app_load(LPCALC lpCalc, LPCTSTR lpszFileName) {
	FILE *labelFile = NULL;
	int i, length;
#ifdef _UNICODE
	char readBuf[1024];
#endif
	TCHAR buffer[1024];
	TCHAR name[256];
	TCHAR *fileName = ((TCHAR *) lpszFileName) + _tcslen(lpszFileName);
	while (*--fileName != '\\');
	fileName++;

	unsigned int equate;
	label_struct *label = &lpCalc->labels[0];	

	_tfopen_s(&labelFile, lpszFileName, _T("r"));
	if (labelFile == NULL) {
		_putts(_T("Error opening label files."));
		return 1;
	}
	
	// Clear out the old labels
	VoidLabels(lpCalc);

	while (!feof(labelFile)) {
#ifdef _UNICODE
		fgets(readBuf, 256, labelFile);
		MultiByteToWideChar(CP_ACP, 0, readBuf, -1, buffer, ARRAYSIZE(buffer));
#else
#ifdef _WINDOWS
		_fgetts(buffer, 256, labelFile);
#else
		fgets(buffer, 256, labelFile);
#endif
#endif
		i = 0;
		if (buffer[0] != ';')
			i = _stscanf_s(buffer, _T("%s = $%X"), name, &equate);
		if (i == 2) {
			length = (int) _tcslen(name);
			if (!label_search_tios(name, equate)) {
				
				label->name = (TCHAR *) malloc((length + 1) * sizeof(TCHAR));
				StringCchCopy(label->name, length + 1, name);

				label->addr = equate & 0xFFFF;

				if ( (equate & 0x0000FFFF) >= 0x4000 && (equate & 0x0000FFFF) < 0x8000) {
					int page_offset = (equate >> 16) & 0xFF;
					
					label->IsRAM = FALSE;
					if (lpCalc->last_transferred_app == NULL) {
						upages_t upage;
						state_userpages(&lpCalc->cpu, &upage);
						label->page = upage.start;
					} else {
						applist_t applist;
						state_build_applist(&lpCalc->cpu, &applist);
						for (u_int i = 0; i < applist.count; i++) {
							int len = 8;
							TCHAR *ptr = applist.apps[i].name + len - 1;
							while (isspace(*ptr--))
								len--;
							if (!_tcsnicmp(fileName, applist.apps[i].name, len)) {
								label->page = applist.apps[i].page;
								break;
							}
						}
					}
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
	for(int i = 0; bcalls[i].address != -1; i++ ) {
		if (bcalls[i].address == address) {
			return bcalls[i].name;
		}
	}
	return NULL;
}


void FindFlags(int flag, int bit, TCHAR **flagstring, TCHAR **bitstring) {
	int i,b;
	for(i = 0; flags83p[i].flag != -1; i++ ) {
		if (flags83p[i].flag == flag) {
			for(b = 0; b < 8; b++) {
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
	
	
	
	
	

