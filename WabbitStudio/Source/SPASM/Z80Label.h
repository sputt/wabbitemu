#pragma once

#include "SPASM_i.h"
#include "storage.h"

class ATL_NO_VTABLE CZ80Label :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IZ80Label, &IID_IZ80Label, &LIBID_SPASM, /*wMajor =*/ 1, /*wMinor =*/ 2>
{
public:

	DECLARE_REGISTRY_RESOURCEID(IDR_Z80ASSEMBLER)

	BEGIN_COM_MAP(CZ80Label)
		COM_INTERFACE_ENTRY(IZ80Label)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

public:
	STDMETHOD(get_Name)(BSTR *lpbstrName);
	STDMETHOD(get_Value)(DWORD *lpdwValue);

	void Initialize(const label_t *label)
	{
		m_label = label;
	}
private:
	const label_t *m_label;
};

