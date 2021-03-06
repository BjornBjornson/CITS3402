#include <stdio.h>
#include <stdlib.h>
#include "omp.h"
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#include "uthash.h"

//  compile:  gcc -fopen -o dfs dfs10.c
//  run:      ./dfs probability
// this version changes the Node map back to a non-pointer to enable new seeded maps

#define L 8             /* Linear edge dimension of map */
#define N L*L           // total number of nodes in a single map
#define NTHREADS 4
// Initialising the side arrays to 0-empty. Will be filled with clusterID numbers as they are found during the search/navigation.
int upSide[L]={0};
int downSide[L]={0};
int leftSide[L]={0};
int rightSide[L]={0};
int upSideP[L]={0};
int downSideP[L]={0};
int leftSideP[L]={0};
int rightSideP[L]={0};

// initialise arrays to store percolated nodes for each section
int perc0[L*4]={0};
int perc1[L*4]={0};
int perc2[L*4]={0};
int perc3[L*4]={0};

// initialise arrays to store whether node_cluster has spanned all rows and/or all columns
int rowPerc[L]={0};
int columnPerc[L]={0};

int top = 0;
int count = 0;
bool popped = false;


struct Node {
        int flag;                       // occupied=1, unoccupied=0, already in a cluster=2+
        int up;                         // (0=not visited,1=visited,2=all directions visited,-1=when parent node)       
        int down;
        int left;
        int right;
        int x;                          //co-ords to be easily accessible by cluster
        int y;
};



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
};

struct Interface{
		char connect;
		int size;
} interface[8];

struct node_cluster{
	int id;
	int row_perc;
	int col_perc;
	int parent;
	int size;
	UT_hash_handle hh;
};
	// followed the manual, probably broken =(
int lowest_num(int start/*, int *counter, int *update*/, struct node_cluster **superclusters){ // could replace this with a return of node_cluster, but was integrating hashmap after it's already tied in.
	struct node_cluster *mid;
	//printf("%i START\n",start);
	HASH_FIND_INT(*superclusters, &start, mid);
	if(mid==NULL){
		//free(mid);
		//printf("I knew it's borked\n");
		return -1; //some sort of error throw here
	}else{
		while(mid->parent!=mid->id){
			//printf("Not so borked\n");
			/*counter[0]=*counter+1;
			int badint= start;
			update[*counter]=badint;
			printf("THIS IS START NOW \n%i\n",update[*counter]);*/
			start=mid->parent;
			HASH_FIND_INT(*superclusters, &start, mid);
		}
		//printf("THIS IS START NOW \n%i\n",update[*counter]);
		//free(mid);
		return start;
	}
}/*
void updater(int j, int count, int *update, struct node_cluster **superclusters){
	printf("here\n");
	for(int Z=0; Z<count;Z++){
		//struct node_cluster *old;
		printf("%i\n",update[Z]);
		struct node_cluster *mid;
		HASH_FIND_INT(*superclusters, &update[Z], mid);
		mid->parent=j;
		//HASH_REPLACE_INT(*superclusters, id, mid, old);
		//free(old);
	}
	
}*/
void side_update(int length, int *side, struct node_cluster **refMap, struct node_cluster **superclusters){
	
	for(int Z =0; Z<length;Z++){ // you can see that this should be iterated across all sides.
		struct node_cluster *link;
		HASH_FIND_INT(*refMap, &side[Z], link);
		if(link!=NULL){
			while(link->parent!=link->id){
			int start = link->parent;
			HASH_FIND_INT(*superclusters, &start, link);
			}
			side[Z]=link->id;
		}
	}
}
//void add_cluster()

