// Z80Assembler.h : Declaration of the CZ80Assembler

#pragma once
#include "resource.h"       // main symbols
#include "list.h"
#include "storage.h"

#include "SPASM_i.h"

typedef struct
{
	char name[64];
	char value[64];
}
default_define_pair_t;

// CZ80Assembler

class ATL_NO_VTABLE CZ80Assembler :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CZ80Assembler, &__uuidof(Z80Assembler)>,
	public IDispatchImpl<IZ80Assembler, &__uuidof(IZ80Assembler), &LIBID_SPASM, 1, 2>
{
public:
	DECLARE_REGISTRY_RESOURCEID(IDR_Z80ASSEMBLER)

	BEGIN_COM_MAP(CZ80Assembler)
		COM_INTERFACE_ENTRY(IZ80Assembler)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

public:
	STDMETHOD(get_Output)(IStream **ppStream);

	STDMETHOD(get_InputFile)(LPBSTR lpbstrInput);
	STDMETHOD(put_InputFile)(BSTR bstrInput);

	STDMETHOD(get_OutputFile)(LPBSTR lpbstrOutput);
	STDMETHOD(put_OutputFile)(BSTR bstrOutput);

	STDMETHOD(ClearDefines)();
	STDMETHOD(AddDefine)(BSTR bstrName, VARIANT varValue);

	STDMETHOD(get_Defines)(IDictionary **ppDefines);
	STDMETHOD(get_StdOut)(ITextStream **ppStdOut);

	STDMETHOD(ClearIncludeDirectories)();
	STDMETHOD(AddIncludeDirectory)(BSTR bstrDirectory);

	STDMETHOD(Assemble)(VARIANT varInput, int *lpReturn);

	STDMETHOD(get_Labels)(LPSAFEARRAY *lpsa);
	

private:
	static LONG m_lIndex;
	static void get_label_callback(label_t *label, CComSafeArray<IDispatch *> *lpsa);

	IDictionaryPtr m_spDictionary;
	ITextStreamPtr m_spStdOut;

	LPSTREAM m_pStmOutput;

	_bstr_t m_bstrInputFile;
	_bstr_t m_bstrOutputFile;

	list_t *default_defines;	
};

OBJECT_ENTRY_AUTO(__uuidof(Z80Assembler), CZ80Assembler)
