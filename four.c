#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>

//  compile:  gcc -std=c99 -Wall -Werror -o percolate percolate.c
//  run:      ./percolate probability

#define L 8        	/* Linear lattice edge dimension */
#define N L*(L+2)	// lattice is rectangular during development, remove all +2's once lattice finalised

struct Node {
	int flag;
	bool visited;
	int count;
};

struct Node* lattice[L+2][L];
//struct Node* stack[N];

int countCluster = 0;
int max = 0;
int max_i = 0; 
int max_j = 0;
int orig_i = 0;
int orig_j = 0;


// this function initiates the lattice with occupied or unoccupied nodes
void seed(double probability)		// size variable will be handy later to modify lattice dimension
{
	double r;
	int seeded;
	int i, j;

	srand(time(NULL));				//should only be called once

	for (i = 0; i < (L+2); i++){			//rows
		for (j = 0; j < L; j++){			//columns

			r = (double)rand()/RAND_MAX*1.0;

			if (r<=probability) seeded = 1;
			else seeded = 0;
			
			struct Node* node = (struct Node*) malloc(sizeof(struct Node));
			node->flag = seeded;		
			node->visited = false;
			node->count = 0;
			lattice[i][j] = node;
		}
	}
}


// check print function
void printBonds(){
		
	int i,j;
	for (i = 0; i < (L+2); i++){
		for (j = 0; j < L; j++){
			if (lattice[i][j]->flag==1){
				if (lattice[i][(j+1)%L]->flag == 1) printf("%i ----- ", lattice[i][j]->flag);
				else printf("%i\t", lattice[i][j]->flag);
			}
			else printf("%i\t", lattice[i][j]->flag);
			
		}
		printf("\n");
		for (j = 0; j < L; j++){
			if (lattice[i][j]->flag==1){
	
				if (lattice[(i+1)%(L+2)][j]->flag == 1) printf("|\t");
				else printf("\t");
			}
			else printf("\t");
		}
		printf("\n");
	}
}


// basic attempt at percolation, work in progress
bool percolate(int i,int j){

	lattice[i][j]->count = countCluster;
	if (max < countCluster) max = countCluster;	
	if (i > max_i) {					//store maximum cluster coordinates
		max_i = i;
		max_j = j;
	}
	if (j > max_j) {
		max_i = i;
		max_j = j;
	}
	if (lattice[i][j]->flag == 1){
		lattice[i][j]->visited = true;
		countCluster++;
		if (i-1 < 0) i = L+2;
		if (j-1 < 0) j = L;
		printf("i: %i, j: %i\t", i, j);
		if (lattice[(i+1)%(L+2)][j]->flag == 1 && !lattice[(i+1)%(L+2)][j]->visited) (i = (i+1)%(L+2));	
		else if (lattice[i][(j+1)%L]->flag == 1 && !lattice[i][(j+1)%L]->visited) (j = (j+1)%L);
		else if (lattice[i][(j-1)%L]->flag == 1 && !lattice[i][(j-1)%L]->visited) (j = (j-1)%L);
		else if (lattice[(i-1)%(L+2)][j]->flag == 1 && !lattice[(i-1)%(L+2)][j]->visited) (i = (i-1)%(L+2));
		else return false;
	} else {
			return false;
	}
	printf("count: %i\t", countCluster);		//debugging print
	if (countCluster>=L && i==orig_i) {
		printf("\n%i,%i\n", max_i,max_j);
		return true;
	}
	else percolate(i,j);
	return false;
}

int main(int argc, char *argv[]){

    if(argc != 2)
    {
        fprintf(stderr, " ERROR\n Usage: ./one probability matrixDimension\n");
        exit(EXIT_FAILURE);             // Exit program indicating failure
    }
    else
    {	
		int m = (8)%8;
		printf("m = %i\n",m);

	    	double p = atof(argv[1]);
		bool percolatedV = false;
		//bool percolatedH = false;

		seed(p);
		printBonds();
		/*for (int j = 0; j < L; j++){		// final code will check each node along top and left edges
			count = 0; 
			max = 0;
			percolatedV = percolate(0,j);
			if (percolatedV) break;
		}
		for (int i = 0; i < (L+2); i++){
			count = 0;
			max = 0;
			percolatedH = percolate(i,0);
			if (percolatedH) break;
		}*/
		countCluster = 0;			// check percolation from origin node
		orig_i = 0;
		orig_j = 0;
		percolatedV = percolate(0,0);
		if (percolatedV) {
			printf("\nlattice can percolate with probability %f\n", p);	
		}
		if (percolatedV) {
			printf("\nlattice can percolate vertically with probability %f\n", p);
		}
		else {
			printf("\nlattice did not percolate vertically with probability %f\n", p);
			printf("maximum cluster distance: %i %i \n", max_i, max_j);
		}
		//free(lattice);
		return 0;
    }
}