int combi(struct node_cluster **all_clusters,int *highest_id, struct node_cluster *SA, int **AA, struct node_cluster *SB, int **BB, int comp_length, int A_height, int B_height){ 
	//struct node_cluster *all_clusters= NULL; // will be persistent and held somewhere.
	
	struct node_cluster *link_A = NULL;
	struct node_cluster *link_B = NULL;
	struct node_cluster *superclusters = NULL;
	
	int numSupers =highest_id[0]+1;
	
	printf("Starting\n");
	for(int i=0;i<L*comp_length;i++){
		printf("\n\n%i:i\n",i);
		struct node_cluster *A;
		struct node_cluster *B;
		HASH_FIND_INT(link_A, &SA[i].id, A);
		HASH_FIND_INT(link_B, &SB[i].id, B);
		printf("%i: old cluster ID for side A\n",SA[i].id);
		if(A){
			printf("%i: linked new cluster ID\n",A->parent);
		}else{
			printf("-1: link_A[SA[i]]\n");
		}
		printf("%i: SB[i]\n",SB[i].id);
		if(B){
			printf("%i: link_B[SB[i]]\n\n",B->parent);
		}else{
			printf("-1: link_B[SB[i]]\n\n");
		}
		printf("Cluster A's full side, with linked cluster number below\n");
		for(int j=0;j<comp_length*L;j++){
			printf("%i::",SA[j].parent);
		}printf("\n");
		for(int j=0; j<comp_length*L;j++){
			struct node_cluster *oldest;
			HASH_FIND_INT(link_A, &SA[j].id, oldest);
			if(oldest){
				printf("%i::",lowest_num(oldest->parent, &superclusters));
			}
			else{
				printf("-1::");
			}
		}printf("\nCluster B's full side, with linked cluster number below\n");
		for(int j=0;j<comp_length*L;j++){
			printf("%i::",SB[j].parent);
		}printf("\n");
		for(int j=0; j<comp_length*L;j++){
			struct node_cluster *oldest;
			HASH_FIND_INT(link_B, &SB[j].id, oldest);
			if(oldest){
				printf("%i::",lowest_num(oldest->parent, &superclusters));
			}
			else{
				printf("-1::");
			}
		}
		printf("\n---------------\n");
		
		if(SA[i].id!=0 && SB[i].id!=0){ //if neither side is empty
			printf("Both have values\n");
			
			if(!A&&!B){ //if neither side's super-link list position exists yet. (hopefully)
				printf("neither connected\n");
				struct node_cluster *newSuper;
				newSuper = malloc(sizeof(struct node_cluster));
				newSuper->id = numSupers;
				newSuper->parent= numSupers;
				newSuper->size = SA[i].size+SB[i].size;
				HASH_ADD_INT(superclusters, id, newSuper);
				struct node_cluster *newSuperA;
				newSuperA = malloc(sizeof(struct node_cluster));
				newSuperA->id=SA[i].id;
				newSuperA->parent = numSupers;
				HASH_ADD_INT(link_A, id, newSuperA);
				struct node_cluster *newSuperB;
				newSuperB = malloc(sizeof(struct node_cluster));
				newSuperB->id=SB[i].id;
				newSuperB->parent = numSupers;
				HASH_ADD_INT(link_B, id, newSuperB);
				numSupers++;
			}
			else if(A&&!B){ //in the case that one already has a super-node_cluster,
				printf("A full\n");
				/*int update[L*comp_length/2];
				int count=0;*/
				int temp = A->parent;
				//A->size+=SB[i].size;
				int j =lowest_num(temp,/* &count, update,*/ &superclusters);
				
				/*if(count!=0){
					struct node_cluster *old;
					updater(j, count, update, &superclusters);
					
					HASH_FIND_INT(link_A,&A->id, old);
					old->parent=j;
					
				}*/
				struct node_cluster *old;
				old=malloc(sizeof(struct node_cluster));
				old->parent=j;
				old->id=SB[i].id;
				HASH_FIND_INT(superclusters, &j, B);
				//old->size=B->size+SB[i].size;
				B->size+=SB[i].size;
				HASH_ADD_INT(link_B, id, old);
				
				
				//add the sizes together
			}
			else if(!A&&B){
				printf("B full\n");
				/*int update[L*comp_length/2];
				int count=0;*/
				int temp = B->parent;
				int j =lowest_num(temp/*, &count, update*/, &superclusters);
				HASH_FIND_INT(superclusters, &j, A);
				A->size+=SA[i].size;
				/*if(count!=0){
					struct node_cluster *old;
					updater(j, count, update, &superclusters);
					
					HASH_FIND_INT(link_B,&B->id, old);
					old->parent=j;
					
				}*/
				struct node_cluster *old;
				old=malloc(sizeof(struct node_cluster));
				old->id=SA[i].id;
				old->parent=j;
				//old->size=B->size+SA[i].size;
				HASH_ADD_INT(link_A, id, old);
				\
				//add the sizes together here
			}
			else if(A&&B){ 
				printf("both have superclusters\n");
				/*int countA =0;
				int countB=0;
				int updateA[L*comp_length/2];
				int updateB[L*comp_length/2];*/
				int temp = A->parent;
				int j =lowest_num(temp,/* &countA, updateA,*/ &superclusters);
				temp = B->parent;
				int k =lowest_num(temp, /*&countB, updateB,*/ &superclusters);
				printf("%i J\n",j);
				printf("%i K\n",k);

				// Now able to see if they have the same root node_cluster.
				//going to congregate to the lowest numbered clusters, since they'll be older-> more likely to be the root of a bunch of other clusters.
				if(j>k){
					struct node_cluster *child;
					struct node_cluster *parent;
					HASH_FIND_INT(superclusters, &k, parent);
					HASH_FIND_INT(superclusters, &j, child);
					parent->size=parent->size+child->size;
					j=k;
					child->parent = parent->id;
					parent->size+=child->size;
				}
				else if(k>j){
					struct node_cluster *child;
					struct node_cluster *parent;
					HASH_FIND_INT(superclusters, &j, parent);
					HASH_FIND_INT(superclusters, &k, child);
					parent->size=parent->size+child->size;
					k=j;
					child->parent = parent->id;
					parent->size+=child->size;
					
					//add the sizes
				}//else do naught, and let the cleanup continue
				
				//Now running updates on stuff ======= By maintaining a short chain length, it'll make editing the node_cluster numbers on the edge arrays faster.
				//Particularly as it's a many->one relationship, with no central record in the node_cluster itself. 
				/*
				if(countA!=0){
					printf("updater A \n");
					updater(j, countA, updateA, &superclusters);
				}
				if(countB!=0){
					printf("%i\n",updateB[0]);
					printf("updater B \n");
					updater(k, countB, updateB, &superclusters);
				}*/
			}
			
		}// Need to update below here ====================================
		else if(SA[i].id!=0 && !(A)){
			printf("A is unclustered, B is zero\n");
			printf("%i:numsupers\n", numSupers);
			struct node_cluster *newSuper;
			newSuper = malloc(sizeof(struct node_cluster));
			newSuper->id = numSupers;
			newSuper->parent= numSupers;
			newSuper->size = SA[i].size;
			HASH_ADD_INT(superclusters, id, newSuper);
			struct node_cluster *newSuperB;
			newSuperB = malloc(sizeof(struct node_cluster));
			newSuperB->id=SA[i].id;
			newSuperB->parent = numSupers;
			HASH_ADD_INT(link_A, id, newSuperB);
			//free(newSuper);
			numSupers++;
		}
		else if((SB[i].id!=0) && !(B)){
			
			printf("B is unclustered, A is zero\n");
			printf("%i:numsupers\n", numSupers);
			struct node_cluster *newSuper;
			newSuper = malloc(sizeof(struct node_cluster));
			newSuper->id = numSupers;
			newSuper->parent= numSupers;
			newSuper->size = SB[i].size;
			HASH_ADD_INT(superclusters, id, newSuper);
			struct node_cluster *newSuperB;
			newSuperB = malloc(sizeof(struct node_cluster));
			newSuperB->id=SB[i].id;
			newSuperB->parent = numSupers;
			HASH_ADD_INT(link_B, id, newSuperB);
			//free(newSuper);
			numSupers++;
			//ditto
		}
		//else ignore;
		//free(A);
		//free(B);
	}
	printf("\n New Side A\n");
	
	//all of this is going to rely on ID's remaining unique, and just iterating upwards.

	//return 1;
	for(int Z=0; Z<L*comp_length;Z++){
		struct node_cluster *my_super;
		if(SA[Z].id==0){
			printf("000,");
		}else{
			HASH_FIND_INT(link_A, &SA[Z].id, my_super);
			printf("%i,",my_super->parent);
		}
	}
	printf("\n New Side B\n");
	for(int Z=0; Z<L*comp_length;Z++){
		struct node_cluster *my_super;
		if(SB[Z].id==0){
			printf("000,");
		}else{
			HASH_FIND_INT(link_B, &SB[Z].id, my_super);
			printf("%i,",my_super->parent);
		}
		
	}
	printf("\n");
	
    for(int Z=0; Z<L*comp_length;Z++){
		
		
		if(SA[Z].id!=0){
			struct node_cluster *my_super;
			struct node_cluster *comparison;
			HASH_FIND_INT(link_A, &SA[Z], my_super);
			int start = my_super->parent;
			/*int count= 0;
			int update[L*comp_length/2];*/
			int j = lowest_num(start, /*&count, update,*/ &superclusters);
			HASH_FIND_INT(*all_clusters, &j, comparison);
			if(comparison==NULL){
				struct node_cluster *temp;
				temp=malloc(sizeof(struct node_cluster));
				temp->id = my_super->id;
				temp->parent = my_super->parent;
				HASH_ADD_INT(*all_clusters, id, temp);
				if(temp->id>highest_id[0]){
					highest_id[0]=temp->id;
				}
			}else{
				//will need to update the size value of the node_cluster, because it will have been combined with other, newer clusters.
			}
		}/*else{
			printf("ZERO\n");
		}*/
		
    }//printf("\n I made the following sides to demonstrate the update proceedure. \n A1-{2,0,0,0,2,0,3,0,3,3}\nA2-{2,0,2,0,3,0,3,0,4,4}\nA3-{4,0,5,0,6,0,7,0,3,3}\n");
	
	// Now going through the original side-list and updating all other side-list information. Then going to remove old entry from the all_clusters hashmap.
	
	printf("\n");
	side_update(L*A_height, AA[0], &link_A, &superclusters);
	side_update(L*A_height, AA[2], &link_A, &superclusters);
	side_update(L*B_height, BB[0], &link_A, &superclusters);
	side_update(L*B_height, BB[2], &link_A, &superclusters);
	side_update(L*comp_length, AA[1], &link_A, &superclusters);
	side_update(L*comp_length, BB[1], &link_A, &superclusters);
	for(int Z =0; Z<(2*A_height+comp_length);Z++){ 
		for(int Y=0; Y<L;Y++){
			printf("%i,",AA[Z][Y]);
		}printf("\n");
	}
	printf("\n");
	printf("NOW B\n");
	for(int Z =0; Z<(2*B_height+comp_length);Z++){ // 
		for(int Y=0; Y<L;Y++){
			printf("%i,",BB[Z][Y]);
		}printf("\n");
	}
	
	for(int Z=0; Z<L*comp_length;Z++){//freeing up unused memory
		struct node_cluster *mid;
		HASH_FIND_INT(link_A, &SA[Z].id, mid);
		if(mid){
			HASH_FIND_INT(*all_clusters, &mid->id, mid);
			if(mid){
				HASH_DEL(*all_clusters, mid);
				free(mid);
			}
			HASH_FIND_INT(superclusters, &mid->id, mid);
			if(mid){
				HASH_DEL(superclusters, mid);
				free(mid);
			}
			HASH_FIND_INT(link_A, &SA[Z].id, mid);
			HASH_DEL(link_A, mid);
			free(mid);
		}
		HASH_FIND_INT(link_B, &SB[Z].id, mid);
		if(mid){
			HASH_FIND_INT(superclusters, &mid->id, mid);
			if(mid){
				HASH_DEL(superclusters, mid);
				free(mid);
			}
			HASH_FIND_INT(link_B, &SB[Z].id, mid);
			HASH_DEL(link_B, mid);
			free(mid);
		}
	}
	free(link_A);
	free(link_B);
	free(superclusters);
	
	return 1;
	
	
}



