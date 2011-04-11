#include "wabbitc.h"

int cast(int arg)
{
	int total;
	int result;
	total = arg * 40;

	result = (unsigned char) total;
	return result;
}