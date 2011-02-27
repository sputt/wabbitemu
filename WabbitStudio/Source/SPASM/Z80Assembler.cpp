// Z80Assembler.cpp : Implementation of CZ80Assembler

#include "stdafx.h"
#include "Z80Assembler.h"

#include "spasm.h"
#include "utils.h"
#include "storage.h"
#include "list.h"

// CZ80Assembler

HRESULT CZ80Assembler::FinalConstruct()
{
	printf("Constructing Z80 Assembler\n");
	m_bstrInput = NULL;
	return S_OK;
}

void CZ80Assembler::FinalRelease()
{
	printf("Destructing Z80 Assembler\n");
	if (m_bstrInput != NULL)
	{
		SysFreeString(m_bstrInput);
	}
}

STDMETHODIMP CZ80Assembler::put_Input(BSTR bstrInput)
{
	if (m_bstrInput != NULL)
	{
		SysFreeString(m_bstrInput);
	}
	m_bstrInput = bstrInput;
	return S_OK;
}

STDMETHODIMP CZ80Assembler::get_Output(IStream **ppOutput)
{
	if (ppOutput == NULL)
	{
		return E_INVALIDARG;
	}

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, out_ptr - output_contents);
	LPBYTE lpBytes = (LPBYTE) GlobalLock(hGlobal);
	memcpy(lpBytes, output_contents, out_ptr - output_contents);
	GlobalUnlock(hGlobal);

	LPSTREAM pStream = NULL;
	HRESULT hr = CreateStreamOnHGlobal(hGlobal, FALSE, &pStream);
	ULARGE_INTEGER ul;
	ul.QuadPart = out_ptr - output_contents;
	pStream->SetSize(ul);
	if (SUCCEEDED(hr))
	{
		*ppOutput = pStream;
	}
	return hr;
}

STDMETHODIMP CZ80Assembler::Assemble(void)
{
	if (output_contents == NULL)
	{
		output_contents = (unsigned char *) malloc(OUTPUT_BUF_SIZE);
	}

	mode = MODE_NORMAL | MODE_COMMANDLINE;
	curr_input_file = strdup("COM Interface");
	output_filename = strdup("C:\\users\\spencer\\desktop\\test.bin");

	CW2CT szInput(m_bstrInput);
	input_contents = szInput;

	init_storage();

	run_assembly();
	return S_OK;
}