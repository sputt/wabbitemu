#pragma once

#include "Wabbitemu_h.h"
#include "keys.h"

typedef CalcKey KEYS;

class CKeypad : IKeypad
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

	// IKeypad
	STDMETHOD(PressKey)(KEYS Key);
	STDMETHOD(ReleaseKey)(KEYS Key);
	STDMETHOD(IsKeyPressed)(KEYS Key, VARIANT_BOOL *lpfIsPressed);
	STDMETHOD(PressVirtKey)(int Key);
	STDMETHOD(ReleaseVirtKey)(int Key);

	CKeypad(CPU_t *cpu)
	{
		m_lRefCount = 1;
		m_cpu = cpu;
	}

private:
	LONG m_lRefCount;

	CPU_t *m_cpu;
};
