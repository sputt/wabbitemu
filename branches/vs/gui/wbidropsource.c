#ifndef WBDED_INC
#include "wbded.h"
#endif

#include "calc.h"

static ULONG STDMETHODCALLTYPE
ids_addref(WB_IDropSource *This)
{
	// increment object reference count
    return InterlockedIncrement(&This->m_lRefCount);
}

static ULONG STDMETHODCALLTYPE
ids_release (WB_IDropSource *This)
{
	LONG count = InterlockedDecrement(&This->m_lRefCount);

	if(count == 0) g_free(This);

	return count;
}

static HRESULT STDMETHODCALLTYPE
ids_queryinterface(	WB_IDropSource *This, 
					REFIID riid, 
					LPVOID *ppvObject)
{
	if (IsEqualGUID (riid, &IID_IUnknown) || IsEqualGUID (riid, &IID_IDropSource)) {
        ids_addref (This);
        *ppvObject = This;
        return S_OK;
    }
 	else if (IsEqualGUID(riid, &IID_IDropSourceNotify) && (This->pDropSourceNotify != NULL)) {
 		return This->pDropSourceNotify->lpVtbl->QueryInterface(&This->pDropSourceNotify, riid, ppvObject);
 	}
 	else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

void InvalidateDragWindow(IDataObject *pDataObject) {
	FORMATETC format = {0};
	
	format.cfFormat = RegisterClipboardFormat("DragWindow");
	format.dwAspect = DVASPECT_CONTENT;
	format.lindex = -1;
	format.tymed = TYMED_HGLOBAL;
	STGMEDIUM stg = {0};
	if(SUCCEEDED(pDataObject->lpVtbl->GetData(pDataObject, &format, &stg))) {
		if(stg.hGlobal) {
			#define WM_INVALIDATEDRAGIMAGE WM_USER + 3   // WM_USER + 2 works, too; WM_USER + 1 hides the drag image

			HWND hWndDragWindow = *((HWND*) GlobalLock(stg.hGlobal));

			SendMessage(hWndDragWindow, WM_INVALIDATEDRAGIMAGE, 0, 0);
			GlobalUnlock(stg.hGlobal);
		}
		ReleaseStgMedium(&stg);
	}	
}


static HRESULT STDMETHODCALLTYPE
ids_querycontinuedrag(	WB_IDropSource* This, 
						BOOL fEscapePressed, 
						DWORD grfKeyState )
{
	
	printf("Entering QueryDragContinue\n");
	InvalidateDragWindow((IDataObject *) This->pDataObject);
	
	// if the <Escape> key has been pressed since the last call, cancel the drop
	if (fEscapePressed == TRUE) {
		
		SendMessage(calcs[gslot].hwndLCD, WM_LBUTTONUP, 0, 0);
		return DRAGDROP_S_CANCEL;
	}
		
	// if the <LeftMouse> button has been released, then do the drop!
	if((grfKeyState & MK_LBUTTON) == 0) 
		return DRAGDROP_S_DROP;
		
	// continue with the drag-drop
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE
ids_givefeedback(	WB_IDropSource* This, 
					DWORD dwEffect ) 
{
	
	BOOL IsShowingLayered = FALSE;
	char *szEffect;
	switch (dwEffect & 0x7) {
	case 0:
		szEffect = "None";
		break;
	case 1:
		szEffect = "Copy";
		break;
	case 2:
		szEffect = "Move";
		break;
	case 4:
		szEffect = "Link";
		break;
	default:
		szEffect = "Unknown";
		break;
	}
	
	printf("[%d] givefeedback: %s\n", clock(), szEffect);
	FORMATETC fmtetc = {0};
	STGMEDIUM stgmed = {0};
	
	fmtetc.cfFormat = (CLIPFORMAT) RegisterClipboardFormat("IsShowingLayered");
	fmtetc.dwAspect = DVASPECT_CONTENT;
	fmtetc.lindex = -1;
	fmtetc.tymed = TYMED_HGLOBAL;
	
	stgmed.pUnkForRelease = NULL;
	stgmed.tymed = TYMED_HGLOBAL;
	
	if (SUCCEEDED(IDataObject_GetData(This->pDataObject, &fmtetc, &stgmed))) {
		BOOL* pB = (BOOL*) GlobalLock(stgmed.hGlobal);
		IsShowingLayered = *pB;
		printf("IsShowingLayered = %d\n", IsShowingLayered);
		GlobalUnlock(stgmed.hGlobal);
		
		ReleaseStgMedium(&stgmed);
		
		
		
		fmtetc.cfFormat = (CLIPFORMAT) RegisterClipboardFormat("IsShowingText");
		fmtetc.dwAspect = DVASPECT_CONTENT;
		fmtetc.lindex = -1;
		fmtetc.tymed = TYMED_HGLOBAL;
		
		stgmed.tymed = TYMED_HGLOBAL;
		stgmed.hGlobal = GlobalAlloc(GHND, sizeof(BOOL));
		if (SUCCEEDED(IDataObject_GetData(This->pDataObject, &fmtetc, &stgmed))) {
			BOOL* pB = (BOOL*) GlobalLock(stgmed.hGlobal);
			printf("IsShowingText = %d\n", *pB);
			GlobalUnlock(stgmed.hGlobal);
			ReleaseStgMedium(&stgmed);
		}
		
		
		
		fmtetc.cfFormat = (CLIPFORMAT) RegisterClipboardFormat("DisableDragText");
		fmtetc.dwAspect = DVASPECT_CONTENT;
		fmtetc.lindex = -1;
		fmtetc.tymed = TYMED_HGLOBAL;
		
		stgmed.tymed = TYMED_HGLOBAL;
		stgmed.hGlobal = GlobalAlloc(GHND, sizeof(BOOL));
		if (SUCCEEDED(IDataObject_GetData(This->pDataObject, &fmtetc, &stgmed))) {
			BOOL* pB = (BOOL*) GlobalLock(stgmed.hGlobal);
			printf("DisableDragText = %d\n", *pB);
			GlobalUnlock(stgmed.hGlobal);
			ReleaseStgMedium(&stgmed);
		}
		
		InvalidateDragWindow((IDataObject *) This->pDataObject);
	} else {
		printf("Failed to even get IsShowingLayered\n");
	}
	
	fmtetc.cfFormat = (CLIPFORMAT) RegisterClipboardFormat("DropDescription");
	fmtetc.dwAspect = DVASPECT_CONTENT;
	fmtetc.lindex = -1;
	fmtetc.tymed = TYMED_HGLOBAL;
	
	stgmed.pUnkForRelease = NULL;
	stgmed.tymed = TYMED_HGLOBAL;
	
	if (SUCCEEDED(IDataObject_GetData(This->pDataObject, &fmtetc, &stgmed))) {
		printf("Succeeded in getting DROPDESCRIPTION\n");
		DROPDESCRIPTION *pdd = (DROPDESCRIPTION *) GlobalLock(stgmed.hGlobal);
		printf("image type: %d\n", pdd->type);
		wprintf(L"text: %s\n", pdd->szMessage);
		GlobalUnlock(stgmed.hGlobal);
	}
	//fmtetc.cfFormat = (CLIPFORMAT) RegisterClipboardFormat("IsShow")
	
	if (IsShowingLayered) {
		SetCursor((HCURSOR) LoadImage(NULL, MAKEINTRESOURCE(OCR_NORMAL), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_SHARED));
		return S_OK;
	} else {
		return DRAGDROP_S_USEDEFAULTCURSORS;
	}
}

static WB_IDropSourceVtbl ids_vtbl = {
	ids_queryinterface,
	ids_addref,
	ids_release,
	ids_querycontinuedrag,
	ids_givefeedback
};


WB_IDropSourceNotify *WB_IDropSourceNotify_new();

WB_IDropSource * WB_IDropSource_new (void) {
	WB_IDropSource *result = g_new0(WB_IDropSource, 1);

	if (result == NULL) return NULL;
	
	result->ids.lpVtbl = (IDropSourceVtbl*) &ids_vtbl;
	result->m_lRefCount = 1;
	
	//result->pDropSourceNotify = (IDropSourceNotify *) WB_IDropSourceNotify_new();

	return result;
}

void SetDropSourceNotifyDataObject(WB_IDropSourceNotify *pDropSourceNotify, WB_IDataObject *pDataObject);

void SetDropSourceDataObject(WB_IDropSource *pDropSource, WB_IDataObject *pDataObject) {
	pDropSource->pDataObject = (IDataObject *) pDataObject;
	if (pDropSource->pDropSourceNotify != NULL)
		SetDropSourceNotifyDataObject((WB_IDropSourceNotify *) pDropSource->pDropSourceNotify, pDataObject); 
}

HRESULT CreateDropSource(WB_IDropSource **ppDropSource)
{
	if(ppDropSource == NULL)
		return E_INVALIDARG;

	*ppDropSource = WB_IDropSource_new();

	return (*ppDropSource) ? S_OK : E_OUTOFMEMORY;
}
