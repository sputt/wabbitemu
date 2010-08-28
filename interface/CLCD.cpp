#include "stdafx.h"

#include "CLCD.h"

STDMETHODIMP CLCD::QueryInterface(REFIID riid, LPVOID *ppvObject)
{
	if (riid == IID_IUnknown)
	{
		this->AddRef();
		*ppvObject = this;
		return S_OK;
	}
	else if (riid == IID_ILCD)
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

