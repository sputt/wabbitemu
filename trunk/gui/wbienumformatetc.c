#include "wbded.h"

static void DeepCopyFormatEtc(FORMATETC *dest, FORMATETC *source) {
	// copy the source FORMATETC into dest
	*dest = *source;
	
	if (source->ptd) {
		// allocate memory for the DVTARGETDEVICE if necessary
		dest->ptd = (DVTARGETDEVICE*) CoTaskMemAlloc(sizeof(DVTARGETDEVICE));

		// copy the contents of the source DVTARGETDEVICE into dest->ptd
		*dest->ptd = *source->ptd;
	}
}

HRESULT 
CreateEnumFormatEtc(	UINT nNumFormats, 
						LPDATAENTRY pde, 
						LPENUMFORMATETC *ppEnumFormatEtc )
{
	if (nNumFormats == 0 || pde == NULL || ppEnumFormatEtc == NULL)
		return E_INVALIDARG;

	*ppEnumFormatEtc = (LPENUMFORMATETC) WB_IEnumFORMATETC_new(nNumFormats, pde);

	return (*ppEnumFormatEtc) ? S_OK : E_OUTOFMEMORY;
}

static ULONG STDMETHODCALLTYPE 
ief_addref(WB_IEnumFORMATETC *This) 
{
	return InterlockedIncrement(&This->m_lRefCount);
}

static HRESULT STDMETHODCALLTYPE
ief_queryinterface(	WB_IEnumFORMATETC *This, 
					REFIID riid, 
					LPVOID *ppvObject ) 
{

	if (IsEqualGUID (riid, &IID_IUnknown)) {
		ief_addref(This);
		*ppvObject = This;
		return S_OK;
    } else 
    if (IsEqualGUID (riid, &IID_IEnumFORMATETC)) {
    	ief_addref(This);
    	*ppvObject = This;
    	return S_OK;
    } else {
    	*ppvObject = NULL;
    	return E_NOINTERFACE;
    }
}

static ULONG STDMETHODCALLTYPE
ief_release(WB_IEnumFORMATETC *This)
{
    
	LONG count = InterlockedDecrement(&This->m_lRefCount);

	if(count == 0) g_free(This);

	return count;
}

static HRESULT STDMETHODCALLTYPE
ief_next(	WB_IEnumFORMATETC* This,
			ULONG celt,
			LPFORMATETC pFormatEtc,
			ULONG *pceltFetched )
{
	if (celt == 0 || pFormatEtc == NULL) return E_INVALIDARG;
		
	ULONG copied;
	for (	copied = 0;
			This->m_nIndex < This->m_cde && copied < celt;
			copied++, This->m_nIndex++ )
	{
		DeepCopyFormatEtc(&pFormatEtc[copied], &This->m_rgde[This->m_nIndex].fe);
	}

	if (pceltFetched != NULL) *pceltFetched = copied;
	
	// did we copy all that was requested?
	if (copied != celt) return S_FALSE;
	
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE
ief_skip(	WB_IEnumFORMATETC *This,
			ULONG celt )
{
	This->m_nIndex += celt;
	if (This->m_nIndex > This->m_cde) return S_FALSE;
		
	return S_FALSE;
}

static HRESULT STDMETHODCALLTYPE
ief_reset(WB_IEnumFORMATETC *This)
{
	This->m_nIndex = 0;

	return S_OK;
}

static HRESULT STDMETHODCALLTYPE
ief_clone(	WB_IEnumFORMATETC *This,
			WB_IEnumFORMATETC **ppEnumFormatEtc )
{
	WB_IEnumFORMATETC *newIEnumFORMATETC;

	if (ppEnumFormatEtc == NULL) return E_INVALIDARG;
	
	newIEnumFORMATETC = WB_IEnumFORMATETC_new(This->m_cde, This->m_rgde);
	
	if (newIEnumFORMATETC == NULL) return E_OUTOFMEMORY;
	
	newIEnumFORMATETC->m_nIndex = This->m_nIndex;
	*ppEnumFormatEtc = newIEnumFORMATETC;
  
	return S_OK;
}

static WB_IEnumFORMATETCVtbl ief_vtbl = {
	ief_queryinterface,
	ief_addref,
	ief_release,
	ief_next,
	ief_skip,
	ief_reset,
	ief_clone
};

WB_IEnumFORMATETC *
WB_IEnumFORMATETC_new(UINT nNumFormats, LPDATAENTRY pde)
{
  	
	WB_IEnumFORMATETC *result = g_new0(WB_IEnumFORMATETC, 1);

	if (result == NULL) return NULL;

	result->ief.lpVtbl = (IEnumFORMATETCVtbl*) &ief_vtbl;
	result->m_nIndex = 0;
	result->m_lRefCount = 1;
	result->m_cde = nNumFormats;
	result->m_rgde = g_new0(DATAENTRY, nNumFormats);
  
	if (result->m_rgde == NULL) return NULL;
  
	UINT i;
	for(i = 0; i < result->m_cde; i++) {	
		DeepCopyFormatEtc(&result->m_rgde[i].fe, &pde[i].fe);
	}

	return result;
}
