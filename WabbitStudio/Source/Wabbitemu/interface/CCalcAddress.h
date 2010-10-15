#pragma once

#include <atlbase.h>
#include <atlcom.h>

#include <map>
#include <string>

#include "Wabbitemu_h.h"
#include "core.h"

class CCalcAddress :
	public IDispatchImpl<ICalcAddress, &IID_ICalcAddress, &LIBID_WabbitemuLib>,
	public CComObjectRootEx<CComMultiThreadModel>
{
public:
	CCalcAddress()
	{
		m_pWabbitemu = NULL;
		m_pPage = NULL;
		m_wAddress = 0;
	};

	~CCalcAddress()
	{
		if (m_pWabbitemu != NULL)
		{
			m_pWabbitemu->Release();
		}
		if (m_pPage != NULL)
		{
			m_pPage->Release();
		}
	}

	BEGIN_COM_MAP(CCalcAddress)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(ICalcAddress)
	END_COM_MAP()

	STDMETHOD(get_Calc)(IWabbitemu **ppWabbitemu);
	STDMETHOD(get_Page)(IPage **ppPage);
	STDMETHOD(get_Address)(LPWORD lpwAddress);

	STDMETHOD(Read)(VARIANT varByteCount, LPVARIANT lpvarResult);
	STDMETHOD(Write)(VARIANT varValue);

	void Initialize(IWabbitemu *pCalc, BOOL IsFlash, int iPage, WORD wAddress);

private:
	IWabbitemu *m_pWabbitemu;
	IPage *m_pPage;
	WORD m_wAddress;
};