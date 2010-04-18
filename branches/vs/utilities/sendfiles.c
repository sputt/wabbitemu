#include <stdlib.h>
#include <string.h>

#ifdef WINVER
#include <windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include "sound.h"
#endif

#include "calc.h"
#include "state.h"
#include "var.h"
#include "link.h"
#include "sendfiles.h"




static int SlotSave =-1;

typedef struct SENDFILES {
	char* FileNames;
	int ram;
//	HANDLE hdlSend;
} SENDFILES_t;


int SizeofFileList(char* FileNames) {
	int i;
	if (FileNames == NULL) return 0;
	for(i = 0; FileNames[i]!=0 || FileNames[i+1]!=0; i++);
	return i+2;
}

char* AppendName(char* FileNames, char* fn) {
	int length;
	char* pnt;
	int i;
	length = strlen(fn);
	if (FileNames == NULL) {
		FileNames = (char *) malloc(length+2);
		memset(FileNames,0,length+2);
		pnt = FileNames;
	} else {
		for(i = 0; FileNames[i]!=0 || FileNames[i+1]!=0; i++);
		i++;
		FileNames = (char *) realloc(FileNames,i+length+2);
		pnt = FileNames+i;
		memset(pnt,0,length+2);
	}
	strcpy(pnt,fn);
	return FileNames;
}
	
__declspec (dllexport) 
void SendFile( char* FileName , int ram ) {
	TIFILE_t *var = importvar(FileName);
	LINK_ERR result;
	if (var != NULL) {
		switch(var->type) {
			case BACKUP_TYPE:
			case VAR_TYPE:
			case FLASH_TYPE:
				calcs[gslot].SendSize = var->length;
				calcs[gslot].cpu.pio.link->vlink_send = 0;
				result = link_send_var(&calcs[gslot].cpu, var, (SEND_FLAG) ram);
				#ifdef WINVER
				switch (result) {
				case LERR_MEM:
					switch (ram) {
					case SEND_RAM:
						MessageBox(NULL, "Not enough free RAM on calculator","Error",MB_OK);
						break;
					case SEND_ARC:
						MessageBox(NULL, "Not enough free Archive on calculator","Error",MB_OK);
						break;
					default:
						MessageBox(NULL, "Not enough free memory on calculator","Error",MB_OK);
						break;
					}
					break;
				case LERR_TIMEOUT:
					MessageBox(NULL, "Link timed out","Error",MB_OK);
					break;
				case LERR_FORCELOAD:
					MessageBox(NULL, "Error force loading an application", "Error", MB_OK);
					break;
				case LERR_CHKSUM:
					MessageBox(NULL, "Link checksum was not correct", "Error", MB_OK);
					break;
				case LERR_MODEL:
					MessageBox(NULL, "Calculator model not correct for this type of file", "Error", MB_OK);
					break;
				case LERR_NOTINIT:
					MessageBox(NULL, "Virtual link was not initialized", "Error", MB_OK);
					break;
				case LERR_LINK:
					MessageBox(NULL, "Virtual link error", "Error", MB_OK);
					break;
				case LERR_FILE:
					MessageBox(NULL, "The file was unable to be sent because it is corrupt", "Error", MB_OK);
					break;
				}
				
				#endif
				if (var->type == FLASH_TYPE) {
					// Rebuild the applist
					state_build_applist(&calcs[gslot].cpu, &calcs[gslot].applist);
					
					int i;
					for (i = 0; i < calcs[gslot].applist.count; i++) {
						if (strncmp((char *) var->flash->name, calcs[gslot].applist.apps[i].name, 8) == 0) {
							calcs[gslot].last_transferred_app = &calcs[gslot].applist.apps[i];
							break;
						}
					}
				}
				break;
			case ROM_TYPE:
			case SAV_TYPE:
				FreeTiFile(var);
				var = NULL;
				rom_load(gslot,FileName);
				break;
			case LABEL_TYPE:
			{
				strcpy(calcs[gslot].labelfn,FileName);
				printf("loading label file for slot %d: %s\n", gslot, FileName);
				VoidLabels(gslot);
				labels_app_load(gslot, calcs[gslot].labelfn);

				break;
			}
			case BREAKPOINT_TYPE:
			{
				
			}
			default:
				#ifdef WINVER
				MessageBox(NULL, "The file was an invalid or unspecified type","Error",MB_OK);
				#endif
				break;
		}
	if (var) FreeTiFile(var);
	} else {
		#ifdef WINVER
		MessageBox(NULL, "Invalid file format","Error",MB_OK);
		#endif
	}
}

