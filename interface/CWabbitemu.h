#pragma once

#include "CZ80.h"
#include "CLCD.h"
#include "CKeypad.h"
#include "CLabelServer.h"

struct tagCALC;

#include "resource.h"

class CWabbitemu :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<CWabbitemu, &CLSID_Wabbitemu>,
	public IProvideClassInfo2Impl<&CLSID_Wabbitemu, &IID_IWabbitemu, &LIBID_WabbitemuLib, 1, 0>,
	public IDispatchImpl<IWabbitemu, &IID_IWabbitemu, &LIBID_WabbitemuLib, 1, 0>
{
public:
	DECLARE_REGISTRY_RESOURCEID(IDR_WABBITEMU)

	BEGIN_COM_MAP(CWabbitemu)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IWabbitemu)
		COM_INTERFACE_ENTRY(IProvideClassInfo2)
		COM_INTERFACE_ENTRY(IProvideClassInfo)
	END_COM_MAP()

	HRESULT FinalConstruct();

	// IWabbitemu methods
	STDMETHODIMP put_Visible(VARIANT_BOOL fVisible);
	STDMETHODIMP get_Visible(VARIANT_BOOL *lpVisible);

	STDMETHODIMP get_CPU(IZ80 **ppZ80);
	STDMETHODIMP get_LCD(ILCD **ppLCD);

	STDMETHODIMP RAM(int Index, IPage **ppPage);
	STDMETHODIMP Flash(int Index, IPage **ppPage);

	STDMETHOD(get_Running)(VARIANT_BOOL *lpfRunning);
	STDMETHOD(put_Running)(VARIANT_BOOL fRunning);

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


	void Fire_OnBreakpoint()
	{
		OutputDebugString(_T("Hello"));
	}

private:
	DWORD m_dwThreadId;
	static DWORD CALLBACK WabbitemuThread(LPVOID lpParam);

	LONG m_lRefCount;

	int m_iSlot;
	VARIANT_BOOL m_fVisible;
	struct tagCALC *m_lpCalc;
	CComPtr<IZ80> m_pZ80;
	CComPtr<ILCD> m_pLCD;
	CKeypad *m_pKeypad;
	HWND m_hwnd;
	UINT_PTR m_idTimer;
	//CComObject<CLabelServer> m_LabelServer;
};

OBJECT_ENTRY_AUTO(CLSID_Wabbitemu, CWabbitemu)
