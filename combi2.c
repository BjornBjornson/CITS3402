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
		int parent;
		int size;
	};
	
int lowest_num(int start, int *counter, int *update, int *superclusters){
	
	while(superclusters[start]!=start){
		counter[0]++;
		update[counter[0]]=start;
		start=superclusters[start];
	}
	return start;
}
void updater(int j, int count, int *update, int *superclusters){
	for(int Z=0; Z<count;Z++){
		superclusters[update[Z]]=j;
	}
	
}


int combi(){
	
	int A1[L]={2,0,0,0,2,0,3,0,3,3};
	int A2[L]={2,0,2,0,3,0,3,0,4,4};
	int A3[L]={4,0,5,0,6,0,7,0,3,3};
	
	int B1[L]={4,4,0,2,2,0,3,0,5,5};
	int B2[L]={2,0,3,0,4,0,5,0,6,6};
	int B3[L]={4,4,0,5,5,0,6,6,0,7};
	
	
	
	int link_A[L]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}; //must remember to check existance while looping through this. will cause errors otherwise.
	int link_B[L]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	int SA[L]={2,0,0,0,2,0,3,0,3,3};// having these as non-pointers will be faster, and probably possible, since they're pretty small.
	int SB[L]={4,4,0,2,2,0,3,0,5,5};
	int numSupers =0;
	int superclusters[L]={0};
	int superSize[L];
	printf("Starting\n");
	for(int i=0;i<L;i++){
		printf("\n\n%i:i\n",i);
		printf("%i: link_A[SA[i]-2]\n",link_A[SA[i]-2]);
		printf("%i: SA[i]\n",SA[i]);
		printf("%i: link_B[SB[i]-2]\n",link_B[SB[i]-2]);
		printf("%i: SB[i]\n\n\n",SB[i]);
		for(int Z=0; Z<5;Z++){
			printf("%i,",link_A[Z]);
		}printf("\n");
		for(int Z=0; Z<5;Z++){
			printf("%i,",link_B[Z]);
		}printf("\n");
		for(int Z=0; Z<L;Z++){
			printf("%i,",superclusters[Z]);
		}printf("\n");
		
		printf("---------------\n");
		if(SA[i]!=0 && SB[i]!=0){ //if neither side is empty
			printf("Both have values\n");
			if((link_A[SA[i]-2]==-1)&&(link_B[SB[i]-2]==-1)){ //if neither side's super-link list position exists yet. (hopefully)
				printf("neither connected\n");
				superclusters[numSupers]=numSupers; //referencing itself, meaning that it hasn't been linked to another cluster yet.
				superSize[numSupers]=2; //will get to recording proper sizes later in development
				link_A[SA[i]-2]=numSupers;
				link_B[SB[i]-2]=numSupers;
				numSupers++;
			}
			else if((link_A[SA[i]-2]!=-1)&&(link_B[SB[i]-2]==-1)){ //in the case that one already has a super-cluster, should probably split it off into a separate function, since it's functionally identical to the next bit
				printf("A full\n");
				int update[L/2];
				int count=0;
				int j =lowest_num(link_A[SA[i]-2], &count, update, superclusters);
				
				if(count!=0){//=====helper 2
					updater(j, count, update, superclusters);
					link_A[SA[i]-2]=j;
				}
				link_B[SB[i]-2]=j;
				superSize[j]++;
				//add the sizes together
			}
			else if((link_A[SA[i]-2]==-1)&&(link_B[SB[i]-2]!=-1)){
				printf("B full\n");
				int update[L/2];
				int count=0;
				int j =lowest_num(link_B[SB[i]-2], &count, update, superclusters);
				
				if(count!=0){
					updater(j, count, update, superclusters);
					link_B[SB[i]-2]=j;
				}
				link_A[SA[i]-2]=j;
				superSize[j]++;
				//add the sizes together here
			}
			else if(link_A[SA[i]-2]!=-1 &&link_B[SB[i]-2]!=-1){ 
				printf("both have superclusters\n");
				int countA =0;
				int countB=0;
				int updateA[L/2];
				int updateB[L/2];
				int j =lowest_num(link_A[SA[i]-2], &countA, updateA, superclusters);
				int k =lowest_num(link_B[SB[i]-2], &countB, updateB, superclusters);

				// Now able to see if they have the same root cluster.
				//going to congregate to the lowest numbered clusters, since they'll be older-> more likely to be the root of a bunch of other clusters.
				if(j>k){
					superclusters[j]=k;
					link_A[SA[i]-2]=k;
					j=k;
					superSize[k]+=superSize[j];
				}
				else if(k>j){
					superclusters[k]=j;
					link_B[SB[i]-2]=j;
					k=j;
					superSize[j]+=superSize[k];
					//add the sizes
				}//else do naught, and let the cleanup continue
				
				//Now running updates on stuff ======= By maintaining a short chain length, it'll make editing the cluster numbers on the edge arrays faster.
				//Particularly as it's a many->one relationship, with no central record in the cluster itself. 
				
				if(countA!=0){
					updater(j, countA, updateA, superclusters);
				}
				if(countB!=0){
					updater(k, countB, updateB, superclusters);
				}
			}
			
		}
		else if(SA[i]!=0 && (link_A[SA[i]-2]==-1)){
			printf("A is unclustered, B is zero\n");
			link_A[SA[i]-2]=numSupers;
			superclusters[numSupers]=numSupers; //referencing itself, meaning that it hasn't been linked to another cluster yet.
			superSize[numSupers]=1; //will get to recording proper sizes later in development
			numSupers++;
			//check for supercluster, add one if missing
		}
		else if((SB[i]!=0) && (link_B[SB[i]-2]==-1)){
			printf("B is unclustered, A is zero\n");
			link_B[SB[i]-2]=numSupers;
			superclusters[numSupers]=numSupers; //referencing itself, meaning that it hasn't been linked to another cluster yet.
			superSize[numSupers]=1; //will get to recording proper sizes later in development
			numSupers++;
			//ditto
		}
		//else ignore;
	}
	for(int i=0;i<numSupers;i++){
		printf("%i,", superclusters[i]);
	}printf("\n -=============----- \n");
	for(int i=0;i<numSupers;i++){
		printf("%i,", superSize[i]);
	}printf("\n -================- \n");
	
	
	// Will split off the next bit into a separate function
	;
	for(int i =0; i< i++){
		if()
	}
	return 1;
	
	
}

int main(int argc, char *argv[]){
	int a =combi();
	return a;
}