#include "stdafx.h"

#include "Wabbitemu_h.h"
#include "WabbitemuClassFactory.h"
#include "CWabbitemu.h"

STDMETHODIMP CWabbitemuClassFactory::QueryInterface(REFIID riid, LPVOID *ppvObject)
{
	if (riid == IID_IUnknown)
	{
		this->AddRef();
		*ppvObject = this;
		return S_OK;
	}
	else if (riid == IID_IClassFactory)
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

STDMETHODIMP CWabbitemuClassFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, LPVOID *ppvObject)
{
	*ppvObject = (LPVOID) new CWabbitemu();
	return S_OK;
}

STDMETHODIMP CWabbitemuClassFactory::LockServer(BOOL fLock)
{
	return S_OK;
}
