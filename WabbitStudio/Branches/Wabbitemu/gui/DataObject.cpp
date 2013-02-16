#include "stdafx.h"

#include "DataObject.h"

CDataObject::CDataObject(FORMATETC *pFormatEtc, STGMEDIUM *pStgMedium, DWORD dwCount) {
	m_lRefCount = 1;
	m_dwNumFormats = dwCount;

	m_pDataEntry = (LPDATAENTRY) malloc(sizeof(DATAENTRY) * dwCount);

	for (DWORD i = 0; i < dwCount; i++) {
		m_pDataEntry[i].fmtetc = pFormatEtc[i];
		m_pDataEntry[i].stgmed = pStgMedium[i];
	}
}

CDataObject::~CDataObject() {
	free(m_pDataEntry);
}

/*
 * Create a DataObject and put it into ppDataObject
 */
HRESULT CreateDataObject(FORMATETC *pFormatEtc, STGMEDIUM *pStgMedium, DWORD dwCount, IDataObject **ppDataObject) {
	if (ppDataObject == NULL)
		return E_INVALIDARG;

	*ppDataObject = new CDataObject(pFormatEtc, pStgMedium, dwCount);
	return (*ppDataObject) ? S_OK : E_OUTOFMEMORY;
}

/* 
 * Create a duplicate global handle to a memory block
 */
HGLOBAL GlobalClone(HGLOBAL hglobIn) {
	HGLOBAL hglobOut = NULL;
	LPVOID pvIn = GlobalLock(hglobIn);
	if (pvIn != NULL) {
		SIZE_T cb = GlobalSize(hglobIn);
		hglobOut = GlobalAlloc(GMEM_FIXED, cb);
		if (hglobOut != NULL) {
			CopyMemory(hglobOut, pvIn, cb);
		}
		GlobalUnlock(hglobIn);
	}
	return hglobOut;
}

/*
 * Search our registered formats for the requested format
 */
HRESULT CDataObject::LookupFormatEtc(FORMATETC *pFormatEtc, LPDATAENTRY *pDataEntry, BOOL fAdd) {
	if (pDataEntry != NULL)
		*pDataEntry = NULL;

	if (pFormatEtc->ptd != NULL)
		return DV_E_DVTARGETDEVICE;

	if (fAdd == TRUE)
		if (pFormatEtc->ptd != NULL)
			return DV_E_DVTARGETDEVICE;

	for (DWORD i = 0; i < m_dwNumFormats; i++) {

		if ((m_pDataEntry[i].fmtetc.lindex == pFormatEtc->lindex) &&
			(m_pDataEntry[i].fmtetc.cfFormat == pFormatEtc->cfFormat) &&
			(m_pDataEntry[i].fmtetc.dwAspect == pFormatEtc->dwAspect))
		{
			if (fAdd == TRUE || (m_pDataEntry[i].fmtetc.tymed & pFormatEtc->tymed) != 0) {
				if (pDataEntry != NULL)
					*pDataEntry = &m_pDataEntry[i];
				return S_OK;
			} else {
				return DV_E_TYMED;
			}
		}
	}

	if (fAdd == FALSE)
		return DV_E_FORMATETC;
	else {
		LPDATAENTRY lpde = (LPDATAENTRY) realloc(m_pDataEntry, sizeof(DATAENTRY) * (m_dwNumFormats + 1));
		if (lpde != NULL) {
			m_pDataEntry = lpde;
			m_pDataEntry[m_dwNumFormats].fmtetc = *pFormatEtc;
			ZeroMemory(&m_pDataEntry[m_dwNumFormats].stgmed, sizeof(STGMEDIUM));
			if (pDataEntry != NULL)
				*pDataEntry = &m_pDataEntry[m_dwNumFormats];

			m_dwNumFormats++;
			return S_OK;
		} else {
			return E_OUTOFMEMORY;
		}
	}
}

HRESULT CDataObject::AddRefStgMedium(STGMEDIUM *pStgMediumIn, STGMEDIUM *pStgMediumOut, BOOL fCopyIn) {
	HRESULT hr = S_OK;
	STGMEDIUM stgmOut = *pStgMediumIn;

	if (pStgMediumIn->pUnkForRelease == NULL && !(pStgMediumIn->tymed & (TYMED_ISTREAM | TYMED_ISTORAGE))) {
		if (fCopyIn == TRUE) {
			if (pStgMediumIn->tymed == TYMED_HGLOBAL) {
				stgmOut.hGlobal = GlobalClone(pStgMediumIn->hGlobal);
				if (stgmOut.hGlobal == NULL)
					hr = E_OUTOFMEMORY;
			} else {
				hr = DV_E_TYMED;
			}
		} else {
			stgmOut.pUnkForRelease = (IUnknown *) this;
		}
	}

	if (SUCCEEDED(hr)) {
		switch (stgmOut.tymed)
		{
		case TYMED_ISTREAM:
			stgmOut.pstm->AddRef();
			break;
		case TYMED_ISTORAGE:
			stgmOut.pstg->AddRef();
			break;
		}
		if (stgmOut.pUnkForRelease != NULL)
			stgmOut.pUnkForRelease->AddRef();

		*pStgMediumOut = stgmOut;
	}

	return hr;
}

