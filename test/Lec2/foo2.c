/**
 *@file foo2.c
 *@author ShineHale
 *@date 2023-03-07
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>



int main(int argc, char* argv[]){
	int x[5] = { 2, 4, 6, 8, 10 };
	int *p = x;
	int **pp = &p;
	(*pp)++;
	(*(*pp))++;
	printf("%d\n", *p);

    return 0;
}