/*      this functions seeds the map with occupied or unoccupied nodes depending on probability entered
        by the user and initialises all node variables with 0. 
*/
void seedSite(double probability, struct Node map[L][L])
{
        double r;
        int seeded;
        int i, j;

        for (i = 0; i < L; i++){  	                    //rows
                for (j = 0; j < L; j++){                //columns

						map[i][j].flag = 0;				//reset flag to 0
			}
		}

        for (i = 0; i < L; i++){  	                    //rows
                for (j = 0; j < L; j++){                //columns

                        r = (double)rand()/RAND_MAX*1.0;
                        //printf("pr: %f\t", r);                                //check print of probability values
                        if (r<=probability) seeded = 1;
                        else seeded = 0;

                        //struct Node node = (struct Node) malloc(sizeof(struct Node));

                        map[i][j].flag = seeded;
                        map[i][j].up = 0;
                        map[i][j].down = 0;
                        map[i][j].left = 0;
                        map[i][j].right = 0;
                        map[i][j].x = j;
                        map[i][j].y = i;
 
/*the following indicates that the top row of the group need not search up, and the bottom not search down, likewise left column need not search left or right column search right.*/

                        if(i==0){
                                map[i][j].up = 2;
                        }
                        else if(i == L-1){
                                map[i][j].down = 2;
                        }
                        else if(j == 0){
                                map[i][j].left = 2;
                        }
                        else if(j == L-1){
                                map[i][j].right = 2;
                        }
                }
        //printf("\n");
        }
}

