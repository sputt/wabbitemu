#include <stdio.h>
#include <string.h>
#include "core.h"
#include "savestate.h"
#include "lcd.h"
#include "link.h"
#include "calc.h"
#include "zlibcmp.h"
//#include "stdint.h"
#include "83psehw.h"

BOOL cmpTags(char* str1,char* str2) {
	int i;
	for(i=0;i<4;i++) {
		if (str1[i] != str2[i]) return FALSE;
	}
	return TRUE;
}

int fputi(unsigned int integer, FILE* stream) {
	int r;
	r = fputc( ( ( integer ) & 0xFF ) , stream);
	if (r == EOF) return EOF;
	r = fputc( ( ( integer >> 8 ) & 0xFF ) , stream);
	if (r == EOF) return EOF;
	r = fputc( ( ( integer >> 16 ) & 0xFF ) , stream);
	if (r == EOF) return EOF;
	r = fputc( ( ( integer >>24 ) & 0xFF ) , stream);
	return r;
}
	
int fgeti(FILE* stream) {
	int r;
	r  = fgetc(stream);
	r += fgetc(stream)<<8;
	r += fgetc(stream)<<16;
	r += fgetc(stream)<<24;
	return r;
}

SAVESTATE_t* CreateSave(char* author, char* comment , int model) {
	SAVESTATE_t* save = (SAVESTATE_t*) malloc(sizeof(SAVESTATE_t));
	if (!save) return NULL;

	save->version_major = CUR_MAJOR;
	save->version_minor = CUR_MINOR;
	save->version_build = CUR_BUILD;

	memset(save->author, 0, sizeof(save->author));
	strncpy(save->author, author, sizeof(save->author));

	memset(save->comment, 0, sizeof(save->comment));
	strncpy(save->comment, comment, sizeof(save->comment));
	
	save->model = model;
	save->chunk_count = 0;
	
	u_int i;
	for(i = 0; i < NumElm(save->chunks); i++) {
		save->chunks[i] = NULL;
	}
	return save;
}

void ClearSave(SAVESTATE_t* save) {
	int i;
	
	if (save == NULL) return;

	for(i=0;i<save->chunk_count;i++) {
		if (save->chunks[i]) {
			if (save->chunks[i]->data) {
				free(save->chunks[i]->data);
				save->chunks[i]->data = NULL;
			}
/*
				if (save->chunks[i]->cpdata) {
					free(save->chunks[i]->cpdata);
					save->chunks[i]->cpdata = NULL;
				}
*/
			free(save->chunks[i]);

			save->chunks[i] = NULL;
		}
	}

}

void FreeSave(SAVESTATE_t* save) {
	ClearSave(save);
	free(save);
}

CHUNK_t* FindChunk(SAVESTATE_t* save,char* tag) {
	int i;
	for(i=0;i<save->chunk_count;i++) {
		if ( cmpTags(save->chunks[i]->tag,tag) == TRUE ) {
			save->chunks[i]->pnt = 0;
			return save->chunks[i];
		}
	}	
	return NULL;
}

CHUNK_t* NewChunk(SAVESTATE_t* save, char* tag) {
	int chunk = save->chunk_count;

	
	if ( FindChunk(save,tag) != NULL ) {
		printf("Error: chunk '%s' already exists",tag);
		return NULL;
	}

	if (save->chunks[chunk] != NULL) {
		puts("Error new chunk was not null.");
	}
	save->chunks[chunk] = (CHUNK_t *) malloc(sizeof(CHUNK_t));
	if (!save->chunks[chunk]) {
		puts("Chunk could not be created");
		return NULL;
	}

	save->chunks[chunk]->tag[0]	= tag[0];
	save->chunks[chunk]->tag[1]	= tag[1];
	save->chunks[chunk]->tag[2]	= tag[2];
	save->chunks[chunk]->tag[3]	= tag[3];
	//memcpy(save->chunks[chunk]->tag, tag, sizeof()
	save->chunks[chunk]->size	= 0;
	save->chunks[chunk]->data	= NULL;
	save->chunks[chunk]->pnt	= 0;
	save->chunk_count++;
	return save->chunks[chunk];
}

BOOL DelChunk(SAVESTATE_t* save,char* tag) {
	int i;
	for(i=0;i<save->chunk_count;i++) {
		if ( cmpTags(save->chunks[i]->tag,tag) == TRUE ) {
			if (save->chunks[i]->data) free(save->chunks[i]->data);
			if (save->chunks[i]) free(save->chunks[i]);
			for(;i<(save->chunk_count-1);i++) {
				save->chunks[i] = save->chunks[i+1];
			}
			save->chunks[i] = NULL;
			save->chunk_count--;
			return TRUE;
		}
	}	
	return FALSE;
}


