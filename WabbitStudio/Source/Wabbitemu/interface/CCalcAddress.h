#pragma once

#include "core.h"
#include "resource.h"

typedef ICalcAddress *LPCALCADDRESS;

class CCalcAddress :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<CCalcAddress, &CLSID_CalcAddress>,
	public IDispatchImpl<ICalcAddress, &IID_ICalcAddress, &LIBID_WabbitemuLib>
{
public:
	DECLARE_REGISTRY_RESOURCEID(IDR_CALCADDRESS)

	BEGIN_COM_MAP(CCalcAddress)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(ICalcAddress)
	END_COM_MAP()

	STDMETHOD(Initialize)(IWabbitemu *pCalc, VARIANT_BOOL IsFlash, int iPage, WORD wAddress);

	STDMETHOD(get_Calc)(IWabbitemu **ppWabbitemu);
	STDMETHOD(get_Page)(IPage **ppPage);
	STDMETHOD(get_Address)(LPWORD lpwAddress);

	STDMETHOD(Read)(VARIANT varByteCount, LPVARIANT lpvarResult);
	STDMETHOD(Write)(VARIANT varValue);

private:
	CComPtr<IWabbitemu> m_pWabbitemu;
	CComPtr<IPage> m_pPage;
	WORD m_wAddress;
};

OBJECT_ENTRY_AUTO(CLSID_CalcAddress, CCalcAddress)
