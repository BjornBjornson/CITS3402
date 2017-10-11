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
struct cluster{
	int id;
	//char source;
	int parent;
	int size;
	UT_hash_handle hh;
};
	// followed the manual, probably broken =(
int lowest_num(int start, int *counter, int *update, struct cluster **superclusters){ // could replace this with a return of cluster, but was integrating hashmap after it's already tied in.
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
			counter[0]++;
			update[counter[0]]=start;
			start=mid->parent;
			HASH_FIND_INT(*superclusters, &start, mid);
		}
		//free(mid);
		return start;
	}
}
void updater(int j, int count, int *update, struct cluster **superclusters){
	
	for(int Z=0; Z<count;Z++){
		//struct cluster *old;
		struct cluster *mid;
		HASH_FIND_INT(*superclusters, &update[Z], mid);
		mid->parent=j;
		//HASH_REPLACE_INT(*superclusters, id, mid, old);
		//free(old);
	}
	
}

//void add_cluster()

int combi(){
	struct cluster *all_clusters= NULL; // will be persistent and held somewhere.
	int highest_id = 109; //keeping track of Id's. It'll be easier in the long run to just keep iterating up.
	struct cluster *link_A = NULL;
	struct cluster *link_B = NULL;
	struct cluster *superclusters = NULL;
	/*int A1[L]={2,0,0,0,2,0,3,0,3,3};
	int A2[L]={2,0,2,0,3,0,3,0,4,4};
	int A3[L]={4,0,5,0,6,0,7,0,3,3};
	
	int B1[L]={4,4,0,2,2,0,3,0,5,5};
	int B2[L]={2,0,3,0,4,0,5,0,6,6};
	int B3[L]={4,4,0,5,5,0,6,6,0,7};*/
	
	
	
	//int link_A[L]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}; //must remember to check existance while looping through this. will cause errors otherwise.
	//int link_B[L]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	int SA[L]={2,0,0,0,2,0,3,0,3,3};// having these as non-pointers will be faster, and probably possible, since they're pretty small.
	int SB[L]={4,4,0,2,2,0,3,0,5,5};
	int numSupers =highest_id+1;
	//int superclusters[L]={0};
	//int superSize[L];
	printf("Starting\n");
	for(int i=0;i<L;i++){
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
		for(int Z=2; Z<7;Z++){
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
		for(int Z=0; Z<L;Z++){
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
				HASH_ADD_INT(link_A, id, newSuperA);
				struct cluster *newSuperB;
				newSuperB = malloc(sizeof(struct cluster));
				newSuperB->id=SB[i];
				HASH_ADD_INT(link_B, id, newSuperB);
				numSupers++;
			}
			else if(A&&!B){ //in the case that one already has a super-cluster,
				printf("A full\n");
				int update[L/2];
				int count=0;
				int temp = A->parent;
				int j =lowest_num(temp, &count, update, &superclusters);
				
				if(count!=0){//=====helper 2
					struct cluster *old;
					updater(j, count, update, &superclusters);
					
					HASH_FIND_INT(link_A,&A->id, old);
					old->parent=j;
					
				}
				struct cluster *old;
				old=malloc(sizeof(struct cluster));
				old->parent=A->parent;
				old->id=SB[i];
				HASH_ADD_INT(link_B, id, old);
				
				
				//add the sizes together
			}
			else if(!A&&B){
				printf("B full\n");
				int update[L/2];
				int count=0;
				int temp = B->parent;
				int j =lowest_num(temp, &count, update, &superclusters);
				
				if(count!=0){
					
					updater(j, count, update, &superclusters);
					B->parent=j;
					
				}
				struct cluster *old;
				old=malloc(sizeof(struct cluster));
				old->parent=j;
				old->size=B->size+1;
				HASH_ADD_INT(link_A, id, old);
				free(old);
				//add the sizes together here
			}
			else if(A&&B){ //fix this up.
				printf("both have superclusters\n");
				int countA =0;
				int countB=0;
				int updateA[L/2];
				int updateB[L/2];
				int temp = A->parent;
				int j =lowest_num(temp, &countA, updateA, &superclusters);
				temp = B->parent;
				int k =lowest_num(temp, &countB, updateB, &superclusters);
				printf("%i J\n",j);
				printf("%i K\n",k);

				// Now able to see if they have the same root cluster.
				//going to congregate to the lowest numbered clusters, since they'll be older-> more likely to be the root of a bunch of other clusters.
				if(j>k){//need to work this and below over again =====================================
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
				
				if(countA!=0){
					updater(j, countA, updateA, &superclusters);
				}
				if(countB!=0){
					updater(k, countB, updateB, &superclusters);
				}
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
			newSuper->id=SA[i];
			HASH_ADD_INT(link_A, id, newSuper);
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
	
	//all of this is going to rely on ID's remaining unique, and just iterating upwards.
	
	struct cluster *iterator;
	struct cluster *my_super;
	struct cluster *comparison;
	//adding the new clusters to the larger map.
	HASH_ITER(hh, superclusters, my_super, iterator){ //this loop uses iterator to track the next item in the map
		while(my_super->id != my_super->parent){
			HASH_FIND_INT(superclusters, &my_super->parent, my_super);
		}
		HASH_FIND_INT(all_clusters, &my_super->id, comparison);
		if(comparison==NULL){
			HASH_ADD_INT(all_clusters, id, my_super);
			if(my_super->id>highest_id){
				highest_id=my_super->id;
			}
		}else{
			printf("ERROR cluster ID EXISTS\n RECHECK THE ID PROVISIONING CODE");
		}
		
	}printf("\n -=============----- \n");
	
	// Now going through the original side-list and updating all other side-list information. Then going to remove old entry from the all_clusters hashmap.
	HASH_ITER(hh, link_A, my_super, iterator){
		
	}
	/*going to
	-go through constructed superclusters, and add them to
	-
	
	
	*/
	
	return 1;
	
	
}

int main(int argc, char *argv[]){
	int a =combi();
	return a;
}