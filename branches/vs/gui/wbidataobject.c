#include "wbded.h"

HRESULT CreateEnumFormatEtc(UINT nNumFormats, LPDATAENTRY pde, LPENUMFORMATETC *);

HGLOBAL GlobalClone(HGLOBAL hglobIn) {
	HGLOBAL hglobOut = NULL;
	
	LPVOID pvIn = GlobalLock(hglobIn);
	if (pvIn) {
		SIZE_T cb = GlobalSize(hglobIn);
		hglobOut = GlobalAlloc(GMEM_FIXED, cb);
		if (hglobOut) {
			CopyMemory(hglobOut, pvIn, cb);
		}
		GlobalUnlock(hglobIn);
	}
	
	return hglobOut;
}

HRESULT FindFORMATETC(	WB_IDataObject *This, 
						FORMATETC *pfe, 
						LPDATAENTRY *ppde, 
						BOOL fAdd ) 
{
	*ppde = NULL;
	
	if (pfe->ptd != NULL) return DV_E_DVTARGETDEVICE;
	
	int ide;
	for (ide = 0; ide < This->m_cde; ide++) {
		if (	This->m_rgde[ide].fe.cfFormat == pfe->cfFormat &&
				This->m_rgde[ide].fe.dwAspect == pfe->dwAspect &&
				This->m_rgde[ide].fe.lindex == pfe->lindex) {
			if (fAdd || (This->m_rgde[ide].fe.tymed & pfe->tymed) ||
					(This->m_rgde[ide].fe.tymed == TYMED_NULL && pfe->tymed == TYMED_NULL)) {
				*ppde = &This->m_rgde[ide];
				return S_OK;
			} else {
				return DV_E_TYMED;
			}
		}
	}
	
	if (!fAdd) return DV_E_FORMATETC;
	
	LPDATAENTRY pdeT = (LPDATAENTRY) realloc(This->m_rgde, 
								sizeof(DATAENTRY) * (This->m_cde+1));
	
	if (pdeT) {
		This->m_rgde = pdeT;
		This->m_cde++;
		This->m_rgde[ide].fe = *pfe;
		ZeroMemory(&pdeT[ide].stgm, sizeof(STGMEDIUM));
		*ppde = &This->m_rgde[ide];
		return S_OK;
	} else {
		return E_OUTOFMEMORY;
	}
}

HRESULT AddRefStgMedium(	WB_IDataObject *This, 
							STGMEDIUM *pstgmIn, 
							STGMEDIUM *pstgmOut, 
							BOOL fCopyIn ) 
{
	HRESULT hres = S_OK;
	STGMEDIUM stgmOut = *pstgmIn;
	
	if (pstgmIn->pUnkForRelease == NULL &&
			!(pstgmIn->tymed & (TYMED_ISTREAM | TYMED_ISTORAGE))) {
		if (fCopyIn) {
			if (pstgmIn->tymed == TYMED_HGLOBAL) {
				stgmOut.hGlobal = GlobalClone(pstgmIn->hGlobal);
				if (!stgmOut.hGlobal) {
					hres = E_OUTOFMEMORY;
				}
			} else {
				hres = DV_E_TYMED;
			}
		} else {
			stgmOut.pUnkForRelease = (IUnknown*) This;
		}
	}
	
	if (SUCCEEDED(hres)) {
		switch (stgmOut.tymed) {
		case TYMED_ISTREAM:
			stgmOut.pstm->lpVtbl->AddRef(stgmOut.pstm);
			break;
		case TYMED_ISTORAGE:
			stgmOut.pstg->lpVtbl->AddRef(stgmOut.pstg);
			break;
		}
		if (stgmOut.pUnkForRelease) {
			stgmOut.pUnkForRelease->lpVtbl->AddRef(stgmOut.pUnkForRelease);
		}
		
		*pstgmOut = stgmOut;
	}
	
	return hres;
}

IUnknown *GetCanonicalIUnknown(IUnknown *punk) {
	IUnknown *punkCanonical;
	
	if (punk && SUCCEEDED(punk->lpVtbl->QueryInterface(punk, (REFIID) &IID_IUnknown, 
							(LPVOID*) &punkCanonical))) {
		punkCanonical->lpVtbl->Release(punkCanonical);
	} else {
		punkCanonical = punk;
	}
	return punkCanonical;
}

static ULONG STDMETHODCALLTYPE 
ido_addref(WB_IDataObject *This) 
{
	return InterlockedIncrement(&This->m_lRefCount);
}

static HRESULT STDMETHODCALLTYPE 
ido_queryinterface(	WB_IDataObject *This, 
					REFIID riid, 
					LPVOID *ppvObject ) 
{
	*ppvObject = NULL;

	if (IsEqualGUID(riid, &IID_IUnknown)) {
		ido_addref(This);
		*ppvObject = This;
		return S_OK;
	} 
	else if (IsEqualGUID(riid, &IID_IDataObject))
    {
		ido_addref(This);
		*ppvObject = This;
		return S_OK;
    }
	else {
		return E_NOINTERFACE;
    }
}

