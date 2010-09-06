#pragma once

#include <windows.h>
#include <tchar.h>
#include "Wabbitemu_h.h"
#include "core.h"

class CZ80 : IZ80
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

	// IZ80 methods
#define INTF_REGPAIR(zh, zl, zhl, zH, zL, zHL) \
	STDMETHODIMP get_##zHL(LPWORD lp##zHL) {*lp##zHL = m_cpu->zhl; return S_OK;}; \
	STDMETHODIMP get_##zH(LPBYTE lp##zH)   {*lp##zH = m_cpu->zh; return S_OK;}; \
	STDMETHODIMP get_##zL(LPBYTE lp##zL)   {*lp##zL = m_cpu->zl; return S_OK;}; \
	STDMETHODIMP put_##zHL(WORD zHL) {m_cpu->zhl = zHL; return S_OK;}; \
	STDMETHODIMP put_##zH(BYTE zH)   {m_cpu->zh = zH; return S_OK;}; \
	STDMETHODIMP put_##zL(BYTE zL)   {m_cpu->zl = zL; return S_OK;}

	INTF_REGPAIR(a, f, af, A, F, AF);
	INTF_REGPAIR(b, c, bc, B, C, BC);
	INTF_REGPAIR(d, e, de, D, E, DE);
	INTF_REGPAIR(h, l, hl, H, L, HL);

	INTF_REGPAIR(ap, fp, afp, AP, FP, AFP);
	INTF_REGPAIR(bp, cp, bcp, BP, CP, BCP);
	INTF_REGPAIR(dp, ep, dep, DP, EP, DEP);
	INTF_REGPAIR(hp, lp, hlp, HP, LP, HLP);

	INTF_REGPAIR(ixh, ixl, ix, IXH, IXL, IX);
	INTF_REGPAIR(iyh, iyl, iy, IYH, IYL, IY);

	STDMETHODIMP get_PC(LPWORD lpPC) {*lpPC = m_cpu->pc; return S_OK;};
	STDMETHODIMP put_PC(WORD PC) {m_cpu->pc = PC; return S_OK;};

	STDMETHODIMP get_SP(LPWORD lpSP) {*lpSP = m_cpu->sp; return S_OK;};
	STDMETHODIMP put_SP(WORD SP) {m_cpu->sp = SP; return S_OK;};

	CZ80(CPU_t *cpu)
	{
		m_lRefCount = 1;
		m_cpu = cpu;
	};

private:
	LONG m_lRefCount;
	CPU_t *m_cpu;
};
