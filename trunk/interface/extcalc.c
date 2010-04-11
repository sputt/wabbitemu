#include "calc.h"
#include "lcd.h"
#include "sendfiles.h"

#ifdef WINVER
#include <windows.h>
#endif

__stdcall __declspec(dllexport) int calc_new(void);
__stdcall __declspec(dllexport) void calc_load(int, const char*);
__stdcall __declspec(dllexport) void calc_pause(int);
__stdcall __declspec(dllexport) void calc_set_lcd(int, HWND);
DWORD calc_run_thread(calc_t*);
__stdcall __declspec(dllexport) void calc_run(int);
__stdcall __declspec(dllexport) void calc_free(int);
__stdcall __declspec(dllexport) int calc_key_press(int slot, int keycode);
__stdcall __declspec(dllexport) int calc_key_release(int slot, int keycode);
__stdcall __declspec(dllexport) Z80State_t calc_get_state(int slot);
__stdcall __declspec(dllexport) void calc_set_state(int slot, Z80State_t zs);
__stdcall __declspec(dllexport) byte calc_read_mem(int, WORD);
__stdcall __declspec(dllexport) void calc_write_mem(int, WORD, BYTE);
__stdcall __declspec(dllexport) u_char* calc_draw_screen(int);
__stdcall __declspec(dllexport) void calc_set_draw_screen(int, HWND);
__stdcall __declspec(dllexport) int calc_set_breakpoint(int, HWND, BOOL, BYTE, WORD);
__stdcall __declspec(dllexport) int calc_clear_breakpoint(int, BOOL, BYTE, WORD);
__stdcall __declspec(dllexport) applist_t* calc_get_applist(int);
__stdcall __declspec(dllexport) symlist_t calc_get_symlist(int);
__stdcall __declspec(dllexport) MEMSTATE* calc_get_memstate(int);
__stdcall __declspec(dllexport) void calc_set_memstate(int, MEMSTATE*);
__stdcall __declspec(dllexport) void calc_step(int);
__stdcall __declspec(dllexport) void calc_step_over(int);
__stdcall __declspec(dllexport) void calc_step_out(int);

/*#ifdef WINVER
DWORD calc_run_thread(int);
#else
int calc_run_thread(calc_t *);
#endif*/

__stdcall
__declspec(dllexport)
int calc_new() {
	int slot = calc_slot_new();
	if (slot == -1)
		return NULL;

	calc_t *calc = &calcs[slot];
	calc->hwndLCD = NULL;
	memset(calc, 0, sizeof(calc_t));

	calc->slot = slot;

#ifdef WINVER
	calc->hdlThread = CreateThread(NULL, 0, &calc_run_thread, calc, 0 /*CREATE_SUSPENDED*/, NULL);
#elif MACVER
	pthread_create(&calc->hdlThread, NULL, calc_run_thread, (void *) calc);
#endif
	return slot;
}

__stdcall
__declspec(dllexport)
void calc_load(int slot, const char *filename) {
	calc_t calc = calcs[slot];
	if (slot == -1) return;

	calc_pause(slot);

	gslot = calc.slot;
	char *fn = AppendName(NULL, filename);
	NoThreadSend(fn, 1);

	calc_run(slot);
}

__stdcall
__declspec(dllexport)
void calc_run(int slot) {
#ifdef WINVER
	calcs[slot].running = TRUE;
	ResumeThread(calcs[slot].hdlThread);
#else
	//pthread code
	calc->running = TRUE;
#endif
}

