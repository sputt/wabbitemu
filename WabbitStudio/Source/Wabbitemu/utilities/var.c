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


int CmpStringCase(char* str1,unsigned char* str2) {
	return strncasecmp(str1,(char *) str2,strlen(str1));
}


int FindRomVersion(int calc,char* string,unsigned char* rom,int size) {
	int i,b;
	if (calc == -1) {
		if ( size == (128*1024) ) calc = TI_82;
		else if ( size == (256*1024) ) calc = TI_83;
		else if ( (size>=(510*1024)) && (size<=(590*1024))  ) calc = TI_83P;
		else if ( (size>=(1016*1024)) && (size<=(1030*1024))  ) calc = TI_84P;
		else if ( (size>=(2044*1024)) && (size<=(2260*1024)) ) calc = TI_83PSE;
		else {
			puts("not a known rom");
			return -1;
		}
	}
	switch(calc) {
		case TI_82:
			for(i=0;i<(size-strlen(catalog)-10);i++) {
				if (CmpStringCase(catalog,rom+i)==0) {
					calc = TI_85;
						for(i=0;i<(size-strlen(self_test)-10);i++) {
							if (CmpStringCase(self_test,rom+i)==0) break;
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
			if (calc!=TI_82) break;

		case TI_83:
			if (calc == TI_83) {
				for(i=0;i<(size-strlen(txt86)-10);i++) {
					if (CmpStringCase(txt86,rom+i)==0) {
						calc = TI_86;
							for(i=0;i<(size-strlen(self_test)-10);i++) {
								if (CmpStringCase(self_test,rom+i)==0) break;
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

			for(i=0;i<(size-strlen(self_test)-10);i++) {
				if (CmpStringCase(self_test,rom+i)==0) break;
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
			for(i=0;i<(size-strlen(txt73)-10);i++) {
				if (CmpStringCase(txt73,rom+i)==0) {
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


void NullTiFile(TIFILE_t* tifile) {
	tifile->var = NULL;	//make sure its null. mostly for freeing later
	tifile->flash = NULL;
	tifile->rom = NULL;
	tifile->save = NULL;
	tifile->backup = NULL;
}


TIFILE_t* importvar(char * FileName, int SlotSave, int ram) {
	FILE * infile = NULL;
	TIFILE_t * tifile;
	int i,tmp;
	unsigned char * ptr;
	char string[8];

	if (!FileName) return NULL;

	for(i = strlen(FileName); (i>4) && (FileName[i]!='.'); i--) {
		if (	FileName[i-3]=='.' &&
				toupper(FileName[i-2])=='L' &&
				toupper(FileName[i-1])=='A' &&
				toupper(FileName[i-0])=='B' ) {
			tifile = (TIFILE_t*) malloc(sizeof(TIFILE_t));
			if (tifile == NULL) {
				fclose(infile);
				return NULL;
			}
			NullTiFile(tifile);
			tifile->type = LABEL_TYPE;
			return tifile;
		} else if (	FileName[i-3]=='.' &&
				toupper(FileName[i-2])=='B' &&
				toupper(FileName[i-1])=='R' &&
				toupper(FileName[i-0])=='K' ) {
			tifile = (TIFILE_t*) malloc(sizeof(TIFILE_t));
			if (tifile == NULL) {
				fclose(infile);
				return NULL;
			}
			NullTiFile(tifile);
			tifile->type = BREAKPOINT_TYPE;
			return tifile;
		}
	}



	infile = fopen(FileName,"rb");
	if (infile == NULL) {
		//puts("Couldn't open");
		return NULL;
	}


	fread(string,1,8,infile);
	rewind(infile);


	if ((memcmp(string,DETECT_STR,8)==0) ||
		(memcmp(string,DETECT_CMP_STR,8)==0) ) {
		tifile = (TIFILE_t*) malloc(sizeof(TIFILE_t));
		if (tifile == NULL) {
			fclose(infile);
			return NULL;
		}

		NullTiFile(tifile);

		tifile->save = ReadSave(infile);

		if (!tifile->save) {
			free(tifile);
			fclose(infile);
			return NULL;
		}
		tifile->model = tifile->save->model;
		tifile->type = SAV_TYPE;
		fclose(infile);
		return tifile;
	}



	if ( memcmp(string,"**TIFL**",8)==0 ) {
		tifile = (TIFILE_t*) malloc(sizeof(TIFILE_t));
		if (tifile == NULL) {
			fclose(infile);
			return NULL;
		}

		NullTiFile(tifile);


		tifile->flash = (TIFLASH_t*) malloc(sizeof(TIFLASH_t));
		if (tifile->flash == NULL) {
			fclose(infile);
			FreeTiFile(tifile);
			return NULL;
		}


/* Import file Header */
		ptr = (unsigned char *) tifile->flash;
		for(i=0; i<TI_FLASH_HEADER_SIZE && !feof(infile); i++) {
			tmp = fgetc(infile);
			if (tmp == EOF) {
				printf("failed to get the whole header\n");
				fclose(infile);
				FreeTiFile(tifile);
				return NULL;
			}
			ptr[i] =tmp;
		}

		if (!strcmp((char*)tifile->flash->name, "basecode"))
		{
			//its an OS we need to load it
			Load_8xu(infile);
			calcs[gslot].mem_c.flash[0x56] = 0x5A;
			calcs[gslot].mem_c.flash[0x57] = 0xA5;
			calc_reset(gslot);
			calc_run_timed(gslot, 200);
			calcs[gslot].cpu.pio.keypad->on_pressed |= KEY_FALSEPRESS;
			calc_run_timed(gslot, 300);
			calcs[gslot].cpu.pio.keypad->on_pressed &= ~KEY_FALSEPRESS;
			fclose(infile);
			free(tifile->flash);
			tifile->flash = NULL;
			tifile->type = SKIP_TYPE;
			return tifile;
		}


		for(i=0; i<256 ; i++) {
//			tifile->flash->rpage[i]		=-1;
			tifile->flash->pagesize[i]	= 0;
			tifile->flash->data[i]		= NULL;
		}

		INTELHEX_t Record;
		int CurrentPage		= -1;
//		int HighestPage		=  0;
		int HighestAddress	=  0;
		int TotalSize		=  0;
		int TotalPages		=  0;
		int done			=  0;
		int reads;
		char linebuf[600];
		char * error;



		while ( !feof( infile ) && !done) {
			memset(linebuf,0,600);
			error = fgets(linebuf,580,infile);
			if (error != linebuf) {
				fclose(infile);
				FreeTiFile(tifile);
				return NULL;
			}
			if (linebuf[0] == 0) memcpy(linebuf, linebuf+1, 579);
			reads = sscanf(linebuf,":%02X%04X%02X%*s",&Record.DataSize,&Record.Address,&Record.Type);
			if (reads < 3) {
				fclose(infile);
				FreeTiFile(tifile);
				return NULL;
			}
			for(i=0; i < Record.DataSize; i++) {
				reads = sscanf(linebuf+9+(i*2),"%02X",&Record.Data[i]);
				if (reads < 1) {
					fclose(infile);
					FreeTiFile(tifile);
					return NULL;
				}
			}
			reads = sscanf(linebuf+9+(Record.DataSize*2),"%02X",&Record.CheckSum);
			if (reads < 1) {
				fclose(infile);
				FreeTiFile(tifile);
				return NULL;
			}

			switch ( Record.Type ) {
				case 00:
					if (CurrentPage > -1) {
						for( i=0;(i < Record.DataSize) && (( i + Record.Address ) < 0x8000); i++) {
							tifile->flash->data[CurrentPage][i+Record.Address - 0x4000] = Record.Data[i];
						}
						if ( HighestAddress < i+Record.Address ) HighestAddress = i+Record.Address;
					}
					break;
				case 01:
					done = 1;
					if (CurrentPage == -1) {
						printf("invalid current page\n");
						fclose(infile);
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
							fclose(infile);
							FreeTiFile(tifile);
							return NULL;
						}
						memset(tifile->flash->data[CurrentPage],0,16384);	//THIS IS IMPORTANT FOR LINKING, APPS FOR NOW
					}
					HighestAddress	=  0;
					break;
				default:
					printf("unknown record\n");
					fclose(infile);
					FreeTiFile(tifile);
					return NULL;
			}
		}

		if (tifile->flash->device == 0x74) {
			tifile->model = TI_73;
		} else if (tifile->flash->device == 0x73) {
			tifile->model = TI_83P;
		} else {
			fclose(infile);
			FreeTiFile(tifile);
			return NULL;
		}
		tifile->type = FLASH_TYPE;

		fclose(infile);
		return tifile;
	}

/* It maybe a rom if it doesn't have the Standard header */
	if (memcmp(string,"**TI73**",8)!=0 &&
		memcmp(string,"**TI82**",8)!=0 &&
		memcmp(string,"**TI83**",8)!=0 &&
		memcmp(string,"**TI83F*",8)!=0 &&
		memcmp(string,"**TI86**",8)!=0) {
		int size,calc;

//		puts("Might be a rom");


		fseek(infile,0,SEEK_END);
		size = ftell(infile);
		fseek(infile,0,SEEK_SET);

		if ( size == (128*1024) ) calc = TI_82;
		else if ( size == (256*1024) ) calc = TI_83;
		else if ( (size>=(510*1024)) && (size<=(590*1024))  ) calc = TI_83P;
		else if ( (size>=(1016*1024)) && (size<=(1030*1024))  ) calc = TI_84P;
		else if ( (size>=(2044*1024)) && (size<=(2260*1024)) ) calc = TI_83PSE;
		else {
			puts("not a known rom");
			fclose(infile);
			return NULL;
		}

//		printf("Might be a %s \n",CalcModelTxt[calc]);

		tifile = (TIFILE_t*) malloc(sizeof(TIFILE_t));
		if (tifile == NULL) {
			fclose(infile);
			return NULL;
		}



		NullTiFile(tifile);


		tifile->rom = (ROM_t*) malloc(sizeof(ROM_t));
		if (tifile->rom == NULL) {
			fclose(infile);
			FreeTiFile(tifile);
			return NULL;
		}

		tifile->rom->data = (unsigned char*) malloc(size);
		if (tifile->rom->data == NULL) {
			fclose(infile);
			FreeTiFile(tifile);
			return NULL;
		}

		for(i=0; i<size && !feof(infile); i++) {
			tmp = fgetc(infile);
			if (tmp == EOF) {
				fclose(infile);
				FreeTiFile(tifile);
				return NULL;
			}
			tifile->rom->data[i] =tmp;
		}
		fclose(infile);
		tifile->rom->size		= size;
		tifile->type			= ROM_TYPE;

		calc = FindRomVersion(calc,tifile->rom->version,tifile->rom->data,size);
		tifile->model			= calc;

//		printf("It's a %s.\nVersion: %s \n",CalcModelTxt[calc],tifile->rom->version);
		return tifile;
	}

	tifile = (TIFILE_t*) malloc(sizeof(TIFILE_t));
	if (tifile == NULL) {
		fclose(infile);
		return NULL;
	}


	NullTiFile(tifile);

/* Import file Header */
	ptr = (unsigned char *) tifile;
	for(i=0; i<TI_FILE_HEADER_SIZE && !feof(infile); i++) {
		tmp = fgetc(infile);
		if (tmp == EOF) {
			fclose(infile);
			FreeTiFile(tifile);
			return NULL;
		}
		ptr[i] =tmp;
	}


	if ( memcmp(tifile->sig,"**TI73**",8)==0 ) tifile->model = TI_73;
	else if ( memcmp(tifile->sig,"**TI82**",8)==0 ) tifile->model = TI_82;
	else if ( memcmp(tifile->sig,"**TI83**",8)==0 ) tifile->model = TI_83;
	else if ( memcmp(tifile->sig,"**TI83F*",8)==0 ) tifile->model = TI_83P;
	else if ( memcmp(tifile->sig,"**TI86**",8)==0 ) tifile->model = TI_86;
	else {
		fclose(infile);
		FreeTiFile(tifile);
		return NULL;
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

	if ( (tifile->model == TI_73 && vartype==0x13) ||
		 (tifile->model == TI_82 && vartype==0x0F) ) {
//		puts("It's a back up");
		tifile->backup = (TIBACKUP_t*) malloc(sizeof(TIBACKUP_t));
		if (tifile->backup == NULL) {
			fclose(infile);
			FreeTiFile(tifile);
			return NULL;
		}
		tifile->backup->data1 = NULL;
		tifile->backup->data2 = NULL;
		tifile->backup->data3 = NULL;

		tifile->backup->headersize	= headersize;
		tifile->backup->length1		= length;
		tifile->backup->vartype		= vartype;

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
		for(i=0; i<tifile->backup->length1 && !feof(infile); i++) {
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
		for(i=0; i<tifile->backup->length2 && !feof(infile); i++) {
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
//		puts("Back up read");
		tifile->type	= BACKUP_TYPE;
//		puts("back up made");
		return tifile;

	}

	tifile->var = (TIVAR_t*) malloc(sizeof(TIVAR_t));
	if (tifile->var == NULL) {
		fclose(infile);
		FreeTiFile(tifile);
		return NULL;
	}

	tifile->type				= VAR_TYPE;
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

			groupFile->var->data = (unsigned char *) malloc(groupFile->var->length);
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
			link_send_var(&calcs[SlotSave].cpu, groupFile, (SEND_FLAG)ram);

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
		free(tifile->var);
		tifile->type = SKIP_TYPE;
		return tifile;
	}
	tifile->var->headersize		= headersize;
	tifile->var->length			= length;
	tifile->var->vartype		= vartype;
	ptr = tifile->var->name;
	for(i=0; i<8 && !feof(infile); i++) {
		tmpread(infile);
		ptr[i] =tmp;
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
		if (tmp == EOF) {
			fclose(infile);
			FreeTiFile(tifile);
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

	for(i = 0; i<tifile->var->length && !feof(infile); i++) {
		tmp = fgetc(infile);
		if (tmp == EOF) {
			fclose(infile);
			FreeTiFile(tifile);
			return NULL;
		}
		tifile->var->data[i] =tmp;
	}


	tifile->chksum = ( fgetc(infile) & 0xFF ) + ( (  fgetc(infile) & 0xFF ) << 8 );

	fclose(infile);
//	puts("var read");

	return tifile;
}


void FreeTiFile(TIFILE_t * tifile) {
	if (!tifile) return;
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
}