static ULONG STDMETHODCALLTYPE 
ido_release(WB_IDataObject *This) 
{
	LONG count = InterlockedDecrement(&This->m_lRefCount);

	if(count == 0) g_free(This);

	return count;
}

static HRESULT STDMETHODCALLTYPE 
ido_getdata(	WB_IDataObject *This, 
				LPFORMATETC pfe, 
				LPSTGMEDIUM pstgm ) 
{
	LPDATAENTRY pde;

	HRESULT hres = FindFORMATETC(This, pfe, &pde, FALSE);
	if (SUCCEEDED(hres)) {
		hres = AddRefStgMedium(This, &pde->stgm, pstgm, FALSE);
	}
	
	return hres;
}

static HRESULT STDMETHODCALLTYPE 
ido_setdata(	WB_IDataObject *This, 
				LPFORMATETC pfe, 
				LPSTGMEDIUM pstgm, 
				BOOL fRelease ) 
{
	LPDATAENTRY pde;
	HRESULT hres = FindFORMATETC(This, pfe, &pde, TRUE);
	if (SUCCEEDED(hres)) {
		if (pde->stgm.tymed) {
			ReleaseStgMedium(&pde->stgm);
			ZeroMemory(&pde->stgm, sizeof(STGMEDIUM));
		}
		
		if (fRelease) {
			pde->stgm = *pstgm;
			hres = S_OK;
		} else {
			hres = AddRefStgMedium(This, pstgm, &pde->stgm, TRUE);
		}
		pde->fe.tymed = pde->stgm.tymed;
		
		// Break possible circular reference loop
		if (GetCanonicalIUnknown(pde->stgm.pUnkForRelease) ==
			GetCanonicalIUnknown((LPUNKNOWN) This)) {
			pde->stgm.pUnkForRelease->lpVtbl->Release(pde->stgm.pUnkForRelease);
			pde->stgm.pUnkForRelease = NULL;
		}
	}
	return hres;
}

static HRESULT STDMETHODCALLTYPE 
ido_getdatahere(	WB_IDataObject *This, 
					LPFORMATETC pFormatEtc, 
					LPSTGMEDIUM pMedium ) 
{
  return DATA_E_FORMATETC;
}

static HRESULT STDMETHODCALLTYPE 
ido_querygetdata(	WB_IDataObject *This, 
					LPFORMATETC pfe ) 
{
	LPDATAENTRY pde;
	return FindFORMATETC(This, pfe, &pde, FALSE);
}

static HRESULT STDMETHODCALLTYPE 
ido_getcanonicalformatetc(	WB_IDataObject *This, 
							LPFORMATETC pFormatEtcIn, 
							LPFORMATETC pFormatEtcOut )
{
	pFormatEtcOut->ptd = NULL;
	return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE
ido_enumformatetc(	WB_IDataObject *This, 
					DWORD dwDirection, 
					LPENUMFORMATETC *ppEnumFormatEtc ) 
{
	if (dwDirection != DATADIR_GET) return E_NOTIMPL;

	return CreateEnumFormatEtc(This->m_cde, This->m_rgde, ppEnumFormatEtc);
}

static HRESULT STDMETHODCALLTYPE 
ido_dadvise(		WB_IDataObject *This, 
					LPFORMATETC pFormatetc, 
					DWORD advf, 
					LPADVISESINK pAdvSink, 
					DWORD *pdwConnection ) 
{
  return E_FAIL;
}

static HRESULT STDMETHODCALLTYPE 
ido_dunadvise(		WB_IDataObject *This, 
					DWORD dwConnection )
{
  return E_FAIL;
}

static HRESULT STDMETHODCALLTYPE
ido_enumdadvise(	WB_IDataObject *This, 
					LPENUMSTATDATA *ppenumAdvise ) {
  return E_FAIL;
}

static WB_IDataObjectVtbl ido_vtbl = {
	ido_queryinterface,
	ido_addref,
	ido_release,
	ido_getdata,
	ido_getdatahere,
	ido_querygetdata,
	ido_getcanonicalformatetc,
	ido_setdata,
	ido_enumformatetc,
	ido_dadvise,
	ido_dunadvise,
	ido_enumdadvise
};

WB_IDataObject *WB_IDataObject_new(int count) {
	WB_IDataObject *result;

 	result = g_new0(WB_IDataObject, 1);
	if (result == NULL) return NULL;
  
	result->ido.lpVtbl = (IDataObjectVtbl*) &ido_vtbl;
	result->m_lRefCount = 1;
	result->ref_count = 1;
	result->m_cde = count;
  
	result->m_rgde  = g_new0(DATAENTRY, count);
	if (result->m_rgde == NULL) return NULL;
	return result;
}


HRESULT CreateDataObject (FORMATETC *format, STGMEDIUM *medium, int count, WB_IDataObject **ppDataObject) {
	if(ppDataObject == NULL) return E_INVALIDARG;

	*ppDataObject = WB_IDataObject_new(count);
	if (*ppDataObject == NULL) return E_OUTOFMEMORY;
	
	u_int i;
	for (i = 0; i < count; i++) {
		(*ppDataObject)->m_rgde[i].fe 	= format[i];
		(*ppDataObject)->m_rgde[i].stgm	= medium[i];
	}

	return S_OK;
}
