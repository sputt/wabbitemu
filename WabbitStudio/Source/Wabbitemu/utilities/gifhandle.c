#include "stdafx.h"

#include "gif.h"
#include "coretypes.h"
#include "lcd.h"
#include "colorlcd.h"
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
	StringCbCopy(gif_fn_backup, sizeof(gif_fn_backup), gif_file_name);
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

uint8_t* generate_gif_image(LCDBase_t *lcd) {
	uint8_t *image = lcd->image(lcd);
	uint8_t *gif = (uint8_t *)malloc(lcd->width * gif_size * lcd->height * gif_size);
	int gif_height = lcd->height * gif_size;
	int gif_width = lcd->width * gif_size;
	
	for (int row = 0; row < gif_height; row++) {
		for (int col = 0; col < gif_width; col++) {
			gif[row * gif_width + col] = image[(row / gif_size) * lcd->width + (col / gif_size)];
		}
	}

	return (uint8_t*) gif;
}

void handle_screenshot() {
	LCD_t* lcd;
	LCDBase_t *lcdBase;
	int i, j;
	u_int shades = 0;
	BOOL running_backup[MAX_CALCS];
	for (i = 0; i < MAX_CALCS; i++) {
		running_backup[i] = calcs[i].running;
		calcs[i].running = FALSE;
		lcdBase = calcs[i].cpu.pio.lcd;
		lcd = (LCD_t *)lcdBase;
		//find the calc with the highest number of shades and use that as our number for the gif
		//since I'm to lazy to implement them individually :P
		if (calcs[i].active && calcs[i].model < TI_84PCSE && lcd && shades < lcd->shades) {
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
				if (calcs[i].active &&  calcs[i].model < TI_84PCSE)
					gif_xs += calcs[i].cpu.pio.lcd->width * gif_size;
		
			}
#endif
			for (int calc_num = 0; calc_num < MAX_CALCS; calc_num++) {
				if (!calcs[calc_num].active || calcs[i].model >= TI_84PCSE)
					continue;
				lcdBase = calcs[calc_num].cpu.pio.lcd;
				lcd = (LCD_t *)lcd;
#ifdef USE_GIF_SIZES
				gif_indiv_xs = lcdBase->width * gif_size;
#else
				gif_xs = SCRXSIZE;
				gif_ys = SCRYSIZE;			
#endif
				gif_base_delay = gif_base_delay_start;
				gif_time = 0;
				gif_newframe = 1;
				gif_colors = lcd->shades + 1;
			
				uint8_t *gif = generate_gif_image(lcdBase);
#ifdef USE_GIF_SIZES
				for (i = 0; i < gif_ys; i++)
					for (j = 0; j < gif_indiv_xs; j++)
						gif_frame[i * gif_xs + j + calc_pos] = gif[i * gif_indiv_xs + j];	
				calc_pos += gif_indiv_xs;
				free(gif);
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
				if (!calcs[calc_num].active || calcs[i].model >= TI_84PCSE)
					continue;
				lcdBase = calcs[calc_num].cpu.pio.lcd;
				lcd = (LCD_t *)lcd;

				uint8_t *gif = generate_gif_image(lcdBase);
#ifdef USE_GIF_SIZES
				gif_indiv_xs = lcdBase->width * gif_size;
				for (i = 0; i < gif_ys; i++)
					for (j = 0; j < gif_indiv_xs; j++)
						gif_frame[i * gif_xs + j + calc_pos] = gif[i * gif_indiv_xs + j];
				calc_pos += gif_indiv_xs;
				free(gif);
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