/*
 * IUnknown methods
 */
ULONG __stdcall CDataObject::AddRef(void) {
	return InterlockedIncrement(&m_lRefCount);
}

ULONG __stdcall CDataObject::Release(void) {
	LONG lRefCount = InterlockedDecrement(&m_lRefCount);
	if (lRefCount == 0)
		delete this;
	return lRefCount;
}

HRESULT __stdcall CDataObject::QueryInterface(REFIID riid, LPVOID *ppvObject) {
	if (riid == IID_IUnknown) {
		this->AddRef();
		*ppvObject = this;
		return S_OK;
	} else if (riid == IID_IDataObject) {
		this->AddRef();
		*ppvObject = this;
		return S_OK;
	} else {
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
}


/*
 * IDataObject methods
 */
HRESULT __stdcall CDataObject::QueryGetData(FORMATETC *pFormatEtc) {
	return LookupFormatEtc(pFormatEtc, NULL, FALSE);
}


HRESULT __stdcall CDataObject::GetData(FORMATETC *pFormatEtc, STGMEDIUM *pStgMedium) {
	LPDATAENTRY lpde;
	if (pFormatEtc == NULL)
	{
		return E_INVALIDARG;
	}
	HRESULT hr = LookupFormatEtc(pFormatEtc, &lpde, FALSE);
	if (SUCCEEDED(hr)) {
		hr = AddRefStgMedium(&lpde->stgmed, pStgMedium, FALSE);
	}

	return hr;
}


static IUnknown *GetCanonicalIUnknown(IUnknown *punk) {
	IUnknown *punkCanonical;
	
	if (punk && SUCCEEDED(punk->QueryInterface(IID_IUnknown, (LPVOID *) &punkCanonical))) {
		punkCanonical->Release();
	} else {
		punkCanonical = punk;
	}
	return punkCanonical;
}


HRESULT __stdcall CDataObject::SetData(FORMATETC *pFormatEtc, STGMEDIUM *pStgMedium, BOOL fRelease) {
	LPDATAENTRY lpde;
	HRESULT hr = LookupFormatEtc(pFormatEtc, &lpde, TRUE);
	if (SUCCEEDED(hr)) {
		if (lpde->stgmed.tymed != TYMED_NULL) {
			ReleaseStgMedium(&lpde->stgmed);
			ZeroMemory(&lpde->stgmed, sizeof(STGMEDIUM));
		}

		if (fRelease == TRUE) {
			lpde->stgmed = *pStgMedium;
			hr = S_OK;
		} else {
			hr = AddRefStgMedium(pStgMedium, &lpde->stgmed, TRUE);
		}

		if (GetCanonicalIUnknown(lpde->stgmed.pUnkForRelease) == GetCanonicalIUnknown((IUnknown *) this)) {
			lpde->stgmed.pUnkForRelease->Release();
			lpde->stgmed.pUnkForRelease = NULL;
		}
	}

	return hr;
}

HRESULT __stdcall CDataObject::EnumFormatEtc (DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc) {
	if (dwDirection == DATADIR_GET) {
		FORMATETC *pFormatEtc = new FORMATETC[m_dwNumFormats];
		for (DWORD i = 0; i < m_dwNumFormats; i++)
			pFormatEtc[i] = m_pDataEntry[i].fmtetc;
		HRESULT hr = SHCreateStdEnumFmtEtc(m_dwNumFormats, pFormatEtc, ppEnumFormatEtc);
		delete pFormatEtc;
		return hr;
	} else {
		return E_NOTIMPL;
	}
}


HRESULT __stdcall CDataObject::DAdvise(FORMATETC *pFormatEtc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection) {
	return OLE_E_ADVISENOTSUPPORTED;
}


HRESULT __stdcall CDataObject::DUnadvise(DWORD dwConnection) {
	return OLE_E_ADVISENOTSUPPORTED;
}


HRESULT __stdcall CDataObject::EnumDAdvise(IEnumSTATDATA **ppEnumAdvise) {
	return OLE_E_ADVISENOTSUPPORTED;
}


HRESULT __stdcall CDataObject::GetDataHere (FORMATETC *pFormatEtc, STGMEDIUM *pStgMedium) {
	return DATA_E_FORMATETC;
}


HRESULT CDataObject::GetCanonicalFormatEtc (FORMATETC *pFormatEct, FORMATETC *pFormatEtcOut) {
	pFormatEtcOut->ptd = NULL;

	return E_NOTIMPL;
}
