#include "stdafx.h"

#include "gui.h"
#include "calc.h"
#include "gif.h"

static HKEY hkeyTarget;

static struct {
	LPCTSTR lpValueName;
	DWORD dwType;
	LONG_PTR Value;
} regDefaults[] = {
	{"cutout", 		REG_DWORD, 	0},
	{"skin",		REG_DWORD,	0},
	{"version", 	REG_SZ, 	(LONG_PTR) "1.5"},
	{"rom_path", 	REG_SZ, 	(LONG_PTR) "z.rom"},
	{"shades",		REG_DWORD,	6},
	{"gif_path", 	REG_SZ,		(LONG_PTR) "wabbitemu.gif"},
	{"gif_autosave",REG_DWORD,	0},
	{"gif_useinc",	REG_DWORD,	0},
	{"lcd_mode",	REG_DWORD,	0}, // perfect gray
	{"lcd_freq",	REG_DWORD,	FPS}, // steady freq
	{"screen_scale",REG_DWORD,  2},
	{"faceplate_color", REG_DWORD, 	0x838587},
	{"exit_save_state", REG_DWORD,  0},
	{"load_files_first", REG_DWORD,  1},
	{"do_backups", REG_DWORD,  0},
	{"num_keys",		REG_DWORD,  5},
	{NULL,			0,			0},
};

HRESULT LoadRegistryDefaults(HKEY hkey) {

	u_int i;
	for (i = 0; regDefaults[i].lpValueName != NULL; i++) {
		DWORD cbData;
		BYTE *lpData;
		switch (regDefaults[i].dwType) {
		case REG_DWORD:
			cbData = sizeof(DWORD);
			lpData = (BYTE*) &regDefaults[i].Value;
			break;
		case REG_SZ:
			cbData = lstrlen((char *) regDefaults[i].Value) + 1;
			lpData = (LPBYTE) regDefaults[i].Value;
			break;
		default:
			cbData = 0;
			lpData = NULL;
		}
		RegSetValueEx(
				hkey,
				regDefaults[i].lpValueName,
				0,
				regDefaults[i].dwType,
				lpData,
				cbData);
	}
	return S_OK;
}

INT_PTR QueryWabbitKey(char *name) {
	HKEY hkeySoftware;
	RegOpenKeyEx(HKEY_CURRENT_USER, "software", 0, KEY_ALL_ACCESS, &hkeySoftware);
	
	HKEY hkeyWabbit;
	DWORD dwDisposition;
	RegCreateKeyEx(hkeySoftware, "Wabbitemu", 0, 
			NULL, REG_OPTION_NON_VOLATILE, 
			KEY_ALL_ACCESS, NULL, &hkeyWabbit, &dwDisposition);
	
	DWORD type;
	DWORD len;
	u_int i;
	for (i = 0; regDefaults[i].lpValueName != NULL; i++) {
		if (strcmp(regDefaults[i].lpValueName, name) == 0) break;
	}
	type = regDefaults[i].dwType;
	len = (type == REG_SZ) ? 256 : sizeof(DWORD);
	static union {
		DWORD dwResult;
		char szResult[256];
	} result;
	
	LONG rqvx_res;
	if (regDefaults[i].lpValueName != NULL) {
		rqvx_res = RegQueryValueEx(hkeyWabbit, name, NULL, NULL, (LPBYTE) &result, &len);
		if (rqvx_res == ERROR_FILE_NOT_FOUND) {
			if (type == REG_DWORD)
				result.dwResult = regDefaults[i].Value;
			else
				strcpy(result.szResult, (char *) regDefaults[i].Value);
		}
	} else {
		MessageBox(NULL, "Could not find registry key", name, MB_OK);
		exit(1);
	}
	
	RegCloseKey(hkeyWabbit);
	RegCloseKey(hkeySoftware);
	
	return (type == REG_SZ) ? (LONG_PTR) result.szResult : result.dwResult;
}

