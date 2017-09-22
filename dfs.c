#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>

//  compile:  gcc -std=c99 -Wall -Werror -o percolate percolate.c
//  run:      ./percolate probability

#define L 8        	/* Linear edge dimension of lattice */
#define N L*L		// total number of nodes in a single lattice

struct Node {
	int flag;			// occupied=1, unoccupied=0
	int up;				// (0=not visited,1=visited,2=all directions visited,-1=when parent node)	
	int down;
	int left;
	int right;
	int x;				//co-ords to be easily accessible by cluster
	int y;
};

struct Node* lattice[L][L];

struct Stack{
	int x;
	int y;
};

struct Stack cluster[N];

int top = 0;

/* 	this functions seeds the lattice with occupied or unoccupied nodes depending on probability entered
	by the user and initialises all node variables with 0. 
*/
void seed(double probability)
{
	double r;
	int seeded;
	int i, j;

	srand(time(NULL));				//should only be called once

	for (i = 0; i < (L); i++){			//rows
		for (j = 0; j < L; j++){		//columns

			r = (double)rand()/RAND_MAX*1.0;
			printf("pr: %f\t", r);				//check print of probability values
			if (r<=probability) seeded = 1;
			else seeded = 0;
			
			struct Node* node = (struct Node*) malloc(sizeof(struct Node));
			node->flag = seeded;		
			node->up = 0;
			node->down = 0;
			node->left = 0;
			node->right = 0;
			node->x = j;
			node->y = i;			
			lattice[i][j] = node;

/*the following indicates that the top row of the group need not search up, and the bottom not search down, likewise left column need not search left or right column search right.*/

			if(i==0){ 
				lattice[i][j]->up = 2;
			}
			else if(i == L-1){
				lattice[i][j]->down = 2;
			}
			else if(j == 0){
				lattice[i][j]->left = 2;
			}
			else if(j == L-1){
				lattice[i][j]->right = 2;
			}

		}
	printf("\n");
	}
}


void printBonds(){
		
	int i,j;
	for (i = 0; i < (L); i++){
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
	
				if (lattice[(i+1)%L][j]->flag == 1) printf("|\t");
				else printf("\t");
			}
			else printf("\t");
		}
		printf("\n");
	}
}

// helper functions for depth first search

void push(int i, int j){
	cluster[top].x = j;
	cluster[top].y = i;
	++top;
}

struct Stack pop(){
	return cluster[top--];
}

struct Stack peek(){
	return cluster[top];
}

bool isStackEmpty(){
	return top == -1;
}

void deadEnd(int i, int j){					//function to change paths to level 2 -> no more options
	int dirx;
	int diry;

	if (top>0){
		dirx = cluster[top-1].x;
		diry = cluster[top-1].y;
	}   
	//only change the single path moving backwards to previous cluster node
	if (dirx-j == 1) lattice[i][j]->right = 2;		
	if (dirx-j == -1) lattice[i][j]->left = 2;
	if (diry-i == 1) lattice[i][j]->down = 2;
	if (diry-i == -1) lattice[i][j]->up = 2;
}

void displayNode(){			
	printf("(%i, %i)\t", peek().x, peek().y);		//prints out lattice co-ordinate
}

// determines next node in the cluster
// if no more paths from the current node returns previous node

struct Node * getNextNode(int nodeIndex) {			
		
		int j = cluster[nodeIndex].x ;
		int i = cluster[nodeIndex].y ;
	
		if (i-1 < 0) i = L;
		if (j-1 < 0) j = L;
		printf("i: %i, j: %i\t", i, j);
		if (lattice[(i+1)%L][j]->flag == 1 && lattice[(i+1)%L][j]->down < 2) (i = (i+1)%L);	
		else if (lattice[i][(j+1)%L]->flag == 1 && lattice[i][(j+1)%L]->right < 2) (j = (j+1)%L);
		else if (lattice[i][(j-1)%L]->flag == 1 && lattice[i][(j-1)%L]->left < 2) (j = (j-1)%L);
		else if (lattice[(i-1)%L][j]->flag == 1 && lattice[(i-1)%L][j]->up < 2) (i = (i-1)%L);
		else {
			deadEnd(cluster[nodeIndex].y, cluster[nodeIndex].x);	// changes path status to 2
			pop();
			return lattice[cluster[top].y][cluster[top].x];
		}

	return lattice[i][j];
}

/* need to finish this function: need to check
1. cluster spans all the rows
2. cluster spans all the columns
3. cluster spans both rows and columns
*/
bool percolated(){
	if (
	return false;
}


// need a function to retain edge nodes which the cluster reaches
void boundaries(){

}

bool depthFirstSearch(int i, int j){

	int q = 0;
	struct Node* temp[L][L];
	
	while(!isStackEmpty() || !percolated()){
	
		cluster[q].x = lattice[i][j]->x;
		cluster[q].y = lattice[i][j]->y;

		displayNode(q);								// print check of each node in the cluster
		push(cluster[q].x, cluster[q].y);			//push node into the stack, q is the current id

		temp[0][0] = getNextNode(q);	
		i = temp[0][0]->y;
		j = temp[0][0]->x;		

	}	

	return false;

}

int main(int argc, char *argv[]){

    if(argc != 2)
    {
        fprintf(stderr, " ERROR\n Usage: ./one probability \n");
        exit(EXIT_FAILURE);             // Exit program indicating failure
    }
    else
    {	
	    double p = atof(argv[1]);

		seed(p);
		printBonds();

		for (int i = 0; i < L; i++){		//DFS of all rows from the left edge
			depthFirstSearch(i,0);
		}

		for (int j = 0; j < L; j++){		//DFS of all columns from the top edge
			depthFirstSearch(0,j);
		}

		return 0;
    }
}