void seedBond(double probability, struct Node map[L][L])
{
        double r1, r2;
        int seeded1, seeded2;
        int i, j;

        for (i = 0; i < L; i++){  	                    //rows
                for (j = 0; j < L; j++){                //columns

				map[i][j].flag = 0;				//reset flag to 0
			}
		}

        for (i = 0; i < L; i++){  	                    //rows
                for (j = 0; j < L; j++){                //columns

                        r1 = (double)rand()/RAND_MAX*1.0;
                        r2 = (double)rand()/RAND_MAX*1.0;
                        //printf("pr: %f\t", r);                                //check print of probability values
                        if (r1>probability) seeded1 = 2;
                        else seeded1 = 0;
                        if (r2>probability) seeded2 = 2;
                        else seeded2 = 0;

                        map[i][j].right = seeded1;
						if (j+1 < L) map[i][j+1].left = seeded1;

                        map[i][j].down = seeded2;
						if (i+1 < L) map[i+1][j].up = seeded2;

                        map[i][j].x = j;
                        map[i][j].y = i;

/*the following indicates that the top row of the group need not search up, and the bottom not search down, likewise left column need not search left or right column search right.*/

                        if(i==0){
                                map[i][j].up = 2;
                        }
                        else if(i == L-1){
                                map[i][j].down = 2;
                        }
                        else if(j == 0){
                                map[i][j].left = 2;
                        }
                        else if(j == L-1){
                                map[i][j].right = 2;
                        }
                }
        //printf("\n");
        }

        for (i = 0; i < L; i++){  	                    //rows
                for (j = 0; j < L; j++){                //columns
					if (map[i][j].right == 0 || map[i][j].down == 0)
                        map[i][j].flag = 1;
					if (map[i][j].left == 0 || map[i][j].up == 0)
                        map[i][j].flag = 1;
				}
		}

}

