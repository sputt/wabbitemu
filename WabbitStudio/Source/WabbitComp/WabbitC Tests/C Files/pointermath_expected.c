#include "wabbitc.h"

int pointermath(int arg)
{
	int *test;
	test = 0;
	test += arg;
	return (int) test;
}
