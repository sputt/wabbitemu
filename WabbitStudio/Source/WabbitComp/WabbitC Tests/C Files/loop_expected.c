int loop(int arg)
{
	int total;
	int i;

	total = 0;
	i = 0;
	for (i = 0; i < arg; i+= 1)
	{
		total += i;
	}

	return total;
}