void printBonds(struct Node map[L][L], int** sides){

        int i,j;
        for (i = 0; i < (L); i++){
                for (j = 0; j < L; j++){
			if(i==0){
				sides[0][j]=map[i][j].flag;
			}
			if(i==L-1){
				sides[2][j]=map[i][j].flag;
			}
			if(j==0){
				sides[3][i]=map[i][j].flag;
			}
			if(j==L-1){
				sides[1][i]=map[i][j].flag;
			}
				
                        if (map[i][j].flag>0){
                                if (map[i][(j+1)%L].flag > 0) printf("%i ----- ", map[i][j].flag);

                                else printf("%i\t", map[i][j].flag);
                        }
                        else printf("%i\t", map[i][j].flag);

                }
                printf("\n");
                for (j = 0; j < L; j++){
                        if (map[i][j].flag>0){

                                if (map[(i+1)%L][j].flag >0) printf("|\t");
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
        //printf("top %i\t",top);
}

struct Stack pop(){
        popped = true;
        return cluster[top--];
}

struct Stack peek(){
        return cluster[top];
}

bool isStackEmpty(){
        return top <= -1;
}

void deadEnd(int i, int j, struct Node map[L][L]){                                     //function to change paths to level 3 . no more options

        map[i][j].right = 2;                   // change entire node to 2 if it's a dead end
        map[i][j].left = 2;
        map[i][j].down = 2;
        map[i][j].up = 2;
}

void displayNode(){
       // printf("(%i, %i)\t", peek().y, peek().x);               //prints out map co-ordinate

}

// determines next node in the cluster
// if no more paths from the current node returns previous node

struct Node getNextNode(struct Node map[L][L]) {
                popped = false;

                int j = peek().x ;
                int i = peek().y ;
                //printf("check1 (%i, %i)\t",i,j);

/*the following first checks if flag on, then path is 0 and if 1 it also checks previous direction to make sure not cycling back on itself.  If no options available then 2 is applied to the node and stack drops the top node
note that I have not assumed the lattice can cycle around itself as I think we want to retain edge nodes assuming it's a small part of a larger area - can change this easily with %L if required*/



                if ((i+1 < L) && map[i+1][j].flag == 1 && map[i+1][j].up == 0) {
                                cluster[top].dir = 'd';
                                map[i][j].down = 1;
                                map[i+1][j].up = 1;
                                i = i+1;
                }
                else if ((j+1 < L) && map[i][j+1].flag == 1 && map[i][j+1].left == 0){
                                cluster[top].dir = 'r';
                                map[i][j].right = 1;
                                map[i][j+1].left = 1;
                                j = j+1;
                }
                else if ((j-1 >= 0) && (map[i][j-1].flag == 1) && (map[i][j-1].right == 0)) {
                                cluster[top].dir = 'l';
                                map[i][j].left = 1;
                                map[i][j-1].right = 1;
                                j = j-1;
                }
                else if ((i-1 >= 0) && (map[i-1][j].flag == 1) && (map[i-1][j].down == 0)) {
                                cluster[top].dir = 'u';
                                map[i][j].up = 1;
                                map[i-1][j].down = 1;
                                i = i-1;
                }

                else if ((i+1 < L) && map[i+1][j].flag == 1 && map[i+1][j].up == 1
                        && cluster[top-1].dir != 'u' && top >= 1){
                                cluster[top].dir = 'd';
                                i = i+1;

                }
                else if ((j+1 < L) && map[i][j+1].flag == 1 && map[i][j+1].left ==1
                        && cluster[top-1].dir != 'l' && top >= 1){
                                cluster[top].dir = 'r';
                                j = j+1;
                }
                else if ((j-1 >= 0) && (map[i][j-1].flag == 1) && (map[i][j-1].right == 1)
                        && cluster[top-1].dir != 'r' && top >= 1)                       {
                                cluster[top].dir = 'l';
                                j = j-1;
                }
                else if ((i-1 >= 0) && (map[i-1][j].flag == 1) && (map[i-1][j].down == 1)
                        && cluster[top-1].dir != 'd' && top >= 1) {
                                cluster[top].dir = 'u';
                                i = i-1;
                }

                else {
                        deadEnd(peek().y, peek().x, map);                    // changes path status to 2
                        pop();
                        //printf("popped check top value= %i\n",top);
                                                                                                        // reduces top index by 1 
                        if (top == -1) return map[i][j];
                        //printf("top2= %i\t",top);                               //then if top==-1 the DFS exits on next loop
                        //printf("top-1  (%i, %i)\t",cluster[top].y,cluster[top].x);

                        return map[cluster[top].y][cluster[top].x];
                }
                //printf("check2 (%i, %i)\t",i,j);

        return map[i][j];
}

/*
This searches on cluster of nodes,
I've changed the return value to true if it proceeds, this will be used to let the process know that I've got another cluster to record on completion.
*/
int depthFirstSearch(int i, int j, int clusterID, struct Node map[L][L]){

        //the cluster ID will be the reference point for the side-arrays, and cluster sizes.

        top = 0;
		if(omp_get_thread_num()==0){
		
		//printf("\n clusterID %i:\n",clusterID);
		//printBonds(map,temp);
		printf("\n");
		}
        if (map[i][j].flag == 0 || map[i][j].flag >1) return 0;
                                                // check origin flag before commencing DFS, 
                                                //aborting if either empty or already searched.
        count = 0;

 
        // reset cluster percolation arrays to zero
        int q;
        for (q=0; q<L; q++){
                rowPerc[q]=0;
                columnPerc[q]=0;
        }


        while(!isStackEmpty()){  // && !percolated(i,j)){

                // include counter until percolation function finished
                //if (count >= L*L) break; //if it has already searched all nodes, it should cease work.

                //if(count>N){break;}                   //note this will never exceed N unless probability==1 --- redundant?


                //initialise temporary holding stack
                struct Node temp[1][1];
                //struct Node tempNode = (struct Node) malloc(sizeof(struct Node));
                temp[0][0].x = 0;
                temp[0][0].y = 0;

                cluster[top].x = map[i][j].x;                          //store co-ordinates in the cluster stack
                cluster[top].y = map[i][j].y;

                rowPerc[i] = clusterID;                                         //store record of which row and column cluster reaches
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
                if(map[i][j].flag == 1){
                       map[i][j].flag = clusterID;
                                //marking the node as both explored, and to which cluster of nodes it belongs- 
                                //for use in both determining largest cluster and tracking the sides of the matricies
                        //rowPerc[i] = clusterID;                                               //store record of which row and column cluster reaches
                        //columnPerc[j] = clusterID;
                        count++;                                                //will only update if there's a new filled node attached.

                }
                displayNode();                                                                  // print check of top node in the cluster
                push(cluster[top].y, cluster[top].x);                   // push node into the stack

                temp[0][0] = getNextNode(map);                                             // allocate new node coords
                i = temp[0][0].y;
                j = temp[0][0].x;
                //printf("check3 (%i, %i)\n",i,j);

                if (!popped) ++top;                                             //want to go back to previous node if no connection made

        }
        //int k;
        /*for (k = 0; k< count; k++){
                printf("cluster[%i] = (%i, %i, %c)\n", k, cluster[k].y, cluster[k].x, cluster[k].dir);
                                                                                                                //prints out map co-ordinate
        }*/
        return count;

}

void storeSides(int k,int upSide[],int downSide[],int leftSide[],int rightSide[]){
        int t;
        switch(k){
                case (0):
                for (t = 0;t<L;t++){
                        perc0[t] = upSide[t];
						perc0[t+L] = rightSide[t];
						perc0[t+L*2] = downSide[L-t-1];
						perc0[t+L*3] = leftSide[L-t-1];
                }
                break;
                case (1):
                for (t = 0;t<L;t++){
                        perc1[t] = upSide[t];
						perc1[t+L] = rightSide[t];
						perc1[t+L*2] = downSide[L-t-1];
						perc1[t+L*3] = leftSide[L-t-1];
                }
                break;
                case (2):
                for (t = 0;t<L;t++){
                        perc2[t] = upSide[t];
						perc2[t+L] = rightSide[t];
						perc2[t+L*2] = downSide[L-t-1];
						perc2[t+L*3] = leftSide[L-t-1];
                }
                break;
                case (3):
                for (t = 0;t<L;t++){
                        perc3[t] = upSide[t];
						perc3[t+L] = rightSide[t];
						perc3[t+L*2] = downSide[L-t-1];
						perc3[t+L*3] = leftSide[L-t-1];
                }
                break;

        }
}

/*
	0	1	2	3	4	5	6	7						0	1	2	3	4	5	6	7			
31									8				31									8		
30									9				30									9		
29									10				29									10		
28					perc0	  		11				28				perc1				11		
27									12		R0		27									12		R1
26									13				26									13		
25									14				25									14		
24									15				24									15		
	23	22	21	20	19	18	17	16						23	22	21	20	19	18	17	16			
																								
				C0														C1						
																								
	0	1	2	3	4	5	6	7						0	1	2	3	4	5	6	7			
31									8				31									8		
30									9				30									9		
29									10				29									10		
28									11		R2		28									11		R3
27				perc2				12				27				perc3				12		
26									13				26									13		
25									14				25									14		
24									15				24									15		
	23	22	21	20	19	18	17	16						23	22	21	20	19	18	17	16			
																								
																								
				C2														C3						
*/
void matchClusters(){
	int n;
	for (n=0; n<8; n++){
		interface[n].connect = 'n';
	}
	for (n=0; n<L; n++){
		if (perc0[n+L] > 0){					
			if (perc0[n+L] == perc1[L*4-n-1]) interface[0].connect = 'y';	//R0
		}
		if (perc0[n+L*2] > 0){
			if (perc0[n+L*2] == perc2[L-n-1]) interface[1].connect = 'y';    //CO
		}
		if (perc0[n+L*3] > 0){
			if (perc0[n+L*3] == perc1[L*2-n-1]) interface[2].connect = 'y';		//R1
		}
		if (perc0[n] > 0){
			if (perc0[n] == perc2[L*3-n-1]) interface[3].connect = 'y';		//C2
		}
		if (perc3[n] > 0){
			if (perc3[n] == perc1[L*3-n-1]) interface[4].connect = 'y';   //C1
		}
		if (perc3[n+L] > 0){
			if (perc3[n+L] == perc2[L*4-n-1]) interface[5].connect = 'y';  //R3
		}		
		if (perc3[n+L*2] > 0){
			if (perc3[n+L*2] == perc1[L-n-1]) interface[6].connect = 'y';  //C3
		}
		if (perc3[n+L*3] > 0){
			if (perc3[n+L*3] == perc2[L*2-n-1]) interface[7].connect = 'y';	//R2
		}
	}

	printf("interface connection = ");
	for (n=0; n<8; n++){
		printf("%c \t",interface[n].connect);
	}
	printf("\n");
}

void search_control(char type, double p,int ***my_sides, struct Percolate perc[NTHREADS][N/2], int high_ID[NTHREADS]){
	          
	
	
	int i, j, n, m, k;


	int clusterID =2;
	char row = 'y';
	char column = 'y';
	srand(time(NULL));                              //should only be called once

	
	struct Node map[NTHREADS][L][L]; //change this to match number of threads
	omp_set_num_threads(NTHREADS);

	#pragma omp parallel
	{
	#pragma omp for private(cluster,upSide, downSide,leftSide,rightSide,upSideP,downSideP,leftSideP,rightSideP,i, j, n, m, k, clusterID,rowPerc,columnPerc,top,popped,count)
	for (k = 0; k<NTHREADS; k++)
	{
		int dfs;
		if (type == 's') seedSite(p, map[omp_get_thread_num()]);
		else seedBond(p, map[omp_get_thread_num()]);
		printBonds(map[omp_get_thread_num()],my_sides[omp_get_thread_num()]);
		clusterID = 2;

		//#pragma omp parallel  //for num_threads(4)
		//{

		//#pragma omp single
		//{
		for(j =0; j<L; j++){
			for(i =0; i<L; i++){
				//printf("%i, %i\n", j,i);
				//int thr = omp_get_thread_num();
				//printf("thread num (%d)\n ", thr);

				dfs = depthFirstSearch(j,i, clusterID, map[omp_get_thread_num()]);
				row = 'y';
				column = 'y';
				if(dfs>0){
					// the following determines if the cluster has percolated over rows and/or columns and records details for each cluster 
					for (n=0; n<L; n++){
						if (rowPerc[n]==0) row = 'n';
						if (columnPerc[n]==0) column = 'n';
					}
					//if (row == 'y' || column == 'y'){
						perc[omp_get_thread_num()][clusterID-2].ID = clusterID;
						perc[omp_get_thread_num()][clusterID-2].size = dfs;
						perc[omp_get_thread_num()][clusterID-2].rows = row;
						perc[omp_get_thread_num()][clusterID-2].cols = column;
					//}
					clusterID++;
				}
			}
		}
		//}
		for (m = 0; m<L; m++){
			upSideP[m] = 0;
			downSideP[m] = 0;
			rightSideP[m] = 0;
			leftSideP[m] = 0;
		}
		//printf("\n570ish \n");
		for (n=0; n<clusterID-2; n++){
			for (m = 0; m<L; m++){
				if (perc[omp_get_thread_num()][n].rows == 'y' || perc[omp_get_thread_num()][n].cols == 'y'){
					if (upSide[m] == perc[omp_get_thread_num()][n].ID){	//fix this to only include a value if clusterID is percolating
						upSideP[m] = 1;
					}
					if (downSide[m] == perc[omp_get_thread_num()][n].ID){	
						downSideP[m] = 1;
					}
					if (rightSide[m] == perc[omp_get_thread_num()][n].ID){
						rightSideP[m] = 1;
					}
					if (leftSide[m] == perc[omp_get_thread_num()][n].ID){	
						leftSideP[m] = 1;
					}
				}
			}
		}
		storeSides(k,upSideP,downSideP,leftSideP,rightSideP);
		printf("k = %i\n",k);
		//printBonds();
		
		//printBonds(map[omp_get_thread_num()]);
		high_ID[omp_get_thread_num()]=clusterID;
		

		}
		}
		
		
		for(int Z=0;Z<NTHREADS;Z++){
			printf("FROM THREAD %i: \n",Z);
			printf("HIGHEST ID: %i\n",high_ID[Z]);
			printBonds(map[Z],my_sides[Z]);
			printf("\n");
			
			printf("\n\n");
		}
		
		
		printf("\n\n");
		//struct node_cluster *all_groups = NULL;
		
		//printBonds(map);
		//printf("1st GROUP: ");
		

		
		//for(int Z=1; Z
		
		printf("\n2ND GROUP: ");
		for(i=0;i<L*4;i++){
				printf("%i  ", perc[1][i].ID);
		}
		printf("\n3RD GROUP: ");
		for(i=0;i<L*4;i++){
				printf("%i  ", perc[2][i].ID);
		}
		printf("\n4TH GROUP: ");
		for(i=0;i<L*4;i++){
				printf("%i:", perc[3][i].ID);
				printf("%c:", perc[3][i].rows);
				printf("%c,  ", perc[3][i].cols);
		}
		printf("\n");
		
		/*int IDblock=0;
		
		for(int Z=0; Z<NTHREADS-1;Z=Z+2){
			
			combi(&all_clusters, &IDlock, node_cluster *SA, int **AA, node_cluster *SB, int **BB, int comp_length, int A_height, int B_height);
			*/
		

		/*for(i=0;i<(clusterID-2);i++){
				printf("Cluster No. %i, size: %i, rows percolating: %c, columns percolating: %c \n",
						perc[i].ID, perc[i].size, perc[i].rows, perc[i].cols);
		}*/
		printf("\n");

		//matchClusters();

		
	
}

void match_control(int ***my_sides, struct Percolate perc[NTHREADS][N/2], int high_ID[NTHREADS]){

	for(int Z=0;Z<NTHREADS;Z++){
		printf("FROM THREAD %i: \n",Z);
		printf("HIGHEST ID: %i\n",high_ID[Z]);

		printf("\n");
		for(int Y=0; Y<4;Y++){
			for(int X=0;X<L;X++){
				printf("%i,",my_sides[Z][Y][X]);
			}printf("\n");
		}
		printf("\n\n");
	}
	int comp_length= 1; 
	int A_height =1; 
	int B_height=1;
/* pointers to free: 
SA
SB
AA
BB


*/
//this bit can be multithreaded easily. just leaving it for now though.
	struct node_cluster **all_clusters;
	all_clusters=calloc(2,sizeof(struct node_cluster*));
	for(int i=0; i<2;i++){
		all_clusters[i]=NULL;
		for(int j=2;j<high_ID[i];j++){
			struct node_cluster *initialising;
			initialising=calloc(1, sizeof(struct node_cluster));
			initialising->id=i;
			initialising->row_perc=perc[2*i][j].rows;
			initialising->col_perc=perc[2*i][j].cols;
			initialising->size=perc[2*i][j].size;
			initialising->parent=i;
			HASH_ADD_INT(all_clusters[2*i], id, initialising);
		}
	}
	for(int MF = 0; MF<2;MF++){//will need to change this when creating differently sized grids(of threads) // using an additional sub-loop to handle non-2X2 grids.
	
	
	struct node_cluster *SA;
	struct node_cluster *SB;
	int **AA=calloc(A_height*2+comp_length, sizeof(int *));
	int **BB=calloc(B_height*2+comp_length, sizeof(int *));
	//insert a case-gate to define direction of joining
	AA[0]=calloc(L*A_height, sizeof(int));
	AA[2]=calloc(L*A_height, sizeof(int));
	AA[1]=calloc(L*comp_length, sizeof(int));
	BB[0]=calloc(L*A_height, sizeof(int));
	BB[2]=calloc(L*A_height, sizeof(int));
	BB[1]=calloc(L*comp_length, sizeof(int));
	for(int i=0; i<L*A_height;i++){
		AA[0][i] = my_sides[MF*2/*need to put in the geometry here*/][0][i];
		AA[2][i] = my_sides[MF*2/*need to put in the geometry here*/][2][i];
	}
	for(int i=0; i<L*B_height;i++){
		BB[0][i] = my_sides[MF*2+1/*need to put in the geometry here*/][0][i];
		BB[2][i] = my_sides[MF*2+1/*need to put in the geometry here*/][2][i];
	}
	for(int i=0; i<L*comp_length;i++){
		AA[1][i] = my_sides[MF*2/*need to put in the geometry here*/][3][i];
		BB[1][i] = my_sides[MF*2+1][1][i];
	}
	
		

	SA=calloc(comp_length*L, sizeof(struct node_cluster));
	SB=calloc(comp_length*L, sizeof(struct node_cluster));
	for(int i=0; i<L;i++){
		if(my_sides[MF*2][1][i]==0){
			SA[i].id=0;
			SA[i].parent = 0;
			SA[i].size=0;
			SA[i].col_perc=0;
			SA[i].row_perc=0;
		}else{
			SA[i].id=my_sides[MF*2][1][i];
			SA[i].parent = my_sides[MF*2][1][i];
			SA[i].size=perc[MF*2][i].size;
			SA[i].col_perc=perc[MF*2][i].rows;
			SA[i].row_perc=perc[MF*2][i].cols;
		}if(my_sides[MF*2+1][3][i]==0){
			SB[i].id=0;
			SB[i].parent = 0;
			SB[i].size=0;
			SB[i].col_perc=0;
			SB[i].row_perc=0;
		}else{
			SB[i].id=my_sides[MF*2+1][3][i];
			SB[i].parent = my_sides[MF*2+1][3][i];
			SB[i].size=perc[MF*2+1][i].size;
			SB[i].col_perc=perc[MF*2+1][i].rows;
			SB[i].row_perc=perc[MF*2+1][i].cols;
		}
	}
//		int combi(struct node_cluster **all_clusters,int *highest_id, struct node_cluster *SA, int **AA, struct node_cluster *SB, int **BB, int comp_length, int A_height, int B_height){ 
	int out = combi(&all_clusters[0],&high_ID[0], SA, AA,SB,BB, comp_length,A_height,B_height);
	printf("%i",out);
	}

	
}

int main(int argc, char *argv[]){
	
    if(argc != 4)
    {
        fprintf(stderr, " ERROR\n Usage: ./perc seedType probability percolationType\n");
										// example: "./perc s 0.55 0"  
        exit(EXIT_FAILURE);             // Exit program indicating failure
    }
    else
    {	
    	double delta;
		char seedType = argv[1][0];			// enter 's' for site percolation, 'b' for bond percolation
		double p = atof(argv[2]);			// enter probability between 0 - 1, eg. 0.55
		//int perc = atof(argv[3]);			// enter 0 for rows, 1 for columns, 2 for both directions
		printf("seedType %c\n", seedType);
		struct timeval start, end;
		gettimeofday(&start, NULL);
		int ***my_sides;
		my_sides=calloc(NTHREADS, sizeof(int *));
		for(int i=0; i<NTHREADS;i++){
			my_sides[i]=calloc(4,sizeof(int *)); //sides of square
			for(int j=0; j<4;j++){
				my_sides[i][j]=calloc(L, sizeof(int));
			}
		}
		struct Percolate perc[NTHREADS][N/2];
		int high_ID[NTHREADS];
		
		
		search_control(seedType, p,my_sides, perc, high_ID);//replace high ID with while (perc[N].ID!=0)
		
		
		
		match_control(my_sides, perc, high_ID/* ? */); 
			//my_sides is pointers, can edit the sides in situ. perhaps remove the pointers of the discarded sides?
		
		
		
		gettimeofday(&end, NULL);
		delta = ((end.tv_sec  - start.tv_sec) * 1000000u +
						end.tv_usec - start.tv_usec) / 1.e6;
		printf("time=%12.10f\n",delta);

        return 0;
    }
}


