#include "stdafx.h"

#include "var.h"
#include "fileutilities.h"
#ifdef _WINDOWS
#include "miniunz.h"
#endif

const char self_test[] = "Self Test?";
const char catalog[] = "CATALOG";
const char txt73[] = "GRAPH  EXPLORER  SOFTWARE";
const char txt86[] = "Already Installed";


#define tmpread( fp ) \
	tmp = fgetc(fp); \
	if (tmp == EOF) { \
		fclose(fp); \
		FreeTiFile(tifile); \
		return NULL; \
	}


int CmpStringCase(const char *str1, unsigned char *str2) {
	return _strnicmp(str1, (char *) str2, strlen(str1));
}

CalcModel FindRomVersion(char *string, unsigned char *rom, u_int size) {
	u_int i;
	int b;
	CalcModel calc;
	if (size == (32 * 1024)) {
		calc = TI_81;
	} else if (size == (128 * 1024)) {
		calc = TI_82;
	} else if (size == (256 * 1024)) {
		calc = TI_83;
	} else if ((size >= (510 * 1024)) && (size <= (590 * 1024))) {
		calc = TI_83P;
	} else if ((size >= (1016 * 1024)) && (size<= (1030 * 1024))) {
		calc = TI_84P;
	} else if ((size >= (2044 * 1024)) && (size<= (2260 * 1024))) {
		calc = TI_83PSE;
	} else if ((size >= (4090 * 1024)) && (size <= (4100 * 1024))) {
		calc = TI_84PCSE;
	} else {
		_putts(_T("not a known rom"));
		return INVALID_MODEL;
	}
	switch (calc) {
		case TI_81:
			//1.1k doesnt ld a,* first
			if (rom[0] == 0xC3) {
				string[0] = '1';
				string[1] = '.';
				string[2] = '1';
				string[3] = 'K';
				string[4] = '\0';
			} else if (rom[1] == 0x17) {
				//2.0V has different val to load
				string[0] = '2';
				string[1] = '.';
				string[2] = '0';
				string[3] = 'V';
				string[4] = '\0';
			} else if (rom[5] == 0x09) {
				//1.6K outs a 0x09
				string[0] = '1';
				string[1] = '.';
				string[2] = '6';
				string[3] = 'K';
				string[4] = '\0';
			} else {
				//assume its a 1.8K for now
				string[0] = '1';
				string[1] = '.';
				string[2] = '8';
				string[3] = 'K';
				string[4] = '\0';
			}
			break;
		case TI_82:
		case TI_85:
			for(i = 0; i < (size - strlen(catalog) - 10); i++) {
				if (!CmpStringCase(catalog, rom + i)) {
					calc = TI_85;
						for(i = 0; i < (size - strlen(self_test) - 10); i++) {
							if (CmpStringCase(self_test, rom + i) == 0) break;
						}
						for(; i < (size - 40); i++) {
							if (isdigit(rom[i])) break;
						}
						if (i < (size - 40)) {
							for(b = 0; (b + i) < (size - 4) && b < 32; b++) {
								if (rom[b + i] != ' ') string[b] = rom[b + i];
								else string[b] = 0;
							}
							string[31] = 0;
						} else {
							string[0] = '?';
							string[1] = '?';
							string[2] = '?';
							string[3] = 0;
						}
					break;
				}
			}
			if (calc != TI_82) break;

		case TI_83:
		case TI_86:
			for(i = 0; i < (size - strlen(txt86) - 10); i++) {
				if (!CmpStringCase(txt86, rom + i)) {
					calc = TI_86;
						for(i = 0; i < size - strlen(self_test) - 10; i++) {
							if (CmpStringCase(self_test, rom + i) == 0) break;
						}
						for(; i < size - 40; i++) {
							if (isdigit(rom[i])) break;
						}
						if (i < size - 40) {
							for(b = 0; (b + i) < (size - 4) && b < 32; b++) {
								if (rom[b + i] != ' ')
									string[b] = rom[b + i];
								else string[b] = 0;
							}
							string[31] = 0;
						} else {
							string[0] = '?';
							string[1] = '?';
							string[2] = '?';
							string[3] = 0;
						}
					break;
				}
			}

			if (calc == TI_86) {
				break;
			}

			for (i = 0; i < (size - strlen(self_test) - 10); i++) {
				if (CmpStringCase(self_test, rom + i)==0) break;
			}
			if ((i + 64) < size) {
				i += 10;
				for(b = 0; b < 32; b++) {
					string[b] = rom[i++];
				}
				string[31] = 0;
			} else {
				string[0] = '?';
				string[1] = '?';
				string[2] = '?';
				string[3] = 0;
			}
			break;
		case TI_73:
		case TI_83P:
			for(i = 0; i < (size - strlen(txt73) - 10); i++) {
				if (CmpStringCase(txt73, rom + i) == 0) {
					calc = TI_73;
					break;
				}
			}
		case TI_84P:
		case TI_83PSE:
		case TI_84PSE:
		case TI_84PCSE:
			i = 0x0064;
			if (calc == TI_84PCSE) {
				// uh ok TI
				i++;
			}
			for(b = 0; b < 32; b++) {
				string[b] = rom[i++];
			}
			string[31] = 0;
			if (calc == TI_83PSE) {
				if (string[0] > '1') {
					calc = TI_84PSE;
				}
			}
			break;
		case INVALID_MODEL:
			return INVALID_MODEL;
	}

	return calc;
}

