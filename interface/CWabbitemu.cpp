#include "stdafx.h"

#include "gui.h"
#include "guiskin.h"
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
	m_lpMainWindow = create_calc_frame_register_events();
	m_lpMainWindow->silent_mode = TRUE;
	if (m_lpMainWindow == NULL || m_lpMainWindow->lpCalc == NULL) {
		MessageBox(NULL, _T("Unable to create main window"), _T("Error"), MB_OK | MB_ICONERROR);
		return E_UNEXPECTED;
	}
	
	m_lpMainWindow->pWabbitemu = this;
	m_lpCalc = m_lpMainWindow->lpCalc;
	calc_register_event(m_lpCalc, ROM_LOAD_EVENT, CreateObjects, this);
	LoadRegistrySettings(m_lpMainWindow, m_lpCalc);

	m_fVisible = VARIANT_FALSE;

	CComObject<CBreakpointCollection>::CreateInstance(&m_pBreakpointCollObj);
	m_pBreakpointCollObj->AddRef();
	m_pBreakpointCollObj->Initialize(m_lpCalc);

	m_idTimer = SetTimer(NULL, 0, TPF, TimerProc);
	return S_OK;
};

void CWabbitemu::FinalRelease()
{
	calc_unregister_event(m_lpCalc, ROM_LOAD_EVENT, &CreateObjects, this);
	destroy_calc_frame(m_lpMainWindow);
	m_pBreakpointCollObj->Release();
}

STDMETHODIMP CWabbitemu::put_Visible(VARIANT_BOOL fVisible)
{
	if (fVisible == m_fVisible)
	{
		return S_FALSE;
	}
	
	if (fVisible == VARIANT_TRUE)
	{
		m_lpMainWindow->silent_mode = FALSE;
		int success = gui_frame_update(m_lpMainWindow);
		if (success == FALSE) {
			DestroyWindow(m_lpMainWindow->hwndFrame);
			return E_FAIL;
		}

		SetProp(m_lpMainWindow->hwndFrame, _T("COMObjectFrame"), (HANDLE) TRUE);
	}
	else
	{
		m_lpMainWindow->silent_mode = TRUE;
		DestroyWindow(m_lpMainWindow->hwndFrame);
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
			if (drv == NULL)
			{
				return;
			}

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
		return E_POINTER;
	}

	return m_pZ80->QueryInterface(IID_IZ80,(LPVOID *) ppZ80);
}

STDMETHODIMP CWabbitemu::get_Memory(IMemoryContext **ppContext)
{
	if (m_pMem == NULL)
	{
		return E_POINTER;
	}

	return m_pMem->QueryInterface(ppContext);
}


STDMETHODIMP CWabbitemu::get_LCD(ILCD **ppLCD)
{
	if (m_pLCD == NULL)
	{
		return E_POINTER;
	}

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
	CPU_stepover(m_lpCalc, m_lpMainWindow->bTIOSDebug);
	return S_OK;
}

STDMETHODIMP CWabbitemu::Reset()
{
	calc_reset(m_lpCalc);
	return S_OK;
}

STDMETHODIMP CWabbitemu::TurnCalcOn() {
	calc_turn_on(m_lpCalc);
	return S_OK;
}

STDMETHODIMP CWabbitemu::get_Breakpoints(IBreakpointCollection **ppBC)
{
	return m_pBreakpointCollObj->QueryInterface(ppBC);
}

void CWabbitemu::CreateObjects(LPCALC lpCalc, LPVOID lParam) {
	CWabbitemu *me = (CWabbitemu *)lParam;
	CComObject<CLCD> *pLCD = NULL;
	CComObject<CLCD>::CreateInstance(&pLCD);
	pLCD->Initialize(lpCalc->cpu.pio.lcd);
	me->m_pLCD = pLCD;

	CComObject<CZ80> *m_pZ80Obj = NULL;
	CComObject<CZ80>::CreateInstance(&m_pZ80Obj);
	m_pZ80Obj->Initialize(&lpCalc->cpu);
	me->m_pZ80 = m_pZ80Obj;

	CComObject<CKeypad> *m_pKeypadObject = NULL;
	CComObject<CKeypad>::CreateInstance(&m_pKeypadObject);
	m_pKeypadObject->Initialize(&lpCalc->cpu);
	me->m_pKeypad = m_pKeypadObject;

	CComObject<CMemoryContext> *m_pMemObj = NULL;
	CComObject<CMemoryContext>::CreateInstance(&m_pMemObj);

	m_pMemObj->Initialize(lpCalc->cpu.mem_c);

	me->m_pMem = m_pMemObj;
}

STDMETHODIMP CWabbitemu::LoadFile(BSTR bstrFileName)
{
	TIFILE *file = importvar(_bstr_t(bstrFileName), TRUE);
	if (file == NULL)
	{
		return E_INVALIDARG;
	}
	
	BOOL result = SendFileToCalc(m_lpMainWindow->hwndFrame, m_lpCalc, _bstr_t(bstrFileName), FALSE);

	if (result == FALSE)
	{
		return E_INVALIDARG;
	}

	return S_OK;
}

STDMETHODIMP CWabbitemu::get_Apps(ITIApplicationCollection **ppAppList)
{
	CComObject<CTIApplicationCollection> *pApplicationCollObj;
	CComObject<CTIApplicationCollection>::CreateInstance(&pApplicationCollObj);
	pApplicationCollObj->AddRef();
	pApplicationCollObj->Initialize(m_pMem, m_lpCalc);

	return pApplicationCollObj->QueryInterface(ppAppList);
}


STDMETHODIMP CWabbitemu::get_Symbols(ITISymbolCollection **ppSymList)
{
	CComObject<CTISymbolCollection> *pSymbolCollObj;
	CComObject<CTISymbolCollection>::CreateInstance(&pSymbolCollObj);
	pSymbolCollObj->AddRef();
	pSymbolCollObj->Initialize((CalcModel) m_lpCalc->model, m_pMem, m_lpCalc);

	return pSymbolCollObj->QueryInterface(ppSymList);
}


STDMETHODIMP CWabbitemu::get_Keypad(IKeypad **ppKeypad)
{
	if (m_pKeypad == NULL) {
		return E_POINTER;
	}

	return m_pKeypad->QueryInterface(IID_IKeypad, (LPVOID *) ppKeypad);
}

STDMETHODIMP CWabbitemu::get_Labels(ILabelServer **)
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