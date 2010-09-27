#include "stdafx.h"

#include "Wabbitemu_h.h"
#include "WabbitemuClassFactory.h"
#include "CWabbitemu.h"
#include "CLabelServer.h"

static CComObject<CLabelServer> g_pLabelServer;

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
	CComObject<CWabbitemu> *pWabbitemu = new CComObject<CWabbitemu>();
	pWabbitemu->AddRef();
	HRESULT hr = pWabbitemu->QueryInterface(riid, ppvObject);
	return hr;
}

STDMETHODIMP CWabbitemuClassFactory::LockServer(BOOL fLock)
{
	return S_OK;
}