int ReadIntelHex(FILE *ifile, INTELHEX_t *ihex) {
	BYTE str[600];
	DWORD i, size, addr, type, byte;
	memset(str, 0x00, ARRAYSIZE(str));
	if (!fgets((char*) str, 580, ifile))
		return 0;
	if (str[0] == 0) memcpy(str, str+1, 579);
	if (sscanf_s((const char*) str, ":%02X%04X%02X%*s", &size, &addr, &type) != 3)
		return 0;
	ihex->DataSize = size;
	ihex->Address = addr;
	ihex->Type = type;
	memset(ihex->Data, 0x00, 256);
	for (i = 0; i < size; i++) {
		if (sscanf_s((const char*)str + 9 + (i * 2), "%02X", &byte) != 1)
			return 0;
		ihex->Data[i] = (BYTE) byte;
	}
	if (sscanf_s((const char*)str + 9 + (i * 2), "%02X", &byte) != 1)
		return 0;
	ihex->CheckSum = byte;
	return 1;
}

#ifdef _WINDOWS
TIFILE_t* ImportZipFile(LPCTSTR filePath, TIFILE_t *tifile) {
	unzFile uf;
	TCHAR path[MAX_PATH];
	uf = unzOpen(filePath);
	GetStorageString(path, sizeof(path));
	StringCbCat(path, sizeof(path), _T("\\Zip"));
	int err = extract_zip(uf, path);
	unzClose(uf);
	return err ? NULL: tifile;
}
#endif

