#include "stdafx.h"

#include "CWabbitemu.h"
#include "CPage.h"

STDMETHODIMP CWabbitemu::QueryInterface(REFIID riid, LPVOID *ppvObject)
{
	if (riid == IID_IUnknown)
	{
		this->AddRef();
		*ppvObject = this;
		return S_OK;
	}
	else if (riid == IID_IWabbitemu)
	{
		this->AddRef();
		*ppvObject = this;
		return S_OK;
	}
	else
	{
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
}

STDMETHODIMP CWabbitemu::put_Visible(VARIANT_BOOL fVisible)
{
	return S_OK;
}

STDMETHODIMP CWabbitemu::get_Visible(VARIANT_BOOL *lpVisible)
{
	return S_OK;
}

STDMETHODIMP CWabbitemu::get_CPU(IZ80 **ppZ80)
{
	return m_pZ80->QueryInterface(IID_IZ80,(LPVOID *) ppZ80);
}

STDMETHODIMP CWabbitemu::Step()
{
	CPU_step(&m_lpCalc->cpu);
	return S_OK;
}

STDMETHODIMP CWabbitemu::StepOver()
{
	//CPU_stepover(&m_lpCalc->cpu);
	return S_OK;
}

STDMETHODIMP CWabbitemu::SetBreakpoint(IPage *pPage, WORD wAddress)
{
	PAGETYPE PageType;
	pPage->get_Type(&PageType);

	int iPage;
	pPage->get_Index(&iPage);

	set_break(&m_lpCalc->mem_c, (PageType == PAGETYPE::RAM), iPage, wAddress);
	return S_OK;
}


STDMETHODIMP CWabbitemu::RAM(int Index, IPage **ppPage)
{
	*ppPage = (IPage *) new CPage(&m_lpCalc->mem_c, PAGETYPE::RAM, Index);
	return S_OK;   
}

STDMETHODIMP CWabbitemu::Flash(int Index, IPage **ppPage)
{
	*ppPage = (IPage *) new CPage(&m_lpCalc->mem_c, PAGETYPE::FLASH, Index);
	return S_OK;   
}

STDMETHODIMP CWabbitemu::Read(WORD Address, LPBYTE lpValue)
{
	*lpValue = mem_read(&m_lpCalc->mem_c, Address);
	return S_OK;
}

STDMETHODIMP CWabbitemu::Write(WORD Address, BYTE Value)
{
	mem_write(&m_lpCalc->mem_c, Address, Value);
	return S_OK;
}

STDMETHODIMP CWabbitemu::LoadFile(BSTR bstrFileName)
{
	char szFileName[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, bstrFileName, -1, szFileName, sizeof(szFileName), NULL, NULL);
	rom_load(m_lpCalc - calcs, szFileName);
	return S_OK;
}

STDMETHODIMP CWabbitemu::get_Apps(SAFEARRAY **ppAppList)
{
	ITypeLib *pTypeLib = NULL;
	HRESULT hr = LoadRegTypeLib(LIBID_WabbitemuLib, 1, 0, GetUserDefaultLCID(), &pTypeLib);
	if (FAILED(hr))
	{
		return hr;
	}
	ITypeInfo *pTypeInfo = NULL;
	hr = pTypeLib->GetTypeInfoOfGuid(__uuidof(TIApplication), &pTypeInfo);
	if (FAILED(hr))
	{
		return hr;
	}

	IRecordInfo *pRecordInfo;
	hr = GetRecordInfoFromTypeInfo(pTypeInfo, &pRecordInfo);
	if (FAILED(hr))
	{
		return hr;
	}
	pTypeInfo->Release();
	pTypeLib->Release();

	applist_t applist;
	state_build_applist(&m_lpCalc->cpu, &applist);

	SAFEARRAYBOUND sab = {0};
	sab.lLbound = 1;
	sab.cElements = applist.count;
	LPSAFEARRAY lpsa = SafeArrayCreateEx(VT_RECORD, 1, &sab, pRecordInfo);
	pRecordInfo->Release();

	TIApplication *pvData = NULL;
	if (SUCCEEDED(SafeArrayAccessData(lpsa, (LPVOID *) &pvData)))
	{
		for (int i = 0; i < sab.cElements; i++)
		{
			WCHAR wszAppName[ARRAYSIZE(applist.apps[i].name)];
			MultiByteToWideChar(CP_ACP, 0, applist.apps[i].name, -1, wszAppName, ARRAYSIZE(wszAppName));

			pvData[i].Name = SysAllocString((OLECHAR *) wszAppName);
			pvData[i].Page = applist.apps[i].page;
			pvData[i].PageCount = applist.apps[i].page_count;
		}

		SafeArrayUnaccessData(lpsa);
	}

	*ppAppList = lpsa;
	return S_OK;
}

STDMETHODIMP CWabbitemu::get_Symbols(SAFEARRAY **ppAppList)
{
	ITypeLib *pTypeLib = NULL;
	HRESULT hr = LoadRegTypeLib(LIBID_WabbitemuLib, 1, 0, GetUserDefaultLCID(), &pTypeLib);
	if (FAILED(hr))
	{
		return hr;
	}
	ITypeInfo *pTypeInfo = NULL;
	hr = pTypeLib->GetTypeInfoOfGuid(__uuidof(TISymbol), &pTypeInfo);
	if (FAILED(hr))
	{
		return hr;
	}

	IRecordInfo *pRecordInfo;
	hr = GetRecordInfoFromTypeInfo(pTypeInfo, &pRecordInfo);
	if (FAILED(hr))
	{
		return hr;
	}
	pTypeInfo->Release();
	pTypeLib->Release();

	symlist_t symlist;
	state_build_symlist_83P(&m_lpCalc->cpu, &symlist);

	SAFEARRAYBOUND sab = {0};
	sab.lLbound = 1;
	sab.cElements = symlist.last - symlist.symbols + 1;
	LPSAFEARRAY lpsa = SafeArrayCreateEx(VT_RECORD, 1, &sab, pRecordInfo);
	pRecordInfo->Release();

	TISymbol *pvData = NULL;
	if (SUCCEEDED(SafeArrayAccessData(lpsa, (LPVOID *) &pvData)))
	{
		for (int i = 0; i < sab.cElements; i++)
		{
			char buffer[256];
			WCHAR wszSymName[256];
			if (Symbol_Name_to_String(&symlist.symbols[i], buffer) == NULL)
			{
				strcpy(buffer, "");
			}
			MultiByteToWideChar(CP_ACP, 0, buffer, -1, wszSymName, ARRAYSIZE(wszSymName));


			pvData[i].Name = SysAllocString(wszSymName);
			pvData[i].Page = symlist.symbols[i].page;
			pvData[i].Version = symlist.symbols[i].version;
			pvData[i].Type = (SYMBOLTYPE) symlist.symbols[i].type_ID;
			pvData[i].Address = symlist.symbols[i].address;
		}

		SafeArrayUnaccessData(lpsa);
	}

	*ppAppList = lpsa;
	return S_OK;
}