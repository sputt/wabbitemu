#include "stdafx.h"

#define _ERRORS_CPP
#include "Errors.h"

#include "spasm.h"
#include "console.h"

static DWORD g_dwLastError;

void SetLastSPASMError(DWORD dwErrorCode, ...)
{
	g_dwLastError = dwErrorCode;

	if (dwErrorCode == SPASM_ERR_SUCCESS)
	{
		return;
	}

	va_list valist;
	va_start(valist, dwErrorCode);

	if (suppress_errors == false)
	{
		set_console_attributes(COLOR_RED);
		printf("%s:%d: error SE%03X: ", curr_input_file, line_num, dwErrorCode);

		for (int i = 0; i < ARRAYSIZE(g_ErrorCodes); i++)
		{
			if (g_ErrorCodes[i].dwCode == dwErrorCode)
			{
				vprintf(g_ErrorCodes[i].lpszDescription, valist);
				break;
			}
		}

		printf("\n");
	}

	va_end(valist);
}

DWORD GetLastSPASMError()
{
	return g_dwLastError;
}