TIFILE_t* ImportFlashFile(FILE *infile, TIFILE_t *tifile) {
	int i;
	for(i = 0; i < 256; i++) {
		tifile->flash->pagesize[i]	= 0;
		tifile->flash->data[i]		= NULL;
	}

	INTELHEX_t record;
	int CurrentPage		= -1;
	int HighestAddress	=  0;
	int TotalSize		=  0;
	int TotalPages		=  0;
	int done			=  0;
	
	if (tifile->flash->type == FLASH_TYPE_OS) {
		// Find the first page, usually after the first line
		do {
			if (!ReadIntelHex(infile, &record)) {
				FreeTiFile(tifile);
				return NULL;
			}
		} while (record.Type != 0x02 || record.DataSize != 2);
		CurrentPage = ((record.Data[0] << 8) | record.Data[1]) & 0xFF;
		if (tifile->flash->data[CurrentPage] == 0) {
			tifile->flash->data[CurrentPage] = (unsigned char *) malloc(PAGE_SIZE);
			if (tifile->flash->data[CurrentPage] == NULL) {
				FreeTiFile(tifile);
				return NULL;
			}
			memset(tifile->flash->data[CurrentPage], 0, PAGE_SIZE);	//THIS IS IMPORTANT FOR LINKING, APPS FOR NOW
		}
		HighestAddress	=  0;
	}


	while (!feof( infile ) && !done) {
		ReadIntelHex(infile, &record);

		switch ( record.Type ) {
			case 00:
				if (CurrentPage > -1) {
					for(i = 0; (i < record.DataSize) && (((i + record.Address) & 0x3FFF) < PAGE_SIZE); i++) {
						tifile->flash->data[CurrentPage][(i + record.Address) & 0x3FFF] = record.Data[i];
					}
					if ( HighestAddress < i + record.Address ) HighestAddress = (int) (i + record.Address);
				}
				break;
			case 01:
				done = 1;
				if (CurrentPage == -1) {
					printf("invalid current page\n");
					FreeTiFile(tifile);
					return NULL;
				}
				TotalSize += (HighestAddress - PAGE_SIZE);
				tifile->flash->pagesize[CurrentPage] = (HighestAddress - PAGE_SIZE);
				tifile->flash->pages = TotalPages;
				break;
			case 02:
				if (CurrentPage > -1) {
					TotalSize += PAGE_SIZE;
					tifile->flash->pagesize[CurrentPage] = (HighestAddress - PAGE_SIZE);
				}
				TotalPages++;
				CurrentPage = ((record.Data[0] << 8) | record.Data[1]) & 0xFF;
				if (tifile->flash->data[CurrentPage] == 0) {
					tifile->flash->data[CurrentPage] = (unsigned char *) malloc(PAGE_SIZE);
					if (tifile->flash->data[CurrentPage] == NULL) {
						FreeTiFile(tifile);
						return NULL;
					}
					memset(tifile->flash->data[CurrentPage], 0, PAGE_SIZE);	//THIS IS IMPORTANT FOR LINKING, APPS FOR NOW
				}
				HighestAddress	=  0;
				break;
			default:
				printf("unknown record\n");
				FreeTiFile(tifile);
				return NULL;
		}
	}

	if (tifile->flash->device == 0x74) {
		tifile->model = TI_73;
	} else if (tifile->flash->device == 0x73) {
		tifile->model = TI_83P;
	} else {
		FreeTiFile(tifile);
		return NULL;
	}
	return tifile;
}

TIFILE_t* ImportROMFile(FILE *infile, TIFILE_t *tifile) {
	size_t size;
	CalcModel calc;

	fseek(infile, 0, SEEK_END);
	size = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	tifile->rom = (ROM_t *) malloc(sizeof(ROM_t));
	if (tifile->rom == NULL) {
		return FreeTiFile(tifile);
	}

	tifile->rom->data = (unsigned char *) malloc(size);
	if (tifile->rom->data == NULL)
		return FreeTiFile(tifile);

	fread(tifile->rom->data, sizeof(unsigned char), size, infile);
	tifile->rom->size		= (int)size;
	calc = FindRomVersion(tifile->rom->version, tifile->rom->data, (int)size);
	if (calc == INVALID_MODEL) {
		return FreeTiFile(tifile);
	}
	tifile->model			= calc;

	return tifile;
}

