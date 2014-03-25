#pragma once

#include "keys.h"


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
	STDMETHOD(IsKeyPressed)(CalcKey Key, VARIANT_BOOL *lpfIsPressed);
	STDMETHOD(PressVirtKey)(int Key);
	STDMETHOD(ReleaseVirtKey)(int Key);

	void Initialize(CPU_t *cpu)
	{
		m_cpu = cpu;
	}

private:
	CPU_t *m_cpu;
};
