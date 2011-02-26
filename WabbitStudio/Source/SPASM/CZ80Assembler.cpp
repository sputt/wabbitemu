#include "stdafx.h"

#include "CZ80Assembler.h"
#include "spasm.h"
#include "utils.h"
#include "storage.h"
#include "list.h"

CZ80Assembler::CZ80Assembler()
{
	printf("Constructing Z80 Assembler\n");
	m_bstrInput = NULL;
}

CZ80Assembler::~CZ80Assembler()
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
	return S_OK;
}

STDMETHODIMP CZ80Assembler::Assemble(void)
{
	mode = MODE_NORMAL | MODE_COMMANDLINE;
	curr_input_file = strdup("COM Interface");
	output_filename = strdup("C:\\users\\spencer\\desktop\\test.bin");

	CW2CT szInput(m_bstrInput);
	input_contents = szInput;

	init_storage();

	run_assembly();
	return S_OK;
}