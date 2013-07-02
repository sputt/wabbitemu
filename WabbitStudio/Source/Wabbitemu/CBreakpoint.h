#pragma once

struct tagCALC;

class ATL_NO_VTABLE CBreakpoint :
	public CComObjectRootEx<CComObjectThreadModel>,
	public IDispatchImpl<IBreakpoint, &IID_IBreakpoint>
{
public:
	BEGIN_COM_MAP(CBreakpoint)
		COM_INTERFACE_ENTRY(IBreakpoint)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	STDMETHOD(get_Address)(ICalcAddress **ppAddress)
	{
		return m_pAddress->QueryInterface(ppAddress);
	}

	STDMETHOD(get_Enabled)(VARIANT_BOOL *pbEnabled)
	{
		*pbEnabled = m_bEnabled;
		return S_OK;
	}
	STDMETHOD(put_Enabled)(VARIANT_BOOL bEnabled)
	{
		m_bEnabled = bEnabled;
		return S_OK;
	}

	HRESULT Initialize(tagCALC *calc, ICalcAddress *pAddress)
	{
		m_pCalc = calc;
		m_pAddress = pAddress;
		m_bEnabled = VARIANT_TRUE;
		return S_OK;
	}

private:
	CComPtr<ICalcAddress> m_pAddress;
	VARIANT_BOOL m_bEnabled;
	tagCALC *m_pCalc;
};
