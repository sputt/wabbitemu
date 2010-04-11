#ifndef CORETYPES_H_
#define CORETYPES_H_

#define NumElm(array) (sizeof (array) / sizeof ((array)[0]))

#include <stdint.h>

#ifdef WINVER
#include <winsock.h>
#else
#include <sys/types.h>
#endif


#ifndef TRUE
#define FALSE (0)
#define TRUE (!FALSE)
typedef int BOOL;
#endif

#include <stdio.h>

#endif /*CORETYPES_H_*/
