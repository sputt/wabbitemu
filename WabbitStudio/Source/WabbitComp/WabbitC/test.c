int test_function(int arg)
{
	return arg - 1;
}

int test()
{
	int i;
	int var = 0;
	i = 0;
	while (test_function(i) - 6)
	{
		var += 20;
		i += 1;
	}
	return var;
}
