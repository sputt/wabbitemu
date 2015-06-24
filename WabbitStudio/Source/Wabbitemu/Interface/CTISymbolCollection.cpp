#include "stdafx.h"

#include "CTISymbolCollection.h"
#include "CTISymbol.h"

HRESULT CTISymbolCollection::Initialize(LPCALC lpCalc, IMemoryContext *pMem) 
{
	symlist_t *symlist = (symlist_t *)malloc(sizeof(symlist_t));
	ZeroMemory(&symlist, sizeof(symlist_t));
	state_build_symlist_83P(&lpCalc->cpu, symlist);

	SYMBOLVECTOR symbols;
	for (u_int i = 0; i < symlist->count; i++) {
		CComObject<CTISymbol> *pSymbolObj;
		HRESULT hr = CComObject<CTISymbol>::CreateInstance(&pSymbolObj);
		ATLENSURE_SUCCEEDED(hr);
		pSymbolObj->Initialize(lpCalc, pMem, symlist->symbols[i]);

		CAdapt< CComPtr<ITISymbol> > pSymbol = pSymbolObj;
		symbols.push_back(pSymbol);
	}

	m_coll = symbols;
	free(symlist);
	return S_OK;
}
