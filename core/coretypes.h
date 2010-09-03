#ifndef CORETYPES_H_
#define CORETYPES_H_

#define NumElm(array) (sizeof (array) / sizeof ((array)[0]))

#ifdef WINVER
#include <stdint.h>
#else
#include <sys/types.h>
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

#include <stdio.h>

#endif /*CORETYPES_H_*/
