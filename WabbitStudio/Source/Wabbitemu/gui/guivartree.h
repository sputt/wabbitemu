#ifndef GUIVARTREE_H
#define GUIVARTREE_H

#include "link.h"
#include "gui.h"


typedef struct {
	int model;
	HTREEITEM hRoot;
	union {
		struct {
			HTREEITEM hAppVar;
			HTREEITEM hEquation;
			HTREEITEM hGDB;
			HTREEITEM hList;
			HTREEITEM hMatrix;
			HTREEITEM hNumber;
			HTREEITEM hPic;
			HTREEITEM hProgram;
			HTREEITEM hString;
			HTREEITEM hGroup;
			HTREEITEM hApplication;
		};
		HTREEITEM hTypes[11];
	};
	int count;
	map<HTREEITEM, apphdr_t *> *apps;
	map<HTREEITEM, symbol83P_t *> *symbols;

	applist_t applist;
	symlist_t sym;
} VARTREEVIEW_t;

enum { 
	TI_ICON_BLANK,
	TI_ICON_84PSE,
	TI_ICON_ARCHIVE,
	TI_ICON_APPVAR,
	TI_ICON_DEVICE_SETTINGS,
	TI_ICON_EQUATIONS,
	TI_ICON_GDB,
	TI_ICON_LIST,
	TI_ICON_MATRIX,
	TI_ICON_NUMBER,
	TI_ICON_PROGRAM,
	TI_ICON_STRING,
	TI_ICON_PIC,
	TI_ICON_APP,
	TI_ICON_GROUP,
	TI_ICON_UKNOWN,
	TI_ICON_FILE,
	TI_ICON_FILE_RAM,
	TI_ICON_FILE_ARC
};


BOOL VarTreeOpen(BOOL refresh);
HWND CreateVarTreeList(HWND hwndParent, LPMAINWINDOW lpMainWindow);
void DeleteChildren(HWND hwnd, HTREEITEM parent);
void RefreshTreeView(BOOL New);
INT_PTR CALLBACK DlgVarlist(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

#endif