#ifdef WINVER
extern HINSTANCE g_hInst;
HWND hwndSend;
static char *current_file_sending;
#endif

void SendFiles( char* FileNames , int ram ) {
	int i;
	int modelsave;
	unsigned char* fn = (unsigned char *)  FileNames;
	calcs[SlotSave].send = TRUE;
	calcs[SlotSave].running = FALSE;
	calcs[SlotSave].CurrentFile = 0;
	calcs[SlotSave].FileCnt = 0;
	i=0;
	while(FileNames[i]!=0) {
		for(;FileNames[i]!=0;i++);
		i++;
		calcs[SlotSave].FileCnt++;
	}

	while (fn[0]!=0) {
		modelsave = calcs[SlotSave].model;
		calcs[SlotSave].CurrentFile++;
		current_file_sending = (char *) fn;
		SendFile((char *) fn,ram);
#ifdef WINVER
		SendMessage(hwndSend, WM_USER, 0, 0);
#endif
		for(;fn[0]!=0;fn++);
		fn++;
	}
	//if (calcs[SlotSave].model == TI_82 && modelsave == calcs[SlotSave].model) end82send(SlotSave);
	calcs[SlotSave].running = TRUE;
	free(FileNames);
	calcs[SlotSave].send = FALSE;
}

#ifdef WINVER



static int CALLBACK EnumFontFamExProc(
  ENUMLOGFONTEX *lpelfe,    // logical-font data
  NEWTEXTMETRICEX *lpntme,  // physical-font data
  DWORD FontType,           // type of font
  LPARAM lParam             // application-defined data
) {
	LOGFONT *lplf = &lpelfe->elfLogFont;
	lplf->lfHeight = -MulDiv(9, GetDeviceCaps(GetDC(NULL), LOGPIXELSY), 72);
	lplf->lfWidth = 0;	
	*((HFONT *) lParam) = CreateFontIndirect(lplf);
	return 0;
}


