#include "stdafx.h"

#include "gif.h"
#include "coretypes.h"
#include "lcd.h"
#include "gifhandle.h"
#ifdef WINVER
#include "fileutilities.h"
#endif


TCHAR *generate_gif_name(TCHAR *fn, int num, TCHAR *dest) {
	size_t i;
	for (i = _tcslen(fn) - 1; i && fn[i] != '.'; i--);
	 	 
	if (i) {
		fn[i] = '\0';
	}
	
	StringCbPrintf(dest, _tcslen(dest) + 4, _T("%s%d.gif"), fn, num);
	
	if (i)  {
		fn[i] = '.';
	}
	return dest;
}

static TCHAR gif_fn_backup[MAX_PATH];
/*
 * Gets where the next screenshot should be saved to.
 * Returns true if ready, false if user cancels
 */
BOOL get_gif_filename() {
	int i;
#ifdef _WINDOWS
	StringCbCopy(gif_fn_backup, sizeof(gif_fn_backup), gif_file_name);
#else
	strcpy(gif_fn_backup, gif_file_name);
#endif
	if (gif_autosave) {
		/* do file save */
		if (gif_use_increasing) {
			FILE *test = NULL;
			BOOL fileExists = FALSE;
			i = 0;
					
			 do {
				generate_gif_name(gif_fn_backup, i, gif_file_name);
#ifdef _WINDOWS
				_tfopen_s(&test, gif_file_name, _T("r"));
#else
				test = fopen(gif_file_name, "r");
#endif
				i++;
				if (test) {
					fclose(test);
					fileExists = TRUE;
				} else {
					fileExists = FALSE;
				}
			} while (fileExists);
		}
	} else {
#ifdef _WINDOWS
#ifndef _WINDLL
		if (SaveFile(gif_file_name, _T("Graphics Interchange Format  (*.gif)\0*.gif\0All Files (*.*)\0*.*\0\0"),
						_T("Wabbitemu GIF File Target"), _T("gif"), 0, 0))
			//if we cancel, mark the menu and set to idle
			return FALSE;
#endif
#endif
	}
	return TRUE;
}

unsigned char* GIFGREYLCD(LCD_t *lpLCD) {
	
	uint8_t temp_gif[LCD_HEIGHT][LCD_WIDTH];

	int level = abs((int) lpLCD->contrast - (int) lpLCD->base_level);
	int base = (lpLCD->contrast - 54) * 24;
	if (base < 0) base = 0;

	if (level > 12) level = 0;
	else level = (12 - level) * (255 - base) / lpLCD->shades / 12;

	u_int row, col;
	for (row = 0; row < LCD_HEIGHT; row++) {
		for (col = 0; col < LCD_MEM_WIDTH; col++) {
			double p0 = 0, p1 = 0, p2 = 0, p3 = 0, p4 = 0, p5 = 0, p6 = 0, p7 = 0;
			u_int i;
			
			for (i = 0; i < lpLCD->shades; i++) {
				u_int u = lpLCD->queue[i][row * 16 + col];
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
			scol[0] = (u_char) p0;//(p0 * level + base);
			scol[1] = (u_char) p1;//(p1 * level + base);
			scol[2] = (u_char) p2;//(p2 * level + base);
			scol[3] = (u_char) p3;//(p3 * level + base);
			scol[4] = (u_char) p4;//(p4 * level + base);
			scol[5] = (u_char) p5;//(p5 * level + base);
			scol[6] = (u_char) p6;//(p6 * level + base);
			scol[7] = (u_char) p7;//(p7 * level + base);
		}
	}
	
	//if (gif_size > 1) {
		for (row = 0; row < LCD_HEIGHT * gif_size; row++) {
			for (col = 0; col < LCD_WIDTH * gif_size; col++) {
				lpLCD->gif[row][col] = temp_gif[row / gif_size][col / gif_size];
			}
		}
	//}
	return (uint8_t*) lpLCD->gif;
}

void handle_screenshot() {
	LCD_t* lcd;
	int i, j;
	u_int shades = 0;
	BOOL running_backup[MAX_CALCS];
	for (i = 0; i < MAX_CALCS; i++) {
		running_backup[i] = calcs[i].running;
		calcs[i].running = FALSE;
		lcd = calcs[i].cpu.pio.lcd;
		//find the calc with the highest number of shades and use that as our number for the gif
		//since I'm to lazy to implement them individually :P
		if (calcs[i].active && lcd && shades < lcd->shades) {
			shades = lcd->shades;
		}
		//we also need to find the size of all the LCDs
	}

	/*if ((gif_write_state != GIF_IDLE) && (!lpCalc->running))
		gif_write_state = GIF_END;*/
	int calc_pos = 0;
	//int num_calcs = calc_count();

	switch (gif_write_state) {
		case GIF_IDLE: {
			gif_newframe = 0;
			break;
		}
		case GIF_START: {
#ifdef USE_GIF_SIZES
			gif_xs = 0;
			gif_ys = 64 * gif_size;
			for (i = 0; i < MAX_CALCS; i++) {
				if (calcs[i].active)
					gif_xs += calcs[i].cpu.pio.lcd->width * gif_size;
		
			}
#endif
			for (int calc_num = 0; calc_num < MAX_CALCS; calc_num++) {
				if (!calcs[calc_num].active)
					continue;
				lcd = calcs[calc_num].cpu.pio.lcd;
#ifdef USE_GIF_SIZES
				gif_indiv_xs = lcd->width * gif_size;
#else
				gif_xs = SCRXSIZE;
				gif_ys = SCRYSIZE;			
#endif
				gif_base_delay = gif_base_delay_start;
				gif_time = 0;
				gif_newframe = 1;
				gif_colors = lcd->shades + 1;
			
				GIFGREYLCD(lcd);
#ifdef USE_GIF_SIZES
				for (i = 0; i < gif_ys; i++)
					for (j = 0; j < gif_indiv_xs; j++)
						gif_frame[i * gif_xs + j + calc_pos] = lcd->gif[i][j];	
				calc_pos += gif_indiv_xs;
			}
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

			for (int calc_num = 0; calc_num < MAX_CALCS; calc_num++) {
				if (!calcs[calc_num].active)
					continue;
				lcd = calcs[calc_num].cpu.pio.lcd;

				GIFGREYLCD(lcd);
#ifdef USE_GIF_SIZES
				gif_indiv_xs = lcd->width * gif_size;
				for (i = 0; i < gif_ys; i++)
					for (j = 0; j < gif_indiv_xs; j++)
						gif_frame[i * gif_xs + j + calc_pos] = lcd->gif[i][j];	
				calc_pos += gif_indiv_xs;
			}
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
#ifdef _WINDOWS
			StringCbCopy(gif_file_name, sizeof(gif_file_name), gif_fn_backup);
#else
			strcpy(gif_file_name, gif_fn_backup);
#endif
			break;
		}
	}
	for (i = 0; i < MAX_CALCS; i++)
		calcs[i].running = running_backup[i];
	
	if (gif_newframe) {
		gif_newframe = 0;
		gif_writer(shades);
		//WriteAVIFrame();
	}
}