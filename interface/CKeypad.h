#pragma once

#include "keys.h"
#include "calc.h"

class ATL_NO_VTABLE CKeypad : 
	public CComObjectRootEx<CComObjectThreadModel>,
	public IDispatchImpl<IKeypad, &__uuidof(IKeypad)>
{
public:
	BEGIN_COM_MAP(CKeypad)
		COM_INTERFACE_ENTRY(IKeypad)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	STDMETHOD(PressKey)(CalcKey Key);
	STDMETHOD(ReleaseKey)(CalcKey Key);
	STDMETHOD(PressReleaseKey)(CalcKey Key);
	STDMETHOD(IsKeyPressed)(CalcKey Key, VARIANT_BOOL *lpfIsPressed);

	void Initialize(LPCALC lpCalc)
	{
		m_lpCalc = lpCalc;
	}

private:
	LPCALC m_lpCalc;
};
