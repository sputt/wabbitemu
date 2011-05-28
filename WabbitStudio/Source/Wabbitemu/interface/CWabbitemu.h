#pragma once

#include <windows.h>
#include <tchar.h>
#include <atlbase.h>
#include <atlcom.h>

#include "Wabbitemu_h.h"
#include "CZ80.h"
#include "CLCD.h"
#include "CKeypad.h"
#include "calc.h"
#include "CLabelServer.h"

class CWabbitemu :
	public IDispatchImpl<IWabbitemu, &IID_IWabbitemu, &LIBID_WabbitemuLib>,
	public CComObjectRootEx<CComMultiThreadModel>
{
public:
	BEGIN_COM_MAP(CWabbitemu)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IWabbitemu)
	END_COM_MAP()

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

	STDMETHODIMP SetBreakpoint(IPage *pPage, WORD wAddress, VARIANT varCalcNotify);

	STDMETHOD(Read)(WORD Address, VARIANT varByteCount, LPVARIANT lpvarResult);
	STDMETHOD(Write)(WORD Address, VARIANT varValue);

	STDMETHODIMP LoadFile(BSTR bstrFileName);

	STDMETHODIMP get_Apps(SAFEARRAY **ppAppList);
	STDMETHODIMP get_Symbols(SAFEARRAY **ppAppList);
	STDMETHOD(get_Keypad)(IKeypad **ppKeypad);
	STDMETHODIMP get_Labels(ILabelServer **ppLabelServer);

	CWabbitemu()
	{
		m_lpCalc = calc_slot_new();
		m_iSlot = m_lpCalc->slot;
		m_pZ80 = new CZ80(&calcs[m_iSlot].cpu);
		m_pLCD = new CLCD(&calcs[m_iSlot].cpu);
		m_pKeypad = new CKeypad(&calcs[m_iSlot].cpu);
		m_fVisible = VARIANT_FALSE;

		m_LabelServer.AddRef();
		m_LabelServer.Initialize(this);

		m_lpCalc->pWabbitemu = this;

		if (m_dwThreadId == 0)
		{
			CreateThread(NULL, 0, WabbitemuThread, (LPVOID) this, 0, &m_dwThreadId);
		}
	};

	

private:
	static DWORD m_dwThreadId;
	static DWORD CALLBACK WabbitemuThread(LPVOID lpParam);

	LONG m_lRefCount;

	int m_iSlot;
	VARIANT_BOOL m_fVisible;
	calc_t *m_lpCalc;
	CZ80 *m_pZ80;
	CLCD *m_pLCD;
	CKeypad *m_pKeypad;
	HWND m_hwnd;
	CComObject<CLabelServer> m_LabelServer;
};
