#pragma once

#include "core.h"

class ATL_NO_VTABLE CZ80 :
	public CComObjectRootEx<CComObjectThreadModel>,
	public IDispatchImpl<IZ80, &IID_IZ80>
{
public:
	BEGIN_COM_MAP(CZ80)
		COM_INTERFACE_ENTRY(IZ80)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

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

	STDMETHOD(get_Halt)(VARIANT_BOOL *pbHalt);
	STDMETHOD(put_Halt)(VARIANT_BOOL bHalt);

	HRESULT Initialize(CPU_t *cpu)
	{
		m_cpu = cpu;
		return S_OK;
	};

private:
	CPU_t *m_cpu;
};