TIFILE_t* ImportBackup(FILE *infile, TIFILE_t *tifile) {
	int i, tmp;
	tifile->backup->data1 = NULL;
	tifile->backup->data2 = NULL;
	tifile->backup->data3 = NULL;

	tmpread(infile);
	tifile->backup->length2 = (unsigned short)tmp;
	tmpread(infile);
	tifile->backup->length2 += (unsigned short)(tmp << 8);

	tmpread(infile);
	tifile->backup->length3 = (unsigned short)tmp;
	tmpread(infile);
	tifile->backup->length3 += (unsigned short)(tmp << 8);

	tmpread(infile);
	tifile->backup->address = (unsigned short)tmp;
	tmpread(infile);
	tifile->backup->address += (unsigned short)(tmp << 8);

	tmpread(infile);
	tifile->backup->length1a = (unsigned short)tmp;
	tmpread(infile);
	tifile->backup->length1a += (unsigned short)(tmp << 8);

	tifile->backup->data1 = (unsigned char *) malloc(tifile->backup->length1);
	if (tifile->backup->data1 == NULL)
		return FreeTiFile(tifile);
	for(i = 0; i < tifile->backup->length1 && !feof(infile); i++) {
		tmpread(infile);
		tifile->backup->data1[i] = (unsigned char)tmp;
	}


	tmpread(infile);
	tifile->backup->length2a = (unsigned short)tmp;
	tmpread(infile);
	tifile->backup->length2a += (unsigned short)(tmp << 8);

	tifile->backup->data2 = (unsigned char *) malloc(tifile->backup->length2);
	if (tifile->backup->data2 == NULL)
		return FreeTiFile(tifile);
	for(i = 0; i < tifile->backup->length2 && !feof(infile); i++) {
		tmpread(infile);
		tifile->backup->data2[i] = (unsigned char)tmp;
	}

	tmpread(infile);
	tifile->backup->length3a = (unsigned short)tmp;
	tmpread(infile);
	tifile->backup->length3a += (unsigned short)(tmp << 8);

	tifile->backup->data3 = (unsigned char *) malloc(tifile->backup->length3);
	if (tifile->backup->data3 == NULL)
		return FreeTiFile(tifile);
	for(i=0; i<tifile->backup->length3 && !feof(infile); i++) {
		tmpread(infile);
		tifile->backup->data3[i] = (unsigned char)tmp;
	}

	tifile->chksum = (fgetc(infile) & 0xFF) + ((fgetc(infile) & 0xFF) << 8);

	tifile->type	= BACKUP_TYPE;
	return tifile;
}

void NullTiFile(TIFILE_t* tifile) {
	ZeroMemory(tifile, sizeof(TIFILE_t));
	tifile->type = VAR_TYPE;
}

TIFILE_t* InitTiFile() {
	TIFILE_t *tifile = (TIFILE_t*) malloc(sizeof(TIFILE_t));
	if (tifile == NULL)
		return NULL;
	NullTiFile(tifile);
	return tifile;
}

void ReadTiFileHeader(FILE *infile, TIFILE_t *tifile) {
	char headerString[8];
	int i, tmp;

	fread(headerString, 1, 8, infile);
	rewind(infile);
	
	if (!_strnicmp(headerString, DETECT_STR, 8) ||
		!_strnicmp(headerString, DETECT_CMP_STR, 8)) {
		tifile->type = SAV_TYPE;
		return;
	}

	if (!_strnicmp(headerString, FLASH_HEADER, 8)) {
		tifile->type = FLASH_TYPE;
		tifile->flash = (TIFLASH_t*) malloc(sizeof(TIFLASH_t));
		ZeroMemory(tifile->flash, sizeof(TIFLASH_t));
		if (tifile->flash == NULL) {
			FreeTiFile(tifile);
			return;
		}

		unsigned char *ptr = (unsigned char *) tifile->flash;
		for(i = 0; i < TI_FLASH_HEADER_SIZE && !feof(infile); i++) {
			tmp = fgetc(infile);
			if (tmp == EOF) {
				_tprintf_s(_T("failed to get the whole header\n"));
				fclose(infile);
				FreeTiFile(tifile);
				return;
			}
			ptr[i] = (unsigned char)tmp;
		}
		return;
	}

	/* It maybe a rom if it doesn't have the Standard header */
	if (_strnicmp(headerString, "**TI73**", 8) &&
		_strnicmp(headerString, "**TI82**", 8) &&
		_strnicmp(headerString, "**TI83**", 8) &&
		_strnicmp(headerString, "**TI83F*", 8) &&
		_strnicmp(headerString, "**TI85**", 8) &&
		_strnicmp(headerString, "**TI86**", 8)) {
		tifile->type = ROM_TYPE;
		return;
	}

	/* Import file Header */
	unsigned char *ptr = (unsigned char *) tifile;
	for(i = 0; i < TI_FILE_HEADER_SIZE && !feof(infile); i++) {
		tmp = fgetc(infile);
		if (tmp == EOF) {
			FreeTiFile(tifile);
			return;
		}
		ptr[i] = (unsigned char)tmp;
	}

	if (!_strnicmp((char *) tifile->sig, "**TI73**", 8)) tifile->model = TI_73;
	else if (!_strnicmp((char *) tifile->sig, "**TI82**", 8)) tifile->model = TI_82;
	else if (!_strnicmp((char *) tifile->sig, "**TI83**", 8)) tifile->model = TI_83;
	else if (!_strnicmp((char *) tifile->sig, "**TI83F*", 8)) tifile->model = TI_83P;
	else if (!_strnicmp((char *) tifile->sig, "**TI85**", 8)) tifile->model = TI_85;
	else if (!_strnicmp((char *) tifile->sig, "**TI86**", 8)) tifile->model = TI_86;
	else {
		FreeTiFile(tifile);
		return;
	}
	return;
}