static LRESULT CALLBACK SendProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HFONT hfontSegoe;
	static HWND hwndProgress;
	static TEXTMETRIC tm;
	switch (Message)
	{
	case WM_CREATE:
	{
		LOGFONT lfSegoe;
		memset(&lfSegoe, 0, sizeof(LOGFONT));
		strcpy(lfSegoe.lfFaceName, "Segoe UI");
		
		if (EnumFontFamiliesEx(GetDC(NULL), &lfSegoe, (FONTENUMPROC) EnumFontFamExProc, (LPARAM) &hfontSegoe, 0) != 0) {
			strcpy(lfSegoe.lfFaceName, "Tahoma");
			EnumFontFamiliesEx(GetDC(NULL), &lfSegoe, (FONTENUMPROC) EnumFontFamExProc, (LPARAM) &hfontSegoe, 0);
		}
		
		SelectObject(GetDC(hwnd), hfontSegoe);
		GetTextMetrics(GetDC(hwnd), &tm);
		
		hwndProgress = CreateWindowEx(
				0,
				PROGRESS_CLASS,
				NULL,
				WS_CHILD | WS_VISIBLE,
				tm.tmAveCharWidth*2, tm.tmHeight * 4, 1, 1,
				hwnd, (HMENU) 0, g_hInst, NULL
		);
		
		return 0;
	}
	case WM_SIZE:
	{
		RECT rc;
		GetClientRect(hwnd, &rc);
		SetWindowPos(hwndProgress, NULL, 0, 0, rc.right - rc.left - tm.tmAveCharWidth*4, tm.tmHeight*3/2, SWP_NOMOVE | SWP_NOZORDER);
		
		rc.top = 0;
		rc.bottom = tm.tmHeight * 6;
		AdjustWindowRect(&rc, WS_SIZEBOX | WS_POPUP, FALSE);
		SetWindowPos(hwnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
		
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
				
		SetBkMode(hdc, TRANSPARENT);
		SelectObject(hdc, hfontSegoe);
		
		char szFile[256];
		sprintf(szFile, "Sending file %d of %d", calcs[SlotSave].CurrentFile, calcs[SlotSave].FileCnt);
		RECT r;
		GetClientRect(hwnd, &r);
		r.bottom = tm.tmHeight*5/2 + tm.tmHeight;
		FillRect(hdc, &r, GetStockBrush(WHITE_BRUSH));
		
		SelectObject(hdc, GetStockObject(DC_PEN));
		SetDCPenColor(hdc, RGB(223,223,233));
		MoveToEx(hdc, 0, r.bottom-1, NULL);
		LineTo(hdc, r.right, r.bottom-1);
		
		r.left = tm.tmAveCharWidth*2;
		r.top = tm.tmHeight*3/4;
		r.right -= tm.tmAveCharWidth*2;
		SetTextColor(hdc, RGB(0, 0, 0));
		DrawText(hdc, szFile, -1, &r, DT_SINGLELINE);
		
		
		OffsetRect(&r, 0, tm.tmHeight);
		r.left += tm.tmAveCharWidth;
		SetTextColor(hdc, RGB(90, 90, 90));

		DrawText(hdc, current_file_sending, -1, &r, DT_SINGLELINE | DT_PATH_ELLIPSIS);
		

		
		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_USER:
	{
		// Update the progress bar
		SendMessage(hwndProgress, PBM_SETSTEP, 1, 0);
		SendMessage(hwndProgress, PBM_SETRANGE, 0, MAKELPARAM(0, calcs[SlotSave].cpu.pio.link->vlink_size/4)); 
		SendMessage(hwndProgress, PBM_SETPOS, calcs[SlotSave].cpu.pio.link->vlink_send/4, 0);
		
		InvalidateRect(hwnd, NULL, FALSE);
		UpdateWindow(hwnd);
		return 0;
	}
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
}

DWORD WINAPI ThreadSendStart( LPVOID lpParam ) {
	SENDFILES_t* sf = (SENDFILES_t *) lpParam;
	int save = 0;
	
	if (calcs[SlotSave].audio->enabled) {
		save = 1;
		pausesound();
	}

	
	SendFiles(sf->FileNames,sf->ram);
	if (save==1) playsound();
	free(sf);	
	SlotSave = -1;
	
	PostMessage(hwndSend, WM_CLOSE, 0, 0);
	
    return 0; 
} 


void ThreadSend( char* FileNames , int ram ) {
	static HANDLE hdlSend = NULL;
	SENDFILES_t* sf;

	if (FileNames == NULL) return;

	if (SlotSave == -1) {
		SlotSave = gslot;
	} else {
		MessageBox(NULL, "Currently sending files please wait...","Error",MB_OK);
		return;
	}
	
	if (calcs[SlotSave].send == TRUE)  {
		MessageBox(NULL, "Currently sending files please wait...","Error",MB_OK);
		return;
	}

	if (hdlSend != NULL) {
		CloseHandle(hdlSend);
		hdlSend = NULL;
	}

	sf = (SENDFILES_t *) malloc(sizeof(SENDFILES_t));
	if (sf != NULL) {
		sf->FileNames	= FileNames;
		sf->ram			= ram;
	}
//	sf->hdlSend		= NULL;

	WNDCLASSEX wcx;
	ZeroMemory(&wcx, sizeof(wcx));
	
	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_DBLCLKS;
	wcx.lpszClassName = "WabbitSendClass";
	wcx.lpfnWndProc = SendProc;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = (HBRUSH) (COLOR_BTNFACE+1);
	RegisterClassEx(&wcx);
	
	
	RECT r;
	GetWindowRect(calcs[SlotSave].hwndLCD, &r);
	
	DWORD SendWidth = (r.right - r.left) * 9 / 10;
	DWORD SendHeight = 90; //10 * HIWORD(GetDialogBaseUnits());
	
	hwndSend = CreateWindowEx(
			0,
			"WabbitSendClass",
			NULL,
			WS_SIZEBOX | WS_POPUP | WS_VISIBLE,
			r.left+(r.right - r.left - SendWidth)/2, r.top+(r.bottom - r.top - SendHeight)/2, SendWidth, SendHeight,
			calcs[SlotSave].hwndLCD, NULL, g_hInst, 0
	);
	
    hdlSend = CreateThread(NULL,0,ThreadSendStart, sf, 0, NULL);  
    
    if ( hdlSend  == NULL) {
		SlotSave = -1;
		free(sf);
		free(FileNames);
		MessageBox(NULL, "Could not create thread to send","Error",MB_OK);
		return;
	}
	return;
}
#endif

void NoThreadSend(const char* FileNames, int ram) {
	if (SlotSave == -1) {
		SlotSave = gslot;
	} else {
		// error;
	}
	
	SendFiles((char*) FileNames, ram);
}



	
	
	
