#include "stdafx.h"

#include "CTIApplication.h"

STDMETHODIMP CTIApplication::get_PageCount(int *lpVersion) {
	*lpVersion = m_App.page_count;
	return S_OK;
}

STDMETHODIMP CTIApplication::get_Page(IPage **lppPage) {
	CComPtr<IPageCollection> pPageColl;
	uint8_t page = m_App.page;
	m_pMem->get_Flash(&pPageColl);

	pPageColl->get_Item(page, lppPage);
	return S_OK;
}

STDMETHODIMP CTIApplication::get_Name(BSTR *lpName) {
	TCHAR real_name[256] = { 0 };
#ifdef _UNICODE
	*lpName = SysAllocString((OLECHAR *)m_App.name);
#else
	WCHAR wszSymName[32];
	MultiByteToWideChar(CP_ACP, 0, m_App.name, -1, wszSymName, ARRAYSIZE(wszSymName));
	*lpName = SysAllocString((OLECHAR *)wszSymName);
#endif
	return S_OK;
}