#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>

//  compile:  gcc -std=c99 -Wall -Werror -o percolate percolate.c
//  run:      ./percolate probability

#define L 6        	/* Linear edge dimension of map */
#define N L*L		// total number of nodes in a single map

struct Node {
	int flag;			// occupied=1, unoccupied=0
	int up;				// (0=not visited,1=visited,2=all directions visited,-1=when parent node)	
	int down;
	int left;
	int right;
	int x;				//co-ords to be easily accessible by cluster
	int y;
};

struct Node* map[L][L];

struct Stack{
	int x;
	int y;
	bool up;
	bool down;
	bool left;
	bool right;
};

struct Stack* cluster[N];

int top = 0;
bool popped = false;


/* 	this functions seeds the map with occupied or unoccupied nodes depending on probability entered
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
			//printf("pr: %f\t", r);				//check print of probability values
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
			map[i][j] = node;

/*the following indicates that the top row of the group need not search up, and the bottom not search down, likewise left column need not search left or right column search right.*/

			if(i==0){ 
				map[i][j]->up = 3;
			}
			else if(i == L-1){
				map[i][j]->down = 3;
			}
			else if(j == 0){
				map[i][j]->left = 3;
			}
			else if(j == L-1){
				map[i][j]->right = 3;
			}
		}
	//printf("\n");
	}
}


void printBonds(){
		
	int i,j;
	for (i = 0; i < (L); i++){
		for (j = 0; j < L; j++){
			if (map[i][j]->flag==1){
				if (map[i][(j+1)%L]->flag == 1) printf("%i ----- ", map[i][j]->flag);
				else printf("%i\t", map[i][j]->flag);
			}
			else printf("%i\t", map[i][j]->flag);
			
		}
		printf("\n");
		for (j = 0; j < L; j++){
			if (map[i][j]->flag==1){
	
				if (map[(i+1)%L][j]->flag == 1) printf("|\t");
				else printf("\t");
			}
			else printf("\t");
		}
		printf("\n");
	}
}

// helper functions for depth first search

void push(int i, int j){
	cluster[top]->x = j;
	cluster[top]->y = i;
	popped = false;
	printf("top %i\t",top);
}

struct Stack * pop(){
	popped = true;
	return cluster[top--];
}

struct Stack * peek(){
	return cluster[top];
}

bool isStackEmpty(){
	return top == -1;
}

void deadEnd(int i, int j){					//function to change paths to level 3 -> no more options

	map[i][j]->right = 3;			// change entire node to 3 if it's a dead end
	map[i][j]->left = 3;
	map[i][j]->down = 3;
	map[i][j]->up = 3;
}

void displayNode(){			
	printf("(%i, %i)\t", peek()->y, peek()->x);		//prints out map co-ordinate
}

// determines next node in the cluster
// if no more paths from the current node returns previous node

struct Node * getNextNode() {			
		popped = false;
		
		int j = peek()->x ;
		int i = peek()->y ;
		printf("check1 (%i, %i)\t",i,j);	

		//struct node active = map[i][j];

		/*struct Stack* temp[1];
		struct Stack* tempStack = (struct Stack*) malloc(sizeof(struct Stack));
		tempStack->x = 0;
		tempStack->y = 0;
		temp[0] = tempStack;*/
	

		// make sure map doesn't return to previous node without exploring other options first
		// ie. a continuous square will keep going round
		// ** at the moment going back and forth between 2 nodes and not reaching else clause
		// also never traversing upwards!!!!!!!!!!!!!
		// booleans used to indicate don't go back to previous node, should pop stack

