#pragma once

#include <windows.h>
#include <tchar.h>
#include "Wabbitemu_h.h"
#include "core.h"

class CZ80 : IZ80
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

	// IZ80 methods
	STDMETHODIMP get_AF(LPWORD lpAF) {*lpAF = m_cpu->af; return S_OK;};
	STDMETHODIMP get_A(LPBYTE lpA)   {*lpA = m_cpu->a; return S_OK;};
	STDMETHODIMP get_F(LPBYTE lpF)   {*lpF = m_cpu->f; return S_OK;};
	STDMETHODIMP put_AF(WORD AF) {m_cpu->af = AF; return S_OK;};
	STDMETHODIMP put_A(BYTE A)   {m_cpu->a = A; return S_OK;};
	STDMETHODIMP put_F(BYTE F)   {m_cpu->f = F; return S_OK;};

	CZ80(CPU_t *cpu)
	{
		m_lRefCount = 1;
		m_cpu = cpu;
	};

private:
	LONG m_lRefCount;
	CPU_t *m_cpu;
};
