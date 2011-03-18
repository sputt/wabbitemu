#include <stdio.h>
#include <assert.h>

#ifndef TEST_FUNCTION
#define TEST_FUNCTION(z) assert("No test function given", 0)
#endif

#define STRINGIFY2(z) #z
#define STRINGIFY(z) STRINGIFY2(z)

int TEST_FUNCTION(int);

int main(int argc, char **argv)
{
	int result = TEST_FUNCTION(atoi(argv[1]));
	printf("Result: %s(%d) = %d\n", STRINGIFY(TEST_FUNCTION), atoi(argv[1]), result);
	return result;
}
