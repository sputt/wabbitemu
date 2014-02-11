#include "stdafx.h"

#include "gui.h"
#include "CLCD.h"
#include "SendFileswindows.h"
#include "CWabbitemu.h"
#include "CPage.h"
#include "dbdisasm.h"
#include "calc.h"
#include "registry.h"
#include "var.h"

#include "CMemoryContext.h"

#define WM_ADDFRAME	(WM_USER+5)
#define WM_REMOVEFRAME (WM_USER+6)

HRESULT CWabbitemu::FinalConstruct()
{
	m_lpCalc = calc_slot_new();
	LoadRegistrySettings(m_lpCalc);
	m_iSlot = m_lpCalc->slot;

	m_fVisible = VARIANT_FALSE;

	m_lpCalc->pWabbitemu = this;

	CComObject<CBreakpointCollection>::CreateInstance(&m_pBreakpointCollObj);
	m_pBreakpointCollObj->AddRef();
	m_pBreakpointCollObj->Initialize(m_lpCalc);

	m_idTimer = SetTimer(NULL, 0, TPF, TimerProc);
	return S_OK;
};

void CWabbitemu::FinalRelease()
{
	m_pBreakpointCollObj->Release();
	calc_slot_free(m_lpCalc);
}

STDMETHODIMP CWabbitemu::put_Visible(VARIANT_BOOL fVisible)
{
	if (fVisible == m_fVisible)
	{
		return S_FALSE;
	}
	if (fVisible == VARIANT_TRUE)
	{
		gui_frame(m_lpCalc);
		SetProp(m_lpCalc->hwndFrame, _T("COMObjectFrame"), (HANDLE) TRUE);
		//HMENU hMenu = GetSystemMenu(m_lpCalc->hwndFrame, FALSE);
		//EnableMenuItem(hMenu, SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
		//SetMenu(m_lpCalc->hwndFrame, NULL);
		//RECT wr;
		//GetWindowRect(m_lpCalc->hwndFrame, &wr);
		//SendMessage(m_lpCalc->hwndFrame, WM_SIZING, WMSZ_BOTTOMRIGHT, (LPARAM) &wr);
		//SetWindowPos(m_lpCalc->hwndFrame, NULL, wr.left, wr.top, wr.right - wr.left, wr.bottom - wr.top, SWP_NOZORDER);

	}
	else
	{
		DestroyWindow(m_lpCalc->hwndFrame);
	}
	m_fVisible = fVisible;
	return S_OK;
}

void CWabbitemu::Fire_OnClose()
{
	m_fVisible = VARIANT_FALSE;

	CComVariant vWabbit((IDispatch *) this);

	int nConnectionIndex;
	int nConnections = this->m_vec.GetSize();
	for (nConnectionIndex =  0;  nConnectionIndex < nConnections; nConnectionIndex++)
	{
		Lock();
		CComPtr<IUnknown> sp = this->m_vec.GetAt(nConnectionIndex);
		Unlock();

		if (sp != NULL)
		{
			CComDispatchDriver drv(sp);

			HRESULT hr = drv.Invoke1(DISPID_CLOSE, &vWabbit);
			if (FAILED(hr))
			{
				OutputDebugString(_T("Failed to invoke\n"));
			}
		}
	}	
}

void CWabbitemu::Fire_OnBreakpoint(waddr *pwaddr)
{
	CComPtr<IBreakpoint> pBreakpoint;
	// Make sure that the breakpoint came from our breakpoint collection
	HRESULT hr = m_pBreakpointCollObj->LookupBreakpoint(*pwaddr, &pBreakpoint);
	if (hr != S_OK)
	{
		return;
	}

	int nConnectionIndex;
	int nConnections = this->m_vec.GetSize();
	for (nConnectionIndex =  0;  nConnectionIndex < nConnections; nConnectionIndex++)
	{
		Lock();
		CComPtr<IUnknown> sp = this->m_vec.GetAt(nConnectionIndex);
		Unlock();

		if (sp != NULL)
		{
			CComDispatchDriver drv(sp);
			VARIANT vBreakpoint;
			VariantInit(&vBreakpoint);

			V_VT(&vBreakpoint) = VT_DISPATCH;
			pBreakpoint->QueryInterface(&V_DISPATCH(&vBreakpoint));

			CComVariant vWabbit((IDispatch *) this);

			HRESULT hr = drv.Invoke2(DISPID_BREAKPOINT, &vWabbit, &vBreakpoint);
			if (FAILED(hr))
			{
				OutputDebugString(_T("Failed to invoke\n"));
			}

			VariantClear(&vBreakpoint);
		}
	}
}

