/* Copyright (c) 2010 Spencer Putt
 * Support for screen-color palettes and variable sized gifs
 * Copyright (c) 2005 Patai Gergely
 * This code is partly based on (i. e. blatantly ripped from) Whirlgif
 * Copyright (c) 1997,1998 by Hans Dinsen-Hansen
 * The algorithms are inspired by those of gifcode.c
 * Copyright (c) 1995,1996 Michael A. Mayer
 * All rights reserved.
 *
 * This software may be freely copied, modified and redistributed
 * without fee provided that above copyright notices are preserved
 * intact on all copies and modified copies.
 *
 * There is no warranty or other guarantee of fitness of this software.
 * It is provided solely "as is". The author(s) disclaim(s) all
 * responsibility and liability with respect to this software's usage
 * or its effect upon hardware or computer systems.
 *
 * The Graphics Interchange format (c) is the Copyright property of
 * Compuserve Incorporated.  Gif(sm) is a Service Mark property of
 * Compuserve Incorporated.
 */
#include "stdafx.h"

#include "calc.h"

#ifdef WINVER // FIXME: ...
#include "gui.h"
#else
#include "coretypes.h"
#endif

//#include "gif.h" // uhh...

#define GIF_FRAME_MAX (256 * 128 * 4)

#define GIF_IDLE 0
#define GIF_START 1
#define GIF_FRAME 2
#define GIF_END 3

#define SCRXSIZE 96
#define SCRYSIZE 64

#define TERMIN 'T'
#define LOOKUP 'L'
#define SEARCH 'S'
#define ARRNO 20
#define BLOKLEN 255
#define BUFLEN 1000



typedef struct GIF_TREE {
	char typ;
	int code;
	BYTE ix;
	struct GIF_TREE **node;
	struct GIF_TREE *nxt;
	struct GIF_TREE *alt;
} GIF_TREE;

int gif_write_state = GIF_IDLE;

char gif_file_name[512]="wabbitemu.gif";
BOOL gif_autosave = FALSE;
BOOL gif_use_increasing = FALSE;
#ifdef HIGH_SHADE_GIF
int gif_colors = 8;
#else
int gif_colors=7;
#endif

int gif_base_delay_start = 4;
u_int gif_size = 2;

WORD gif_base_delay;
int gif_file_size = 0;
WORD gif_delay;
int gif_xs;
int gif_ys;
int gif_frame_x;
int gif_frame_y;
int gif_frame_xs;
int gif_frame_ys;
BYTE gif_frame[GIF_FRAME_MAX];
int gif_time;
int gif_newframe;

int gif_file_num = 0;
BOOL gif_bw = FALSE;


int chainlen = 0, maxchainlen = 0, nodecount = 0, lookuptypes = 0, nbits;
short need = 8;
GIF_TREE *empty[256], GifRoot = {LOOKUP, 0, 0, empty, NULL, NULL},
	*topNode, *baseNode, **nodeArray, **lastArray;

void gif_clear_tree(int cc, GIF_TREE *root) {
	int i;
	GIF_TREE *newNode, **xx;

	maxchainlen = 0; lookuptypes = 1;
	nodecount = 0;
	nodeArray = root->node;
	xx= nodeArray;
	for (i = 0; i < ARRNO; i++ ) {
		memmove(xx, empty, 256 * sizeof(GIF_TREE**));
		xx += 256;
	}
	topNode = baseNode;
	for(i = 0; i < cc; i++) {
		root->node[i] = newNode = ++topNode;
		newNode->nxt = NULL;
		newNode->alt = NULL;
		newNode->code = i;
		newNode->ix = i;
		newNode->typ = TERMIN;
		newNode->node = empty;
		nodecount++;
	}
}

unsigned char *gif_code_to_buffer(int code, short n, unsigned char *buf) {
	int mask;

	if (n < 0) {
		if(need < 8) { buf++; *buf = 0x0; }
		need = 8;
		return buf;
	}

	while (n >= need) {
		mask = (1 << need) - 1;
		*buf += (mask & code) << (8 - need);
		buf++;
		*buf = 0x0;
		code = code >> need;
		n -= need;
		need = 8;
	}
	if (n) {
		mask = (1 << n) - 1;
		*buf += (mask & code) << (8 - need);
		need -= n;
	}
	return buf;
}

