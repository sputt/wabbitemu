// Z80Assembler.cpp : Implementation of CZ80Assembler

#include "stdafx.h"
#include <comdef.h>
#include "Z80Assembler.h"

#include "spasm.h"
#include "utils.h"
#include "storage.h"
#include "list.h"

// CZ80Assembler

HRESULT CZ80Assembler::FinalConstruct()
{
	printf("Constructing Z80 Assembler\n");
	m_pStmOutput = NULL;
	return S_OK;
}

void CZ80Assembler::FinalRelease()
{
	printf("Destructing Z80 Assembler\n");
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
	*lpbstrInputFile = m_bstrInputFile;
	return S_OK;
}

STDMETHODIMP CZ80Assembler::put_OutputFile(BSTR bstrOutputFile)
{
	m_bstrOutputFile = bstrOutputFile;
	return S_OK;
}

STDMETHODIMP CZ80Assembler::get_OutputFile(LPBSTR lpbstrOutputFile)
{
	*lpbstrOutputFile = m_bstrOutputFile;
	return S_OK;
}


STDMETHODIMP CZ80Assembler::ClearDefines()
{
	list_free(default_defines, true);
	default_defines = NULL;
	return S_OK;
}

STDMETHODIMP CZ80Assembler::AddDefine(BSTR bstrName, VARIANT varValue)
{
//	default_define_pair_t *ddp = (default_define_pair_t*)malloc_chk (sizeof(*ddp));
//	strncpy(ddp->name, lpName, sizeof(ddp->name));
//	if (lpValue == NULL) {
//		strcpy(ddp->value, "1");
//	} else {
//		strncpy(ddp->value, lpValue, sizeof(ddp->value));
//	}
//	default_defines = list_append(default_defines, ddp);
//#ifdef LOG
//	fprintf(logfile, "AddDefine: %s %d\n", ddp->name, ddp->value);
//	fflush(logfile);
//#endif
//	return 0;
	return E_NOTIMPL;
}

STDMETHODIMP CZ80Assembler::ClearIncludeDirectories()
{
	list_free(include_dirs, true);
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

	init_storage();
	*lpInt = run_assembly();

	free_storage();

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