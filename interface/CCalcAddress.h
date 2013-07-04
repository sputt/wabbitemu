#pragma once

#include "core.h"

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

	STDMETHOD(Initialize)(IPage *pPage, WORD wAddress);

	STDMETHOD(get_Page)(IPage **ppPage);
	STDMETHOD(get_Address)(LPWORD lpwAddress);

	STDMETHOD(Read)(VARIANT varByteCount, LPVARIANT lpvarResult);
	STDMETHOD(Write)(VARIANT varValue);

	static inline waddr_t ToWAddr(ICalcAddress *pAddress)
	{
		waddr_t waddr;
		pAddress->get_Address(&waddr.addr);

		CComPtr<IPage> pPage;
		pAddress->get_Page(&pPage);

		VARIANT_BOOL isFlash;
		pPage->get_IsFlash(&isFlash);
		waddr.is_ram = isFlash == VARIANT_TRUE ? FALSE : TRUE;
		int nPage;
		pPage->get_Index(&nPage);
		waddr.page = nPage;
		return waddr;
	}

private:
	CComPtr<IPage> m_pPage;
	WORD m_wAddress;
};

OBJECT_ENTRY_AUTO(CLSID_CalcAddress, CCalcAddress)
