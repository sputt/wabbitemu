#pragma once

#include "CCalcAddress.h"
#include "core.h"

class CLabelServer :
	public IDispatchImpl<ILabelServer, &IID_ILabelServer, &LIBID_WabbitemuLib>,
	public CComObjectRoot
{
public:
	CLabelServer()
	{
		m_fIsCaseSensitive = FALSE;
		m_pWabbitemu = NULL;
	};

	~CLabelServer()
	{
		if (m_pWabbitemu != NULL)
		{
			m_pWabbitemu->Release();
		}
	}

	BEGIN_COM_MAP(CLabelServer)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(ILabelServer)
	END_COM_MAP( )

	// ILabelServer methods
	STDMETHOD(get_CaseSensitive)(VARIANT_BOOL *pfIsCaseSensitive);
	STDMETHOD(put_CaseSensitive)(VARIANT_BOOL fIsCaseSensitive);

	STDMETHOD(Find)(VARIANT varCriteria, ILabel **ppLabel);
	STDMETHOD(Load)(BSTR bstrFileName);

	void Initialize(IWabbitemu *pWabbitemu)
	{
		m_pWabbitemu = pWabbitemu;
	};

private:
	VARIANT_BOOL m_fIsCaseSensitive;
	IWabbitemu *m_pWabbitemu;
};

class CLabel :
	public CComObjectRoot,
	public IDispatchImpl<ILabel, &IID_ILabel, &LIBID_WabbitemuLib>
{
public:
	CLabel()
	{
		m_bstrName = NULL;
	};
	~CLabel()
	{
		if (m_bstrName != NULL)
		{
			SysFreeString(m_bstrName);
		}
	};

	BEGIN_COM_MAP(CLabel)
		COM_INTERFACE_ENTRY(ILabel)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

public:
	// ICalcAddress methods
	STDMETHOD(get_Calc)(IWabbitemu **ppWabbitemu);
	STDMETHOD(get_Page)(IPage **ppPage);
	STDMETHOD(get_Address)(LPWORD lpwAddress);

	STDMETHOD(Read)(VARIANT varByteCount, LPVARIANT lpvarResult);
	STDMETHOD(Write)(VARIANT varValue);

	// ILabel methods
	STDMETHOD(get_Name)(BSTR *bstrName);

	void Initialize(const BSTR bstrName, IWabbitemu *pCalc, BOOL IsFlash, int iPage, WORD wAddress)
	{
		m_bstrName = SysAllocString(bstrName);
		m_CalcAddress.Initialize(pCalc, IsFlash, iPage, wAddress);
	}

private:
	BSTR m_bstrName;
	CComObject<CCalcAddress> m_CalcAddress;
};
