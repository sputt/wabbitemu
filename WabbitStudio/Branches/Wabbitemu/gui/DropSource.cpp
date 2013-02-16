#include "stdafx.h"

#include "DropSource.h"

CDropSource::CDropSource() {
	m_lRefCount = 1;
}

CDropSource::~CDropSource() {

}

HRESULT CreateDropSource(IDropSource **ppDropSource) {
	*ppDropSource = new CDropSource();
	return S_OK;
}

/*
 * IUnknown methods
 */
ULONG __stdcall CDropSource::AddRef(void) {
	return InterlockedIncrement(&m_lRefCount);
}

ULONG __stdcall CDropSource::Release(void) {
	LONG lRefCount = InterlockedDecrement(&m_lRefCount);
	if (lRefCount == 0)
		delete this;
	return lRefCount;
}

HRESULT __stdcall CDropSource::QueryInterface(REFIID riid, LPVOID *ppvObject) {
	if (riid == IID_IUnknown) {
		this->AddRef();
		*ppvObject = this;
		return S_OK;
	} else if (riid == IID_IDropSource) {
		this->AddRef();
		*ppvObject = this;
		return S_OK;
	} else {
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
}


/*
 * IDropSource methods
 */
HRESULT __stdcall CDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyPress) {
	if (fEscapePressed == TRUE)
		return DRAGDROP_S_CANCEL;

	if ((grfKeyPress & MK_LBUTTON) == 0)
		return DRAGDROP_S_DROP;

	return S_OK;
}


HRESULT __stdcall CDropSource::GiveFeedback(DWORD dwEffect) {
	return DRAGDROP_S_USEDEFAULTCURSORS;
}