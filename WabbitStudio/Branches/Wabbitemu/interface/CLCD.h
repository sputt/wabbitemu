#pragma once

#include "core.h"
#include "lcd.h"
#include "Wabbitemu_h.h"

class CLCD : ILCD
{
public:
	// IUnknown methods
	STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObject);
	STDMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement(&m_lRefCount);
	};
	STDMETHODIMP_(ULONG) Release()
	{
		if (InterlockedDecrement(&m_lRefCount) == 0)
		{
			delete this;
			return 0;
		}
		else
		{
			return m_lRefCount;
		}
	};

	// ILCD methods
	STDMETHOD (get_Display)(LPSAFEARRAY *ppsa)
	{
		SAFEARRAYBOUND sab = {0};
		sab.cElements = 64 * 128;
		sab.lLbound = 0;
		LPSAFEARRAY psa = SafeArrayCreate(VT_UI1, 1, &sab);

		LPBYTE lpData = NULL;
		SafeArrayAccessData(psa, (LPVOID *) &lpData);
		memcpy(lpData, LCD_image(m_cpu->pio.lcd), 128 * 64);
		SafeArrayUnaccessData(psa);

		*ppsa = psa;
		return S_OK;
	}

	STDMETHOD(Draw)(BYTE Display[8192])
	{
		memcpy(Display, LCD_image(m_cpu->pio.lcd), 128 * 64);
		return S_OK;
	}

	STDMETHOD(GetByteArray)(BYTE Display[8192])
	{
		memcpy(Display, LCD_image(m_cpu->pio.lcd), 128 * 64);
		return S_OK;
	}

	CLCD(CPU_t *cpu)
	{
		m_cpu = cpu;
	}


private:
	LONG m_lRefCount;
	CPU_t *m_cpu;
};
