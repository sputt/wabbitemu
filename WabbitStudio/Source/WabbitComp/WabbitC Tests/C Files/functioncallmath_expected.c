int function1(int arg)
{
	return arg + arg;
}

int function2(int arg)
{
	return arg + (arg + arg);
}

int functioncallmath(int arg)
{
	return function1(arg) + function2(arg);
}