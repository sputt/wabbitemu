#ifndef TYPES_H
#define TYPES_H

#ifdef WINVER
#include <windows.h>
#else
#include <sys/types.h>
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint8_t BYTE;
typedef intptr_t INT_PTR;
#endif

#endif