__stdcall
__declspec(dllexport)
void calc_free(int slot) {
	if (slot == -1)
		return;

	if (calcs[slot].active) {
		calcs[slot].active = FALSE;
		#ifdef WINVER
		/* don't forget to change this when audio for non-Windows
		 * builds is implemented, or bad things happen! */
		KillSound(calcs[slot].audio);
		#endif
		printf("Freeing memory\n");
		free(calcs[slot].mem_c.flash);
		free(calcs[slot].mem_c.ram);
		printf("Freeing hardware\n");
		free(calcs[slot].cpu.pio.link);
		printf("freeing keypad\n");
		free(calcs[slot].cpu.pio.keypad);
		printf("freeing stdint\n");
		free(calcs[slot].cpu.pio.stdint);
		printf("freeing se aux %p\n", calcs[slot].cpu.pio.se_aux);
		free(calcs[slot].cpu.pio.se_aux);
		printf("freeing lcd\n");
		free(calcs[slot].cpu.pio.lcd);
		printf("Done freeing\n");
	}
}

__stdcall
__declspec(dllexport)
void calc_pause(int slot) {
#ifdef WINVER
	calcs[slot].running = FALSE;
	SuspendThread(calcs[slot].hdlThread);
#else
	//pthread code
	calc->running = FALSE;
#endif
}
__stdcall
__declspec(dllexport)
void calc_set_lcd(int slot, HWND hwndLCD)
{
	calcs[slot].hwndLCD = hwndLCD;
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
				HDC hdc = GetDC(calc->hwndLCD);
				HDC hdcTest = CreateCompatibleDC(hdc);
				if (hdcTest != NULL && calc->hwndLCD != NULL)
					PaintLCD(calc->hwndLCD, hdcTest);
				// If we are really behind, keep running until we catch up
				while (difference >= TPF) {
					calc_run_frame(calc->slot);
					difference -= TPF;
					HDC hdc = GetDC(calc->hwndLCD);
					HDC hdcTest = CreateCompatibleDC(hdc);
					if (hdcTest != NULL && calc->hwndLCD != NULL)
						PaintLCD(calc->hwndLCD, hdcTest);
				}

			// Frame skip if we're too far ahead.
			} else difference += TPF;

			prevTimer = dwTimer;

			Sleep(TPF);
		}
	}

	return 0;
}

__stdcall
__declspec(dllexport)
int calc_key_press(int slot, int keycode) {
	keypad_key_press(&calcs[slot].cpu, keycode);
	return keycode;
}

__stdcall
__declspec(dllexport)
int calc_key_release(int slot, int keycode) {
	keypad_key_release(&calcs[slot].cpu, keycode);
	return keycode;
}


__stdcall
__declspec(dllexport)
Z80State_t calc_get_state(int slot) {
	Z80State_t zs;
	CPU_t *cpu = &calcs[slot].cpu;

	zs.AF = cpu->af;
	zs.BC = cpu->bc;
	zs.DE = cpu->de;
	zs.HL = cpu->hl;
	zs.IX = cpu->ix;
	zs.IY = cpu->iy;

	zs.AFP = cpu->afp;
	zs.BCP = cpu->bcp;
	zs.DEP = cpu->dep;
	zs.HLP = cpu->hlp;

	zs.PC = cpu->pc;
	zs.SP = cpu->sp;

	zs.I = cpu->i;
	zs.R = cpu->r;
	zs.Bus = cpu->bus;
	zs.IFF1 = cpu->iff1;
	zs.IFF2 = cpu->iff2;

	zs.IMode = cpu->imode;
	zs.SecondsElapsed = tc_elapsed(cpu->timer_c);

	zs.Frequency = calcs[gslot].timer_c.freq;
	zs.Halt = cpu->halt;

	return zs;
}

__stdcall
__declspec(dllexport)
void calc_set_state(int slot, Z80State_t zs) {
	BOOL running_backup = calcs[slot].running;

	calcs[slot].running = FALSE;
	CPU_t *cpu = &calcs[slot].cpu;

	cpu->af = zs.AF;
	cpu->bc = zs.BC;
	cpu->de = zs.DE;
	cpu->hl = zs.HL;

	cpu->afp = zs.AFP;
	cpu->bcp = zs.BCP;
	cpu->dep = zs.DEP;
	cpu->hlp = zs.HLP;

	cpu->ix = zs.IX;
	cpu->iy = zs.IY;

	cpu->pc = zs.PC;
	cpu->sp = zs.SP;

	cpu->i = zs.I;
	cpu->iff1 = zs.IFF1;
	cpu->iff2 = zs.IFF2;

	cpu->imode = zs.IMode;

	calcs[slot].timer_c.freq = zs.Frequency;
	cpu->halt = zs.Halt;

	calcs[slot].running = running_backup;
}

