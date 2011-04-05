#include <stdlib.h>

#ifndef TEST_FUNCTION
#define TEST_FUNCTION(z)
#endif

#define STRINGIFY2(z) #z
#define STRINGIFY(z) STRINGIFY2(z)

int TEST_FUNCTION(int);

extern unsigned char __stack[512];
extern unsigned char* __sp;
extern int __hl;

int main(int argc, char **argv)
{
	int result;
	__hl = 0xDEADBEEF;
	__sp = __stack + sizeof(__stack) - sizeof(int);
	*((int *) &__stack[sizeof(__stack) - sizeof(int)]) = atoi(argv[1]);
	result = TEST_FUNCTION(atoi(argv[1]));
	return (__hl == 0xDEADBEEF) ? result : __hl;
}
