#ifndef GUI_H
#define GUI_H

#define SKIN_WIDTH	314
#define SKIN_HEIGHT	688

#define WABBITVER		"1.1"

#define g_szWebPage		"http://www.revsoft.org/emu"

#define g_szAppName 	"z80"
#define g_szDebugName 	"debug"
#define g_szDisasmName 	"disasm"
#define g_szRegName 	"reg"
#define g_szMemName		"mem"
#define g_szLinkName	"link"
#define g_szLCDName 	"wabbitlcd"
#define g_szWabbitName 	"wabbit"
#define g_szToolbar		"wabtool"
#define g_szSubbar		"wabsub"

#define ID_DISASM 	0
#define ID_REG 		1
#define ID_MEM		3
#define ID_STACK	4
#define ID_WABBIT	5
#define ID_DISASMSIZE 6
#define ID_SIZE 6
#define ID_TOOLBAR 7
#define ID_SUBBAR 8

#define ID_STATUSBAR	50


#define REG_SWAPID		80
#define REG_CHK_Z		88
#define REG_CHK_C		89
#define REG_CHK_S		90
#define REG_CHK_PV		91
#define REG_CHK_HC		92
#define REG_CHK_N		93
#define REG_CHK_HALT	94
#define REG_CHK_IFF1	95
#define REG_CHK_IFF2	96

#define DB_UPDATE		1

#define REG_UPDATE		1


#define MEM_BYTE		1
#define MEM_WORD		2
#define MEM_DWORD		4

int gui_debug(int);
int gui_frame(int);
int gui_frame_update(int);
void gui_draw();
void Set_LCD_Loc(HWND);
int __stdcall AboutDialogProc(HWND, UINT, WPARAM, LPARAM);

#endif