__stdcall
__declspec(dllexport)
byte calc_read_mem(int slot, WORD address) {
	byte value = mem_read(calcs[slot].cpu.mem_c, address);
	return value;
}

__stdcall
__declspec(dllexport)
void calc_write_mem(int slot, WORD address, BYTE value) {
	mem_write(calcs[slot].cpu.mem_c, address, value);
}

__stdcall
__declspec(dllexport)
u_char* calc_draw_screen(int slot) {
	u_char *image = LCD_image(calcs[slot].cpu.pio.lcd);
	return image;
}

__stdcall
__declspec(dllexport)
void calc_set_draw_screen(int slot, HWND hwnd) {
	calcs[slot].hwndLCD = hwnd;
}

__stdcall
__declspec(dllexport)
int calc_set_breakpoint(int slot, HWND hwndCallback, BOOL is_ram, BYTE page, WORD address) {
	if (is_ram > TRUE)
		is_ram = TRUE;

	if (is_ram == TRUE) {
		if (page >= calcs[slot].mem_c.ram_pages)
			return 1;
	} else {
		if (page >= calcs[slot].mem_c.flash_pages)
			return 1;
	}

	calcs[slot].ole_callback = hwndCallback;

	set_break(&calcs[slot].mem_c, is_ram, page, address);
	return 0;
}

__stdcall
__declspec(dllexport)
int calc_clear_breakpoint(int slot, BOOL is_ram, BYTE page, WORD address) {
	if (is_ram > TRUE)
		is_ram = TRUE;

	if (is_ram == TRUE) {
		if (page >= calcs[slot].mem_c.ram_pages)
			return 1;
	} else {
		if (page >= calcs[slot].mem_c.flash_pages)
			return 1;
	}

	calcs[slot].mem_c.breaks[is_ram % 2][PAGE_SIZE * page + mc_base(address)] = 0;
	//calcs[slot].running = TRUE;
	return 0;
}

__stdcall
__declspec(dllexport)
applist_t* calc_get_applist(int slot) {
	applist_t* applist = NULL;
	state_build_applist(&calcs[slot].cpu, applist);

	return applist;
}

__stdcall
__declspec(dllexport)
symlist_t calc_get_symlist(int slot) {
	symlist_t symlist;
	state_build_symlist_83P(&calcs[slot].cpu, &symlist);

	return symlist;
}

__stdcall
__declspec(dllexport)
MEMSTATE* calc_get_memstate(int slot) {
	int i;
	MEMSTATE* mem = NULL;
	for (i = 0; i < 4; i++) {
		bank_t *bank = &calcs[slot].mem_c.banks[i];
		mem[i].is_ram = bank->ram;
		mem[i].page = bank->page;
	}
	return mem;
}
__stdcall
__declspec(dllexport)
void calc_set_memstate(int slot, MEMSTATE* mem) {
	int i;
	for (i = 0; i < 4; i++) {
		bank_t *bank = &calcs[slot].mem_c.banks[i];
		bank->ram = mem[i].is_ram;
		bank->page = mem[i].page;
	}
}

__stdcall
__declspec(dllexport)
void calc_step(int slot) {
	CPU_step(&calcs[slot].cpu);
}

__stdcall
__declspec(dllexport)
void calc_step_over(int slot) {
	CPU_stepover(&calcs[slot].cpu);
}

__stdcall
__declspec(dllexport)
void calc_step_out(int slot) {
	CPU_stepout(&calcs[slot].cpu);
}

/*#ifdef WINVER
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
}*/
