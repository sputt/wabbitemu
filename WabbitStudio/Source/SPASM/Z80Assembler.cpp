// Z80Assembler.cpp : Implementation of CZ80Assembler

#include "stdafx.h"
#include "Z80Assembler.h"
#include "Z80Label.h"

#include "spasm.h"
#include "utils.h"
#include "storage.h"
#include "list.h"
#include "hash.h"

extern hash_t *label_table;

LONG CZ80Assembler::m_lIndex;

HRESULT CZ80Assembler::FinalConstruct()
{
	init_storage();
	m_pStmOutput = NULL;
	return S_OK;
}

void CZ80Assembler::FinalRelease()
{
	free_storage();
}

STDMETHODIMP CZ80Assembler::get_Output(IStream **ppOutput)
{
	if (ppOutput == NULL)
	{
		return E_INVALIDARG;
	}

	if (m_pStmOutput == NULL)
	{
		return E_NOT_VALID_STATE;
	}
	return m_pStmOutput->QueryInterface(IID_PPV_ARGS(ppOutput));
}

STDMETHODIMP CZ80Assembler::put_InputFile(BSTR bstrInputFile)
{
	m_bstrInputFile = bstrInputFile;
	return S_OK;
}

STDMETHODIMP CZ80Assembler::get_InputFile(LPBSTR lpbstrInputFile)
{
	*lpbstrInputFile = SysAllocString(m_bstrInputFile);
	return S_OK;
}

STDMETHODIMP CZ80Assembler::put_OutputFile(BSTR bstrOutputFile)
{
	m_bstrOutputFile = bstrOutputFile;
	return S_OK;
}

STDMETHODIMP CZ80Assembler::get_OutputFile(LPBSTR lpbstrOutputFile)
{
	*lpbstrOutputFile = SysAllocString(m_bstrOutputFile);
	return S_OK;
}


STDMETHODIMP CZ80Assembler::ClearDefines()
{
	list_free(default_defines, true, NULL);
	default_defines = NULL;
	return S_OK;
}

STDMETHODIMP CZ80Assembler::AddDefine(BSTR bstrName, VARIANT varValue)
{
	curr_input_file = strdup("COM Interface");

	if (V_VT(&varValue) == VT_EMPTY || V_VT(&varValue) == VT_ERROR)
	{
		V_VT(&varValue) = VT_UI4;
		V_UI4(&varValue) = 1;
	}

	VARIANT varDefine;
	VariantInit(&varDefine);
	V_VT(&varDefine) = VT_BSTR;
	V_BSTR(&varDefine) = SysAllocString(L"");
	HRESULT hr = VariantChangeType(&varDefine, &varValue, 0, VT_BSTR);
	if (FAILED(hr))
	{
		return hr;
	}

	CW2A szName(bstrName);

	bool fRedefined = false;
	define_t *define = add_define(strdup(szName), &fRedefined);
	free(curr_input_file);

	if (define != NULL)
	{
		CW2A szContents(V_BSTR(&varDefine));
		define->contents = strdup(szContents);
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

STDMETHODIMP CZ80Assembler::ClearIncludeDirectories()
{
	list_free(include_dirs, true, NULL);
	include_dirs = NULL;
	return S_OK;
}

STDMETHODIMP CZ80Assembler::AddIncludeDirectory(BSTR bstrDirectory)
{
	CW2CT szInput(bstrDirectory);
	include_dirs = list_append(include_dirs, strdup(szInput));
	return S_OK;
}

STDMETHODIMP CZ80Assembler::Assemble(VARIANT varInput, int *lpInt)
{
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, output_buf_size); 
	output_contents = (unsigned char *) GlobalLock(hGlobal);

	if (V_VT(&varInput) == VT_BSTR)
	{
		mode = MODE_NORMAL | MODE_COMMANDLINE;
		
		curr_input_file = strdup("COM Interface");

		CW2CT szInput(V_BSTR(&varInput));
		input_contents = strdup(szInput);

		output_filename = strdup(m_bstrOutputFile);
	}
	else
	{
		mode = MODE_NORMAL;

		curr_input_file = strdup(m_bstrInputFile);
		output_filename = strdup(m_bstrOutputFile);
	}

	*lpInt = run_assembly();

	GlobalUnlock(hGlobal);

	free(curr_input_file);
	free(output_filename);

	if (m_pStmOutput != NULL)
	{
		m_pStmOutput->Release();
	}

	LPSTREAM pStream = NULL;
	HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);
	ULARGE_INTEGER ul;
	ul.QuadPart = out_ptr - output_contents;
	pStream->SetSize(ul);
	
	m_pStmOutput = pStream;

	return S_OK;
}

void CZ80Assembler::get_label_callback(label_t *label, CComSafeArray<IDispatch *> *lpsa)
{
	CComObject<CZ80Label> *pLabelObj = NULL;
	CComObject<CZ80Label>::CreateInstance(&pLabelObj);
	pLabelObj->AddRef();
	pLabelObj->Initialize(label);

	IZ80Label *pLabel;
	pLabelObj->QueryInterface(&pLabel);
	
	pLabelObj->Release();

	lpsa->SetAt(m_lIndex++, pLabel, FALSE);
}

STDMETHODIMP CZ80Assembler::get_Labels(LPSAFEARRAY *ppsa)
{
	CComSafeArray<IDispatch *> sa((ULONG) label_table->used);

	m_lIndex = sa.GetLowerBound(0);
	hash_enum(label_table, (HASH_ENUM_CALLBACK) get_label_callback, &sa);

	LPSAFEARRAY lpsa;
	sa.CopyTo(&lpsa);

	*ppsa = lpsa;
	return S_OK;
}
