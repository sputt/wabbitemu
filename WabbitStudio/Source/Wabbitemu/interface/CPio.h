#pragma once

#include "Wabbitemu_h.h"
#include "core.h"

class CPio : IPio
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


	CPio(CPU_t *cpu)
	{
		m_lRefCount = 1;
		m_cpu = cpu;
	}

private:
	LONG m_lRefCount;

	CPU_t *m_cpu;
};
