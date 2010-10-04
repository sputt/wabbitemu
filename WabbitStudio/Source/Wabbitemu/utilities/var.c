#include "stdafx.h"

#include "var.h"
#include "calc.h"

char self_test[] = "Self Test?";
char catalog[] = "CATALOG";
char txt73[] = "GRAPH  EXPLORER  SOFTWARE";
char txt86[] = "Already Installed";


#define tmpread( fp ) \
	tmp = fgetc(fp); \
	if (tmp == EOF) { \
		fclose(fp); \
		FreeTiFile(tifile); \
		return NULL; \
	}


int CmpStringCase(char* str1, unsigned char* str2) {
	return _strnicmp(str1, (char *) str2, strlen(str1));
}


int FindRomVersion(int calc, char *string, unsigned char *rom, int size) {
	int i,b;
	if (calc == -1) {
		if ( size == (128*1024) ) calc = TI_82;
		else if ( size == (256*1024) ) calc = TI_83;
		else if ( (size>=(510*1024)) && (size<=(590*1024))  ) calc = TI_83P;
		else if ( (size>=(1016*1024)) && (size<=(1030*1024))  ) calc = TI_84P;
		else if ( (size>=(2044*1024)) && (size<=(2260*1024)) ) calc = TI_83PSE;
		else {
			_putts(_T("not a known rom"));
			return -1;
		}
	}
	switch (calc) {
		case TI_82:
			for(i = 0; i < (size - strlen(catalog) - 10); i++) {
				if (CmpStringCase(catalog, rom + i)==0) {
					calc = TI_85;
						for(i = 0; i < (size - strlen(self_test) - 10); i++) {
							if (CmpStringCase(self_test, rom + i) == 0) break;
						}
						for(; i < (size - 40); i++) {
							if (isdigit(rom[i])) break;
						}
						if (i < (size - 40)) {
							for(b = 0; (b + i) < (size-4) && b  <32; b++) {
								if (rom[b+i] != ' ') string[b] = rom[b+i];
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
			if (calc!=TI_82) break;

		case TI_83:
			if (calc == TI_83) {
				for(i=0;i<(size - strlen(txt86) - 10);i++) {
					if (CmpStringCase(txt86, rom + i)==0) {
						calc = TI_86;
							for(i=0;i<(size - strlen(self_test) - 10); i++) {
								if (CmpStringCase(self_test, rom + i) == 0) break;
							}
							for(;i<(size-40);i++) {
								if (isdigit(rom[i])) break;
							}
							if (i<(size-40)) {
								for(b=0;(b+i)<(size-4) && b<32;b++) {
									if (rom[b+i] != ' ') string[b] = rom[b+i];
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
			}
			if (calc==TI_86) break;

			for(i=0;i<(size - strlen(self_test) - 10); i++) {
				if (CmpStringCase(self_test, rom + i)==0) break;
			}
			if ((i+64)<size) {
				i+=10;
				for(b=0;b<32;b++) {
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
		case TI_83P:
			for(i = 0; i < (size - strlen(txt73) - 10); i++) {
				if (CmpStringCase(txt73, rom + i)==0) {
					calc = TI_73;
					break;
				}
			}
		case TI_84P:
		case TI_83PSE:
		case TI_84PSE:
			i = 0x0064;
			for(b=0;b<32;b++) {
				string[b] = rom[i++];
			}
			string[31] = 0;
			if (calc == TI_83PSE) {
				if (string[0] > '1') {
					calc = TI_84PSE;
				}
			}
			break;
	}
	return calc;
}

TIFILE_t* ImportFlashFile(FILE *infile, TIFILE_t *tifile) {
	int i, tmp;
	for(i = 0; i < 256; i++) {
		tifile->flash->pagesize[i]	= 0;
		tifile->flash->data[i]		= NULL;
	}

	INTELHEX_t Record;
	int CurrentPage		= -1;
	int HighestAddress	=  0;
	int TotalSize		=  0;
	int TotalPages		=  0;
	int done			=  0;
	int reads;
	char linebuf[600];
	char *error;
	
	while (!feof( infile ) && !done) {
		memset(linebuf, 0, ARRAYSIZE(linebuf));
		error = fgets(linebuf,580,infile);
		if (error != linebuf) {
			FreeTiFile(tifile);
			return NULL;
		}
		if (linebuf[0] == 0) memcpy(linebuf, linebuf+1, 579);
#ifdef WINVER
		reads = sscanf_s(linebuf, ":%02X%04X%02X%*s", &Record.DataSize, &Record.Address, &Record.Type);
#else
		reads = sscanf(linebuf, ":%02X%04X%02X%*s", &Record.DataSize, &Record.Address, &Record.Type);
#endif
		if (reads < 3) {
			FreeTiFile(tifile);
			return NULL;
		}
		for(i = 0; i < Record.DataSize; i++) {
#ifdef WINVER
			reads = sscanf_s(linebuf+9+(i*2),"%02X",&Record.Data[i]);
#else
			reads = sscanf(linebuf+9+(i*2),"%02X",&Record.Data[i]);
#endif
			if (reads < 1) {
				FreeTiFile(tifile);
				return NULL;
			}
		}
#ifdef WINVER
		reads = sscanf_s(linebuf+9+(Record.DataSize*2), "%02X", &Record.CheckSum);
#else
		reads = sscanf(linebuf+9+(Record.DataSize*2), "%02X", &Record.CheckSum);
#endif
		if (reads < 1) {
			FreeTiFile(tifile);
			return NULL;
		}

		switch ( Record.Type ) {
			case 00:
				if (CurrentPage > -1) {
					for( i=0;(i < Record.DataSize) && (( i + Record.Address ) < 0x8000); i++) {
						tifile->flash->data[CurrentPage][i+Record.Address - 0x4000] = Record.Data[i];
					}
					if ( HighestAddress < i+Record.Address ) HighestAddress = (int) (i + Record.Address);
				}
				break;
			case 01:
				done = 1;
				if (CurrentPage == -1) {
					printf("invalid current page\n");
					FreeTiFile(tifile);
					return NULL;
				}
				TotalSize += (HighestAddress - 0x4000);
				tifile->flash->pagesize[CurrentPage] = (HighestAddress - 0x4000);
				tifile->flash->pages = TotalPages;
				break;
			case 02:
				if (CurrentPage > -1) {
					TotalSize += 0x4000;
					tifile->flash->pagesize[CurrentPage] = (HighestAddress - 0x4000);
				}
				TotalPages++;
				CurrentPage = Record.Data[1];
				if (tifile->flash->data[CurrentPage]==0) {
					tifile->flash->data[CurrentPage] = (unsigned char *) malloc(16384);
					if (tifile->flash->data[CurrentPage] == NULL) {
						FreeTiFile(tifile);
						return NULL;
					}
					memset(tifile->flash->data[CurrentPage], 0, 16384);	//THIS IS IMPORTANT FOR LINKING, APPS FOR NOW
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
	int calc, i, tmp;

	fseek(infile, 0, SEEK_END);
	size = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	if (size == (128*1024) ) calc = TI_82;
	else if (size == (256*1024) ) calc = TI_83;
	else if ((size>=(510*1024)) && (size<=(590*1024))  ) calc = TI_83P;
	else if ((size>=(1016*1024)) && (size<=(1030*1024))  ) calc = TI_84P;
	else if ((size>=(2044*1024)) && (size<=(2260*1024)) ) calc = TI_83PSE;
	else {
		puts("not a known rom");
		FreeTiFile(tifile);
		return NULL;
	}

	tifile->rom = (ROM_t*) malloc(sizeof(ROM_t));
	if (tifile->rom == NULL) {
		FreeTiFile(tifile);
		return NULL;
	}

	tifile->rom->data = (unsigned char *) malloc(size);
	if (tifile->rom->data == NULL) {
		FreeTiFile(tifile);
		return NULL;
	}

	for(i = 0; i < size && !feof(infile); i++) {
		tmp = fgetc(infile);
		if (tmp == EOF) {
			FreeTiFile(tifile);
			return NULL;
		}
		tifile->rom->data[i] = tmp;
	}
	tifile->rom->size		= size;
	calc = FindRomVersion(calc, tifile->rom->version, tifile->rom->data, size);
	tifile->model			= calc;

	return tifile;
}

TIFILE_t* ImportBackup(FILE *infile, TIFILE_t *tifile) {
	int i, tmp;
	tifile->backup = (TIBACKUP_t *) malloc(sizeof(TIBACKUP_t));
	if (tifile->backup == NULL) {
		FreeTiFile(tifile);
		return NULL;
	}
	tifile->backup->data1 = NULL;
	tifile->backup->data2 = NULL;
	tifile->backup->data3 = NULL;

	tmpread(infile);
	tifile->backup->length2 = tmp;
	tmpread(infile);
	tifile->backup->length2 += tmp<<8;

	tmpread(infile);
	tifile->backup->length3 = tmp;
	tmpread(infile);
	tifile->backup->length3 += tmp<<8;

	tmpread(infile);
	tifile->backup->address = tmp;
	tmpread(infile);
	tifile->backup->address += tmp<<8;

	tmpread(infile);
	tifile->backup->length1a = tmp;
	tmpread(infile);
	tifile->backup->length1a += tmp<<8;

	tifile->backup->data1 = (unsigned char *) malloc(tifile->backup->length1);
	if (tifile->backup->data1 == NULL) {
		fclose(infile);
		FreeTiFile(tifile);
		return NULL;
	}
	for(i = 0; i < tifile->backup->length1 && !feof(infile); i++) {
		tmpread(infile);
		tifile->backup->data1[i] =tmp;
	}


	tmpread(infile);
	tifile->backup->length2a = tmp;
	tmpread(infile);
	tifile->backup->length2a += tmp<<8;

	tifile->backup->data2 = (unsigned char *) malloc(tifile->backup->length2);
	if (tifile->backup->data2 == NULL) {
		fclose(infile);
		FreeTiFile(tifile);
		return NULL;
	}
	for(i = 0; i < tifile->backup->length2 && !feof(infile); i++) {
		tmpread(infile);
		tifile->backup->data2[i] =tmp;
	}

	tmpread(infile);
	tifile->backup->length3a = tmp;
	tmpread(infile);
	tifile->backup->length3a += tmp<<8;

	tifile->backup->data3 = (unsigned char *) malloc(tifile->backup->length3);
	if (tifile->backup->data3 == NULL) {
		fclose(infile);
		FreeTiFile(tifile);
		return NULL;
	}
	for(i=0; i<tifile->backup->length3 && !feof(infile); i++) {
		tmpread(infile);
		tifile->backup->data3[i] =tmp;
	}

	tifile->chksum = ( fgetc(infile) & 0xFF ) + ( (  fgetc(infile) & 0xFF ) << 8 );

	fclose(infile);
	tifile->type	= BACKUP_TYPE;
	return tifile;
}

void NullTiFile(TIFILE_t* tifile) {
	tifile->var = NULL;	//make sure its null. mostly for freeing later
	tifile->flash = NULL;
	tifile->rom = NULL;
	tifile->save = NULL;
	tifile->backup = NULL;
}

TIFILE_t* InitTiFile() {
	TIFILE_t *tifile = (TIFILE_t*) malloc(sizeof(TIFILE_t));
	if (tifile == NULL)
		return NULL;
	NullTiFile(tifile);
	return tifile;
}

void ReadTiFileHeader(FILE *infile, TIFILE_t *tifile) {
	char headerString[16];
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
			ptr[i] = tmp;
		}
		return;
	}

	/* It maybe a rom if it doesn't have the Standard header */
	if (_strnicmp(headerString, "**TI73**", 8) &&
		_strnicmp(headerString, "**TI82**", 8) &&
		_strnicmp(headerString, "**TI83**", 8) &&
		_strnicmp(headerString, "**TI83F*", 8) &&
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
		ptr[i] = tmp;
	}

	if (!_strnicmp((char *) tifile->sig, "**TI73**", 8)) tifile->model = TI_73;
	else if (!_strnicmp((char *) tifile->sig, "**TI82**", 8)) tifile->model = TI_82;
	else if (!_strnicmp((char *) tifile->sig, "**TI83**", 8)) tifile->model = TI_83;
	else if (!_strnicmp((char *) tifile->sig, "**TI83F*", 8)) tifile->model = TI_83P;
	else if (!_strnicmp((char *) tifile->sig, "**TI86**", 8)) tifile->model = TI_86;
	else {
		FreeTiFile(tifile);
		return;
	}
	return;
}

TIFILE_t* ImportVarData(FILE *infile, TIFILE_t *tifile) {
	switch (tifile->type) {
		case ROM_TYPE:
			return ImportROMFile(infile, tifile);
			break;
		case FLASH_TYPE:
			return ImportFlashFile(infile, tifile);
		case SAV_TYPE:
			tifile->save = ReadSave(infile);
			if (!tifile->save)
				return FreeTiFile(tifile);
			tifile->model = tifile->save->model;
			return tifile;
	}

	int i, tmp;
	unsigned short headersize;
	unsigned short length;
	unsigned char vartype, *ptr;
	unsigned short length2;

	tmpread(infile);
	length2 = tmp;
	tmpread(infile);
	length2 += tmp<<8;

	tmpread(infile);
	headersize = tmp;
	tmpread(infile);
	headersize += tmp<<8;

	tmpread(infile);
	length = tmp;
	tmpread(infile);
	length += tmp<<8;

	tmpread(infile);
	vartype = tmp;

	if ((tifile->model == TI_73 && vartype==0x13) ||
		(tifile->model == TI_82 && vartype==0x0F)) {
		tifile->backup->headersize	= headersize;
		tifile->backup->length1		= length;
		tifile->backup->vartype		= vartype;
		return ImportBackup(infile, tifile);
	}

	tifile->var = (TIVAR_t *) malloc(sizeof(TIVAR_t));
	if (tifile->var == NULL) {
		FreeTiFile(tifile);
		return NULL;
	}

	tifile->type				= VAR_TYPE;

	tifile->var->headersize		= headersize;
	tifile->var->length			= length;
	tifile->var->vartype		= vartype;
	ptr = tifile->var->name;
	for(i = 0; i < 8 && !feof(infile); i++) {
		tmpread(infile);
		ptr[i] = tmp;
	}

	if (tifile->model == TI_83P) {
		tmp = fgetc(infile);
		if (tmp == EOF) {
			fclose(infile);
			FreeTiFile(tifile);
			return NULL;
		}
		ptr[i++] =tmp;
		tmp = fgetc(infile);
		if (tmp == EOF)
			return FreeTiFile(tifile);
		ptr[i++] =tmp;
	} else {
		ptr[i++] =0;
		ptr[i++] =0;
	}
	tmp = fgetc(infile);
	if (tmp == EOF) {
		fclose(infile);
		FreeTiFile(tifile);
		return NULL;
	}
	ptr[i++] =tmp;
	tmp = fgetc(infile);
	if (tmp == EOF) {
		fclose(infile);
		FreeTiFile(tifile);
		return NULL;
	}
	ptr[i++] =tmp;

	tifile->var->data = (unsigned char *) malloc(tifile->var->length);
	if (tifile->var->data == NULL) {
		fclose(infile);
		FreeTiFile(tifile);
		return NULL;
	}

	i = 0;
	if (tifile->model == TI_86)
		fgetc(infile);

	for(i = 0; i < tifile->var->length && !feof(infile); i++) {
		tmp = fgetc(infile);
		if (tmp == EOF) {
			fclose(infile);
			FreeTiFile(tifile);
			return NULL;
		}
		tifile->var->data[i] =tmp;
	}


	tifile->chksum = ( fgetc(infile) & 0xFF ) + ( (  fgetc(infile) & 0xFF ) << 8 );
}

TIFILE_t* newimportvar(LPCTSTR filePath) {
	FILE *infile = NULL;
	TIFILE_t *tifile;
	
	TCHAR extension[_MAX_EXT];
	_tsplitpath_s(filePath, NULL, 0, NULL, 0, NULL, 0, extension, ARRAYSIZE(extension));

	tifile = InitTiFile();
	if (tifile == NULL)
		return NULL;

	if (!_tcsicmp(extension, _T(".lab"))) {
		tifile->type = LABEL_TYPE;
		return tifile;
	}

	if (!_tcsicmp(extension, _T(".brk"))) {
		tifile->type = BREAKPOINT_TYPE;
		return tifile;
	}
#ifdef WINVER
	_tfopen_s(&infile, filePath, _T("rb"));
#else
	infile = fopen(filePame, "rb");
#endif
	if (infile == NULL) {
		FreeTiFile(tifile);
		return NULL;
	}

	ReadTiFileHeader(infile, tifile);

	ImportVarData(infile, tifile);
	fclose(infile);
	return tifile;
}

TIFILE_t* importvar(LPCTSTR FileName, int SlotSave, int ram) {
	FILE * infile = NULL;
	TIFILE_t * tifile;
	size_t i;
	int tmp;
	_TUCHAR *ptr;
	TCHAR string[8];

	if ( memcmp(string, _T("**TIFL**"),8)==0 ) {

/* Import file Header */
		/*if (!_tcscmp((const char *) tifile->flash->name, _T("basecode")))
		{
			//its an OS we need to load it
			assert("Not current supported", 0);
			Load_8xu(infile);
			calcs[gslot].mem_c.flash[0x56] = 0x5A;
			calcs[gslot].mem_c.flash[0x57] = 0xA5;
			//is it ok to clear the cert here?
			calc_reset(&calcs[gslot]);
			calc_turn_on(&calcs[gslot]);
			fclose(infile);
			free(tifile->flash);
			tifile->flash = NULL;
			tifile->type = SKIP_TYPE;
			return tifile;
		}*/
	}

	unsigned short headersize;
	unsigned short length;
	unsigned char vartype;
	unsigned short length2;

	tmpread(infile);
	length2 = tmp;
	tmpread(infile);
	length2 += tmp<<8;

	tmpread(infile);
	headersize = tmp;
	tmpread(infile);
	headersize += tmp<<8;

	tmpread(infile);
	length = tmp;
	tmpread(infile);
	length += tmp<<8;

	tmpread(infile);
	vartype = tmp;

	if (length2 > length + 17)
	{
		while (length2 > length + 17) {
			TIFILE_t* groupFile = (TIFILE_t*) malloc(sizeof(TIFILE_t));
			if (groupFile == NULL)
			{
				fclose(infile);
				return NULL;
			}
			NullTiFile(groupFile);
			groupFile->var = (TIVAR_t*) malloc(sizeof(TIVAR_t));
			groupFile->type = VAR_TYPE;
			if (groupFile->var == NULL) {
				fclose(infile);
				FreeTiFile(groupFile);
				return NULL;
			}
			groupFile->var->headersize		= headersize;
			groupFile->var->length			= length;
			groupFile->var->vartype			= vartype;
			groupFile->var->version 		= 0;
			ptr = groupFile->var->name;
			//ptr[0] = vartype;
			for(i=0; i<8 && !feof(infile); i++) {
				tmpread(infile);
				ptr[i] =tmp;
			}

			//read in file data
			if (tifile->model == TI_83P) {
				tmp = fgetc(infile);
				if (tmp == EOF) {
					fclose(infile);
					FreeTiFile(groupFile);
					return NULL;
				}
				ptr[i++] =tmp;
				tmp = fgetc(infile);
				if (tmp == EOF) {
					fclose(infile);
					FreeTiFile(groupFile);
					return NULL;
				}
				ptr[i++] =tmp;
			} else {
				ptr[i++] =0;
				ptr[i++] =0;
			}
			tmp = fgetc(infile);
			if (tmp == EOF) {
				fclose(infile);
				FreeTiFile(groupFile);
				return NULL;
			}
			ptr[i++] =tmp;
			tmp = fgetc(infile);
			if (tmp == EOF) {
				fclose(infile);
				FreeTiFile(groupFile);
				return NULL;
			}
			ptr[i++] =tmp;

			groupFile->var->data = (_TUCHAR *) malloc(groupFile->var->length);
			if (groupFile->var->data == NULL) {
				fclose(infile);
				FreeTiFile(groupFile);
				return NULL;
			}

			i = 0;
			if (groupFile->model == TI_86)
				fgetc(infile);

			for(i = 0; i<groupFile->var->length && !feof(infile); i++) {
				tmp = fgetc(infile);
				if (tmp == EOF) {
					fclose(infile);
					FreeTiFile(groupFile);
					return NULL;
				}
				groupFile->var->data[i] =tmp;
			}
			if (SlotSave != -1)
			{
				link_send_var(&calcs[SlotSave].cpu, groupFile, (SEND_FLAG)ram);
			}

			//groupFile->chksum = ( fgetc(infile) & 0xFF ) + ( (  fgetc(infile) & 0xFF ) << 8 );
			length2 -= length + headersize;

			tmp = fgetc(infile);
			if (tmp == EOF) {
				FreeTiFile(groupFile);
				goto Done_Group;
			}

			headersize = tmp;
			tmp = fgetc(infile);
			if (tmp == EOF) {
				FreeTiFile(groupFile);
				goto Done_Group;
			}
			headersize += tmp<<8;

			tmp = fgetc(infile);
			if (tmp == EOF) {
				FreeTiFile(groupFile);
				goto Done_Group;
			}
			length = tmp;
			tmp = fgetc(infile);
			if (tmp == EOF) {
				FreeTiFile(groupFile);
				goto Done_Group;
			}
			length += tmp<<8;

			tmp = fgetc(infile);
			if (tmp == EOF) {
				FreeTiFile(groupFile);
				goto Done_Group;
			}
			vartype = tmp;
			FreeTiFile(groupFile);
		}
Done_Group:
		fclose(infile);
		tifile->var->data = NULL;
		tifile->type = SKIP_TYPE;
		return tifile;
	}
	

	return tifile;
}


TIFILE_t* FreeTiFile(TIFILE_t * tifile) {
	if (!tifile) return NULL;
	if (tifile->save) FreeSave(tifile->save);

	if (tifile->var) {
		if (tifile->var->data) free(tifile->var->data);
		free(tifile->var);
	}
	if (tifile->flash) {
		int i;
		for (i=0;i<256;i++) {
			if (tifile->flash->data[i]) free(tifile->flash->data[i]);
		}
		free(tifile->flash);
	}
	if (tifile->rom) {
		if (tifile->rom->data) free(tifile->rom->data);
		free(tifile->rom);
	}
	free(tifile);
	tifile = NULL;
	return NULL;
}