int gif_encode(FILE *fout, BYTE *pixels, int depth, int siz) {
	GIF_TREE *first = &GifRoot, *newNode, *curNode;
	BYTE *end;
	int cc, eoi, next, tel = 0;
	int fsize;
	short cLength;

	unsigned char *pos, *buffer;

	empty[0] = NULL;
	need = 8;

	nodeArray = empty;
	memmove(++nodeArray, empty, 255 * sizeof(GIF_TREE **));
	if ((buffer = (unsigned char*)malloc((BUFLEN + 1) * sizeof(char))) == NULL) return -1;
	buffer++;
	pos = buffer;
	buffer[0] = 0x0;

	cc = (depth == 1) ? 0x4 : 1 << depth;
	fputc((depth == 1) ? 2 : depth, fout);
	fsize = 1;
	eoi = cc + 1;
	next = cc + 2;
	cLength = (depth == 1) ? 3 : depth + 1;

	if ((topNode = baseNode = (GIF_TREE*)malloc(sizeof(GIF_TREE) * 4094)) == NULL) return -1;
	if ((nodeArray = first->node = (GIF_TREE**)malloc(256 * sizeof(GIF_TREE*) * ARRNO)) == NULL) return -1;
	lastArray = nodeArray + (256 * ARRNO - cc);
	gif_clear_tree(cc, first);
	pos = gif_code_to_buffer(cc, cLength, pos);
	end = pixels+siz;
	curNode = first;
	while(pixels < end) {
		if ( curNode->node[*pixels] != NULL ) {
			curNode = curNode->node[*pixels];
			tel++;
			pixels++;
			chainlen++;
			continue;
		} else if ( curNode->typ == SEARCH ) {
			newNode = curNode->nxt;
			while ( newNode->alt != NULL ) {
				if ( newNode->ix == *pixels ) break;
				newNode = newNode->alt;
			}
			if (newNode->ix == *pixels ) {
				tel++;
				pixels++;
				chainlen++;
				curNode = newNode;
				continue;
			}
		}

		newNode = ++topNode;
		switch (curNode->typ) {
			case LOOKUP:
				newNode->nxt = NULL;
				newNode->alt = NULL,
				curNode->node[*pixels] = newNode;
				break;
			case SEARCH:
				if (nodeArray != lastArray) {
					nodeArray += cc;
					curNode->node = nodeArray;
					curNode->typ = LOOKUP;
					curNode->node[*pixels] = newNode;
					curNode->node[(curNode->nxt)->ix] = curNode->nxt;
					lookuptypes++;
					newNode->nxt = NULL;
					newNode->alt = NULL,
					curNode->nxt = NULL;
					break;
				}
			case TERMIN:
				newNode->alt = curNode->nxt;
				newNode->nxt = NULL,
				curNode->nxt = newNode;
				curNode->typ = SEARCH;
				break;
		}
		newNode->code = next;
		newNode->ix = *pixels;
		newNode->typ = TERMIN;
		newNode->node = empty;
		nodecount++;

		pos = gif_code_to_buffer(curNode->code, cLength, pos);
		if (chainlen > maxchainlen) maxchainlen = chainlen;
		chainlen = 0;
		if (pos-buffer > BLOKLEN) {
			buffer[-1] = BLOKLEN;
			fwrite(buffer - 1, 1, BLOKLEN + 1, fout);
			fsize += BLOKLEN + 1;
			buffer[0] = buffer[BLOKLEN];
			buffer[1] = buffer[BLOKLEN + 1];
			buffer[2] = buffer[BLOKLEN + 2];
			buffer[3] = buffer[BLOKLEN + 3];
			pos -= BLOKLEN;
		}
		curNode = first;

		if (next == (1 << cLength)) cLength++;
		next++;
		if (next == 0xfff) {
			gif_clear_tree(cc, first);
			pos = gif_code_to_buffer(cc, cLength, pos);
			if (pos - buffer > BLOKLEN) {
				buffer[-1] = BLOKLEN;
				fwrite(buffer-1, 1, BLOKLEN + 1, fout);
				fsize += BLOKLEN + 1;
				buffer[0] = buffer[BLOKLEN];
				buffer[1] = buffer[BLOKLEN + 1];
				buffer[2] = buffer[BLOKLEN + 2];
				buffer[3] = buffer[BLOKLEN + 3];
				pos -= BLOKLEN;
			}
			next = cc + 2;
			cLength = (depth == 1) ? 3 : depth + 1;
		}
	}

	pos = gif_code_to_buffer(curNode->code, cLength, pos);
	if(pos - buffer > BLOKLEN - 3) {
		buffer[-1] = BLOKLEN - 3;
		fwrite(buffer - 1, 1, BLOKLEN - 2, fout);
		fsize += BLOKLEN - 2;
		buffer[0] = buffer[BLOKLEN - 3];
		buffer[1] = buffer[BLOKLEN - 2];
		buffer[2] = buffer[BLOKLEN - 1];
		buffer[3] = buffer[BLOKLEN];
		buffer[4] = buffer[BLOKLEN + 1];
		pos -= BLOKLEN - 3;
	}
	pos = gif_code_to_buffer(eoi, cLength, pos);
	pos = gif_code_to_buffer(0x0, -1, pos);
	buffer[-1] = pos - buffer;

	fwrite(buffer - 1, pos - buffer + 1, 1, fout);
	fsize += (int) (pos - buffer + 1);
	free(buffer - 1); free(first->node); free(baseNode);
	return fsize;
}

