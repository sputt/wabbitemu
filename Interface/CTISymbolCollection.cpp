#include "stdafx.h"

#include "CTISymbolCollection.h"
#include "CTISymbol.h"

HRESULT CTISymbolCollection::Initialize(CalcModel model, IMemoryContext *pMem, LPCALC lpCalc) 
{
	symlist_t symlist;
	ZeroMemory(&symlist, sizeof(symlist_t));
	state_build_symlist_83P(&lpCalc->cpu, &symlist);

	SYMBOLVECTOR symbols;
	for (u_int i = 0; i < symlist.count; i++) {
		CComObject<CTISymbol> *pSymbolObj;
		HRESULT hr = CComObject<CTISymbol>::CreateInstance(&pSymbolObj);
		pSymbolObj->Initialize(model, pMem, symlist.symbols[i]);

		CAdapt< CComPtr<ITISymbol> > pSymbol = pSymbolObj;
		symbols.push_back(pSymbol);
	}

	m_coll = symbols;
	return S_OK;
}
