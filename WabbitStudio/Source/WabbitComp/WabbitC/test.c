#include "wabbitc.h"

int factorial(int n) {
	if (n == 0) {
		return 1;
	} else {
		int factor = factorial(n - 1);
		return n * factor;
	}
}
/*int loop(int a)
{
	/*int b;
	int t1;
	int t2;
	int t3;
	int t4;
	int t5;
	int t6;
	int d;
	b = 4-2;
	t1 = b/2;
	t2 = a*t1;
	t3 = t2*b;
	t4 = t3+c;
	t5 = t2*b;
	t6 = t5+c;
	d = t4*t6;*
}*/