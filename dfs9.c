#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>

//  compile:  gcc -std=c99 -Wall -Werror -o percolate percolate.c
//  run:      ./percolate probability

#define L 10     	/* Linear edge dimension of map */
#define N L*L		// total number of nodes in a single map

struct Node {
	int flag;			// occupied=1, unoccupied=0, already in a cluster=2+
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
	char dir;
} cluster[N];

struct Percolate{
	int ID;
	int size;
	char rows; 
	char cols;
} perc[N];

// Initialising the side arrays to 0-empty. Will be filled with clusterID numbers as they are found during the search/navigation.
int upSide[L]={0};
int downSide[L]={0};
int leftSide[L]={0};
int rightSide[L]={0};

// initialise arrays to store whether cluster has spanned all rows and/or all columns
int rowPerc[L]={0};
int columnPerc[L]={0};

int top = 0;
int count = 0;
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
				map[i][j]->up = 2;
			}
			else if(i == L-1){
				map[i][j]->down = 2;
			}
			else if(j == 0){
				map[i][j]->left = 2;
			}
			else if(j == L-1){
				map[i][j]->right = 2;
			}
		}
	//printf("\n");
	}
}


void printBonds(){
		
	int i,j;
	for (i = 0; i < (L); i++){
		for (j = 0; j < L; j++){
			if (map[i][j]->flag>0){
				if (map[i][(j+1)%L]->flag > 0) printf("%i ----- ", map[i][j]->flag);
				else printf("%i\t", map[i][j]->flag);
			}
			else printf("%i\t", map[i][j]->flag);
			
		}
		printf("\n");
		for (j = 0; j < L; j++){
			if (map[i][j]->flag>0){
	
				if (map[(i+1)%L][j]->flag >0) printf("|\t");
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
	popped = false;
	printf("top %i\t",top);
}

struct Stack pop(){
	popped = true;
	return cluster[top--];
}

struct Stack peek(){
	return cluster[top];
}

bool isStackEmpty(){
	return top == -1;
}

void deadEnd(int i, int j){					//function to change paths to level 3 -> no more options

	map[i][j]->right = 2;			// change entire node to 2 if it's a dead end
	map[i][j]->left = 2;
	map[i][j]->down = 2;
	map[i][j]->up = 2;
}

void displayNode(){			
	printf("(%i, %i)\t", peek().y, peek().x);		//prints out map co-ordinate
}

// determines next node in the cluster
// if no more paths from the current node returns previous node

struct Node * getNextNode() {			
		popped = false;
		
		int j = peek().x ;
		int i = peek().y ;
		printf("check1 (%i, %i)\t",i,j);	

/*the following first checks if flag on, then path is 0 and if 1 it also checks previous direction to make sure not cycling back on itself.  If no options available then 2 is applied to the node and stack drops the top node

note that I have not assumed the lattice can cycle around itself as I think we want to retain edge nodes assuming it's a small part of a larger area - can change this easily with %L if required*/
		
	
	
		if ((i+1 < L) && map[i+1][j]->flag == 1 && map[i+1][j]->up == 0) {
				cluster[top].dir = 'd';		
				map[i][j]->down = 1;
				map[i+1][j]->up = 1;
				i = i+1;	
		}
		else if ((j+1 < L) && map[i][j+1]->flag == 1 && map[i][j+1]->left == 0){
				cluster[top].dir = 'r';		
				map[i][j]->right = 1;
				map[i][j+1]->left = 1;
				j = j+1;
		}
		else if ((j-1 >= 0) && (map[i][j-1]->flag == 1) && (map[i][j-1]->right == 0)) {
				cluster[top].dir = 'l';		
				map[i][j]->left = 1;
				map[i][j-1]->right = 1;
				j = j-1;
		}
		else if ((i-1 >= 0) && (map[i-1][j]->flag == 1) && (map[i-1][j]->down == 0)) {
				cluster[top].dir = 'u';		
				map[i][j]->up = 1;
				map[i-1][j]->down = 1;
				i = i-1;
		}		

		else if ((i+1 < L) && map[i+1][j]->flag == 1 && map[i+1][j]->up == 1 
			&& cluster[top-1].dir != 'u' && top >= 1){
				cluster[top].dir = 'd';		
				i = i+1;	
		}
		else if ((j+1 < L) && map[i][j+1]->flag == 1 && map[i][j+1]->left ==1 
			&& cluster[top-1].dir != 'l' && top >= 1){
				cluster[top].dir = 'r';		
				j = j+1;
		}
		else if ((j-1 >= 0) && (map[i][j-1]->flag == 1) && (map[i][j-1]->right == 1) 
			&& cluster[top-1].dir != 'r' && top >= 1) 			{
				cluster[top].dir = 'l';		
				j = j-1;
		}
		else if ((i-1 >= 0) && (map[i-1][j]->flag == 1) && (map[i-1][j]->down == 1) 
			&& cluster[top-1].dir != 'd' && top >= 1) {
				cluster[top].dir = 'u';		
				i = i-1;
		}

		else {
			deadEnd(peek().y, peek().x);			// changes path status to 2
			pop();	
			printf("popped check top value= %i\n",top);	 
													// reduces top index by 1 
			if (top == -1) return map[i][j];
			printf("top2= %i\t",top);				//then if top==-1 the DFS exits on next loop
			printf("top-1  (%i, %i)\t",cluster[top].y,cluster[top].x);	

			return map[cluster[top].y][cluster[top].x];	
		}
		printf("check2 (%i, %i)\t",i,j);	

	return map[i][j];
}

/*
This searches on cluster of nodes,
I've changed the return value to true if it proceeds, this will be used to let the process know that I've got another cluster to record on completion.
*/
int depthFirstSearch(int i, int j, int clusterID){ 

	//the cluster ID will be the reference point for the side-arrays, and cluster sizes.

	top = 0;
	
	if (map[i][j]->flag == 0 || map[i][j]->flag >1) return 0;		
						// check origin flag before commencing DFS, 
						//aborting if either empty or already searched.
	count = 0;

	// reset cluster percolation arrays to zero
	for (int n=0; n<L; n++){
		rowPerc[n]=0;
		columnPerc[n]=0;
	}


	while(!isStackEmpty()){  // && !percolated(i,j)){

		// include counter until percolation function finished
		//if (count >= L*L) break; //if it has already searched all nodes, it should cease work.
		
		//if(count>N){break;}			//note this will never exceed N unless probability==1 --- redundant?
		
		
		//initialise temporary holding stack
		struct Node* temp[1][1];
		struct Node* tempNode = (struct Node*) malloc(sizeof(struct Node));
		tempNode->x = 0;
		tempNode->y = 0;
		temp[0][0] = tempNode;
		
		cluster[top].x = map[i][j]->x;				//store co-ordinates in the cluster stack
		cluster[top].y = map[i][j]->y;

		rowPerc[i] = clusterID;						//store record of which row and column cluster reaches
		columnPerc[j] = clusterID;

//=============== these switches control access to the side-record arrays==================

		switch(i){
			case 0:
				upSide[j]=clusterID;
				break;
			case (L-1):
				downSide[j]=clusterID;
				break;
		}
		switch(j){
			case 0:
				leftSide[i]=clusterID;
				break;
			case (L-1):
				rightSide[i]=clusterID;
				break;
		}
		if(map[i][j]->flag == 1){
			map[i][j]->flag = clusterID; 		
				//marking the node as both explored, and to which cluster of nodes it belongs- 
				//for use in both determining largest cluster and tracking the sides of the matricies
			//rowPerc[i] = clusterID;						//store record of which row and column cluster reaches
			//columnPerc[j] = clusterID;
			count++; 						//will only update if there's a new filled node attached.
											
		}
		displayNode();									// print check of top node in the cluster
		push(cluster[top].y, cluster[top].x);			// push node into the stack
		
		temp[0][0] = getNextNode();						// allocate new node coords
		i = temp[0][0]->y;
		j = temp[0][0]->x;		
		printf("check3 (%i, %i)\n",i,j);	

		if (!popped) ++top;						//want to go back to previous node if no connection made

	}	

	for (int k = 0; k< count; k++){
		printf("cluster[%i] = (%i, %i, %c)\n", k, cluster[k].y, cluster[k].x, cluster[k].dir);		
														//prints out map co-ordinate
	}
	return count;

}
/*
will probably have to prepare some sort of thing to figure out how large the box will end up, and prepare memory locations for the side arrays.
also: since it wraps around on both sides, keeping track of the span is more important. Bother. Might be easier to quietly ignore the vertical wraparound.
*/

/*
some sort of for loop determined in master core, using the size of the unit to split up the grid with a pair of for loops: 
these loops can be used to determine which section of edge will be used for comparison next. Might be able to do this in quadrants too. 
So the four threads on the master can independantly proccess incoming data, and then have a big mash-together at the end.

*/
void cluster_combiner(){
	int A[5]; //must remember to check existance while looping through this. will cause errors otherwise.
	int B[5];
	int SA[L]={2,0,2,0,0,3,3,0,3,3};// having these as non-pointers will be faster, and probably possible, since they're pretty small.
	int SB[L]={4,4,0,2,2,0,3,0,5,5};
	int numSupers =0;
	int superclusters[L]={0};
	int superSize[L];
	for(int i=0;i<L;i++){
		if(SA[i]!=0 && SB[i]!=0){ //if neither side is empty
			if(!A[SA[i]-2]&&!B[SB[i]-2]){ //if neither side's super-link list position exists yet. (hopefully)
				A[numSupers]=numSupers;
				B[numSupers]=numSupers;
				superclusters[numSupers]=numSupers; //referencing itself, meaning that it hasn't been linked to another cluster yet.
				superSize[numSupers]=1; //will get to recording proper sizes later in development
				numSupers++;
			}else if((A[SA[i]-2]&&!B[SB[i]-2])||(!A[SA[i]-2]&&B[SB[i]-2]))
			int working =1;
			int update[L]={0}; //going to be used to keep the dependency chains as short as possible.
			
		}else if(SA[i]!=0){
			//check for supercluster, add one if missing
		}else if(SB[i]!=0){
			//ditto
		}
		//else ignore;
	}
	/*
	for ease of understanding: consider it set A and set B.
	Treating set A preferentially because it makes an easy convention.
	code logic:
	int numSupers =0; //will hold the number of created superclusters (will get to that in a moment)
	create new int[L]. <supercluster hence forth>.   the lowest indexed connected supercluster 
	
	two arrays <linkArrays> (A[], B[]) to hold supercluster links from each of the old cluster arrays. - make each size(N/2) so they can be indexed by original ID? possibly use calloc/pointers. dynamic arrays woo.
	
	
	afterwards will just append any un-rebound clusters at a new, sequential cluster ID.
	go through the paired side-arrays:
		if two non-zero matches:
			check set A&B- 
				neither has supercluster (even if one is an empty node): make new supercluster with a value the same as it's index, give both A&B the supercluster's index.
				if only one is has a binding: Follow the superclusters: i=supercluster[X] while X!=i {keep searching} give final value to new binding.
				in both bound to superclusters: 
					Check both superclusters: if value!= index ->
					create a temporary int array for each set. size L should do it.
					check supercluster[index] until value== index on each side. use the temp arrays to store the index steps that were needed.
					If they're already connected: should end up with the same number. 
					Otherwise: set the lower of the two numbers as int temp_id
					By doing this Every time it should keep the actual time spent doing this relatively short, since any long chains will be turned into short ones very quickly.
					<this bit could be parralelised>
					int i=0;
					while(temp_arrayA[i]){
						A[temp_arrayA[i]]=temp_id;
						i++;
					}
					i=0;
					while(temp_arrayB[i]){
						B[temp_arrayB[i]]=temp_id;
						i++;
					}
		
		At this stage it should have a pair of linkArrays with either NULL or an int.
		
		int count=0;
		struct cluster new_clusters[some_size];
		for each item in linkArray A then B: //
			if item (ie, !=NULL){
				int new_id= <find terminating supercluster number> -should be quick
				for each in old_side:
				new_side=new_id; <for every side>
				new_clusters->size += old_clusters[item.id]->size;
				if new_clusters[item.index]{
					if(id!=index){
						count++;
						new_clusters[count]=new_clusters[item.index];
						new_clusters[item.index]=item;
					}else{
						new_clusters[item.index]->size +=item.size;
					}
				}else{
					new_clusters[count]=item;
					count++;
				}
			}else{
				new_clusters[count]=old_clusters[item.index];
				count++;
			}
			
		
		
	
	*/
	
}

void search_manager(int p, int size){
	//TODO: migrate code from main
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
		int dfs;
		int clusterID = 2;
		char row = 'y';
		char column = 'y';

		seed(p);
		printBonds();
		
		/*
		===================================New functionality ===============================
		The DFS function return the cluster's size, so now the boolean becomes obsolete - size 0 means no new cluster.
		Rhe search pattern itself edits the boundry arrays, and they have values relating to the cluster in the places:
		eg. [0,2,2,0,0,0,4,4,0,5,0,2]
		*/
		
		/*furthermore:
		I'm also thinking on how to deal with internal nodes. we may end up checking the entire grid anyway to possibly find the largest cluster, rather than just the edges.
		On the other hand, once lim->(infinity) the likelihood of an internal cluster being the largest (compared to mulit-frame spanning clusters) approaches 0.
		On the third, mutant hand, as p-> 0 the likelyhood gets larger, since the filled nodes get more sparse
		So the current build will search every square for an un-searched node.
		*/

		
		for(int j =0; j<L; j++){
			for(int i =0; i<L; i++){
				//printf("%i, %i\n", j,i);
				dfs = depthFirstSearch(j,i, clusterID);
				row = 'y';
				column = 'y';
				if(dfs>0){
					// the following determines if the cluster has percolated over rows and/or columns and records details for each cluster 
					for (int n=0; n<L; n++){
						if (rowPerc[n]==0) row = 'n';
						if (columnPerc[n]==0) column = 'n';
					}
					perc[clusterID-2].ID = clusterID;
					perc[clusterID-2].size = dfs;
					perc[clusterID-2].rows = row;
					perc[clusterID-2].cols = column;

					clusterID++;
				}
			}
		}
		printBonds();
		printf("TOP LNE: ");
		for(int i=0;i<L;i++){
			printf("%i  ", upSide[i]);
		}
		printf("\nBOTTOM LINE: ");
		for(int i=0;i<L;i++){
			printf("%i  ", downSide[i]);
		}
		printf("\nLEFT SIDE: ");
		for(int i=0;i<L;i++){
			printf("%i  ", leftSide[i]);
		}
		printf("\nRIGHT SIDE: ");
		for(int i=0;i<L;i++){
			printf("%i  ", rightSide[i]);
		}
		printf("\n");

		for(int i=0;i<(clusterID-2);i++){
			printf("Cluster No. %i, size: %i, rows percolating: %c, columns percolating: %c \n", 
				perc[i].ID, perc[i].size, perc[i].rows, perc[i].cols);
		}
		printf("\n");


		return 0;
    }
}