		if (!cluster[top]->up && (i+1 < L) && map[i+1][j]->flag == 1 && map[i+1][j]->up == 0) {
				(i = (i+1)%L);	
				cluster[top]->down = true;		
		}
		else if (!cluster[top]->left && (j+1 < L) && map[i][j+1]->flag == 1 && map[i][j+1]->left == 0){
			(j = (j+1)%L);
			cluster[top]->right = true;
		}
		else if (!cluster[top]->right && (j-1 >= 0) && (map[i][j-1]->flag == 1) 
			&& (map[i][(j-1)%L]->right == 0)) {
				(j = (j-1)%L);
				cluster[top]->left = true;
		}
		else if (!cluster[top]->down && (i-1 >= 0) && (map[i-1][j]->flag == 1) 
			&& (map[(i-1)%L][j]->down == 0)) {
				(i = (i-1)%L);
				cluster[top]->up = true;
		}		
		else {
			deadEnd(peek()->y, peek()->x);			// changes path status to 2
			pop();									// reduces top index by 1 
			if (top == -1) return map[i][j];
			printf("top2= %i\t,",top);				//top should == -1 if the first node was 0 but it's 
													//not getting here, then if top==-1 the DFS exits
			printf("top-1 (%i, %i)\t",cluster[top]->y,cluster[top]->x);	

			return map[cluster[top]->y][cluster[top]->x];	
						// if top == -1 then it's trying to access a negative array num: poss seg fault
		}
		printf("check2 (%i, %i)\t",i,j);	
		/*printf("down = %s\n", cluster[top]->down ? "true" : "false");
		printf("up = %s\n", cluster[top]->up ? "true" : "false");
		printf("left = %s\n", cluster[top]->left ? "true" : "false");
		printf("right = %s\n", cluster[top]->right ? "true" : "false");*/

	return map[i][j];
}

/* need to finish this function: need to check
1. cluster spans all the rows
2. cluster spans all the columns
3. cluster spans both rows and columns
*/
bool percolated(int i, int j){
	
	return false;
}

// need a function to retain edge nodes which the cluster reaches
void boundaries(){

}

bool depthFirstSearch(int i, int j){
	printf("reaching here3\n");
	// initialise cluster stack

	top = 0;
	
	if (map[i][j]->flag == 0) return false;			// check origin flag before commencing DFS
	//int count = 0;
	while(!isStackEmpty() || !percolated(i,j)){
		//count++;
		// must initialise stack here or every cluster array points to the same address (and same value)
		// not sure if this is working
		struct Stack* stack = (struct Stack*) malloc(sizeof(struct Stack));

		stack->x = 0;
		stack->y = 0;	
		stack->up = false;
		stack->down = false;
		stack->left = false;
		stack->right = false;
		cluster[top] = stack;

		//initialise temporary holding stack
		struct Node* temp[1][1];
		struct Node* tempNode = (struct Node*) malloc(sizeof(struct Node));
		tempNode->x = 0;
		tempNode->y = 0;
		temp[0][0] = tempNode;
	
		printf("reaching here6\n");
		cluster[top]->x = map[i][j]->x;				//store co-ordinates in the cluster stack
		cluster[top]->y = map[i][j]->y;

		displayNode();									// print check of top node in the cluster
		push(cluster[top]->y, cluster[top]->x);			// push node into the stack
		
		temp[0][0] = getNextNode();						// allocate new node coords
		i = temp[0][0]->y;
		j = temp[0][0]->x;		
		printf("check3 (%i, %i)\t",i,j);	

		if (!popped) ++top;								//want to go back to previous node if no connection made

	}	
	/*for (int k = 0; k< count; k++){
		printf("cluster[%i] = (%i, %i)\n", k, cluster[k]->y, cluster[k]->x);		//prints out map co-ordinate
	}*/
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
		bool dfs;

		seed(p);
		printBonds();
		dfs = depthFirstSearch(0,0);
		printf("dfs origin = %s\n", dfs ? "true" : "false");

		/*for (int i = 0; i < L; i++){		//DFS of all rows from the left edge
			depthFirstSearch(i,0);
		}

		for (int j = 0; j < L; j++){		//DFS of all columns from the top edge
			depthFirstSearch(0,j);
		}*/


		return 0;
    }
}




