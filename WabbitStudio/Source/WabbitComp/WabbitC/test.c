int global;

int function(int arg)
{
	return arg;
}

void function2()
{
	global = 80;
}

int main()
{
	int result;
	if (20)
	{
		int test;
		test = 40;
		function(test);
	}
	else
	{
		function2();
		result = global;
	}

	return result;
}