STDMETHODIMP CWabbitemu::get_Visible(VARIANT_BOOL *lpVisible)
{
	*lpVisible = m_fVisible;
	return S_OK;
}

STDMETHODIMP CWabbitemu::get_CPU(IZ80 **ppZ80)
{
	if (m_pZ80 == NULL)
	{
		return S_FALSE;
	}
	return m_pZ80->QueryInterface(IID_IZ80,(LPVOID *) ppZ80);
}

STDMETHODIMP CWabbitemu::get_Memory(IMemoryContext **ppContext)
{
	return m_pMem->QueryInterface(ppContext);
}


STDMETHODIMP CWabbitemu::get_LCD(ILCD **ppLCD)
{
	return m_pLCD->QueryInterface(IID_ILCD,(LPVOID *) ppLCD);
}

STDMETHODIMP CWabbitemu::Run()
{
	m_lpCalc->running = TRUE;
	return S_OK;
}

STDMETHODIMP CWabbitemu::Break()
{
	m_lpCalc->running = FALSE;
	return S_OK;
}

STDMETHODIMP CWabbitemu::Step()
{
	CPU_step(&m_lpCalc->cpu);
	return S_OK;
}

STDMETHODIMP CWabbitemu::StepOver()
{
	CPU_stepover(m_lpCalc);
	return S_OK;
}

STDMETHODIMP CWabbitemu::get_Breakpoints(IBreakpointCollection **ppBC)
{
	return m_pBreakpointCollObj->QueryInterface(ppBC);
}

STDMETHODIMP CWabbitemu::LoadFile(BSTR bstrFileName)
{
	TIFILE *file = importvar(_bstr_t(bstrFileName), TRUE);
	
	SendFileToCalc(m_lpCalc, _bstr_t(bstrFileName), FALSE);
	if (file->type == ROM_TYPE || file->type == SAV_TYPE)
	{
		CComObject<CLCD> *pLCD = NULL;
		CComObject<CLCD>::CreateInstance(&pLCD);
		pLCD->Initialize(m_lpCalc->cpu.pio.lcd);
		m_pLCD = pLCD;

		CComObject<CZ80> *m_pZ80Obj = NULL;
		CComObject<CZ80>::CreateInstance(&m_pZ80Obj);
		m_pZ80Obj->Initialize(&m_lpCalc->cpu);
		m_pZ80 = m_pZ80Obj;

		CComObject<CKeypad> *m_pKeypadObject = NULL;
		CComObject<CKeypad>::CreateInstance(&m_pKeypadObject);
		m_pKeypadObject->Initialize(&m_lpCalc->cpu);
		m_pKeypad = m_pKeypadObject;

		CComObject<CMemoryContext> *m_pMemObj = NULL;
		CComObject<CMemoryContext>::CreateInstance(&m_pMemObj);

		m_pMemObj->Initialize(m_lpCalc->cpu.mem_c);

		m_pMem = m_pMemObj;

		extern keyprog_t keygrps[256];
		extern keyprog_t keysti83[256];
		extern keyprog_t keysti86[256];
		if (m_lpCalc->model == TI_86 || m_lpCalc->model == TI_85) {
			memcpy(keygrps, keysti86, sizeof(keyprog_t) * 256);
		} else {
			memcpy(keygrps, keysti83, sizeof(keyprog_t) * 256);
		}
	}
	return S_OK;
}

STDMETHODIMP CWabbitemu::get_Apps(SAFEARRAY **ppAppList)
{
	CComPtr<ITypeLib> pTypeLib;
	HRESULT hr = LoadRegTypeLib(LIBID_WabbitemuLib, 1, 0, GetUserDefaultLCID(), &pTypeLib);
	ATLENSURE_SUCCEEDED(hr);

	CComPtr<ITypeInfo> pTypeInfo;
	hr = pTypeLib->GetTypeInfoOfGuid(__uuidof(TIApplication), &pTypeInfo);
	ATLENSURE_SUCCEEDED(hr);

	CComPtr<IRecordInfo> pRecordInfo;
	hr = GetRecordInfoFromTypeInfo(pTypeInfo, &pRecordInfo);
	ATLENSURE_SUCCEEDED(hr);

	applist_t applist;
	ZeroMemory(&applist, sizeof(applist_t));
	state_build_applist(&m_lpCalc->cpu, &applist);

	SAFEARRAYBOUND sab = {0};
	sab.lLbound = 0;
	sab.cElements = applist.count;
	LPSAFEARRAY lpsa = SafeArrayCreateEx(VT_RECORD, 1, &sab, pRecordInfo);

	TIApplication *pvData = NULL;
	if (SUCCEEDED(SafeArrayAccessData(lpsa, (LPVOID *) &pvData)))
	{
		for (u_int i = 0; i < sab.cElements; i++)
		{
#ifdef _UNICODE
			pvData[i].Name = SysAllocString((OLECHAR *) applist.apps[i].name);
#else
			WCHAR wszAppName[ARRAYSIZE(applist.apps[i].name)];
			MultiByteToWideChar(CP_ACP, 0, applist.apps[i].name, -1, wszAppName, ARRAYSIZE(wszAppName));
			pvData[i].Name = SysAllocString((OLECHAR *) wszAppName);
#endif
			CComPtr<IPageCollection> pPageColl;
			m_pMem->get_Flash(&pPageColl);

			pPageColl->get_Item(applist.apps[i].page, &pvData[i].Page);

			pvData[i].PageCount = applist.apps[i].page_count;
		}

		SafeArrayUnaccessData(lpsa);
	}

	*ppAppList = lpsa;
	return S_OK;
}

