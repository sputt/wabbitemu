#pragma once

#include "Wabbitemu_h.h"
#include "keys.h"

typedef Calc_Key KEYS;

class CKeypad : 
	public CComObjectRootEx<CComObjectThreadModel>,
	public IDispatchImpl<IKeypad, &__uuidof(IKeypad)>
{
public:
	BEGIN_COM_MAP(CKeypad)
		COM_INTERFACE_ENTRY(IKeypad)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	STDMETHOD(PressKey)(KEYS Key);
	STDMETHOD(ReleaseKey)(KEYS Key);
	STDMETHOD(IsKeyPressed)(KEYS Key, VARIANT_BOOL *lpfIsPressed);
	STDMETHOD(PressVirtKey)(int Key);
	STDMETHOD(ReleaseVirtKey)(int Key);

	void Initialize(CPU_t *cpu)
	{
		m_cpu = cpu;
	}

private:
	CPU_t *m_cpu;
};
