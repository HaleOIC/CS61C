/**
 *@file foo1.c
 *@author ShineHale
 *@date 2023-03-07
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>


void foo(int *x, int *y) {
	int t;
	if (*x > *y) {
		t = *y;
		*y = *x;
		*x = t;
	}
}

int main(int argc, char* argv[]){
    int a = 3, b = 2, c = 1;
	foo(&a, &b);
	printf("a = %d, b = %d, c = %d\n", a, b, c);
	foo(&b, &c);
	printf("a = %d, b = %d, c = %d\n", a, b, c);
	foo(&a, &b);
	printf("a = %d, b = %d, c = %d\n", a, b, c);
    return 0;
}

