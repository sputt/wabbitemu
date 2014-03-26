#include "stdafx.h"

#include "CTIApplication.h"
#include "exportvar.h"

STDMETHODIMP CTIApplication::get_PageCount(int *lpVersion) {
	*lpVersion = m_App.page_count;
	return S_OK;
}

STDMETHODIMP CTIApplication::get_Page(IPage **lppPage) {
	CComPtr<IPageCollection> pPageColl;
	u_int page = m_App.page;
	m_pMem->get_Flash(&pPageColl);

	pPageColl->get_Item(page, lppPage);
	return S_OK;
}

STDMETHODIMP CTIApplication::get_Name(BSTR *lpName) {
#ifdef _UNICODE
	*lpName = SysAllocString((OLECHAR *)m_App.name);
#else
	WCHAR wszSymName[32];
	MultiByteToWideChar(CP_ACP, 0, m_App.name, -1, wszSymName, ARRAYSIZE(wszSymName));
	*lpName = SysAllocString((OLECHAR *)wszSymName);
#endif
	return S_OK;
}

STDMETHODIMP CTIApplication::Export(BSTR bstrFileName) {
	MFILE *file = ExportApp(m_lpCalc, _bstr_t(bstrFileName), &m_App);
	if (file == NULL) {
		return E_FAIL;
	}

	mclose(file);
	return S_OK;
}