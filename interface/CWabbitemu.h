#pragma once

#include <windows.h>
#include <tchar.h>
#include "Wabbitemu_h.h"
#include "CZ80.h"
#include "calc.h"

class CWabbitemu : IWabbitemu
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

	// IWabbitemu methods
	STDMETHODIMP put_Visible(VARIANT_BOOL fVisible);
	STDMETHODIMP get_Visible(VARIANT_BOOL *lpVisible);

	STDMETHODIMP get_CPU(IZ80 **ppZ80);

	STDMETHODIMP RAM(int Index, IPage **ppPage);
	STDMETHODIMP Flash(int Index, IPage **ppPage);

	STDMETHODIMP Step();
	STDMETHODIMP StepOver();

	STDMETHODIMP SetBreakpoint(IPage *pPage, WORD wAddress);

	STDMETHODIMP Read(WORD Address, LPBYTE lpValue);
	STDMETHODIMP Write(WORD Address, BYTE Value);

	STDMETHODIMP LoadFile(BSTR bstrFileName);

	STDMETHODIMP get_Apps(SAFEARRAY **ppAppList);
	STDMETHODIMP get_Symbols(SAFEARRAY **ppAppList);

	CWabbitemu()
	{
		m_lRefCount = 1;
		int slot = calc_slot_new();
		m_lpCalc = &calcs[slot];
		m_pZ80 = new CZ80(&calcs[slot].cpu);
	};

private:
	LONG m_lRefCount;
	calc_t *m_lpCalc;
	CZ80 *m_pZ80;
};