void gif_writer() {
	static FILE *fp;
	// flags = 1 111 0 010
	BYTE gif_header[205] = {'G', 'I', 'F', '8', '9', 'a', 96, 0, 64, 0, 0xf2, 0x0f, 0};
	static BYTE gif_info[31] = {
		0x21, 0xff, 0x0b, 'N', 'E', 'T', 'S', 'C', 'A', 'P', 'E', '2', '.', '0', 3, 1, 0, 0, 0,
		0x21, 0xfe, 8, 'W', 'a', 'b', 'b', 'i', 't', 0, 0, 0
	};
	static BYTE gif_img[18] = {0x21, 0xf9, 4, 5, 11, 0, 0x0f, 0, 0x2c, 0, 0, 0, 0, 96, 0, 64, 0, 0};
	static BYTE gif_frame_old[GIF_FRAME_MAX];
	static BYTE gif_frame_out[GIF_FRAME_MAX];

	switch (gif_write_state) {
		case GIF_IDLE:
			gif_file_size = 0;
			break;
		case GIF_START: {
			int i;
			gif_colors = calcs[gslot].cpu.pio.lcd->shades + 1;
			for (i = 0; i < gif_colors; i++) {
#ifdef HIGH_SHADE_GIF
				double color_ratio = 1.0 - ((double) i / (double) (gif_colors - 1));
				printf("ratio: %lf\n", color_ratio);
				
#define LCD_HIGH_MUL 6
				
				if (gif_bw) {
					gif_header[13 + i * 3] = (BYTE) (0xFF * color_ratio);
					gif_header[14 + i * 3] = (BYTE) (0xFF * color_ratio);
					gif_header[15 + i * 3] = (BYTE) (0xFF * color_ratio);
				} else {
					gif_header[13 + i * 3] = (BYTE) ((0x9E - (0x9E/LCD_HIGH_MUL)) * color_ratio + (0x9E/LCD_HIGH_MUL));
					gif_header[14 + i * 3] = (BYTE) ((0xAB - (0xAB/LCD_HIGH_MUL)) * color_ratio + (0xAB/LCD_HIGH_MUL));
					gif_header[15 + i * 3] = (BYTE) ((0x88 - (0x88/LCD_HIGH_MUL)) * color_ratio + (0x88/LCD_HIGH_MUL));
				}
#else
				gif_header[13 + i * 3] = 255 - i * 255 / (gif_colors-1);
				gif_header[14 + i * 3] = 255 - i * 255 / (gif_colors-1);
				gif_header[15 + i * 3] = 255 - i * 255 / (gif_colors-1);
#endif
			}
			
			int palette_bits = 2;
			if (gif_colors > 8)
				palette_bits = 3;
			
			gif_header[10] = 0xF0 | palette_bits;
			gif_frame_xs = gif_xs;
			gif_frame_ys = gif_ys;
			gif_frame_x = 0;
			gif_frame_y = 0;
			gif_header[6] = gif_xs;
			gif_header[7] = gif_xs >> 8;
			gif_header[8] = gif_ys;
			gif_header[9] = gif_ys >> 8;
			fp = fopen(gif_file_name, "wb");
			fwrite(gif_header, 13 + (3 * (1 << (palette_bits+1))), 1, fp);
			fwrite(gif_info, 31, 1, fp);
			gif_file_size = 236;
			gif_write_state = GIF_FRAME;
			gif_delay = gif_base_delay;
			gif_img[3] = 4;
			memcpy(gif_frame_old, gif_frame, GIF_FRAME_MAX);
			memcpy(gif_frame_out, gif_frame, GIF_FRAME_MAX);
			break;
		}
		case GIF_FRAME: {
			int i, j, k, l;
			for (i = 0; i < gif_xs * gif_ys; i++)
				if (gif_frame[i] != gif_frame_old[i]) break;
			if (i == gif_xs * gif_ys) {
				gif_delay += gif_base_delay;
			} else {
				gif_img[3] = 5;
				gif_img[4] = (BYTE) gif_delay;
				gif_img[5] = gif_delay >> 8;
				gif_img[9] = gif_frame_x;
				gif_img[10] = gif_frame_x >> 8;
				gif_img[11] = gif_frame_y;
				gif_img[12] = gif_frame_y >> 8;
				gif_img[13] = gif_frame_xs;
				gif_img[14] = gif_frame_xs >> 8;
				gif_img[15] = gif_frame_ys;
				gif_img[16] = gif_frame_ys >> 8;
				gif_delay = gif_base_delay;
				fwrite(gif_img, 18, 1, fp);
				i = gif_encode(fp, gif_frame_out, 8, gif_frame_xs * gif_frame_ys);
				if (i == -1) {
					gif_file_size = -1;
					gif_write_state = GIF_END;
				} else {
					fputc(0, fp);
					gif_file_size += i + 19;
				}
				gif_frame_x = gif_xs;
				gif_frame_y = gif_ys;
				gif_frame_xs = 0;
				gif_frame_ys = 0;
				for (i = 0; i < gif_ys; i++)
					for (j = 0; j < gif_xs; j++) {
						if (gif_frame[i * gif_xs + j] != gif_frame_old[i * gif_xs + j]) {
						   if (gif_frame_x > j) gif_frame_x = j;
						   if (gif_frame_y > i) gif_frame_y = i;
						   if (gif_frame_xs < j) gif_frame_xs = j;
						   if (gif_frame_ys < i) gif_frame_ys = i;
						}
					}
				if (gif_frame_x == gif_xs) {
					gif_frame_x = 0;
					gif_frame_y = 0;
					gif_frame_xs = 1;
					gif_frame_ys = 1;
				} else {
					gif_frame_xs -= gif_frame_x - 1;
					gif_frame_ys -= gif_frame_y - 1;
				}
				k = gif_frame_y * gif_xs + gif_frame_x;
				for (i = 0; i < gif_frame_ys; i++)
					for (j = 0; j < gif_frame_xs; j++) {
						l = gif_frame[i * gif_xs + j + k];
						gif_frame_out[i * gif_frame_xs + j] = (l == gif_frame_old[i * gif_xs + j + k]) ? 0x0f : l;
					}
				memcpy(gif_frame_old, gif_frame, GIF_FRAME_MAX);
			}
			break;
		}
		case GIF_END: {
			if (!fp)
				break;
			int i;
			gif_img[4] = (BYTE) gif_delay;
			gif_img[5] = gif_delay >> 8;
			gif_img[9] = gif_frame_x;
			gif_img[10] = gif_frame_x >> 8;
			gif_img[11] = gif_frame_y;
			gif_img[12] = gif_frame_y >> 8;
			gif_img[13] = gif_frame_xs;
			gif_img[14] = gif_frame_xs >> 8;
			gif_img[15] = gif_frame_ys;
			gif_img[16] = gif_frame_ys >> 8;
			fwrite(gif_img, 18, 1, fp);
			i = gif_encode(fp, gif_frame_out, 8, gif_frame_xs * gif_frame_ys);
			if (i == -1) {
				gif_file_size = -1;
				gif_write_state = GIF_IDLE;
			} else {
				fputc(0, fp);
				gif_file_size += i + 19;
			}
			fputc(0x3b, fp);
			fclose(fp);
			gif_file_size += 1;
			gif_write_state = GIF_IDLE;
			break;
		}
	}
}
