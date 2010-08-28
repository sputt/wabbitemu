#pragma once

#include <windows.h>
#include <tchar.h>
#include "Wabbitemu_h.h"
#include "CZ80.h"
#include "CLCD.h"
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
	STDMETHODIMP get_LCD(ILCD **ppLCD);

	STDMETHODIMP RAM(int Index, IPage **ppPage);
	STDMETHODIMP Flash(int Index, IPage **ppPage);

	STDMETHOD(get_Running)(VARIANT_BOOL *lpfRunning)
	{
		*lpfRunning = (m_lpCalc->running == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
		return S_OK;
	}
	STDMETHOD(put_Running)(VARIANT_BOOL fRunning)
	{
		m_lpCalc->running = (fRunning == VARIANT_TRUE) ? TRUE : FALSE;
		return S_OK;
	}

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
		m_pLCD = new CLCD(&calcs[slot].cpu);

		m_hThread = CreateThread(NULL, 0, WabbitemuThread, (LPVOID) this, 0, NULL);
	};

private:
	static DWORD CALLBACK WabbitemuThread(LPVOID lpParam);

	LONG m_lRefCount;
	calc_t *m_lpCalc;
	CZ80 *m_pZ80;
	CLCD *m_pLCD;
	HANDLE m_hThread;
	HWND m_hwnd;
};
