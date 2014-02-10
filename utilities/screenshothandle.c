#include "stdafx.h"

#include "gif.h"
#include "coretypes.h"
#include "lcd.h"
#include "colorlcd.h"
#include "screenshothandle.h"
#ifdef WINVER
#include "fileutilities.h"
#endif

TCHAR pngext[5] = _T("png");
TCHAR gifext[5] = _T("ext");


TCHAR *generate_screenshot_name(TCHAR *fn, int num, TCHAR *dest, TCHAR *ext) {
	size_t i;
	for (i = _tcslen(fn) - 1; i && fn[i] != '.'; i--);
	 	 
	if (i) {
		fn[i] = '\0';
	}
	
	StringCbPrintf(dest, _tcslen(dest) + _tcslen(ext) + 1, _T("%s%d.%s"), fn, num, ext);
	
	if (i)  {
		fn[i] = '.';
	}
	return dest;
}

static TCHAR screenshot_fn_backup[MAX_PATH];

void get_next_filename(TCHAR *ext) {
	FILE *test = NULL;
	BOOL fileExists = FALSE;
	int i = 0;

	do {
		generate_screenshot_name(screenshot_fn_backup, i, screenshot_file_name, ext);
#ifdef _WINDOWS
		_tfopen_s(&test, screenshot_file_name, _T("r"));
#else
		test = fopen(screenshot_file_name, "r");
#endif
		i++;
		if (test) {
			fclose(test);
			fileExists = TRUE;
		}
		else {
			fileExists = FALSE;
		}
	} while (fileExists);
}

/*
 * Gets where the next screenshot should be saved to.
 * Returns true if ready, false if user cancels
 */
BOOL get_screenshot_filename(TCHAR *ext) {
	StringCbCopy(screenshot_fn_backup, sizeof(screenshot_fn_backup), screenshot_file_name);
	if (screenshot_autosave) {
		/* do file save */
		if (screenshot_use_increasing) {
			get_next_filename(ext);
		}
	} else {
		int index;
		if (_tcscmp(ext, pngext) == 0) {
			index = 2;
		} else {
			index = 1;
		}

#ifdef _WINDOWS
#ifndef _WINDLL
		if (SaveFile(screenshot_file_name, _T("Graphics Interchange Format  (*.gif)\0*.gif\0Portable Network Graphics  (*.png)\0*.png\0All Files (*.*)\0*.*\0\0"),
						_T("Wabbitemu Screenshot File Target"), ext, 0, index))
			//if we cancel, mark the menu and set to idle
			return FALSE;
#endif
#endif
	}
	return TRUE;
}

uint8_t* generate_gif_image(LCDBase_t *lcd, int gif_size) {
	uint8_t *image = lcd->image(lcd);
	uint8_t *gif = (uint8_t *)malloc(lcd->width * gif_size * lcd->height * gif_size);
	int gif_height = lcd->height * gif_size;
	int gif_width = lcd->display_width * gif_size;

	for (int row = 0; row < gif_height; row++) {
		for (int col = 0; col < gif_width; col++) {
			uint8_t color = 0;
			if (lcd->bytes_per_pixel > 1) {
				int idx = (row / gif_size) * lcd->width * lcd->bytes_per_pixel + (col / gif_size) * lcd->bytes_per_pixel;
				uint8_t b = image[idx];
				uint8_t g = image[idx + 1];
				uint8_t r = image[idx + 2];
				color = (uint8_t) gif_convert_color_to_index(r, g, b);
			}
			else {
				int part = 255 / gif_colors;
				color = image[(row / gif_size) * lcd->width * lcd->bytes_per_pixel + (col / gif_size) * lcd->bytes_per_pixel];
				color = (color + (part / 2)) / part;
			}

			gif[row * gif_width + col] = color;
		}
	}

	free(image);
	return (uint8_t*) gif;
}

void handle_screenshot() {
	LCDBase_t* lcd;
	int i, j;
	u_int shades = 0;
	BOOL running_backup[MAX_CALCS];
	int size;
	for (i = 0; i < MAX_CALCS; i++) {
		running_backup[i] = calcs[i].running;
		calcs[i].running = FALSE;
		lcd = calcs[i].cpu.pio.lcd;

		if (calcs[i].active && calcs[i].model < TI_84PCSE && lcd && shades < ((LCD_t *) lcd)->shades) {
			shades = ((LCD_t *)lcd)->shades;
			size = screenshot_size;
		}
		else if (calcs[i].model == TI_84PCSE)
		{
			shades = 255;
			size = screenshot_color_size;
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
			gif_xs = 0;
			
			for (i = 0; i < MAX_CALCS; i++) {
				if (calcs[i].active) {
					gif_xs += calcs[i].cpu.pio.lcd->display_width * size;
					gif_ys = calcs[i].cpu.pio.lcd->height * size;
				}
			}

			gif_clear_palette();

			for (int calc_num = 0; calc_num < MAX_CALCS; calc_num++) {
				if (!calcs[calc_num].active)
					continue;
				lcd = calcs[calc_num].cpu.pio.lcd;

				gif_indiv_xs = lcd->display_width * size;
				gif_base_delay = gif_base_delay_start;
				gif_time = 0;
				gif_newframe = 1;
				gif_colors = shades + 1;
			
				uint8_t *gif = generate_gif_image(lcd, size);
				for (i = 0; i < gif_ys; i++)
					for (j = 0; j < gif_indiv_xs; j++)
						gif_frame[i * gif_xs + j + calc_pos] = gif[i * gif_indiv_xs + j];	
				calc_pos += gif_indiv_xs;
				free(gif);
			}
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

					uint8_t *gif = generate_gif_image(lcd, size);
					gif_indiv_xs = lcd->display_width * size;
					for (i = 0; i < gif_ys; i++)
						for (j = 0; j < gif_indiv_xs; j++)
							gif_frame[i * gif_xs + j + calc_pos] = gif[i * gif_indiv_xs + j];
					calc_pos += gif_indiv_xs;
					free(gif);
				}
			}
			break;
		}
		case GIF_END:
		{
			gif_newframe = 1;
			gif_file_num++;
			StringCbCopy(screenshot_file_name, sizeof(screenshot_file_name), screenshot_fn_backup);
			break;
		}
	}
	for (i = 0; i < MAX_CALCS; i++)
		calcs[i].running = running_backup[i];
	
	if (gif_newframe) {
		gif_newframe = 0;
		gif_writer(shades);
	}
}