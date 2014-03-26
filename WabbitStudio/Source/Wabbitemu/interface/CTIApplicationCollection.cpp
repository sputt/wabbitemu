#include "stdafx.h"

#include "CTIApplicationCollection.h"
#include "CTIApplication.h"

HRESULT CTIApplicationCollection::Initialize(IMemoryContext *pMem, LPCALC lpCalc) {
	applist_t applist;
	ZeroMemory(&applist, sizeof(applist_t));
	state_build_applist(&lpCalc->cpu, &applist);

	APPLICATIONVECTOR apps;
	for (u_int i = 0; i < applist.count; i++) {
		CComObject<CTIApplication> *pAppObj;
		HRESULT hr = CComObject<CTIApplication>::CreateInstance(&pAppObj);
		ATLENSURE_SUCCEEDED(hr);
		pAppObj->Initialize(pMem, lpCalc, applist.apps[i]);

		CAdapt< CComPtr<ITIApplication> > pApp = pAppObj;
		apps.push_back(pApp);
	}

	m_coll = apps;
	return S_OK;
}
