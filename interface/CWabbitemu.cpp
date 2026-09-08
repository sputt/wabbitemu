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
#include "pngexport.h"

#include "CMemoryContext.h"

#define WM_ADDFRAME	(WM_USER+5)
#define WM_REMOVEFRAME (WM_USER+6)

HRESULT CWabbitemu::FinalConstruct()
{
	m_lpMainWindow = _Module.GetFirstMainWindow();
	m_ownsFrame = (m_lpMainWindow == NULL);
	if (m_ownsFrame) {
		m_lpMainWindow = create_calc_frame_register_events();
	}
	if (m_ownsFrame) {
		m_lpMainWindow->silent_mode = TRUE;
	}
	if (m_lpMainWindow == NULL || m_lpMainWindow->lpCalc == NULL) {
		MessageBox(NULL, _T("Unable to create main window"), _T("Error"), MB_OK | MB_ICONERROR);
		return E_UNEXPECTED;
	}
	
	m_lpMainWindow->pWabbitemu = this;
	m_lpCalc = m_lpMainWindow->lpCalc;
	calc_register_event(m_lpCalc, ROM_LOAD_EVENT, CreateObjects, this);
	LoadRegistrySettings(m_lpMainWindow, m_lpCalc);
	if (!m_ownsFrame) {
		// The normal GUI instance has already loaded its ROM, so its ROM-load
		// event has passed. Build the COM wrappers explicitly.
		CreateObjects(m_lpCalc, this);
	} else {
		// Automation-only instances do not pass through normal startup.
		if (rom_load(m_lpCalc, m_lpCalc->rom_path) == FALSE) {
			return E_FAIL;
		}
		CreateObjects(m_lpCalc, this);
	}

	m_fVisible = VARIANT_FALSE;

	CComObject<CBreakpointCollection>::CreateInstance(&m_pBreakpointCollObj);
	m_pBreakpointCollObj->AddRef();
	m_pBreakpointCollObj->Initialize(m_lpCalc);

	// The GUI already installs TimerProc. A second NULL-hwnd timer shares
	// TimerProc's static lag counters and skips frames, which looks like a
	// slow calc that still uses almost no CPU.
	if (m_ownsFrame) {
		timeBeginPeriod(1);
		m_idTimer = SetTimer(NULL, 0, TPF, TimerProc);
	} else {
		m_idTimer = 0;
	}
	return S_OK;
};

