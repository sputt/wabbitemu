#pragma once

#include <windows.h>
#include <ole2.h>

class CWabbitemuClassFactory : IClassFactory
{
public:
	// IUnknown methods
	STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObject);
	STDMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement(&m_lRefCount);
	};
	STDMETHODIMP_(ULONG) Release()
	{
		if (InterlockedDecrement(&m_lRefCount) == 0)
		{
			delete this;
			return 0;
		}
		else
		{
			return m_lRefCount;
		}
	};

	CWabbitemuClassFactory()
	{
		m_lRefCount = 1;
	};

	// IClassFactory methods
	STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, LPVOID *ppvObject);
	STDMETHODIMP LockServer(BOOL fLock);

private:
	LONG m_lRefCount;
};
