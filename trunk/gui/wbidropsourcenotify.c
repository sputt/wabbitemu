#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "wbded.h"

const IID IID_IDropSourceNotify = {0x0000012B, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

WB_IDropSourceNotify *WB_IDropSourceNotify_new();
void WB_IDropSourceNotify_delete(WB_IDropSourceNotify *pDropSourceNotify); 

static HRESULT __stdcall idsn_queryinterface(WB_IDropSourceNotify *This, REFIID riid, PVOID *ppvObject);
static ULONG __stdcall idsn_addref(WB_IDropSourceNotify *This);
static ULONG __stdcall idsn_release(WB_IDropSourceNotify *This);
static HRESULT __stdcall idsn_dragentertarget(WB_IDropSourceNotify *This, HWND hwndTarget);
static HRESULT __stdcall idsn_dragleavetarget(WB_IDropSourceNotify *This);

static WB_IDropSourceNotifyVtbl idsn_vtbl = {
		idsn_queryinterface,
		idsn_addref,
		idsn_release,
		idsn_dragentertarget,
		idsn_dragleavetarget
};

/*
 * IUnknown Methods
 */
static HRESULT __stdcall idsn_queryinterface(WB_IDropSourceNotify *This, REFIID riid, LPVOID *ppvObject) {
	if (IsEqualGUID (riid, &IID_IUnknown) || IsEqualGUID (riid, &IID_IDropSourceNotify))
    {
        idsn_addref (This);
        *ppvObject = This;
        return S_OK;
    }
 	else
    {
        *ppvObject = 0;
        return E_NOINTERFACE;
    }
}

static ULONG __stdcall idsn_addref(WB_IDropSourceNotify *This) {
    return InterlockedIncrement(&This->m_lRefCount);
}

static ULONG __stdcall idsn_release(WB_IDropSourceNotify *This) {
	LONG count = InterlockedDecrement(&This->m_lRefCount);
	if(count == 0) 
		WB_IDropSourceNotify_delete(This);

	return count;
}

/*
 * IDropSourceNotify Methods
 */
static HRESULT __stdcall idsn_dragentertarget(WB_IDropSourceNotify *This, HWND hwndTarget) {
	FORMATETC fmtetc = {0};
	STGMEDIUM stgmed = {0};
	
	if (This->pDataObject == NULL)
		return S_OK;
	
	printf("We are enterting DragEnterTarget!\n");
	
	
	fmtetc.cfFormat = (CLIPFORMAT) RegisterClipboardFormat("IsShowingText");
	fmtetc.dwAspect = DVASPECT_CONTENT;
	fmtetc.lindex = -1;
	fmtetc.tymed = TYMED_HGLOBAL;
	
	stgmed.tymed = TYMED_HGLOBAL;
	stgmed.hGlobal = GlobalAlloc(GHND, sizeof(BOOL));
	if(stgmed.hGlobal) {
		BOOL* pB = (BOOL*) GlobalLock(stgmed.hGlobal);
		*pB = FALSE;
		GlobalUnlock(stgmed.hGlobal);
		
		//IDataObject_SetData((IDataObject *) This->pDataObject, &fmtetc, &stgmed, TRUE); 
		This->pDataObject->lpVtbl->SetData((IDataObject *) This->pDataObject, &fmtetc, &stgmed, TRUE);
	}
	
	fmtetc.cfFormat = (CLIPFORMAT) RegisterClipboardFormat("DisableDragText");
	fmtetc.dwAspect = DVASPECT_CONTENT;
	fmtetc.lindex = -1;
	fmtetc.tymed = TYMED_HGLOBAL;
	
	stgmed.tymed = TYMED_HGLOBAL;
	stgmed.hGlobal = GlobalAlloc(GHND, sizeof(BOOL));
	if(stgmed.hGlobal) {
		BOOL* pB = (BOOL*) GlobalLock(stgmed.hGlobal);
		*pB = TRUE;
		GlobalUnlock(stgmed.hGlobal);
		
		IDataObject_SetData((IDataObject *) This->pDataObject, &fmtetc, &stgmed, TRUE); 
	}
	
	
	fmtetc.cfFormat = (CLIPFORMAT) RegisterClipboardFormat("DropDescription");
	fmtetc.dwAspect = DVASPECT_CONTENT;
	fmtetc.lindex = -1;
	fmtetc.tymed = TYMED_HGLOBAL;
	
	stgmed.tymed = TYMED_HGLOBAL;
	stgmed.hGlobal = GlobalAlloc(GHND, sizeof(DROPDESCRIPTION));
	if (stgmed.hGlobal) {
		DROPDESCRIPTION *pdd = (DROPDESCRIPTION *) GlobalLock(stgmed.hGlobal);
		pdd->type = DROPIMAGE_INVALID;
		GlobalUnlock(stgmed.hGlobal);
		
		IDataObject_SetData((IDataObject *) This->pDataObject, &fmtetc, &stgmed, TRUE); 
	}
	
	
	return S_OK;
}

static HRESULT __stdcall idsn_dragleavetarget(WB_IDropSourceNotify *This) {
	return S_OK;
}

/*
 * WB_IDropSourceNotify constructor
 */
WB_IDropSourceNotify *WB_IDropSourceNotify_new() {
	WB_IDropSourceNotify *pDropSourceNotify = (WB_IDropSourceNotify *) malloc(sizeof(WB_IDropSourceNotify));
	
	if (pDropSourceNotify) {
		pDropSourceNotify->idsn.lpVtbl = (IDropSourceNotifyVtbl*) &idsn_vtbl;
		pDropSourceNotify->m_lRefCount = 1;
	}
	
	return pDropSourceNotify;
}

void WB_IDropSourceNotify_delete(WB_IDropSourceNotify *pDropSourceNotify) {
	if (pDropSourceNotify->pDataObject) {
		IDataObject_Release((IDataObject *) pDropSourceNotify->pDataObject);
	}
	free(pDropSourceNotify);
}


void SetDropSourceNotifyDataObject(WB_IDropSourceNotify *pDropSourceNotify, WB_IDataObject *pDataObject) {
	IDataObject_QueryInterface((IDataObject *) pDataObject, &IID_IDataObject, (PVOID *) &pDropSourceNotify->pDataObject);
}
