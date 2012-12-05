#include "stdafx.h"

#include "Wabbitemu_h.h"
#include "WabbitemuClassFactory.h"
#include "calc.h"
#include "gui.h"
#include "Registry.h"
#include "label.h"
#include "lcd.h"
#include "keys.h"

extern HINSTANCE g_hInst;

static void (*fnFrameCallback)(void) = NULL;
static int BasePage = 0;

#ifdef _WINDLL
__declspec (dllexport) BOOL WINAPI DllMain(HINSTANCE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	g_hInst = hModule;
	BOOL success =_Module.DllMain(ul_reason_for_call, lpReserved);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ZeroMemory(calcs, sizeof(calcs));
		LoadRegistrySettings(&calcs[0]);
		break;
	}
	return success;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID * ppvObj)
{ 
	return _Module.GetClassObject(rclsid, riid, ppvObj);
}

STDAPI DllCanUnloadNow(void)
{
	return _Module.DllCanUnloadNow();
}

STDAPI DllRegisterServer(void)
{
	return _Module.DllRegisterServer(TRUE);
}

STDAPI DllUnregisterServer(void)
{
	return _Module.DllUnregisterServer(TRUE);
}

#endif

#pragma pack(1)
typedef struct {
	BYTE ID, f, x, w, y, h, z, d, anim_ctr;
	WORD anim_ptr;
} ZOBJECT;
#pragma pack()

#pragma pack(1)
typedef struct {
	BYTE alive, x, w, y, h, anim_ctr;
	WORD key_ptr, anim_ptr;
} ZANIMATE;
#pragma pack()

// 0 = anim
// 1 = obj
// 2 = enemy
__declspec (dllexport) 
int FindAnimationInfo(int type, unsigned short addr, int *frame, unsigned short *base_addr) {
	TCHAR *name = NULL;
	if (type == 0) {
		name = _T("ANIMATE");
	} else if (type == 1) {
		name = _T("OBJECT");
	} else {
		name = _T("ENEMY");
	}
	
	TCHAR label_name[64];
	_tprintf_s(label_name, _T("%s_ANIM_PAGE"), name);
	label_struct *lab = lookup_label(&calcs[0], label_name);
	if (lab == NULL) {
		return 0;
	}
	
	waddr_t waddr;
	waddr.addr = mc_base(addr) - 1;
	waddr.is_ram = FALSE;
	waddr.page = BasePage - (mc_base(lab->addr) - 1);
	
	// while the counts aren't 0
	while (wmem_read(&calcs[0].mem_c, waddr) != 0) {
		waddr.addr+=3;
	}
	
	
	unsigned short restart_addr = 0;
	waddr.addr++;
	restart_addr = wmem_read(&calcs[0].mem_c, waddr);
	waddr.addr++;
	restart_addr += (wmem_read(&calcs[0].mem_c, waddr) << 8);
	
	if (base_addr != NULL)
		*base_addr = restart_addr;
	if (frame != NULL)
		*frame = (addr - (restart_addr + 1)) / 3;
	return 1;
}

__declspec (dllexport) 
ZOBJECT *GetZeldaObjectArray() {
	label_struct *lab = lookup_label(&calcs[0], _T("object_array"));
	if (lab == NULL) {
		return NULL;
	}
	
	return (ZOBJECT *) &calcs[0].mem_c.ram[calcs[0].mem_c.banks[mc_bank(lab->addr)].page * PAGE_SIZE + mc_base(lab->addr)];
}

__declspec (dllexport) 
ZANIMATE *GetZeldaAnimateArray() {
	label_struct *lab = lookup_label(&calcs[0], _T("animate_array"));
	if (lab == NULL) {
		return NULL;
	}
	
	return (ZANIMATE *) &calcs[0].mem_c.ram[calcs[0].mem_c.banks[mc_bank(lab->addr)].page * PAGE_SIZE + mc_base(lab->addr)];
}

__declspec (dllexport)
unsigned char *GetZeldaMapData() {
	label_struct *lab = lookup_label(&calcs[0], _T("map_data"));
	if (lab == NULL) {
		return NULL;
	}
	
	return (unsigned char *) &calcs[0].mem_c.ram[calcs[0].mem_c.banks[mc_bank(lab->addr)].page * PAGE_SIZE + mc_base(lab->addr)];
}

__declspec (dllexport)
void *GetZeldaDrawQueue() {
	label_struct *lab = lookup_label(&calcs[0], _T("draw_queue"));
	if (lab == NULL) {
		return NULL;
	}
	
	return (void *) &calcs[0].mem_c.ram[calcs[0].mem_c.banks[mc_bank(lab->addr)].page * PAGE_SIZE + mc_base(lab->addr)];
}

__declspec (dllexport)
int GetZeldaDrawQueueCount() {
	label_struct *lab = lookup_label(&calcs[0], _T("draw_count"));
	if (lab == NULL) {
		return 0;
	}
	
	return calcs[0].mem_c.ram[calcs[0].mem_c.banks[mc_bank(lab->addr)].page * PAGE_SIZE + mc_base(lab->addr)];
}

__declspec (dllexport) 
unsigned char *GetLCDImage() {
	return LCD_image(calcs[0].cpu.pio.lcd);
}

__declspec (dllexport)
void PressKey(WORD wKey) {
	keypad_key_press(&calcs[0].cpu, wKey);
}

__declspec (dllexport)
void ReleaseKey(WORD wKey) {
	keypad_key_release(&calcs[0].cpu, wKey);
}

__declspec (dllexport)
void EnableCalc(BOOL fEnable) {
	calcs[0].active = fEnable;
	calcs[0].running = fEnable;
}

__declspec (dllexport)
void HookZelda(void (*fnCallback)(void) ) {
	
	label_struct *lab = lookup_label(&calcs[0], _T("sort_done"));
	if (lab == NULL) {
		return;
	}
	u_int i;
	for (i = 0; i < calcs[0].applist.count; i++) {
		if (_tcsnicmp(calcs[0].applist.apps[i].name, _T("ZELDA"), 5) == 0) {
			BasePage = calcs[0].applist.apps[i].page;
			break;
		}
	}
	waddr_t waddr;
	waddr.addr = lab->addr;
	waddr.page = lab->page;
	waddr.is_ram = lab->IsRAM;
	set_break(&calcs[0].mem_c, waddr);

	label_struct *menu_lab = lookup_label(&calcs[0], _T("sub_sync"));
	if (menu_lab != NULL)
	{
		waddr.addr = menu_lab->addr;
		waddr.page = menu_lab->page;
		waddr.is_ram = menu_lab->IsRAM;
		set_break(&calcs[0].mem_c, waddr);
	}
	fnFrameCallback = fnCallback;
}

// returns bytes read
__declspec(dllexport)
int ReadVariable(TCHAR *name, void *data, int bytes) {
	int address = 0;
	label_struct *lab = lookup_label(&calcs[0], name);

	if (lab == NULL) {
		address = _tstoi(name);
	}
	else
	{
		address = lab->addr;
	}
	
	int i;
	for (i = 0; i < bytes; i++) {
		((_TUCHAR *) data)[i] = mem_read(&calcs[0].mem_c, address + i);
	}
	return bytes;
}

/*int gui_debug(int slot) {
	if (fnFrameCallback != NULL) {
		fnFrameCallback();
	}
	CPU_step(&calcs[0].cpu);
	calcs[0].running = TRUE;
	return 0;
}*/
