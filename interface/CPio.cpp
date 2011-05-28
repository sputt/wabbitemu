#include "stdafx.h"

#include "CPio.h"


STDMETHODIMP CPio::QueryInterface(REFIID riid, LPVOID *ppvObject)
{
	if (riid == IID_IUnknown)
	{
		this->AddRef();
		*ppvObject = this;
		return S_OK;
	}
	else if (riid == IID_IKeypad)
	{
		this->AddRef();
		*ppvObject = this;
		return S_OK;
	}
	else
	{
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
}
