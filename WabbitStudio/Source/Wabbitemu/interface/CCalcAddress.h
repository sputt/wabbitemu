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

	//IReadWrite
	STDMETHOD(ReadByte)(LPBYTE lpbResult);
	STDMETHOD(ReadWord)(LPWORD lpwResult);
	STDMETHOD(Read)(WORD wCount, LPSAFEARRAY *ppsaResult);
	STDMETHOD(WriteByte)(BYTE bValue);
	STDMETHOD(WriteWord)(WORD wValue);
	STDMETHOD(Write)(SAFEARRAY *psaValue);

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

	static inline HRESULT FromWAddr(IMemoryContext *pMem, waddr_t waddr, ICalcAddress **ppCalcAddress)
	{
		CComObject<CCalcAddress> *pObj = NULL;
		HRESULT hr = CComObject<CCalcAddress>::CreateInstance(&pObj);
		if (SUCCEEDED(hr))
		{
			pObj->AddRef();

			CComPtr<IPage> pPage;
			CComPtr<IPageCollection> pPageColl;
			if (waddr.is_ram)
			{
				pMem->get_RAM(&pPageColl);
			}
			else
			{
				pMem->get_Flash(&pPageColl);
			}
			pPageColl->get_Item(waddr.page, &pPage);
			hr = pObj->Initialize(pPage, waddr.addr);
			if (SUCCEEDED(hr))
			{
				hr = pObj->QueryInterface(ppCalcAddress);
			}
			pObj->Release();
		}
		return hr;
	}

	static inline HRESULT Clone(ICalcAddress *pSrc, ICalcAddress **ppDst)
	{
		CComPtr<IPage> pPage;
		WORD wAddr;

		pSrc->get_Address(&wAddr);
		pSrc->get_Page(&pPage);

		CComObject<CCalcAddress> *pObj = NULL;
		HRESULT hr = CComObject<CCalcAddress>::CreateInstance(&pObj);
		if (SUCCEEDED(hr))
		{
			pObj->AddRef();

			hr = pObj->Initialize(pPage, wAddr);
			if (SUCCEEDED(hr))
			{
				hr = pObj->QueryInterface(ppDst);
			}

			pObj->Release();
		}
		return hr;
	}

private:
	CComPtr<IPage> m_pPage;
	WORD m_wAddress;
};

OBJECT_ENTRY_AUTO(CLSID_CalcAddress, CCalcAddress)
