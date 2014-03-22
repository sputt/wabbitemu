#include "stdafx.h"

#include "CTISymbol.h"

STDMETHODIMP CTISymbol::get_Type(SymbolType *lpSymbolType) {
	*lpSymbolType = (SymbolType) m_Symbol.type_ID;
	return S_OK;
}

STDMETHODIMP CTISymbol::get_Version(BYTE *lpVersion) {
	*lpVersion = m_Symbol.version;
	return S_OK;
}

STDMETHODIMP CTISymbol::get_Page(IPage **lppPage) {
	CComPtr<IPageCollection> pPageColl;
	uint8_t page = m_Symbol.page;
	if (page == 0) {
		// if page == 0 its on a RAM page
		m_pMem->get_RAM(&pPageColl);
		// < 0xC000 is ram page 1, above ram page 0
		page = m_Symbol.address < 0xC000;
	} else {
		m_pMem->get_Flash(&pPageColl);
	}
	
	pPageColl->get_Item(page, lppPage);
	return S_OK;
}

STDMETHODIMP CTISymbol::get_Address(WORD *lpAddr) {
	*lpAddr = m_Symbol.address;
	return S_OK;
}

STDMETHODIMP CTISymbol::get_Name(BSTR *lpName) {
	TCHAR real_name[256] = { 0 };
	Symbol_Name_to_String(m_Model, &m_Symbol, real_name);
#ifdef _UNICODE
	*lpName = SysAllocString((OLECHAR *)real_name);
#else
	WCHAR wszSymName[32];
	MultiByteToWideChar(CP_ACP, 0, real_name, -1, wszSymName, ARRAYSIZE(wszSymName));
	*lpName = SysAllocString((OLECHAR *)wszSymName);
#endif
	return S_OK;
}