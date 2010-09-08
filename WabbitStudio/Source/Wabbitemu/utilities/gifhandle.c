#include "stdafx.h"

#include "calc.h"
#include "gif.h"
#ifdef WINVER // ...oops
#include "resource.h"
#endif
#ifndef WINVER
#include "types.h"
#endif
#include "lcd.h"
#include "gifhandle.h"
#ifdef WINVER
#include "guioptions.h"
#endif


char *generate_gif_name(char *fn, int num, char *dest) {
	int i;
	for (i = strlen(fn) - 1; 
	 	 i && fn[i] != '.';
	 	 i--);
	 	 
	if (i) fn[i] = '\0';
	
	sprintf(dest, "%s%d.gif", fn, num);
	
	if (i) fn[i] = '.';
	return dest;
}

#ifdef HIGH_SHADE_GIF
unsigned char* GIFGREYLCD() {
	LCD_t* lcd = calcs[gslot].cpu.pio.lcd;

	
	uint8_t temp_gif[LCD_HEIGHT][LCD_WIDTH];
	u_int row, col;
	for (row = 0; row < LCD_HEIGHT; row++) {
		for (col = 0; col < LCD_MEM_WIDTH; col++) {
			double p0=0,p1=0,p2=0,p3=0,p4=0,p5=0,p6=0,p7=0;
			int i;
			
			for (i = 0; i < lcd->shades; i++) {
				u_int u = lcd->queue[i][row * 16 + col];
				p7 += u & 1; u >>= 1;
				p6 += u & 1; u >>= 1;
				p5 += u & 1; u >>= 1;
				p4 += u & 1; u >>= 1;
				p3 += u & 1; u >>= 1;
				p2 += u & 1; u >>= 1;
				p1 += u & 1; u >>= 1;
				p0 += u;
			}
			
			// Convert lcd shades to gif
			u_char *scol = &temp_gif[row][col * 8];
			scol[0] = (u_char) p0;
			scol[1] = (u_char) p1;
			scol[2] = (u_char) p2;
			scol[3] = (u_char) p3;
			scol[4] = (u_char) p4;
			scol[5] = (u_char) p5;
			scol[6] = (u_char) p6;
			scol[7] = (u_char) p7;
		}
	}
	
	//if (gif_size > 1) {
		for (row = 0; row < LCD_HEIGHT*gif_size; row++) {
			for (col = 0; col < LCD_WIDTH*gif_size; col++) {
				lcd->gif[row][col] = temp_gif[row/gif_size][col/gif_size];
			}
		}
	//}
	return (uint8_t*) lcd->gif;
}

#else
unsigned char* GIFGREYLCD() {
	LCD_t* lcd = calcs[gslot].cpu.pio.lcd;
	unsigned int tmp;
	int x,y,i,bit,col;
	for(y=0;y<LCD_HEIGHT*gif_size;y++) {
		for(x=0;x<LCD_WIDTH*gif_size;x++) {
			bit = 7-((x/gif_size)&0x7);
			col = (x/gif_size)>>3;
			tmp=0;
			for(i=0;i<(lcd->shades);i++) {
				if (lcd->queue[i]) tmp +=((lcd->queue[i][((y/gif_size)*LCD_MEM_WIDTH)+col]>>bit)&0x01);
			}
			lcd->gif[y][x] = tmp;
		}
	}
	return (u_char*) lcd->gif;
}
#endif

void handle_screenshot() {
#ifdef WINVER
	FILE* testfile;
	int result;
	LCD_t* lcd = calcs[gslot].cpu.pio.lcd;
	int i, j, s, marked;
	static char gif_fn_backup[MAX_PATH];
	BOOL running_backup = calcs[gslot].running;

	if ((gif_write_state != GIF_IDLE) && (!calcs[gslot].running)) gif_write_state = GIF_END;
	
	calcs[gslot].running = FALSE;
	switch (gif_write_state) {
		case GIF_IDLE:
		{
			gif_newframe = 0;
			break;
		}
		case GIF_START:
		{
			strcpy(gif_fn_backup, gif_file_name);
			
			if (gif_autosave) {
				/* do file save */
				if (gif_use_increasing) {
					char fn[MAX_PATH];
					FILE *test = (FILE*) 1;
					
					for (i = 0; test; i++) {
						generate_gif_name(gif_file_name, i, fn);
						test = fopen(fn, "r");
						if (test) fclose(test);
					}
					
					strcpy(gif_file_name, fn);
				}
			} else {
#ifndef _WINDLL
				if (SetGifName(TRUE)) {
					calcs[gslot].gif_disp_state = GDS_ENDING;
					SendMessage(calcs[gslot].hwndFrame, WM_COMMAND, MAKEWPARAM(IDM_FILE_GIF, 0), 0);
					gif_write_state = GIF_IDLE;
					break;
				}
#endif
			}
#ifdef USE_GIF_SIZES
			gif_xs = lcd->width*gif_size*calc_count();
			//FIXME: Add in lcd height variable
			gif_ys = 64*gif_size;
#else
			gif_xs = SCRXSIZE;
			gif_ys = SCRYSIZE;			
#endif
			gif_base_delay = gif_base_delay_start;
			gif_time = 0;
			gif_newframe = 1;
			
			gif_colors = lcd->shades + 1;
			
			GIFGREYLCD();
			
#ifdef USE_GIF_SIZES
			for (i = 0; i < gif_ys; i++)
				for (j = 0; j < gif_xs; j++)
					gif_frame[i * gif_xs + j] = lcd->gif[i][j];		
#else
			for (i = 0; i < SCRYSIZE; i++) {
				for (j = 0; j < SCRXSIZE; j++) {
					gif_frame[i * gif_xs + j] = lcd->gif[i][j];
				}
			}
#endif
			//WriteRIFFHeader();
			break;
		}
		case GIF_FRAME:
		{
			gif_time += 1;
			if (gif_time >= gif_base_delay) {
				gif_time -= gif_base_delay;
				gif_newframe = 1;

				GIFGREYLCD();

#ifdef USE_GIF_SIZES
				for (i = 0; i < gif_ys; i++)
					for (j = 0; j < gif_xs; j++)
						gif_frame[i * gif_xs + j] = lcd->gif[i][j];		
#else
				for (i = 0; i < SCRYSIZE; i++) {
					for (j = 0; j < SCRXSIZE; j++) {
						gif_frame[i * gif_xs + j] = lcd->gif[i][j];
					}
				}
#endif
			}
			break;
		}
		case GIF_END:
		{
			//WriteRIFFIndex();
			gif_newframe = 1;
			gif_file_num++;
			
			strcpy(gif_file_name, gif_fn_backup);
			break;
		}
	}
	calcs[gslot].running = running_backup;
	
	if (gif_newframe) {
		gif_newframe = 0;
		gif_writer();
		//WriteAVIFrame();
	}
#endif
}