static int length2 = 0;

TIFILE_t* ImportVarFile(FILE *infile, TIFILE_t *tifile, int varNumber) {
	int i;
	int tmp;
	unsigned short headersize;
	unsigned short length;
	unsigned char vartype, *ptr;

	if (varNumber == 0) {
		tmpread(infile);
		length2 = (unsigned short)tmp;
		tmpread(infile);
		length2 += (unsigned short)(tmp << 8);
	}

	tmpread(infile);
	headersize = (unsigned short)tmp;
	tmpread(infile);
	headersize += (unsigned short)(tmp << 8);

	tmpread(infile);
	length = (unsigned short)tmp;
	tmpread(infile);
	length += (unsigned short)(tmp << 8);

	tmpread(infile);
	vartype = (unsigned char)tmp;

	if ((tifile->model == TI_73 && vartype == 0x13) ||
		(tifile->model == TI_82 && vartype == 0x0F) ||
		(tifile->model == TI_85 && vartype == 0x1D))
	{
		tifile->backup = (TIBACKUP_t *)malloc(sizeof(TIBACKUP_t));
		if (tifile->backup == NULL) {
			return FreeTiFile(tifile);
		}

		tifile->backup->headersize = headersize;
		tifile->backup->length1 = length;
		tifile->backup->vartype = vartype;
		return ImportBackup(infile, tifile);
	}

	if (length2 > length + 17 || tifile->type == GROUP_TYPE) {
		tifile->type = GROUP_TYPE;
	} else {
		tifile->type = VAR_TYPE;
	}

	tifile->var = (TIVAR_t *)malloc(sizeof(TIVAR_t));
	tifile->vars[varNumber] = tifile->var;
	if (tifile->var == NULL)
		return FreeTiFile(tifile);

	char name_length = 8;
	if (tifile->model == TI_86 || tifile->model == TI_85) {
		//skip name length
		tmpread(infile);
		name_length = (char)tmp;
	}

	tifile->var->name_length = name_length;
	tifile->var->headersize = headersize;
	tifile->var->length = length;
	tifile->var->vartype = vartype;
	ptr = tifile->var->name;

	if (tifile->model == TI_86) {
		name_length = 8;
	}

	for (i = 0; i < name_length && !feof(infile); i++) {
		tmpread(infile);
		ptr[i] = (unsigned char)tmp;
	}

	if (tifile->model == TI_83P) {
		tmpread(infile);
		ptr[i++] = (unsigned char)tmp;
		tmpread(infile);
		ptr[i++] = (unsigned char)tmp;
	} else {
		ptr[i++] = 0;
		ptr[i++] = 0;
	}
	tmp = fgetc(infile);
	if (tmp == EOF)
		return FreeTiFile(tifile);
	ptr[i++] = (unsigned char)tmp;
	tmp = fgetc(infile);
	if (tmp == EOF)
		return FreeTiFile(tifile);
	ptr[i++] = (unsigned char)tmp;

	tifile->var->data = (unsigned char *)malloc(tifile->var->length);
	if (tifile->var->data == NULL) {
		return FreeTiFile(tifile);
	}

	for (i = 0; i < tifile->var->length && !feof(infile); i++) {
		tmp = fgetc(infile);
		if (tmp == EOF) {
			return FreeTiFile(tifile);
		}
		tifile->var->data[i] = (unsigned char)tmp;
	}

	if (tifile->type == GROUP_TYPE) {
		if (varNumber != 0) {
			return tifile;
		}
		while (tifile != NULL) {
			length2 -= tifile->var->length + 17;
			if (length2 <= 0) {
				break;
			}
			tifile = ImportVarFile(infile, tifile, ++varNumber);
		}
	}

	tifile->chksum = (fgetc(infile) & 0xFF) + ((fgetc(infile) & 0xFF) << 8);
	return tifile;
}

