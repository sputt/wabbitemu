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
#define SPASM_ERR_UNKNOWN_OPCODE			0x113
#define SPASM_ERR_EQUATE_MISSING_LABEL		0x114
#define SPASM_ERR_EXCEEDED_RECURSION_LIMIT	0x115
#define SPASM_ERR_UNMATCHED_IF				0x116
#define SPASM_ERR_STRAY_PREOP				0x117
#define SPASM_ERR_EXPRESSION_EXPECTED		0x118
#define SPASM_ERR_SYNTAX					0x119
#define SPASM_ERR_JUMP_EXCEEDED				0x120
#define SPASM_ERR_INDEX_OFFSET_EXCEEDED		0x121
#define SPASM_ERR_NAME_EXPECTED				0x122
#define SPASM_ERR_NO_PREVIOUS_DEFINE		0x123
#define SPASM_ERR_ELIF_WITHOUT_IF			0x124

#define SPASM_ERR_INVALID_DECIMAL_DIGIT		0x200
#define SPASM_ERR_INVALID_HEX_DIGIT			0x201
#define SPASM_ERR_INVALID_BINARY_DIGIT		0x202

#define SPASM_ERR_FCREATE_NOFILE			0x301

#define SPASM_ERR_SIGNER_MISSING_LENGTH		0x501
#define SPASM_ERR_SIGNER_PRGM_TYPE			0x502
#define SPASM_ERR_SIGNER_MISSING_PAGES		0x503
#define SPASM_ERR_SIGNER_MISSING_NAME		0x504
#define SPASM_ERR_SIGNER_ROOM_FOR_SIG		0x505


#define SPASM_WARN_TRUNCATING_8				0x800
#define SPASM_WARN_TRUNCATING_16			0x801

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
	{SPASM_ERR_UNKNOWN_OPCODE,			_T("Unknown opcode '%s'")},
	{SPASM_ERR_EQUATE_MISSING_LABEL,	_T("Equate is missing corresponding label")},
	{SPASM_ERR_EXCEEDED_RECURSION_LIMIT,_T("Recursion depth limit exceeded")},
	{SPASM_ERR_EXPRESSION_EXPECTED,		_T("Expecting an expression, statement ended early")},
	{SPASM_ERR_SYNTAX,					_T("Unknown syntax")},
	{SPASM_ERR_JUMP_EXCEEDED,			_T("Relative jump distance exceeded (distance %d)")},
	{SPASM_ERR_INDEX_OFFSET_EXCEEDED,	_T("Index register offset exceeded (offset %d)")},
	{SPASM_ERR_NAME_EXPECTED,			_T("Expecting a name, expression ended early")},
	{SPASM_ERR_NO_PREVIOUS_DEFINE,		_T("No previous define to continue")},
	{SPASM_ERR_ELIF_WITHOUT_IF,			_T("Use of #ELIF outside of an #IF expression")},

	{SPASM_ERR_INVALID_DECIMAL_DIGIT,	_T("Invalid digit '%c' in the decimal number '%s'")},
	{SPASM_ERR_INVALID_HEX_DIGIT,		_T("Invalid digit '%c' in the hexadecimal number '%s'")},
	{SPASM_ERR_INVALID_BINARY_DIGIT,	_T("Invalid digit '%c' in the binary number '%s'")},

	{SPASM_ERR_FCREATE_NOFILE,			_T("No buffer was selected for the fcreate call")},
	{SPASM_ERR_UNMATCHED_IF,			_T("Unbalanced #IF/#ENDIF")},
	{SPASM_ERR_STRAY_PREOP,				_T("Stray #%s")},

	{SPASM_ERR_SIGNER_MISSING_LENGTH,	_T("Length field missing")},
	{SPASM_ERR_SIGNER_PRGM_TYPE,		_T("Program type field missing or incorrect")},
	{SPASM_ERR_SIGNER_MISSING_PAGES,	_T("Page count field missing")},
	{SPASM_ERR_SIGNER_MISSING_NAME,		_T("Name field missing")},
	{SPASM_ERR_SIGNER_ROOM_FOR_SIG,		_T("Not enough room for signature on last page")},

	{SPASM_WARN_TRUNCATING_8,			_T("Value too large for 8-bits, truncation required")},
	{SPASM_WARN_TRUNCATING_16,			_T("Value too large for 16-bits, truncation required")},
}
#endif
;

void SetLastSPASMError(DWORD dwErrorCode, ...);
void SetLastSPASMWarning(DWORD dwErrorCode, ...);
//DWORD GetLastSPASMError();
int StartSPASMErrorSession(void);
int GetSPASMErrorSessionErrorCount(int nSession);
bool IsSPASMErrorSessionFatal(int nSession);
void ReplaySPASMErrorSession(int nSession);
void EndSPASMErrorSession(int nSession);
void ClearSPASMErrorSessions(void);
void AddSPASMErrorSessionAnnotation(int nSession, LPCTSTR lpszFormat, ...);
bool IsErrorInSPASMErrorSession(int nSession, DWORD dwErrorCode);
#ifdef _TEST
DWORD GetLastSPASMError();
#endif