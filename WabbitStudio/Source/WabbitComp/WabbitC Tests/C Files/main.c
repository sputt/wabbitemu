#include <stdlib.h>

#ifndef TEST_FUNCTION
#define TEST_FUNCTION(z)
#endif

#define STRINGIFY2(z) #z
#define STRINGIFY(z) STRINGIFY2(z)

#if PASS_COUNT > 2
void TEST_FUNCTION();
#else
int TEST_FUNCTION(int);
#endif

extern unsigned char __stack[2048];
extern unsigned char* __sp;
extern int __hl;

int main(int argc, char **argv)
{
	int result;
	__hl = 0xDEADBEEF;
	__sp = __stack + sizeof(__stack) - (2 * sizeof(int));
	*((int *) &__stack[sizeof(__stack) - sizeof(int)]) = atoi(argv[1]);
#if PASS_COUNT > 2
	TEST_FUNCTION();
#else
	result = TEST_FUNCTION(atoi(argv[1]));
#endif
	return (__hl == 0xDEADBEEF) ? result : __hl;
}
