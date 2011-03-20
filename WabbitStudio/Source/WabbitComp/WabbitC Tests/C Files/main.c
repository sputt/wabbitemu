#ifndef TEST_FUNCTION
#define TEST_FUNCTION(z)
#endif

#define STRINGIFY2(z) #z
#define STRINGIFY(z) STRINGIFY2(z)

int TEST_FUNCTION(int);

int main(int argc, char **argv)
{
	int result = TEST_FUNCTION(atoi(argv[1]));
	return result;
}
