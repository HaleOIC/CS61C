/**
 *@file Ex5.c
 *@author ShineHale
 *@date 2023-03-07
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>



int main(int argc, char* argv[]){
	printf("The corresponding distribution is :\n");
	int a[10] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 489};
	int i = 0;
	for ( i = 0; i < 10; i++ ) printf("%d ", a[i]);
	printf("\n");
    return 0;
}

