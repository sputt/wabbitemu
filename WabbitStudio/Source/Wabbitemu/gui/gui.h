#ifndef GUI_H
#define GUI_H

#include "calc.h"

#define SKIN_WIDTH	314
#define SKIN_HEIGHT	688

#define WABBITVER		_T("1.5")

#define g_szWebPage		_T("http://wabbit.codeplex.com")//"http://www.revsoft.org/emu"
#ifdef _WIN64
#define g_szDownload	_T("http://group.revsoft.org/wabbitemu64.exe")//"http://wabbit.codeplex.com/releases/view/44625#DownloadId=122222"
#else
#define g_szDownload	_T("http://group.revsoft.org/wabbitemu.exe")//"http://wabbit.codeplex.com/releases/view/44625#DownloadId=122222"
#endif

#define g_szAppName 	_T("z80")
#define g_szDebugName 	_T("debug")
#define g_szDisasmName 	_T("disasm")
#define g_szRegName 	_T("reg")
#define g_szMemName		_T("mem")
#define g_szLinkName	_T("link")
#define g_szLCDName 	_T("wabbitlcd")
#define g_szWabbitName 	_T("wabbit")
#define g_szToolbar		_T("wabtool")
#define g_szSubbar		_T("wabsub")

#define ID_DISASM 	0
#define ID_REG 		1
#define ID_MEMTAB   2
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

int gui_debug(calc_t *lpCalc);
int gui_frame(calc_t *lpCalc);
int gui_frame_update(calc_t *lpCalc);
void gui_draw(calc_t *lpCalc);
INT_PTR CALLBACK AboutDialogProc(HWND, UINT, WPARAM, LPARAM);
TCHAR* LoadRomIntialDialog();
#ifdef USE_DIRECTX
#include <d3d9.h>
extern IDirect3DDevice9 *pd3dDevice; // Direct3D Rendering Device 
#endif
static HACCEL haccelmain;

void RegisterWindowClasses(void);
VOID CALLBACK TimerProc(HWND hwnd, UINT Message, UINT_PTR idEvent, DWORD dwTimer);

#endif
