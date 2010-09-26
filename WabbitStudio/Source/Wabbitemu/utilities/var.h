#ifndef VAR_H
#define VAR_H

#include "savestate.h"

typedef struct INTELHEX {
	int DataSize;
	int Address;
	int Type;
	unsigned int Data[256];
	int CheckSum;
} INTELHEX_t;

#pragma pack(1)

typedef struct TIFLASH {
	unsigned char sig[8];
	unsigned char rev[2];
	unsigned char flag;
	unsigned char object;
	unsigned char date[4];
	unsigned char namelength;
	unsigned char name[8];
	unsigned char filler[23];
	unsigned char device;
	unsigned char type;
	unsigned char filler2[24];
	unsigned int hexsize;
//	int rpage[256];
	int pagesize[256];
	unsigned char * data[256];
//	unsigned short chksum;

	unsigned int pages;		//total number of pages.

} TIFLASH_t;



typedef struct ROM {
	int size;
	char version[32];
	unsigned char * data;
} ROM_t;

typedef struct TIBACKUP {
	unsigned short headersize;		// size of the header up to name, sometimes ignored
	unsigned short length1;			// data size
	unsigned char vartype;			// what type of varible
	unsigned short length2;			// data size
	unsigned short length3;			// data size
	unsigned short address;			// duplicate of data size
	
	unsigned short length1a;		// Repeats of the data length.
	unsigned char * data1;			// pointer to data

	unsigned short length2a;		// data size
	unsigned char * data2;			// pointer to data

	unsigned short length3a;		// data size
	unsigned char * data3;			// pointer to data


} TIBACKUP_t;

typedef struct TIVAR {
	unsigned short headersize;		// size of the header up to name, sometimes ignored
	unsigned short length;			// data size
	unsigned char vartype;			// what type of varible
	unsigned char name[8];			// null padded name
	unsigned char version;			// 0 83+only
	unsigned char flag;				// bit 7 is if flash 83+only
	unsigned short length2;			// duplicate of data size
	unsigned char * data;			// pointer to data
} TIVAR_t;

typedef struct TIFILE {
	unsigned char sig[8];
	unsigned char subsig[3];
	unsigned char comment[42];
	unsigned short length;
	TIVAR_t * var;
	unsigned short chksum;
	int model;
	int type;
	ROM_t * rom;
	TIFLASH_t * flash;
	SAVESTATE_t* save;
	TIBACKUP_t* backup;
} TIFILE_t;


#pragma pack()

#define TI_FLASH_HEADER_SIZE (8+2+1+1+4+1+8+23+1+1+24+4)	
#define TI_FILE_HEADER_SIZE (8+3+42/*+2*/)	
#define TI_VAR_HEADER_SIZE (2+2+1+8)


#define ROM_TYPE	1		//os
#define FLASH_TYPE	2		//Flash application
#define VAR_TYPE	3		//most varibles can be supported under an umbrella type
#define SAV_TYPE	4		//Wabbit specific saves.
#define BACKUP_TYPE	5		//Wabbit specific saves.
#define LABEL_TYPE	6		//Lab file
#define BREAKPOINT_TYPE 7	//breakpoint file
#define SKIP_TYPE 	8

int FindRomVersion(int, char*, unsigned char*, int);
TIFILE_t* importvar(char *, int, int);
void FreeTiFile(TIFILE_t *);

#endif
