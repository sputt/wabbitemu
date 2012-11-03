#ifndef GUI_H
#define GUI_H

#include "calc.h"

#define SKIN_WIDTH	350
#define SKIN_HEIGHT	725

#define WABBITVER		_T("1.5")

#define g_szWebPage		_T("http://wabbit.codeplex.com")
#ifdef _WIN64
#define g_szDownload	_T("http://buckeyedude.zapto.org/Revsoft/Wabbitemu/x64/Wabbitemu.exe")
#else
#define g_szDownload	_T("http://buckeyedude.zapto.org/Revsoft/Wabbitemu/Wabbitemu.exe")
#endif
#define g_szVersionFile _T("http://buckeyedude.zapto.org/Revsoft/Wabbitemu/Version.txt")
#define g_szWhatsNewFile _T("http://buckeyedude.zapto.org/Revsoft/Wabbitemu/WhatsNew.txt")

#define g_szAppName 	_T("z80")
#define g_szDebugName 	_T("debug")
#define g_szDisasmName 	_T("disasm")
#define g_szRegName 	_T("reg")
#define g_szMemName		_T("mem")
#define g_szWatchName	_T("watchpoints")
#define g_szLinkName	_T("link")
#define g_szLCDName 	_T("wabbitlcd")
#define g_szWabbitName 	_T("wabbit")
#define g_szToolbar		_T("wabtool")
#define g_szSubbar		_T("wabsub")
#define g_szDetachedName 	_T("wabbitdetached")
#define g_szDetachedLCDName _T("wabbitdetachedlcd")
#define g_szSmallButtonsName _T("WABBITSMALLBUTTON")

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

HWND gui_debug(calc_t *lpCalc);
int gui_frame(calc_t *lpCalc);
void gui_draw(calc_t *lpCalc);
void GetFileCurrentVersionString(TCHAR *buf, size_t len);
extern HACCEL haccelmain;

void RegisterWindowClasses(void);
TCHAR *GetWhatsNewText();
VOID CALLBACK TimerProc(HWND hwnd, UINT Message, UINT_PTR idEvent, DWORD dwTimer);

#endif
