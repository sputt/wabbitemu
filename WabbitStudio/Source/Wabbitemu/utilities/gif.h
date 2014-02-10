#ifndef GIF_H
#define GIF_H

#include "calc.h"
#include "coretypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GIF_FRAME_MAX (320 * 240 * MAX_CALCS)

#define GIF_IDLE 0
#define GIF_START 1
#define GIF_FRAME 2
#define GIF_END 3

extern int gif_write_state;
extern int gif_file_size;
extern TCHAR screenshot_file_name[512];
extern BOOL screenshot_autosave;
extern BOOL screenshot_use_increasing;
extern u_int screenshot_size;
extern u_int screenshot_color_size;
extern WORD gif_base_delay;
extern int gif_xs;
extern int gif_indiv_xs;
extern int gif_ys;
extern BYTE gif_frame[GIF_FRAME_MAX];
extern int gif_time;
extern int gif_newframe;
extern int gif_colors;
extern int gif_base_delay_start;
extern int gif_file_num;
extern BOOL gif_bw;
extern BOOL gif_color;

void gif_writer(int shades);
int gif_convert_color_to_index(int r, int g, int b);
void gif_clear_palette();

#ifdef __cplusplus
}
#endif
#endif // GIF_H