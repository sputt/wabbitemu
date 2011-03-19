int fibonacci(int n)
{
	if (n == 0)
	{
		return 0;
	}
	else
	{
		return fibonacci(n - 2) + fibonacci(n - 1);
	}
}