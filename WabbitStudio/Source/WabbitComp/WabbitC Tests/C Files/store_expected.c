#include "wabbitc.h"

int store(int arg)
{
	int buffer[32];
	int i;
	int total;

	for (i = 0; i < arg; i += 1)
	{
		buffer[i] = i;
	}

	total = 0;
	for (i = 0; i < arg; i += 1)
	{
		total += buffer[i];
	}

	return total;
}