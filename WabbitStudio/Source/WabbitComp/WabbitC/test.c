/*char ToLower(char ch) {
	if (ch >= 'A' && ch <= 'Z')
		return ch + 'a' - 'A';
	return ch;
}*/

int function(int arg)
{
	return 20 + arg;
}

int test()
{
	int arg;
	int arg2;

	arg = arg + 20 + arg2 + function(45);
	return arg;
}
