typedef void (*fp)(void *, char *);

int cast(int arg)
{
	void *test = (void (*(*)[32])()) arg;
	return test;
}