/*
STDMETHODIMP CWabbitemu::get_Symbols(SAFEARRAY **ppAppList)
{
//	ITypeLib *pTypeLib = NULL;
//	HRESULT hr = LoadRegTypeLib(LIBID_WabbitemuLib, 1, 0, GetUserDefaultLCID(), &pTypeLib);
//	if (FAILED(hr))
//	{
//		return hr;
//	}
//	ITypeInfo *pTypeInfo = NULL;
//	hr = pTypeLib->GetTypeInfoOfGuid(__uuidof(TISymbol), &pTypeInfo);
//	if (FAILED(hr))
//	{
//		return hr;
//	}
//
//	IRecordInfo *pRecordInfo;
//	hr = GetRecordInfoFromTypeInfo(pTypeInfo, &pRecordInfo);
//	if (FAILED(hr))
//	{
//		return hr;
//	}
//	pTypeInfo->Release();
//	pTypeLib->Release();
//
//	symlist_t symlist;
//	state_build_symlist_83P(&m_lpCalc->cpu, &symlist);
//
//	SAFEARRAYBOUND sab = {0};
//	sab.lLbound = 0;
//	sab.cElements = (u_int) (symlist.last - symlist.symbols + 1);
//	LPSAFEARRAY lpsa = SafeArrayCreateEx(VT_RECORD, 1, &sab, pRecordInfo);
//	pRecordInfo->Release();
//
//	TISymbol *pvData = NULL;
//	if (SUCCEEDED(SafeArrayAccessData(lpsa, (LPVOID *) &pvData)))
//	{
//		for (u_int i = 0; i < sab.cElements; i++)
//		{
//			WCHAR wszSymName[256];
//#ifdef _UNICODE
//			if (Symbol_Name_to_String(m_lpCalc->cpu.pio.model, &symlist.symbols[i], wszSymName) == NULL)
//				StringCbCopy(wszSymName, sizeof(wszSymName), _T(""));
//#else
//			TCHAR buffer[256];
//			if (Symbol_Name_to_String(m_lpCalc->cpu.pio.model, &symlist.symbols[i], buffer) == NULL)
//				StringCbCopy(buffer, sizeof(wszSymName), _T(""));
//			MultiByteToWideChar(CP_ACP, 0, buffer, -1, wszSymName, ARRAYSIZE(wszSymName));
//#endif
//			pvData[i].Name = SysAllocString(wszSymName);
//			pvData[i].Page = symlist.symbols[i].page;
//			pvData[i].Version = symlist.symbols[i].version;
//			pvData[i].Type = (SYMBOLTYPE) symlist.symbols[i].type_ID;
//			pvData[i].Address = symlist.symbols[i].address;
//		}
//
//		SafeArrayUnaccessData(lpsa);
//	}
//
//	*ppAppList = lpsa;
	return S_OK;
}
*/

STDMETHODIMP CWabbitemu::get_Keypad(IKeypad **ppKeypad)
{
	return m_pKeypad->QueryInterface(IID_IKeypad, (LPVOID *) ppKeypad);
}

STDMETHODIMP CWabbitemu::get_Labels(ILabelServer **ppLabelServer)
{
	return E_NOTIMPL;
	//return m_LabelServer.QueryInterface(IID_ILabelServer, (LPVOID *) ppLabelServer);
}

STDMETHODIMP CWabbitemu::get_Running(VARIANT_BOOL *lpfRunning)
{
	*lpfRunning = (m_lpCalc->running == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CWabbitemu::get_Model(CALC_MODEL *lpCalcModel)
{
	*lpCalcModel = (CALC_MODEL) (m_lpCalc->model);
	return S_OK;
}