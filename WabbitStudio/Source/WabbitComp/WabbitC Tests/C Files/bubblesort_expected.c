#include "wabbitc.h"

int BubbleSortAlgorithm(int *a, int n)
{
	int i = 1;
	int j;
	int tmp;

	if (n <= 1) 
		return -1;
	do
	{
		for (j = n - 1; j >= i; --j) {
			if (a[j - 1] > a[j]) {
				tmp = a[j - 1];
				a[j - 1] = a[j]; 
				a[j] = tmp;
			}
		}
	} while (++i < n);
	return 0;
}

int BubbleSort(int n) {
	int i;
	int a[10];
	for(i = 0; i < 10; ++i) {
		a[i] = 10 - i;
	}
	BubbleSortAlgorithm(a, n);
	return a[0];
}