#ifndef CORETYPES_H_
#define CORETYPES_H_

#define NumElm(array) (sizeof (array) / sizeof ((array)[0]))

#include <stdint.h>

#ifndef _WINDOWS
typedef char TCHAR;
typedef void *LPVOID;
typedef const char *LPCTSTR;
typedef intptr_t INT_PTR;
typedef uint8_t BYTE, *LPBYTE;
typedef uint16_t WORD, *LPWORD;
typedef uint32_t DWORD, *LPDWORD;

#ifndef MAX_PATH
#define MAX_PATH 256
#endif
#endif

#ifndef TRUE
#define FALSE (0)
#define TRUE (!FALSE)
#ifdef WINVER
typedef int BOOL;
#else
typedef signed char BOOL;
#endif
#endif

#endif /*CORETYPES_H_*/
