#include <msxhal.h>
#include <stdarg.h>

int kk();

int fibonaccy(int a) {
	
	if (a<2) return 1;
	return fibonaccy(a-1) + fibonaccy(a-2);
}

int kk() {
	
	static int l = 4;
	l++;
	return l;
}


