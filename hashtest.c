#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#include "uthash.h"
#define L 10     	/* Linear edge dimension of map */
#define N L*L		// total number of nodes in a single map

void testPass(int **A, int size){
	int a =sizeof(*A);
	int b = sizeof(A[0]);
	int c = sizeof(A[0][0]);
	printf(":%i\n",A[0][0]);
	for(int **B=A; B!=A+size;B++){
		for(int *j=*B; j!=*B+L;j++){
			printf(":%i",j[0]);
		}printf(":\n");
	}
	printf("\n%i, %i, %i\n", a,b,c);
}
int main(int argc, char *argv[]){
	int ** A;
	A=calloc(3,sizeof(int *));
	for(int i=0;i<3;i++){
		A[i]=calloc(L, sizeof(int));
	}
	testPass(A, 3);
	
	return 2;
}