void CheckPNT(CHUNK_t* chunk) {
	if (chunk->size < chunk->pnt) {
		printf("Chunk size is %d while pnt is %d \n",chunk->size,chunk->pnt);
	}
}

BOOL WriteChar(CHUNK_t* chunk, unsigned char value) {
	unsigned char* tmppnt;
	tmppnt = (unsigned char *) realloc(chunk->data,chunk->size+sizeof(char));
	if (tmppnt == NULL) {
		puts("Error could not realloc data");
		return FALSE;
	}
	chunk->data = tmppnt;
	chunk->data[chunk->size] = value;
	chunk->size += sizeof(char);
	return TRUE;
}


BOOL WriteShort(CHUNK_t* chunk, unsigned short value) {
	int i;
	unsigned char* tmppnt;
	unsigned char* pnt = (unsigned char*)(&value);
	tmppnt = (unsigned char *) realloc(chunk->data,chunk->size+sizeof(short));
	if (tmppnt == NULL) {
		puts("Error could not realloc data");
		return FALSE;
	}
	chunk->data = tmppnt;
#ifdef __BIG_ENDIAN__
	for(i=sizeof(short)-1;i>=0;i--) {
#else
	for(i=0;i<sizeof(short);i++) {
#endif	
		chunk->data[i+chunk->size] = *pnt++;
	}
	chunk->size += sizeof(short);
	return TRUE;
}
BOOL WriteInt(CHUNK_t* chunk, unsigned int value) {
	int i;
	unsigned char* tmppnt;
	unsigned char* pnt = (unsigned char*)(&value);
	tmppnt = (unsigned char*) realloc(chunk->data,chunk->size+sizeof(int));
	if (tmppnt == NULL) {
		puts("Error could not realloc data");
		return FALSE;
	}
	chunk->data = tmppnt;
#ifdef __BIG_ENDIAN__
	for(i=sizeof(int)-1;i>=0;i--) {
#else
	for(i=0;i<sizeof(int);i++) {
#endif
		chunk->data[i+chunk->size] = *pnt++;
	}
	chunk->size += sizeof(int);
	return TRUE;
}

BOOL WriteLong(CHUNK_t* chunk, unsigned long long value) {
	int i;
	unsigned char* tmppnt;
	unsigned char* pnt = (unsigned char*)(&value);
	tmppnt = (unsigned char*) realloc(chunk->data,chunk->size+sizeof(long long));
	if (tmppnt == NULL) {
		puts("Error could not realloc data");
		return FALSE;
	}
	chunk->data = tmppnt;
#ifdef __BIG_ENDIAN__
	for(i=sizeof(long long)-1;i>=0;i--) {
#else
	for(i=0;i<sizeof(long long);i++) {
#endif
		chunk->data[i+chunk->size] = *pnt++;
	}
	chunk->size += sizeof(long long);
	return TRUE;
}

BOOL WriteFloat(CHUNK_t* chunk, float value) {
	int i;
	unsigned char* tmppnt;
	unsigned char* pnt = (unsigned char*)(&value);
	tmppnt = (unsigned char*) realloc(chunk->data,chunk->size+sizeof(float));
	if (tmppnt == NULL) {
		puts("Error could not realloc data");
		return FALSE;
	}
	chunk->data = tmppnt;
#ifdef __BIG_ENDIAN__
	for(i=sizeof(float)-1;i>=0;i--) {
#else
	for(i=0;i<sizeof(float);i++) {
#endif
		chunk->data[i+chunk->size] = *pnt++;
	}
	chunk->size += sizeof(float);
	return TRUE;
}	
BOOL WriteDouble(CHUNK_t* chunk, double value) {
	int i;
	unsigned char* tmppnt;
	unsigned char* pnt = (unsigned char*)(&value);
	tmppnt = (unsigned char*) realloc(chunk->data,chunk->size+sizeof(double));
	if (tmppnt == NULL) {
		puts("Error could not realloc data");
		return FALSE;
	}
	chunk->data = tmppnt;
#ifdef __BIG_ENDIAN__
	for(i=sizeof(double)-1;i>=0;i--) {
#else
	for(i=0;i<sizeof(double);i++) {
#endif
		chunk->data[i+chunk->size] = *pnt++;
	}
	chunk->size += sizeof(double);
	return TRUE;
}

BOOL WriteBlock(CHUNK_t* chunk, unsigned char* pnt, int length) {
	int i;
	unsigned char* tmppnt;
	tmppnt = (unsigned char*) realloc(chunk->data,chunk->size+length);
	if (tmppnt == NULL) {
		puts("Error could not realloc data");
		return FALSE;
	}
	chunk->data = tmppnt;
	for(i=0;i<length;i++) {
		chunk->data[i+chunk->size] = pnt[i];
	}
	chunk->size += length;
	return TRUE;
}		
	

	
unsigned char ReadChar(CHUNK_t* chunk) {
	unsigned char value;
	value = chunk->data[chunk->pnt];
	chunk->pnt += sizeof(char);
	CheckPNT(chunk);
	return value;
}

unsigned short ReadShort(CHUNK_t* chunk) {
	int i;
	unsigned short value;
	unsigned char* pnt = (unsigned char*)(&value);
#ifdef __BIG_ENDIAN__
	for(i=sizeof(short)-1;i>=0;i--) {
#else
	for(i=0;i<sizeof(short);i++) {
#endif
		*pnt++ = chunk->data[i+chunk->pnt];
	}
	chunk->pnt += sizeof(short);
	CheckPNT(chunk);
	return value;
}

unsigned int ReadInt(CHUNK_t* chunk) {
	int i;
	unsigned int value;
	unsigned char* pnt = (unsigned char*)(&value);
#ifdef __BIG_ENDIAN__
	for(i=sizeof(int)-1;i>=0;i--) {
#else
	for(i=0;i<sizeof(int);i++) {
#endif
		*pnt++ = chunk->data[i+chunk->pnt];
	}
	chunk->pnt += sizeof(int);
	CheckPNT(chunk);
	return value;
}

float ReadFloat(CHUNK_t* chunk) {
	int i;
	float value;
	unsigned char* pnt = (unsigned char*)(&value);
#ifdef __BIG_ENDIAN__
	for(i=sizeof(float)-1;i>=0;i--) {
#else
	for(i=0;i<sizeof(float);i++) {
#endif
		*pnt++ = chunk->data[i+chunk->pnt];
	}
	chunk->pnt += sizeof(float);
	CheckPNT(chunk);
	return value;
}


double ReadDouble(CHUNK_t* chunk) {
	int i;
	double value;
	unsigned char* pnt = (unsigned char*)(&value);
#ifdef __BIG_ENDIAN__
	for(i=sizeof(double)-1;i>=0;i--) {
#else
	for(i=0;i<sizeof(double);i++) {
#endif
		*pnt++ = chunk->data[i+chunk->pnt];
	}
	chunk->pnt += sizeof(double);
	CheckPNT(chunk);
	return value;
}

unsigned long long ReadLong(CHUNK_t* chunk) {
	int i;
	unsigned long long value;
	unsigned char* pnt = (unsigned char*)(&value);
#ifdef __BIG_ENDIAN__
	for(i=sizeof(long long)-1;i>=0;i--) {
#else
	for(i=0;i<sizeof(long long);i++) {
#endif
		*pnt++ = chunk->data[i+chunk->pnt];
	}
	chunk->pnt += sizeof(long long);
	CheckPNT(chunk);
	return value;
}

void ReadBlock(CHUNK_t* chunk, unsigned char* pnt, int length) {
	int i;
	for(i=0;i<length;i++) {
		pnt[i] = chunk->data[i+chunk->pnt];
	}
	chunk->pnt += length;
	CheckPNT(chunk);
}





void SaveCPU(SAVESTATE_t* save, CPU_t* cpu) {
	int i;
	if (!cpu) return;
	CHUNK_t* chunk = NewChunk(save,CPU_tag);
	
	WriteChar(chunk,cpu->a);
	WriteChar(chunk,cpu->f);
	WriteChar(chunk,cpu->b);
	WriteChar(chunk,cpu->c);
	WriteChar(chunk,cpu->d);
	WriteChar(chunk,cpu->e);
	WriteChar(chunk,cpu->h);
	WriteChar(chunk,cpu->l);
	
	WriteChar(chunk,cpu->ap);
	WriteChar(chunk,cpu->fp);
	WriteChar(chunk,cpu->bp);
	WriteChar(chunk,cpu->cp);
	WriteChar(chunk,cpu->dp);
	WriteChar(chunk,cpu->ep);
	WriteChar(chunk,cpu->hp);
	WriteChar(chunk,cpu->lp);
	
	WriteChar(chunk,cpu->ixl);
	WriteChar(chunk,cpu->ixh);
	WriteChar(chunk,cpu->iyl);
	WriteChar(chunk,cpu->iyh);

	WriteShort(chunk,cpu->pc);
	WriteShort(chunk,cpu->sp);

	WriteChar(chunk,cpu->i);
	WriteChar(chunk,cpu->r);
	WriteChar(chunk,cpu->bus);
	
	WriteInt(chunk,cpu->imode);

	WriteInt(chunk,cpu->interrupt);
	WriteInt(chunk,cpu->ei_block);
	WriteInt(chunk,cpu->iff1);
	WriteInt(chunk,cpu->iff2);
	WriteInt(chunk,cpu->halt);
	
	WriteInt(chunk,cpu->read);
	WriteInt(chunk,cpu->write);
	WriteInt(chunk,cpu->output);
	WriteInt(chunk,cpu->input);
	WriteInt(chunk,cpu->prefix);
	
	
	/*pio*/
	for(i=0; i<256; i++) {
		WriteInt(chunk,cpu->pio.interrupt[i]);
		WriteInt(chunk,cpu->pio.skip_factor[i]);
		WriteInt(chunk,cpu->pio.skip_count[i]);
	}
}
	
void SaveMEM(SAVESTATE_t* save, memc* mem) {
	int i;
	if (!mem) return;
	CHUNK_t* chunk = NewChunk(save,MEM_tag);

	WriteInt(chunk,mem->flash_size);
	WriteInt(chunk,mem->flash_pages);
	WriteInt(chunk,mem->ram_size);
	WriteInt(chunk,mem->ram_pages);
	WriteInt(chunk,mem->step);
	WriteChar(chunk,mem->cmd);
	
	WriteInt(chunk,mem->boot_mapped);
	WriteInt(chunk,mem->flash_locked);
	WriteInt(chunk,mem->flash_version);	


	for(i=0;i<5;i++) {
		WriteInt(chunk,mem->banks[i].page);
		WriteInt(chunk,mem->banks[i].read_only);
		WriteInt(chunk,mem->banks[i].ram);
		WriteInt(chunk,mem->banks[i].no_exec);
	}
	
	WriteInt(chunk,mem->read_OP_flash_tstates);
	WriteInt(chunk,mem->read_NOP_flash_tstates);
	WriteInt(chunk,mem->write_flash_tstates);
	WriteInt(chunk,mem->read_OP_ram_tstates);
	WriteInt(chunk,mem->read_NOP_ram_tstates);
	WriteInt(chunk,mem->write_ram_tstates);
	
	WriteInt(chunk,mem->upper);
	WriteInt(chunk,mem->lower);

	chunk = NewChunk(save,ROM_tag);
	WriteBlock(chunk,mem->flash,mem->flash_size);

	chunk = NewChunk(save,RAM_tag);
	WriteBlock(chunk,mem->ram,mem->ram_size);
	
	chunk = NewChunk(save,REMAP_tag);
	WriteInt(chunk, mem->port27_remap_count);
	WriteInt(chunk, mem->port28_remap_count);
}

void SaveTIMER(SAVESTATE_t* save, timerc* time) {
	if (!time) return;
	CHUNK_t* chunk = NewChunk(save,TIMER_tag);
	WriteLong(chunk,time->tstates);
	WriteLong(chunk,time->freq);
	WriteDouble(chunk,tc_elapsed(time));
	WriteDouble(chunk,time->lasttime);
}

void SaveLINK(SAVESTATE_t* save, link_t* link) {
	CHUNK_t* chunk = NewChunk(save,LINK_tag);
	WriteChar(chunk,link->host);
}

void SaveSTDINT(SAVESTATE_t* save, STDINT_t *stdint) {
	int i;
	if (!stdint) return;
	CHUNK_t* chunk = NewChunk(save,STDINT_tag);
	WriteChar(chunk,stdint->intactive);

	WriteDouble(chunk,stdint->lastchk1);
	WriteDouble(chunk,stdint->timermax1);
	WriteDouble(chunk,stdint->lastchk2);
	WriteDouble(chunk,stdint->timermax2);
	for(i=0;i<4;i++) {
		WriteDouble(chunk,stdint->freq[i]);
	}
	WriteInt(chunk,stdint->mem);
	WriteInt(chunk,stdint->xy);
}

void SaveSE_AUX(SAVESTATE_t* save, SE_AUX_t *se_aux) {
	int i;
	if (!se_aux) return;
	CHUNK_t* chunk = NewChunk(save,SE_AUX_tag);
	
	
	WriteChar(chunk,se_aux->clock.enable);
	WriteInt(chunk,se_aux->clock.set);
	WriteInt(chunk,se_aux->clock.base);
	WriteDouble(chunk,se_aux->clock.lasttime);
	
	for(i=0;i<7;i++) {
		WriteChar(chunk,se_aux->delay.reg[i]);
	}
	
	for(i=0;i<6;i++) {
		WriteInt(chunk,se_aux->md5.reg[i]);
	}
	WriteChar(chunk,se_aux->md5.s);
	WriteChar(chunk,se_aux->md5.mode);
	
	
	WriteChar(chunk,se_aux->linka.link_enable);
	WriteChar(chunk,se_aux->linka.in);
	WriteChar(chunk,se_aux->linka.out);
	WriteChar(chunk,se_aux->linka.working);
	WriteInt(chunk,se_aux->linka.receiving);
	WriteInt(chunk,se_aux->linka.read);
	WriteInt(chunk,se_aux->linka.ready);
	WriteInt(chunk,se_aux->linka.error);
	WriteInt(chunk,se_aux->linka.sending);
	WriteDouble(chunk,se_aux->linka.last_access);
	WriteInt(chunk,se_aux->linka.bit);
	
	WriteDouble(chunk,se_aux->xtal.lastTime);
	WriteLong(chunk,se_aux->xtal.ticks);
	for(i=0;i<3;i++) {
		WriteLong(chunk,se_aux->xtal.timers[i].lastTstates);
		WriteDouble(chunk,se_aux->xtal.timers[i].lastTicks);
		WriteDouble(chunk,se_aux->xtal.timers[i].divsor);
		WriteInt(chunk,se_aux->xtal.timers[i].loop);
		WriteInt(chunk,se_aux->xtal.timers[i].interrupt);
		WriteInt(chunk,se_aux->xtal.timers[i].underflow);
		WriteInt(chunk,se_aux->xtal.timers[i].generate);
		WriteInt(chunk,se_aux->xtal.timers[i].active);
		WriteChar(chunk,se_aux->xtal.timers[i].clock);
		WriteChar(chunk,se_aux->xtal.timers[i].count);
		WriteChar(chunk,se_aux->xtal.timers[i].max);
	}
}

void SaveLCD(SAVESTATE_t* save, LCD_t* lcd) {
	if (!lcd) return;
	CHUNK_t* chunk = NewChunk(save,LCD_tag);

	WriteInt(chunk,lcd->active);
	WriteInt(chunk,lcd->word_len);
	WriteInt(chunk,lcd->x);
	WriteInt(chunk,lcd->y);
	WriteInt(chunk,lcd->z);
	WriteInt(chunk,lcd->cursor_mode);
	WriteInt(chunk,lcd->contrast);
	WriteInt(chunk,lcd->base_level);
	WriteBlock(chunk,lcd->display,DISPLAY_SIZE);

	WriteInt(chunk,lcd->front);
	WriteBlock(chunk,(unsigned char *) lcd->queue,LCD_MAX_SHADES*DISPLAY_SIZE);
	
	WriteInt(chunk,lcd->shades);
	WriteInt(chunk,lcd->mode);
	WriteDouble(chunk,lcd->time);
	WriteDouble(chunk,lcd->ufps);
	WriteDouble(chunk,lcd->ufps_last);
	WriteDouble(chunk,lcd->lastgifframe);
	WriteDouble(chunk,lcd->write_avg);
	WriteDouble(chunk,lcd->write_last);
}

SAVESTATE_t* SaveSlot( int slot ) {
	SAVESTATE_t* save;
	BOOL runsave;
	if (calcs[slot].active == FALSE) return NULL;


	runsave = calcs[slot].running;
	calcs[slot].running = FALSE;
	
	save = CreateSave("Revsoft","Test save",calcs[slot].model);

	SaveCPU(save,&calcs[slot].cpu);

	SaveMEM(save,&calcs[slot].mem_c);

	SaveTIMER(save,&calcs[slot].timer_c);

	SaveLCD(save,calcs[slot].cpu.pio.lcd);

	SaveLINK(save,calcs[slot].cpu.pio.link);

	SaveSTDINT(save,calcs[slot].cpu.pio.stdint);

	SaveSE_AUX(save,calcs[slot].cpu.pio.se_aux);

	calcs[slot].running = runsave;
	return save;
}
	
	

	
	
	
	

void LoadCPU(SAVESTATE_t* save, CPU_t* cpu) {
	CHUNK_t* chunk = FindChunk(save,CPU_tag);
	
	chunk->pnt = 0;
	
	cpu->a = ReadChar(chunk);
	cpu->f = ReadChar(chunk);
	cpu->b = ReadChar(chunk);
	cpu->c = ReadChar(chunk);
	cpu->d = ReadChar(chunk);
	cpu->e = ReadChar(chunk);
	cpu->h = ReadChar(chunk);
	cpu->l = ReadChar(chunk);
	
	cpu->ap = ReadChar(chunk);
	cpu->fp = ReadChar(chunk);
	cpu->bp = ReadChar(chunk);
	cpu->cp = ReadChar(chunk);
	cpu->dp = ReadChar(chunk);
	cpu->ep = ReadChar(chunk);
	cpu->hp = ReadChar(chunk);
	cpu->lp = ReadChar(chunk);

	cpu->ixl = ReadChar(chunk);
	cpu->ixh = ReadChar(chunk);
	cpu->iyl = ReadChar(chunk);
	cpu->iyh = ReadChar(chunk);
	
	cpu->pc = ReadShort(chunk);
	cpu->sp = ReadShort(chunk);

	cpu->i = ReadChar(chunk);
	cpu->r = ReadChar(chunk);
	cpu->bus = ReadChar(chunk);

	cpu->imode = ReadInt(chunk);

	cpu->interrupt = ReadInt(chunk);
	cpu->ei_block = ReadInt(chunk);
	cpu->iff1 = ReadInt(chunk);
	cpu->iff2 = ReadInt(chunk);
	cpu->halt = ReadInt(chunk);
	
	cpu->read = ReadInt(chunk);
	cpu->write = ReadInt(chunk);
	cpu->output = ReadInt(chunk);
	cpu->input = ReadInt(chunk);
	
	cpu->prefix = ReadInt(chunk);
	int i;
	for(i=0; i<256; i++) {
		cpu->pio.interrupt[i] = ReadInt(chunk);
		cpu->pio.skip_factor[i] = ReadInt(chunk);
		cpu->pio.skip_count[i] = ReadInt(chunk);
	}
	
}



void LoadMEM(SAVESTATE_t* save, memc* mem) {
	int i;
	CHUNK_t* chunk = FindChunk(save,MEM_tag);
	chunk->pnt = 0;
	mem->flash_size	= ReadInt(chunk);
	mem->flash_pages= ReadInt(chunk);
	mem->ram_size	= ReadInt(chunk);
	mem->ram_pages	= ReadInt(chunk);
	mem->step		= ReadInt(chunk);
	mem->cmd		= ReadChar(chunk);
	mem->boot_mapped= ReadInt(chunk);
	mem->flash_locked= ReadInt(chunk);
	mem->flash_version = ReadInt(chunk);
	
	for(i=0;i<5;i++) {
		mem->banks[i].page		= ReadInt(chunk);
		mem->banks[i].read_only	= ReadInt(chunk);
		mem->banks[i].ram		= ReadInt(chunk);
		mem->banks[i].no_exec	= ReadInt(chunk);
		if (mem->banks[i].ram) {
			mem->banks[i].addr = mem->ram+(mem->banks[i].page*PAGE_SIZE);
		} else {
			mem->banks[i].addr = mem->flash+(mem->banks[i].page*PAGE_SIZE);
		}

	}
	
	mem->read_OP_flash_tstates	= ReadInt(chunk);
	mem->read_NOP_flash_tstates	= ReadInt(chunk);
	mem->write_flash_tstates	= ReadInt(chunk);
	mem->read_OP_ram_tstates	= ReadInt(chunk);
	mem->read_NOP_ram_tstates	= ReadInt(chunk);
	mem->write_ram_tstates		= ReadInt(chunk);

	mem->upper = ReadInt(chunk);
	mem->lower = ReadInt(chunk);

	chunk = FindChunk(save,ROM_tag);
	chunk->pnt = 0;
	ReadBlock(chunk,mem->flash,mem->flash_size);
	
	chunk = FindChunk(save,RAM_tag);
	chunk->pnt = 0;
	ReadBlock(chunk,mem->ram,mem->ram_size);	

	
	chunk = FindChunk(save,REMAP_tag);
	if (chunk) {
		chunk->pnt = 0;
		mem->port27_remap_count = ReadInt(chunk);
		mem->port28_remap_count = ReadInt(chunk);
	}
}


void LoadTIMER(SAVESTATE_t* save, timerc* time) {
	CHUNK_t* chunk = FindChunk(save,TIMER_tag);
	chunk->pnt = 0;
	time->tstates	= ReadLong(chunk);
	time->freq		= ReadLong(chunk);
	time->elapsed	= ReadDouble(chunk);
	time->lasttime	= ReadDouble(chunk);	// this isn't used.
}



void LoadLCD(SAVESTATE_t* save, LCD_t* lcd) {
	CHUNK_t* chunk = FindChunk(save,LCD_tag);
	chunk->pnt = 0;
	lcd->active		= ReadInt(chunk);
	lcd->word_len	= ReadInt(chunk);
	lcd->x			= ReadInt(chunk);
	lcd->y			= ReadInt(chunk);
	lcd->z			= ReadInt(chunk);
	lcd->cursor_mode		= (LCD_CURSOR_MODE) ReadInt(chunk);
	lcd->contrast	= ReadInt(chunk);
	lcd->base_level	= ReadInt(chunk);
	ReadBlock(chunk,lcd->display,DISPLAY_SIZE);
	lcd->front		= ReadInt(chunk);
	ReadBlock(chunk,(unsigned char *) lcd->queue,LCD_MAX_SHADES*DISPLAY_SIZE);
	lcd->shades		= ReadInt(chunk);
	lcd->mode		= (LCD_MODE) ReadInt(chunk);
	lcd->time		= ReadDouble(chunk);
	lcd->ufps		= ReadDouble(chunk);
	lcd->ufps_last	= ReadDouble(chunk);
	lcd->lastgifframe= ReadDouble(chunk);
	lcd->write_avg	= ReadDouble(chunk);
	lcd->write_last = ReadDouble(chunk);
}



void LoadLINK(SAVESTATE_t* save, link_t* link) {
	CHUNK_t* chunk	= FindChunk(save,LINK_tag);
	chunk->pnt = 0;
	link->host		= ReadChar(chunk);
}

void LoadSTDINT(SAVESTATE_t* save, STDINT_t* stdint) {
	int i;
	CHUNK_t* chunk		= FindChunk(save,STDINT_tag);
	chunk->pnt = 0;
	stdint->intactive	= ReadChar(chunk);
	stdint->lastchk1	= ReadDouble(chunk);
	stdint->timermax1	= ReadDouble(chunk);
	stdint->lastchk2	= ReadDouble(chunk);
	stdint->timermax2	= ReadDouble(chunk);
	for(i=0;i<4;i++) {
		stdint->freq[i]	= ReadDouble(chunk);
	}
	stdint->mem			= ReadInt(chunk);
	stdint->xy			= ReadInt(chunk);
}

void LoadSE_AUX(SAVESTATE_t* save, SE_AUX_t *se_aux) {
	int i;
	if (!se_aux) return;
	CHUNK_t* chunk = FindChunk(save,SE_AUX_tag);
	if (!chunk) return;
	
	
	se_aux->clock.enable		= ReadChar(chunk);
	se_aux->clock.set			= ReadInt(chunk);
	se_aux->clock.base			= ReadInt(chunk);
	se_aux->clock.lasttime		= ReadDouble(chunk);
	
	for(i=0;i<7;i++) {
		se_aux->delay.reg[i]	= ReadChar(chunk);
	}
	
	for(i=0;i<6;i++) {
		se_aux->md5.reg[i]		= ReadInt(chunk);
	}
	se_aux->md5.s				= ReadChar(chunk);
	se_aux->md5.mode			= ReadChar(chunk);
	
	
	se_aux->linka.link_enable	= ReadChar(chunk);
	se_aux->linka.in			= ReadChar(chunk);
	se_aux->linka.out			= ReadChar(chunk);
	se_aux->linka.working		= ReadChar(chunk);
	se_aux->linka.receiving		= ReadInt(chunk);
	se_aux->linka.read			= ReadInt(chunk);
	se_aux->linka.ready			= ReadInt(chunk);
	se_aux->linka.error			= ReadInt(chunk);
	se_aux->linka.sending		= ReadInt(chunk);
	se_aux->linka.last_access	= ReadDouble(chunk);
	se_aux->linka.bit			= ReadInt(chunk);
		
	se_aux->xtal.lastTime		= ReadDouble(chunk);
	se_aux->xtal.ticks			= ReadLong(chunk);

	for(i=0;i<3;i++) {
		se_aux->xtal.timers[i].lastTstates	= ReadLong(chunk);
		se_aux->xtal.timers[i].lastTicks	= ReadDouble(chunk);
		se_aux->xtal.timers[i].divsor		= ReadDouble(chunk);
		se_aux->xtal.timers[i].loop			= ReadInt(chunk);
		se_aux->xtal.timers[i].interrupt	= ReadInt(chunk);
		se_aux->xtal.timers[i].underflow	= ReadInt(chunk);
		se_aux->xtal.timers[i].generate		= ReadInt(chunk);
		se_aux->xtal.timers[i].active		= ReadInt(chunk);
		se_aux->xtal.timers[i].clock		= ReadChar(chunk);
		se_aux->xtal.timers[i].count		= ReadChar(chunk);
		se_aux->xtal.timers[i].max			= ReadChar(chunk);
	}
}


void LoadSlot(SAVESTATE_t* save, int slot) {
	BOOL runsave;
	
	if (calcs[slot].active == FALSE){
		puts("Slot was not active");
		return;
	}
	if (save == NULL) {
		puts("Save was null");
		return;
	}
	
	runsave = calcs[slot].running;
	calcs[slot].running = FALSE;
	
	LoadCPU(save,&calcs[slot].cpu);
	LoadMEM(save,&calcs[slot].mem_c);
	LoadTIMER(save,&calcs[slot].timer_c);
	LoadLCD(save,calcs[slot].cpu.pio.lcd);
	LoadLINK(save,calcs[slot].cpu.pio.link);
	LoadSTDINT(save,calcs[slot].cpu.pio.stdint);
	LoadSE_AUX(save,calcs[slot].cpu.pio.se_aux);
	calcs[slot].running = runsave;
}

char* GetRomOnly(SAVESTATE_t* save,int* size) {
	CHUNK_t* chunk = FindChunk(save,ROM_tag);
	*size = 0;
	if (!chunk) return NULL;
	*size = chunk->size;
	return (char *) chunk->data;
}


void WriteSave(const char * fn,SAVESTATE_t* save,int compress) {
	int i;
	FILE* ofile;
	FILE* cfile;
	char tmpfn[L_tmpnam];
	
	if (!save) {
		puts("Save was null for write");
		return;
	}
	if (compress == 0) {
		ofile = fopen(fn,"wb");
	} else {
		tmpnam(tmpfn);
		ofile = fopen(tmpfn,"wb");
	}
		
	if (!ofile) {
		puts("Could not open save file for write");
		return;
	}

	fputs(DETECT_STR,ofile);

	fputi(SAVE_HEADERSIZE,ofile);	
	
	fputi(save->version_major,ofile);
	fputi(save->version_minor,ofile);
	fputi(save->version_build,ofile);
	fputi(save->model,ofile);
	fputi(save->chunk_count,ofile);
	fwrite(save->author,1,32,ofile);
	fwrite(save->comment,1,64,ofile);
	
	for(i=0;i<save->chunk_count;i++) {
		fputc(save->chunks[i]->tag[0],ofile);
		fputc(save->chunks[i]->tag[1],ofile);
		fputc(save->chunks[i]->tag[2],ofile);
		fputc(save->chunks[i]->tag[3],ofile);
		fputi(save->chunks[i]->size,ofile);
		fwrite(save->chunks[i]->data,1,save->chunks[i]->size,ofile);
	}
	fclose(ofile);
	#ifdef WINVER // FIXME! FIXME! FIXME!
	if (compress) {
		cfile = fopen(fn,"wb");
		if (!cfile) {
			puts("Could not open compress file for write");
			return;
		}
		ofile = fopen(tmpfn,"rb");
		if (!ofile) {
			puts("Could not open tmp file for read");
			return;
		}
		fputs(DETECT_CMP_STR,cfile);
		switch(compress) {
#ifndef _WINDLL
			case ZLIB_CMP:
				fputc(ZLIB_CMP,cfile);
				def(ofile,cfile,9);
				break;
#endif
			default:
				puts("Error bad compression format selected.");
				break;
		}
		fclose(ofile);
		fclose(cfile);
		remove(tmpfn);
	}
	#endif
}

SAVESTATE_t* ReadSave(FILE* ifile) {
	int i;
	int compressed = FALSE;
	int chunk_offset,chunk_count;
	char string[128];
	char tmpfn[L_tmpnam];
	SAVESTATE_t* save;
	CHUNK_t* chunk;
	FILE* tmpfile;

	fread(string,1,8,ifile);
	string[8]=0;
	if (strncmp(DETECT_CMP_STR,string,8)==0) {
		i = fgetc(ifile);
		tmpnam(tmpfn);
		tmpfile = fopen(tmpfn,"wb");
		if (!tmpfile) {
			puts("Could not open tmp file for write");
			return NULL;
		}
		#ifdef WINVER // FIXME! FIXME! FIXME!
		switch(i) {
#ifndef _WINDLL
			case ZLIB_CMP:
				inf(ifile,tmpfile);
				break;
#endif
			default:
				puts("Compressed save is not compatible.");
				fclose(tmpfile);
				remove(tmpfn);
				return NULL;
		}
		#endif
		fclose(tmpfile);
		ifile = fopen(tmpfn,"rb");	//this is not a leak, ifile gets closed
									// outside of this routine.
		if (!ifile) {
			puts("Could not open tmp file for read");
			return NULL;
		}
		compressed = TRUE;
		fread(string,1,8,ifile);
	}
		
	if (strncmp(DETECT_STR,string,8)!=0){

		puts("Readsave detect string failed.");
		if (compressed == TRUE) fclose(ifile);
		return NULL;
	}		
	
	save = (SAVESTATE_t *) malloc(sizeof(SAVESTATE_t));
	if (!save) {
		puts("Save could not be allocated");
		if (compressed == TRUE) fclose(ifile);
		return NULL;
	}

	chunk_offset = fgeti(ifile);
	
	save->version_major = fgeti(ifile);
	save->version_minor = fgeti(ifile);
	save->version_build = fgeti(ifile);


	if (save->version_major != CUR_MAJOR) {
		fclose(ifile);
		puts("Save not compatible at all, sorry\n");
		free(save);
		return NULL;
	}

	save->model = fgeti(ifile);

	chunk_count = fgeti(ifile);
	fread(save->author,1,32,ifile);
	fread(save->comment,1,64,ifile);

	fseek(ifile,chunk_offset+8+4,SEEK_SET);
	
	for(i=0;i<512;i++) {
		save->chunks[i] = NULL;
	}
	save->chunk_count = 0;
	for(i=0;i<chunk_count;i++) {
		string[0]	= fgetc(ifile);
		string[1]	= fgetc(ifile);
		string[2]	= fgetc(ifile);
		string[3]	= fgetc(ifile);
		string[4]	= 0;
		chunk		= NewChunk(save,string);
		chunk->size	= fgeti(ifile);
		chunk->data	= (unsigned char *) malloc(chunk->size);
		fread(chunk->data,1,chunk->size,ifile);
	}
	if (compressed == TRUE) {
		fclose(ifile);
		remove(tmpfn);
	}
/* check for read errors... */
	return save;
}