HRESULT LoadRegistrySettings(void) {
	HKEY hkeySoftware;
	RegOpenKeyEx(HKEY_CURRENT_USER, "software", 0, KEY_ALL_ACCESS, &hkeySoftware);
	
	HKEY hkeyWabbit;
	DWORD dwDisposition;
	RegCreateKeyEx(hkeySoftware, "Wabbitemu", 0, 
			NULL, REG_OPTION_NON_VOLATILE, 
			KEY_ALL_ACCESS, NULL, &hkeyWabbit, &dwDisposition);
	
	if (dwDisposition == REG_CREATED_NEW_KEY)
		LoadRegistryDefaults(hkeyWabbit);
	
	strcpy(calcs[gslot].rom_path, (char *) QueryWabbitKey("rom_path"));
	calcs[gslot].SkinEnabled = QueryWabbitKey("skin");
	calcs[gslot].bCutout = QueryWabbitKey("cutout");
	calcs[gslot].Scale = QueryWabbitKey("screen_scale");
	calcs[gslot].FaceplateColor = QueryWabbitKey("faceplate_color");
	exit_save_state = QueryWabbitKey("exit_save_state");
	load_files_first = QueryWabbitKey("load_files_first");
	do_backups = QueryWabbitKey("do_backups");
	int num_entries = QueryWabbitKey("num_keys");
	//need to load accelerators
	// querywabbitkey doesnt work because its a REG_BINARY
	/*ACCEL buf[256];
	DWORD dwCount = sizeof(buf);
	LONG res = RegQueryValueEx(hkeyWabbit, "accelerators", NULL, NULL, (LPBYTE)buf, &dwCount);
	if (res == ERROR_SUCCESS){
		DestroyAcceleratorTable(haccelmain);
		haccelmain = CreateAcceleratorTable(buf, num_entries);
	}*/
	/*
	 * 		{"gif_path", 	REG_SZ,		"wabbitemu.gif"},
		{"gif_autosave",REG_DWORD,	0},
		{"gif_useinc",	REG_DWORD,	0},
		*/
	
	strcpy(gif_file_name, (char *) QueryWabbitKey("gif_path"));
	gif_autosave = QueryWabbitKey("gif_autosave");
	gif_use_increasing = QueryWabbitKey("gif_useinc");
	
	//RegCloseKey(hkeyWabbit);
	hkeyTarget = hkeyWabbit;
	RegCloseKey(hkeySoftware);

	calcs[gslot].bCutout = QueryWabbitKey("cutout");
	
	return S_OK;
}

void SaveWabbitKey(char *name, int type, void *value) {
	int len;

	if (type == REG_DWORD) {
		len = sizeof(DWORD);
	} else if (type == REG_SZ) {
		len = strlen((char *) value) + 1;
	}
	
	RegSetValueEx(hkeyTarget, name, 0, type, (LPBYTE) value, len);
	
}


HRESULT SaveRegistrySettings(void) {
	if (hkeyTarget)
		RegCloseKey(hkeyTarget);
	HKEY hkeyWabbit;
	HRESULT res;
	res = RegOpenKeyEx(HKEY_CURRENT_USER, "software\\Wabbitemu", 0, KEY_ALL_ACCESS, &hkeyWabbit);
	if (SUCCEEDED(res)) {
		hkeyTarget = hkeyWabbit;
		
		SaveWabbitKey("cutout", REG_DWORD, &calcs[gslot].bCutout);
		SaveWabbitKey("skin", REG_DWORD, &calcs[gslot].SkinEnabled);
		SaveWabbitKey("rom_path", REG_SZ, &calcs[gslot].rom_path);
		SaveWabbitKey("gif_path", REG_SZ, &gif_file_name);
		SaveWabbitKey("gif_autosave", REG_DWORD, &gif_autosave);
		SaveWabbitKey("gif_useinc", REG_DWORD, &gif_use_increasing);
		SaveWabbitKey("exit_save_state", REG_DWORD, &exit_save_state);
		SaveWabbitKey("load_files_first", REG_DWORD, &load_files_first);
		SaveWabbitKey("do_backups", REG_DWORD, &do_backups);

		SaveWabbitKey("faceplate_color", REG_DWORD, &calcs[gslot].FaceplateColor);
		SaveWabbitKey("custom_skin", REG_DWORD, &calcs[gslot].bCustomSkin);
		SaveWabbitKey("skin_path", REG_SZ, &calcs[gslot].skin_path);
		SaveWabbitKey("keymap_path", REG_SZ, &calcs[gslot].keymap_path);
		/*ACCEL buf[256];
		DWORD dwCount = sizeof(buf);
		DWORD dwType = NULL;
		LONG res = RegQueryValueEx(hkeyWabbit, "accelerators", NULL, &dwType,
			(LPBYTE)buf, &dwCount);
		if (res == ERROR_SUCCESS){
			DestroyAcceleratorTable(haccelmain);
			haccelmain = CreateAcceleratorTable(buf, dwCount);
		}*/
		
		SaveWabbitKey("shades", REG_DWORD, &calcs[gslot].cpu.pio.lcd->shades);
		SaveWabbitKey("lcd_mode", REG_DWORD, &calcs[gslot].cpu.pio.lcd->mode);
		DWORD steady = (DWORD) ( 1.0 / calcs[gslot].cpu.pio.lcd->steady_frame);
		SaveWabbitKey("lcd_freq", REG_DWORD, &steady);
		
		SaveWabbitKey("screen_scale", REG_DWORD, &calcs[gslot].Scale);

	}
	RegCloseKey(hkeyWabbit);
	return S_OK;
}