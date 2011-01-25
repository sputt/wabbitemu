#include "stdafx.h"

#define _ERRORS_CPP
#include "Errors.h"

#include "spasm.h"
#include "console.h"
#include "list.h"

typedef struct tagERRORINSTANCE
{
	LPSTR lpszFileName;
	int line_num;
	DWORD dwErrorCode;
	int nSession;
	bool fSuppressErrors;
	bool fWasPrinted;
	LPTSTR lpszErrorText;
} ERRORINSTANCE, *LPERRORINSTANCE;

static DWORD g_dwLastError;
static list_t *g_ErrorList;
static int g_nErrorSession = 0;

static void PrintSPASMError(const LPERRORINSTANCE lpError)
{
	assert(lpError != NULL);
	if (lpError->dwErrorCode != SPASM_ERR_SUCCESS)
	{
		set_console_attributes(COLOR_RED);
		printf("%s\n", lpError->lpszErrorText);
	}
}

int StartSPASMErrorSession(void)
{
	LPERRORINSTANCE lpLastInstance = (LPERRORINSTANCE) g_ErrorList->data;
	if (lpLastInstance->nSession == -1)
	{
		lpLastInstance->nSession = g_nErrorSession;
		lpLastInstance->fSuppressErrors = suppress_errors;
	}
	else
	{
		LPERRORINSTANCE lpErr = (LPERRORINSTANCE) malloc(sizeof(ERRORINSTANCE));
		lpErr->dwErrorCode = SPASM_ERR_SUCCESS;
		lpErr->line_num = -1;
		lpErr->lpszFileName = NULL;
		lpErr->fSuppressErrors = suppress_errors;
		lpErr->nSession = g_nErrorSession;
		g_ErrorList = list_prepend(g_ErrorList, lpErr);
	}
	suppress_errors = true;
	return g_nErrorSession++;
}

int GetSPASMErrorSessionErrorCount(int nSession)
{
	int nCount = 0;
	list_t *pList = g_ErrorList;
	while (((LPERRORINSTANCE) pList->data)->nSession != nSession)
	{
		pList = pList->next;
		nCount++;
	}
	assert(pList != NULL);
	return nCount;
}

bool IsSPASMErrorSessionFatal(int nSession)
{
	bool fIsFatal = false;
	list_t *pList = g_ErrorList;
	while (((LPERRORINSTANCE) pList->data)->nSession != nSession)
	{
		DWORD dwError = ((LPERRORINSTANCE) pList)->dwErrorCode;
		if (!(dwError == SPASM_ERR_LOCAL_LABEL_FORWARD_REF ||
			  dwError == SPASM_ERR_LABEL_NOT_FOUND ||
			  dwError == SPASM_ERR_SUCCESS))
		{
			fIsFatal = true;
			break;
		}
		pList = pList->next;
	}
	assert(pList != NULL);
	return fIsFatal;
}

void ReplaySPASMErrorSession(int nSession)
{
	list_t *pList = g_ErrorList;
	while (((LPERRORINSTANCE) pList->data)->nSession != nSession)
	{
		((LPERRORINSTANCE) pList->data)->fWasPrinted = true;
		pList = pList->next;
		assert(pList != NULL);
	}

	if (((LPERRORINSTANCE) pList->data)->fSuppressErrors == true)
	{
		return;
	}

	pList = g_ErrorList;
	while (((LPERRORINSTANCE) pList->data)->nSession != nSession)
	{
		PrintSPASMError((LPERRORINSTANCE) pList->data);
		pList = pList->next;
	}
}

void EndSPASMErrorSession(int nSession)
{
	list_t *pList = g_ErrorList;
	
	list_t *pPrev = NULL;
	while (((LPERRORINSTANCE) pList->data)->nSession != nSession)
	{
		if (((LPERRORINSTANCE) g_ErrorList->data)->fWasPrinted == false)
		{
		   if (pPrev == NULL)
		   {
			   g_ErrorList = pList->next;
		   }
		   else
		   {
			   pPrev->next = pList->next;
		   }
		}
		pPrev = pList;
		pList = pList->next;
		assert(pList != NULL);
	}
	((LPERRORINSTANCE) pList->data)->nSession = -1;
	suppress_errors = ((LPERRORINSTANCE) pList->data)->fSuppressErrors;
}


void ClearSPASMErrorSessions(void)
{
	list_free(g_ErrorList, true);
	g_nErrorSession = 0;

	LPERRORINSTANCE lpErr = (LPERRORINSTANCE) malloc(sizeof(ERRORINSTANCE));
	lpErr->dwErrorCode = SPASM_ERR_SUCCESS;
	lpErr->line_num = -1;
	lpErr->lpszFileName = NULL;
	lpErr->fSuppressErrors = false;
	lpErr->nSession = -1;

	g_ErrorList = list_prepend(NULL, lpErr);
}


void SetLastSPASMError(DWORD dwErrorCode, ...)
{
	g_dwLastError = dwErrorCode;

	if (dwErrorCode == SPASM_ERR_SUCCESS)
	{
		return;
	}

	va_list valist;
	va_start(valist, dwErrorCode);

	LPERRORINSTANCE lpErr = (LPERRORINSTANCE) malloc(sizeof(ERRORINSTANCE));
	lpErr->dwErrorCode = dwErrorCode;
	lpErr->line_num = line_num;
	lpErr->lpszFileName = strdup(curr_input_file);
	lpErr->fSuppressErrors = suppress_errors;
	lpErr->fWasPrinted = false;
	
	TCHAR szBuffer[256];
	TCHAR szDescription[128] = _T("An error occurred");

	for (int i = 0; i < ARRAYSIZE(g_ErrorCodes); i++)
	{
		if (g_ErrorCodes[i].dwCode == lpErr->dwErrorCode)
		{
			StringCchVPrintf(szDescription, ARRAYSIZE(szDescription),
				g_ErrorCodes[i].lpszDescription, valist);
			break;
		}
	}
	StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), _T("%s:%d: error SE%03X: %s"),
		lpErr->lpszFileName, lpErr->line_num, lpErr->dwErrorCode, szDescription);

	lpErr->lpszErrorText = strdup(szBuffer);

	g_ErrorList = list_prepend(g_ErrorList, (LPVOID) lpErr);

	if (suppress_errors == false)
	{
		PrintSPASMError(lpErr);
	}

	va_end(valist);
}

DWORD GetLastSPASMError()
{
	return g_dwLastError;
}
