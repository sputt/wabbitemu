#pragma once

typedef struct tagSPASMERROR
{
	DWORD dwCode;
	LPCTSTR lpszDescription;
}
SPASMERROR, *LPSPASMERROR;

#define SPASM_ERR_SUCCESS					0x000
#define SPASM_ERR_FILE_NOT_FOUND			0x001
#define SPASM_ERR_NO_ACCESS					0x002

#define SPASM_ERR_LOCAL_LABEL_FORWARD_REF	0x100
#define SPASM_ERR_LOCAL_LABEL_SYNTAX		0x101
#define SPASM_ERR_ARG_USED_WITHOUT_VALUE	0x102
#define SPASM_ERR_OPERATOR_EXPECTED			0x103
#define SPASM_ERR_VALUE_EXPECTED			0x104
#define SPASM_ERR_BAD_VALUE_PREFIX			0x105
#define SPASM_ERR_LABEL_NOT_FOUND			0x106
#define SPASM_ERR_INVALID_ADDRESS			0x107
#define SPASM_ERR_SIZE_MUST_BE_POSITIVE		0x108
#define SPASM_ERR_FILENAME_EXPECTED			0x109
#define SPASM_ERR_INVALID_OPERANDS			0x110
#define SPASM_ERR_UNKNOWN_PREOP				0x111
#define SPASM_ERR_UNKNOWN_DIRECTIVE			0x112

#define SPASM_ERR_INVALID_DECIMAL_DIGIT		0x200
#define SPASM_ERR_INVALID_HEX_DIGIT			0x201
#define SPASM_ERR_INVALID_BINARY_DIGIT		0x202

#ifndef _ERRORS_CPP
extern
#endif
SPASMERROR g_ErrorCodes[]
#ifdef _ERRORS_CPP
=
{
	{SPASM_ERR_FILE_NOT_FOUND,			_T("Could not find the file '%s'")},
	{SPASM_ERR_NO_ACCESS,				_T("Could not access the file '%s'")},

	{SPASM_ERR_LOCAL_LABEL_SYNTAX,		_T("Error in local label's syntax (had leading +/- but no _)")},
	{SPASM_ERR_LOCAL_LABEL_FORWARD_REF,	_T("Reference to local label which doesn't exist")},
	{SPASM_ERR_ARG_USED_WITHOUT_VALUE,	_T("Argument '%s' used without value")},
	{SPASM_ERR_OPERATOR_EXPECTED,		_T("Expecting an operator, found '%c' instead")},
	{SPASM_ERR_VALUE_EXPECTED,			_T("Expecting a value, expression ended early")},
	{SPASM_ERR_LABEL_NOT_FOUND,			_T("Could not find label or macro '%s'")},
	{SPASM_ERR_BAD_VALUE_PREFIX,		_T("Unrecognized value prefix '%c'")},
	{SPASM_ERR_INVALID_ADDRESS,			_T("The value '%s' is not a valid Z80 address")},
	{SPASM_ERR_SIZE_MUST_BE_POSITIVE,	_T("The value '%s' is a size and must be positive")},
	{SPASM_ERR_FILENAME_EXPECTED,		_T("Expecting a filename, none was provided")},
	{SPASM_ERR_INVALID_OPERANDS,		_T("The opcode %s was given invalid operands")},
	{SPASM_ERR_UNKNOWN_PREOP,			_T("Unknown preprocessor command '#%s'")},
	{SPASM_ERR_UNKNOWN_DIRECTIVE,		_T("Unknown assembler directive '.%s'")},

	{SPASM_ERR_INVALID_DECIMAL_DIGIT,	_T("Invalid digit '%c' in the decimal number '%s'")},
	{SPASM_ERR_INVALID_HEX_DIGIT,		_T("Invalid digit '%c' in the hexadecimal number '%s'")},
	{SPASM_ERR_INVALID_BINARY_DIGIT,	_T("Invalid digit '%c' in the binary number '%s'")},

}
#endif
;

void SetLastSPASMError(DWORD dwErrorCode, ...);
DWORD GetLastSPASMError();
int StartSPASMErrorSession(void);
int GetSPASMErrorSessionErrorCount(int nSession);
bool IsSPASMErrorSessionFatal(int nSession);
void ReplaySPASMErrorSession(int nSession);
void EndSPASMErrorSession(int nSession);
void ClearSPASMErrorSessions(void);