void CWabbitemu::FinalRelease()
{
	calc_unregister_event(m_lpCalc, ROM_LOAD_EVENT, &CreateObjects, this);
	if (m_idTimer) {
		KillTimer(NULL, m_idTimer);
		m_idTimer = 0;
		timeEndPeriod(1);
	}
	if (m_ownsFrame) {
		destroy_calc_frame(m_lpMainWindow);
	}
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
			return E_FAIL;
		}

		SetProp(m_lpMainWindow->hwndFrame, _T("COMObjectFrame"), (HANDLE) TRUE);
	}
	else
	{
		m_lpMainWindow->silent_mode = TRUE;
		//int success = gui_frame_update(m_lpMainWindow);
		BOOL success = ShowWindow(m_lpMainWindow->hwndFrame, SW_HIDE);
		if (success == FALSE) {
			return E_FAIL;
		}
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

STDMETHODIMP CWabbitemu::put_Speed(int speed)
{
	m_lpCalc->speed = speed;
	return S_OK;
}

STDMETHODIMP CWabbitemu::get_Speed(int *lpSpeed)
{
	*lpSpeed = m_lpCalc->speed;
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
	m_pKeypadObject->Initialize(lpCalc);
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

STDMETHODIMP CWabbitemu::SaveFile(BSTR FileName, BSTR Author, BSTR Comment, int Compress)
{
	SAVESTATE_t *save = SaveSlot(m_lpCalc, _bstr_t(Author), _bstr_t(Comment));
	WriteSave(_bstr_t(FileName), save, Compress);
	return S_OK;
}

STDMETHODIMP CWabbitemu::StartRecordGIF(BSTR FileName)
{
	calc_start_screenshot(_bstr_t(FileName));
	_Module.SetGIFState(GDS_STARTING);
	return S_OK;
}

STDMETHODIMP CWabbitemu::StopRecordGIF()
{
	_Module.SetGIFState(GDS_ENDING);
	calc_stop_screenshot();
	return S_OK;
}

STDMETHODIMP CWabbitemu::SavePNG(BSTR FileName)
{
	LPCALC lpCalc = m_lpCalc;
	if (lpCalc == NULL) {
		LPMAINWINDOW lpMainWindow = _Module.GetFirstMainWindow();
		lpCalc = lpMainWindow ? lpMainWindow->lpCalc : NULL;
	}
	if (lpCalc == NULL) {
		return E_POINTER;
	}
	BOOL wasRunning = lpCalc->running;
	lpCalc->running = FALSE;
	export_png(lpCalc, _bstr_t(FileName));
	lpCalc->running = wasRunning;
	return S_OK;
}

STDMETHODIMP CWabbitemu::get_Apps(ITIApplicationCollection **ppAppList)
{
	CComObject<CTIApplicationCollection> *pApplicationCollObj;
	CComObject<CTIApplicationCollection>::CreateInstance(&pApplicationCollObj);
	pApplicationCollObj->AddRef();
	pApplicationCollObj->Initialize(m_pMem, m_lpCalc);

	HRESULT hr = pApplicationCollObj->QueryInterface(ppAppList);
	pApplicationCollObj->Release();
	return hr;
}


STDMETHODIMP CWabbitemu::get_Symbols(ITISymbolCollection **ppSymList)
{
	CComObject<CTISymbolCollection> *pSymbolCollObj;
	CComObject<CTISymbolCollection>::CreateInstance(&pSymbolCollObj);
	pSymbolCollObj->AddRef();
	pSymbolCollObj->Initialize(m_lpCalc, m_pMem);

	HRESULT hr = pSymbolCollObj->QueryInterface(ppSymList);
	pSymbolCollObj->Release();
	return hr;
}


STDMETHODIMP CWabbitemu::get_Keypad(IKeypad **ppKeypad)
{
	LPCALC lpCalc = m_lpCalc;
	if (lpCalc == NULL) {
		LPMAINWINDOW lpMainWindow = _Module.GetFirstMainWindow();
		lpCalc = lpMainWindow ? lpMainWindow->lpCalc : NULL;
	}
	if (ppKeypad == NULL || lpCalc == NULL) {
		return E_POINTER;
	}
	CComObject<CKeypad> *keypad = NULL;
	HRESULT hr = CComObject<CKeypad>::CreateInstance(&keypad);
	if (FAILED(hr) || keypad == NULL) {
		return E_OUTOFMEMORY;
	}
	keypad->Initialize(lpCalc);
	return keypad->QueryInterface(IID_IKeypad, (LPVOID *) ppKeypad);
}

STDMETHODIMP CWabbitemu::PressReleaseKey(CalcKey Key)
{
	CComObject<CKeypad> *keypad = NULL;
	if (m_lpCalc == NULL || FAILED(CComObject<CKeypad>::CreateInstance(&keypad)) || keypad == NULL) {
		return E_POINTER;
	}
	keypad->Initialize(m_lpCalc);
	return keypad->PressReleaseKey(Key);
}

STDMETHODIMP CWabbitemu::PressKeyCode(LONG KeyCode)
{
	static const struct { int group; int bit; } keyMaps[] =
	{
		{0, 3}, {0, 1}, {0, 2}, {0, 0}, {6, 5}, {6, 6}, {6, 7}, {5, 7},
		{4, 7}, {3, 7}, {6, 4}, {6, 3}, {6, 2}, {6, 1}, {6, 0}, {5, 6},
		{4, 6}, {3, 6}, {2, 6}, {1, 6}, {5, 5}, {4, 5}, {3, 5}, {2, 5},
		{1, 5}, {5, 4}, {4, 4}, {3, 4}, {2, 4}, {1, 4}, {5, 3}, {4, 3},
		{3, 3}, {2, 3}, {1, 3}, {5, 2}, {4, 2}, {3, 2}, {2, 2}, {1, 2},
		{5, 1}, {4, 1}, {3, 1}, {2, 1}, {1, 1}, {5, 0}, {4, 0}, {3, 0},
		{2, 0}, {1, 0}
	};
	if (m_lpCalc == NULL || KeyCode < 0 || KeyCode >= ARRAYSIZE(keyMaps)) {
		return E_INVALIDARG;
	}
	press_key(m_lpCalc, keyMaps[KeyCode].group, keyMaps[KeyCode].bit);
	return S_OK;
}

STDMETHODIMP CWabbitemu::PressKeyFor(LONG KeyCode, LONG DurationMs)
{
	static const struct { int group; int bit; } keyMaps[] =
	{
		{0, 3}, {0, 1}, {0, 2}, {0, 0}, {6, 5}, {6, 6}, {6, 7}, {5, 7},
		{4, 7}, {3, 7}, {6, 4}, {6, 3}, {6, 2}, {6, 1}, {6, 0}, {5, 6},
		{4, 6}, {3, 6}, {2, 6}, {1, 6}, {5, 5}, {4, 5}, {3, 5}, {2, 5},
		{1, 5}, {5, 4}, {4, 4}, {3, 4}, {2, 4}, {1, 4}, {5, 3}, {4, 3},
		{3, 3}, {2, 3}, {1, 3}, {5, 2}, {4, 2}, {3, 2}, {2, 2}, {1, 2},
		{5, 1}, {4, 1}, {3, 1}, {2, 1}, {1, 1}, {5, 0}, {4, 0}, {3, 0},
		{2, 0}, {1, 0}
	};
	if (m_lpCalc == NULL || KeyCode < 0 || KeyCode >= ARRAYSIZE(keyMaps) || DurationMs <= 0) {
		return E_INVALIDARG;
	}
	m_lpCalc->fake_running = TRUE;
	keypad_press(&m_lpCalc->cpu, keyMaps[KeyCode].group, keyMaps[KeyCode].bit);
	calc_run_tstates(m_lpCalc, (m_lpCalc->cpu.timer_c->freq * DurationMs) / 1000);
	keypad_release(&m_lpCalc->cpu, keyMaps[KeyCode].group, keyMaps[KeyCode].bit);
	calc_run_tstates(m_lpCalc, m_lpCalc->cpu.timer_c->freq / 32);
	m_lpCalc->fake_running = FALSE;
	return S_OK;
}

STDMETHODIMP CWabbitemu::SetKeyState(LONG KeyCode, VARIANT_BOOL Pressed)
{
	static const struct { int group; int bit; } keyMaps[] =
	{
		{0, 3}, {0, 1}, {0, 2}, {0, 0}, {6, 5}, {6, 6}, {6, 7}, {5, 7},
		{4, 7}, {3, 7}, {6, 4}, {6, 3}, {6, 2}, {6, 1}, {6, 0}, {5, 6},
		{4, 6}, {3, 6}, {2, 6}, {1, 6}, {5, 5}, {4, 5}, {3, 5}, {2, 5},
		{1, 5}, {5, 4}, {4, 4}, {3, 4}, {2, 4}, {1, 4}, {5, 3}, {4, 3},
		{3, 3}, {2, 3}, {1, 3}, {5, 2}, {4, 2}, {3, 2}, {2, 2}, {1, 2},
		{5, 1}, {4, 1}, {3, 1}, {2, 1}, {1, 1}, {5, 0}, {4, 0}, {3, 0},
		{2, 0}, {1, 0}
	};
	if (m_lpCalc == NULL || KeyCode < 0 || KeyCode >= ARRAYSIZE(keyMaps)) {
		return E_INVALIDARG;
	}
	if (Pressed == VARIANT_TRUE) {
		keypad_press(&m_lpCalc->cpu, keyMaps[KeyCode].group, keyMaps[KeyCode].bit);
	} else {
		keypad_release(&m_lpCalc->cpu, keyMaps[KeyCode].group, keyMaps[KeyCode].bit);
	}
	return S_OK;
}

STDMETHODIMP CWabbitemu::AdvanceMilliseconds(LONG DurationMs)
{
	if (m_lpCalc == NULL || DurationMs <= 0) {
		return E_INVALIDARG;
	}
	m_lpCalc->fake_running = TRUE;
	calc_run_tstates(m_lpCalc, ((time_t)m_lpCalc->cpu.timer_c->freq * DurationMs) / 1000);
	m_lpCalc->fake_running = FALSE;
	return S_OK;
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

STDMETHODIMP CWabbitemu::get_Model(CalcModel *lpCalcModel)
{
	*lpCalcModel = m_lpCalc->model;
	return S_OK;
}
