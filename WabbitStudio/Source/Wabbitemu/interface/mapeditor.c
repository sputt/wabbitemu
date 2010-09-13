#include "stdafx.h"

#include "WabbitemuClassFactory.h"
#include "calc.h"
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
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ZeroMemory(calcs, sizeof(calcs));
		break;
	}
	return TRUE;
}

HRESULT CALLBACK DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID * ppvObj)
{ 
    HRESULT hr = E_OUTOFMEMORY;
    *ppvObj = NULL;

    CWabbitemuClassFactory *pClassFactory = new CWabbitemuClassFactory();
    if (pClassFactory != NULL)
	{
        hr = pClassFactory->QueryInterface(riid, ppvObj);
        pClassFactory->Release();
    }
    return hr;
}

HRESULT CALLBACK DllCanUnloadNow(void)
{
	return S_FALSE;
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
	char *name = NULL;
	if (type == 0) {
		name = "ANIMATE";
	} else if (type == 1) {
		name = "OBJECT";
	} else {
		name = "ENEMY";
	}
	
	char label_name[64];
#ifdef WINVER
	sprintf_s(label_name, "%s_ANIM_PAGE", name);
#else
	sprintf(label_name, "%s_ANIM_PAGE", name);
#endif
	label_struct *lab = lookup_label(label_name);
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
	label_struct *lab = lookup_label("object_array");
	if (lab == NULL) {
		return NULL;
	}
	
	return (ZOBJECT *) &calcs[0].mem_c.ram[calcs[0].mem_c.banks[mc_bank(lab->addr)].page * PAGE_SIZE + mc_base(lab->addr)];
}

__declspec (dllexport) 
ZANIMATE *GetZeldaAnimateArray() {
	label_struct *lab = lookup_label("animate_array");
	if (lab == NULL) {
		return NULL;
	}
	
	return (ZANIMATE *) &calcs[0].mem_c.ram[calcs[0].mem_c.banks[mc_bank(lab->addr)].page * PAGE_SIZE + mc_base(lab->addr)];
}

__declspec (dllexport)
unsigned char *GetZeldaMapData() {
	label_struct *lab = lookup_label("map_data");
	if (lab == NULL) {
		return NULL;
	}
	
	return (unsigned char *) &calcs[0].mem_c.ram[calcs[0].mem_c.banks[mc_bank(lab->addr)].page * PAGE_SIZE + mc_base(lab->addr)];
}

__declspec (dllexport)
void *GetZeldaDrawQueue() {
	label_struct *lab = lookup_label("draw_queue");
	if (lab == NULL) {
		return NULL;
	}
	
	return (void *) &calcs[0].mem_c.ram[calcs[0].mem_c.banks[mc_bank(lab->addr)].page * PAGE_SIZE + mc_base(lab->addr)];
}

__declspec (dllexport)
int GetZeldaDrawQueueCount() {
	label_struct *lab = lookup_label("draw_count");
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
	gslot = 0;
	
	calcs[0].active = fEnable;
	calcs[0].running = fEnable;
}

__declspec (dllexport)
void HookZelda(void (*fnCallback)(void) ) {
	
	label_struct *lab = lookup_label("sort_done");
	if (lab == NULL) {
		return;
	}
	u_int i;
	for (i = 0; i < calcs[0].applist.count; i++) {
		if (_strnicmp(calcs[0].applist.apps[i].name, "ZELDA", 5) == 0) {
			BasePage = calcs[0].applist.apps[i].page;
			break;
		}
	}
	set_break(&calcs[0].mem_c, lab->IsRAM, lab->page, lab->addr);

	label_struct *menu_lab = lookup_label("sub_sync");
	if (menu_lab != NULL)
	{
		set_break(&calcs[0].mem_c, menu_lab->IsRAM, menu_lab->page, menu_lab->addr);
	}
	fnFrameCallback = fnCallback;
}

// returns bytes read
__declspec(dllexport)
int ReadVariable(char *name, void *data, int bytes) {
	int address = 0;
	label_struct *lab = lookup_label(name);

	if (lab == NULL) {
		address = atoi(name);
	}
	else
	{
		address = lab->addr;
	}
	
	int i;
	for (i = 0; i < bytes; i++) {
		((unsigned char *) data)[i] = mem_read(&calcs[0].mem_c, address + i);
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
