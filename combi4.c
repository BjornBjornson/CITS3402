#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#include "uthash.h"
#define L 10     	/* Linear edge dimension of map */
#define N L*L		// total number of nodes in a single map
/*
int lowest(int *clusters, int *map){
	//remember to unbind all temporary pointers 
	return 1;
}*/
struct grid_Side{
	int placeholder;
};
struct cluster{
	int id;
	//char source;
	int parent;
	int size;
	UT_hash_handle hh;
};
	// followed the manual, probably broken =(
int lowest_num(int start/*, int *counter, int *update*/, struct cluster **superclusters){ // could replace this with a return of cluster, but was integrating hashmap after it's already tied in.
	struct cluster *mid;
	printf("%i START\n",start);
	HASH_FIND_INT(*superclusters, &start, mid);
	if(mid==NULL){
		//free(mid);
		printf("I knew it's borked\n");
		return -1; //some sort of error throw here
	}else{
		while(mid->parent!=mid->id){
			printf("Not so borked\n");
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
void updater(int j, int count, int *update, struct cluster **superclusters){
	printf("here\n");
	for(int Z=0; Z<count;Z++){
		//struct cluster *old;
		printf("%i\n",update[Z]);
		struct cluster *mid;
		HASH_FIND_INT(*superclusters, &update[Z], mid);
		mid->parent=j;
		//HASH_REPLACE_INT(*superclusters, id, mid, old);
		//free(old);
	}
	
}*/
void side_update(int length, int *side, struct cluster **refMap, struct cluster **superclusters){
	for(int Z =0; Z<length;Z++){ // you can see that this should be iterated across all sides.
		struct cluster *link;
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

int combi(struct cluster **all_clusters,int *highest_id, int *SA, int **AA, int *SB, int **BB, int comp_length, int A_height, int B_height){ //tried to wrangle it- failed. should be simple - passing two variable sized arrays of int[L] arrays, and one integer.
	//struct cluster *all_clusters= NULL; // will be persistent and held somewhere.
	
	struct cluster *link_A = NULL;
	struct cluster *link_B = NULL;
	struct cluster *superclusters = NULL;
	
	int numSupers =highest_id[0]+1;
	
	printf("Starting\n");
	for(int i=0;i<L*comp_length;i++){
		printf("\n\n%i:i\n",i);
		struct cluster *A;
		struct cluster *B;
		HASH_FIND_INT(link_A, &SA[i], A);
		HASH_FIND_INT(link_B, &SB[i], B);
		printf("%i: SA[i]\n",SA[i]);
		if(A){
			printf("%i: link_A[SA[i]]\n",A->parent);
		}else{
			printf("-1: link_A[SA[i]]\n");
		}
		
		
		printf("%i: SB[i]\n\n\n",SB[i]);
		if(B){
			printf("%i: link_B[SB[i]]\n",B->parent);
		}else{
			printf("-1: link_B[SB[i]]\n");
		}
		struct cluster *older;
		for(int Z=2; Z<7;Z++){ //only works on test data.
			HASH_FIND_INT(link_A, &Z, older);
			if(older){
				printf("%i,",older->parent);
			}else{
				printf("-1,");
			}
			
		}printf("\n");
		for(int Z=2; Z<7;Z++){
			HASH_FIND_INT(link_B, &Z, older);
			if(older){
				printf("%i,",older->parent);
			}else{
				printf("-1,");
			}
		}printf("\n");
		
		for(int Z=0; Z<L*comp_length;Z++){
			HASH_FIND_INT(superclusters, &Z, older);
			if(older){
				printf("%i,",older->parent);
			}else{
				printf("-1,");
			}
		}printf("\n");
		//free(older);
		printf("---------------\n");
		
		if(SA[i]!=0 && SB[i]!=0){ //if neither side is empty
			printf("Both have values\n");
			
			if(!A&&!B){ //if neither side's super-link list position exists yet. (hopefully)
				printf("neither connected\n");
				struct cluster *newSuper;
				newSuper = malloc(sizeof(struct cluster));
				newSuper->id = numSupers;
				newSuper->parent= numSupers;
				newSuper->size = 2;
				HASH_ADD_INT(superclusters, id, newSuper);
				struct cluster *newSuperA;
				newSuperA = malloc(sizeof(struct cluster));
				newSuperA->id=SA[i];
				newSuperA->parent = numSupers;
				HASH_ADD_INT(link_A, id, newSuperA);
				struct cluster *newSuperB;
				newSuperB = malloc(sizeof(struct cluster));
				newSuperB->id=SB[i];
				newSuperB->parent = numSupers;
				HASH_ADD_INT(link_B, id, newSuperB);
				numSupers++;
			}
			else if(A&&!B){ //in the case that one already has a super-cluster,
				printf("A full\n");
				/*int update[L*comp_length/2];
				int count=0;*/
				int temp = A->parent;
				int j =lowest_num(temp,/* &count, update,*/ &superclusters);
				
				/*if(count!=0){
					struct cluster *old;
					updater(j, count, update, &superclusters);
					
					HASH_FIND_INT(link_A,&A->id, old);
					old->parent=j;
					
				}*/
				struct cluster *old;
				old=malloc(sizeof(struct cluster));
				old->parent=j;
				old->id=SB[i];
				HASH_ADD_INT(link_B, id, old);
				
				
				//add the sizes together
			}
			else if(!A&&B){
				printf("B full\n");
				/*int update[L*comp_length/2];
				int count=0;*/
				int temp = B->parent;
				int j =lowest_num(temp/*, &count, update*/, &superclusters);
				
				/*if(count!=0){
					struct cluster *old;
					updater(j, count, update, &superclusters);
					
					HASH_FIND_INT(link_B,&B->id, old);
					old->parent=j;
					
				}*/
				struct cluster *old;
				old=malloc(sizeof(struct cluster));
				old->id=SA[i];
				old->parent=j;
				old->size=B->size+1;
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

				// Now able to see if they have the same root cluster.
				//going to congregate to the lowest numbered clusters, since they'll be older-> more likely to be the root of a bunch of other clusters.
				if(j>k){
					struct cluster *child;
					struct cluster *parent;
					HASH_FIND_INT(superclusters, &k, parent);
					HASH_FIND_INT(superclusters, &j, child);
					parent->size=parent->size+child->size;
					j=k;
					child->parent = parent->id;
				}
				else if(k>j){
					struct cluster *child;
					struct cluster *parent;
					HASH_FIND_INT(superclusters, &j, parent);
					HASH_FIND_INT(superclusters, &k, child);
					parent->size=parent->size+child->size;
					k=j;
					child->parent = parent->id;
					
					//add the sizes
				}//else do naught, and let the cleanup continue
				
				//Now running updates on stuff ======= By maintaining a short chain length, it'll make editing the cluster numbers on the edge arrays faster.
				//Particularly as it's a many->one relationship, with no central record in the cluster itself. 
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
		else if(SA[i]!=0 && !(A)){
			printf("A is unclustered, B is zero\n");
			printf("%i:numsupers\n", numSupers);
			struct cluster *newSuper;
			newSuper = malloc(sizeof(struct cluster));
			newSuper->id = numSupers;
			newSuper->parent= numSupers;
			newSuper->size = 1;
			HASH_ADD_INT(superclusters, id, newSuper);
			struct cluster *newSuperB;
			newSuperB = malloc(sizeof(struct cluster));
			newSuperB->id=SA[i];
			newSuperB->parent = numSupers;
			HASH_ADD_INT(link_A, id, newSuperB);
			//free(newSuper);
			numSupers++;
		}
		else if((SB[i]!=0) && !(B)){
			
			printf("B is unclustered, A is zero\n");
			printf("%i:numsupers\n", numSupers);
			struct cluster *newSuper;
			newSuper = malloc(sizeof(struct cluster));
			newSuper->id = numSupers;
			newSuper->parent= numSupers;
			newSuper->size = 1;
			HASH_ADD_INT(superclusters, id, newSuper);
			struct cluster *newSuperB;
			newSuperB = malloc(sizeof(struct cluster));
			newSuperB->id=SB[i];
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
		struct cluster *my_super;
		if(SA[Z]==0){
			printf("000,");
		}else{
			HASH_FIND_INT(link_A, &SA[Z], my_super);
			printf("%i,",my_super->parent);
		}
		
	}
	printf("\n New Side B\n");
	for(int Z=0; Z<L*comp_length;Z++){
		struct cluster *my_super;
		if(SB[Z]==0){
			printf("000,");
		}else{
			HASH_FIND_INT(link_B, &SB[Z], my_super);
			printf("%i,",my_super->parent);
		}
		
	}
	printf("\n");
	
    for(int Z=0; Z<L*comp_length;Z++){
		
		
		if(SA[Z]!=0){
			struct cluster *my_super;
			struct cluster *comparison;
			HASH_FIND_INT(link_A, &SA[Z], my_super);
			int start = my_super->parent;
			/*int count= 0;
			int update[L*comp_length/2];*/
			int j = lowest_num(start, /*&count, update,*/ &superclusters);
			HASH_FIND_INT(*all_clusters, &j, comparison);
			if(comparison==NULL){
				struct cluster *temp;
				temp=malloc(sizeof(struct cluster));
				temp->id = my_super->id;
				temp->parent = my_super->parent;
				HASH_ADD_INT(*all_clusters, id, temp);
				if(temp->id>highest_id[0]){
					highest_id[0]=temp->id;
				}
			}else{
				//will need to update the size value of the cluster, because it will have been combined with other, newer clusters.
			}
		}/*else{
			printf("ZERO\n");
		}*/
		
    }printf("\n I made the following sides to demonstrate the update proceedure. \n A1-{2,0,0,0,2,0,3,0,3,3}\nA2-{2,0,2,0,3,0,3,0,4,4}\nA3-{4,0,5,0,6,0,7,0,3,3}\n");
	
	// Now going through the original side-list and updating all other side-list information. Then going to remove old entry from the all_clusters hashmap.
	
	/*
	reminder of the other sides (dummy data)
	int A1[L]={2,0,0,0,2,0,3,0,3,3};
	int A2[L]={2,0,2,0,3,0,3,0,4,4};
	int A3[L]={4,0,5,0,6,0,7,0,3,3};
	)
	*/
	
	
	
	printf("\n");
	for(int Z =0; Z<(2*A_height+comp_length);Z++){ //edit this to be based on the size, rather than a hard-variable.
		side_update(L, AA[Z], &link_A, &superclusters);
		for(int Y=0; Y<L;Y++){
			printf("%i,",AA[Z][Y]);
		}printf("\n");
	}
	printf("\n");
	printf("NOW B\n");
	for(int Z =0; Z<(2*B_height+comp_length);Z++){ // 
		side_update(L, BB[Z], &link_B, &superclusters);
		for(int Y=0; Y<L;Y++){
			printf("%i,",BB[Z][Y]);
		}printf("\n");
	}
	
	for(int Z=0; Z<L*comp_length;Z++){//freeing up unused memory
		struct cluster *mid;
		HASH_FIND_INT(link_A, &SA[Z], mid);
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
			HASH_FIND_INT(link_A, &SA[Z], mid);
			HASH_DEL(link_A, mid);
			free(mid);
		}
		HASH_FIND_INT(link_B, &SB[Z], mid);
		if(mid){
			HASH_FIND_INT(superclusters, &mid->id, mid);
			if(mid){
				HASH_DEL(superclusters, mid);
				free(mid);
			}
			HASH_FIND_INT(link_B, &SB[Z], mid);
			HASH_DEL(link_B, mid);
			free(mid);
		}
	}
	free(link_A);
	free(link_B);
	free(superclusters);
	
	return 1;
	
	
}

int main(int argc, char *argv[]){
	struct cluster *all_clusters =NULL;
	int AA[4][L]={{2,0,0,0,2,0,3,0,3,3},{2,2,0,0,3,3,0,4,4,0},{2,0,2,0,3,0,3,0,4,4},{4,0,5,0,6,0,7,0,3,3}};

	int BB[4][L]={{4,4,0,2,2,0,3,0,5,5},{2,0,3,0,4,0,5,0,6,6},{2,0,3,0,4,0,5,0,6,6},{4,4,0,5,5,0,6,6,0,7}};
	
	int ** A;
	A=calloc(4,sizeof(int *));
	for(int i=0;i<4;i++){
		A[i]=calloc(L, sizeof(int));
		for(int j=0; j<L;j++){
			A[i][j]=AA[i][j];
		}
	}
	int ** B;
	B=calloc(4,sizeof(int *));
	for(int i=0;i<4;i++){
		B[i]=calloc(L, sizeof(int));
		for(int j=0; j<L;j++){
			B[i][j]=BB[i][j];
		}
	}
	int highest_id = 109; //keeping track of Id's. It'll be easier in the long run to just keep iterating up.

	/*
		For melding the second face, don't increase cluster numbering. shuffle everything down to the lowest cluster, and update the sides after that- 
		since there will be constant values, it's not the same problem scenario as with two un-related squares
		*/
	
	int SA[2*L]={2,0,0,0,2,0,3,0,3,3,0,0,0,0,2,0,3,0,3,3};// having these as non-pointers will be faster, and probably possible, since they're pretty small.
	int SB[2*L]={4,4,0,2,2,0,3,0,5,5,0,4,0,2,2,0,3,0,5,5};
	//testPass(A);
	int a =combi(&all_clusters,&highest_id, SA, A, SB,B,2,1,1);
	printf("HI ID:%i\n",highest_id);
	return a;
}