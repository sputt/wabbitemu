#include "calc.h"
#include "sendfiles.h"

#ifdef WINVER
#include <windows.h>
#endif

HINSTANCE g_hInst;

void calc_load(calc_t *, const char*);
void calc_pause(calc_t *);
void calc_run(calc_t *);

#ifdef WINVER
DWORD calc_run_thread(calc_t *);
#else
int calc_run_thread(calc_t *);
#endif

calc_t *calc_new(void) {
	int slot = calc_slot_new();
	if (slot == -1)
		return NULL;
	
	calc_t *calc = &calcs[slot];
	memset(calc, 0, sizeof(calc_t));
	
	calc->slot = slot;

#ifdef WINVER
	calc->hdlThread = CreateThread(NULL, 0, &calc_run_thread, calc, CREATE_SUSPENDED, NULL);
#elif MACVER
	pthread_create(&calc->hdlThread, NULL, calc_run_thread, (void *) calc);
#endif
	return calc;
}

void calc_load(calc_t *calc, const char *filename) {
	if (calc == NULL) return;

	calc_pause(calc);
	
	gslot = calc->slot;
	char *fn = AppendName(NULL, filename);
	NoThreadSend(fn, 1);
	
	calc_run(calc);
}

void calc_run(calc_t *calc) {
#ifdef WINVER
	calc->running = TRUE;
	ResumeThread(calc->hdlThread);
#else
	//pthread code
	calc->running = TRUE;
#endif
}

void calc_pause(calc_t *calc) {
#ifdef WINVER
	calc->running = FALSE;
	SuspendThread(calc->hdlThread);
#else
	//pthread code
	calc->running = FALSE;
#endif
}

#ifdef WINVER
DWORD calc_run_thread(calc_t *calc) {
#else
int calc_run_thread(calc_t *calc) {
#endif
	clock_t difference = 0, prevTimer = clock();
	clock_t dwTimer;
	
	for (;;) {
		if (calc->running) {
			dwTimer = clock();
	
			// Keep a running sum of how much our actual times differ
			// from where we are supposed to be
			difference += (dwTimer - (prevTimer + TPF));
	
			// Are we greater than Ticks Per Frame that would call for
			// a frame skip? (in other words run a frame if it's not
			// time to skip)
			if (difference > -TPF) {
				calc_run_frame(calc->slot);
				// If we are really behind, keep running until we catch up
				while (difference >= TPF) {
					calc_run_frame(calc->slot);
					difference -= TPF;
				}
				
			// Frame skip if we're too far ahead.
			} else difference += TPF;
	
			prevTimer = dwTimer;
			
			Sleep(TPF);
		}
	}
	
	return 0;
}

#ifdef BUILD_DLL
__stdcall
__declspec(dllexport) 
#endif
int calc_key_press(calc_t *calc, int keycode) {
	keypad_key_press(&calc->cpu, keycode);
	return keycode;
}

#ifdef BUILD_DLL
__stdcall
__declspec(dllexport) 
#endif
int calc_key_release(calc_t *calc, int keycode) {
	keypad_key_release(&calc->cpu, keycode);
	return keycode;
}

#ifdef WINVER
void calc_draw(calc_t *calc, HDC hdcDest) {
#elif QTVER
uint8_t calc_draw(calc_t *calc) {
#endif

#ifdef WINVER
#define MAX_SHADES 255
	BITMAPINFO *bi;
	bi = malloc(sizeof(BITMAPINFOHEADER) + (MAX_SHADES+1)*sizeof(RGBQUAD));
	bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi->bmiHeader.biWidth = 128;
	bi->bmiHeader.biHeight = -64;
	bi->bmiHeader.biPlanes = 1;
	bi->bmiHeader.biBitCount = 8;
	bi->bmiHeader.biCompression = BI_RGB;
	bi->bmiHeader.biSizeImage = 0;
	bi->bmiHeader.biXPelsPerMeter = 0;
	bi->bmiHeader.biYPelsPerMeter = 0;
	bi->bmiHeader.biClrUsed = MAX_SHADES+1;
	bi->bmiHeader.biClrImportant = MAX_SHADES+1;
	
	//#define LCD_LOW (RGB(0x9E, 0xAB, 0x88))			
	int i;
	#define LCD_HIGH	255
	for (i = 0; i <= MAX_SHADES; i++) {
		bi->bmiColors[i].rgbRed = (0x9E*(256-(LCD_HIGH/MAX_SHADES)*i))/255;
		bi->bmiColors[i].rgbGreen = (0xAB*(256-(LCD_HIGH/MAX_SHADES)*i))/255;
		bi->bmiColors[i].rgbBlue = (0x88*(256-(LCD_HIGH/MAX_SHADES)*i))/255;				
	}	
	uint8_t *screen = LCD_image(calc->cpu.pio.lcd) ;		
	StretchDIBits(
		hdcDest,
		//rc.left, rc.top, rc.right - rc.left,  rc.bottom - rc.top,
		0, 0, 96*2, 64*2,
		0, 0, 96, 64,
		screen,
		bi,
		DIB_RGB_COLORS,
		SRCCOPY);
	
	 
	int total = 0;
	for (i = 0; i < 128*64; i++) {
		total += screen[i];
	}
	
	free(bi);
#elif QTVER
    return LCD_image(calc->cpu.pio.lcd) ;
#endif    
}