TIFILE_t* ImportVarData(FILE *infile, TIFILE_t *tifile) {
	switch (tifile->type) {
		case ROM_TYPE:
			return ImportROMFile(infile, tifile);
		case FLASH_TYPE:
			return ImportFlashFile(infile, tifile);
		case SAV_TYPE:
			tifile->save = ReadSave(infile);
			if (tifile->save == NULL) {
				return FreeTiFile(tifile);
			}

			tifile->model = tifile->save->model;
			return tifile;
		case VAR_TYPE:
			return ImportVarFile(infile, tifile, 0);
		default:
			return NULL;
	}	
}

TIFILE_t* importvar(LPCTSTR filePath, BOOL only_check_header) {
	FILE *infile = NULL;
	TIFILE_t *tifile;
	
	TCHAR extension[5] = _T("");
	const TCHAR *pext = _tcsrchr(filePath, _T('.'));
	if (pext != NULL) {
		StringCbCopy(extension, sizeof(extension), pext);
	}

	tifile = InitTiFile();
	if (tifile == NULL) {
		return NULL;
	}

	if (!_tcsicmp(extension, _T(".lab"))) {
		tifile->type = LABEL_TYPE;
		return tifile;
	}

	if (!_tcsicmp(extension, _T(".brk"))) {
		tifile->type = BREAKPOINT_TYPE;
		return tifile;
	}

#ifdef _WINDOWS
	if (!_tcsicmp(extension, _T(".tig")) || !_tcsicmp(extension, _T(".zip")) ) {
		tifile->type = ZIP_TYPE;
		if (!only_check_header) {
			ImportZipFile(filePath, tifile);
		}
		return tifile;
	}
#endif

	_tfopen_s(&infile, filePath, _T("rb"));
	if (infile == NULL) {
		return FreeTiFile(tifile);
	}

	ReadTiFileHeader(infile, tifile);
	// The last part is to make sure we don't allow files that cant be imported but
	// assumed to be ROMs until we try to read data. Why? because we don't read the
	// size of the data till we import. Since importing a ROM is fast and I don't
	// care enough to fix as this was meant for speed checking files on drop
	if (only_check_header && tifile->type != ROM_TYPE && tifile->type != SAV_TYPE) {
		fclose(infile);
		return tifile;
	}

	tifile = ImportVarData(infile, tifile);
	fclose(infile);
	return tifile;
}

TIFILE_t* FreeTiFile(TIFILE_t * tifile) {
	if (!tifile) return NULL;
	if (tifile->save) FreeSave(tifile->save);

	int i = 0;
	while(tifile->vars[i] != NULL) {
		if (tifile->vars[i]->data) free(tifile->vars[i]->data);
		free(tifile->vars[i]);
		tifile->vars[i] = NULL;
		i++;
	}
	if (tifile->flash) {
		int i;
		for (i = 0; i < 256; i++) {
			if (tifile->flash->data[i]) free(tifile->flash->data[i]);
		}
		free(tifile->flash);
	}
	if (tifile->rom) {
		if (tifile->rom->data) free(tifile->rom->data);
		free(tifile->rom);
	}
	free(tifile);
	return NULL;
}
