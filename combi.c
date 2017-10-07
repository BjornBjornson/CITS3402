#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#define L 10     	/* Linear edge dimension of map */
#define N L*L		// total number of nodes in a single map
/*
int lowest(int *clusters, int *map){
	//remember to unbind all temporary pointers 
	return 1;
}*/
int main(int argc, char *argv[]){
	int A[5]={-1,-1,-1,-1,-1}; //must remember to check existance while looping through this. will cause errors otherwise.
	int B[5]={-1,-1,-1,-1,-1};
	int SA[L]={2,0,0,0,2,0,3,0,3,3};// having these as non-pointers will be faster, and probably possible, since they're pretty small.
	int SB[L]={4,4,0,2,2,0,3,0,5,5};
	int numSupers =0;
	int superclusters[L]={0};
	int superSize[L];
	printf("Starting\n");
	for(int i=0;i<L;i++){
		printf("\n\n%i:i\n",i);
		printf("%i: A[SA[i]-2]\n",A[SA[i]-2]);
		printf("%i: SA[i]\n",SA[i]);
		printf("%i: B[SB[i]-2]\n",B[SB[i]-2]);
		printf("%i: SB[i]\n\n\n",SB[i]);
		for(int Z=0; Z<5;Z++){
			printf("%i,",A[Z]);
		}printf("\n");
		for(int Z=0; Z<5;Z++){
			printf("%i,",B[Z]);
		}printf("\n");
		for(int Z=0; Z<L;Z++){
			printf("%i,",superclusters[Z]);
		}printf("\n");
		
		printf("---------------\n");
		if(SA[i]!=0 && SB[i]!=0){ //if neither side is empty
			printf("Both have values\n");
			if((A[SA[i]-2]==-1)&&(B[SB[i]-2]==-1)){ //if neither side's super-link list position exists yet. (hopefully)
				printf("neither connected\n");
				superclusters[numSupers]=numSupers; //referencing itself, meaning that it hasn't been linked to another cluster yet.
				superSize[numSupers]=2; //will get to recording proper sizes later in development
				A[SA[i]-2]=numSupers;
				B[SB[i]-2]=numSupers;
				numSupers++;
			}
			else if((A[SA[i]-2]!=-1)&&(B[SB[i]-2]==-1)){ //in the case that one already has a super-cluster, should probably split it off into a separate function, since it's functionally identical to the next bit
				printf("A full\n");
				int j =A[SA[i]-2];
				printf("%i: J\n",j);
				int update[L/2];
				int count=0;
				while(superclusters[j]!=j){
					count++;
					update[count]=j;
					j=superclusters[j];
				}
				printf("%i: J\n",j);
				if(count!=0){
					for(int Z=0; Z<count;Z++){
						superclusters[update[Z]]=j;
					}
					A[SA[i]-2]=j;
				}
				B[SB[i]-2]=j;
				superSize[j]++;
				//add the sizes together
			}
			else if((A[SA[i]-2]==-1)&&(B[SB[i]-2]!=-1)){
				printf("B full\n");
				int j =B[SB[i]-2];
				int update[L/2];
				int count=0;
				while(superclusters[j]!=j){
					count++;
					update[count]=j;
					j=superclusters[j];
				}
				if(count!=0){
					for(int Z=0; Z<count;Z++){
						superclusters[update[Z]]=j;
					}
					B[SB[i]-2]=j;
				}
				A[SA[i]-2]=j;
				superSize[j]++;
				//add the sizes together here
			}
			else if(A[SA[i]-2]!=-1 &&B[SB[i]-2]!=-1){
				printf("both have superclusters\n");
				int j =B[SB[i]-2];
				int updateB[L/2];
				int updateA[L/2];
				int countB=0;
				while(superclusters[j]!=j){
					countB++;
					updateB[countB]=j;
					j=superclusters[j];
				}
				
				int countA=0;
				int k=A[SA[i]-2];
				while(superclusters[k]!=k){
					countA++;
					updateA[countA]=k;
					k=superclusters[k];
				}
				// Now able to see if they have the same root cluster.
				//going to congregate to the lowest numbered clusters, since they'll be older-> more likely to be the root of a bunch of other clusters.
				if(j<k){
					superclusters[k]=j;
					A[SA[i]-2]=j;
					k=j;
					superSize[j]+=superSize[k];
				}
				else if(k<j){
					superclusters[j]=k;
					B[SB[i]-2]=k;
					j=k;
					superSize[k]+=superSize[j];
					//add the sizes
				}//else do naught, and let the cleanup continue
				
				//Now running updates on stuff ======= By maintaining a short chain length, it'll make editing the cluster numbers on the edge arrays faster.
				//Particularly as it's a many->one relationship, with no central record in the cluster itself. 
				if(countA!=0){
					for(int Z=0; Z<countA;Z++){
						superclusters[updateA[Z]]=k;
					}
				}
				if(countB!=0){
					for(int Z=0; Z<countB;Z++){
						superclusters[updateB[Z]]=j;
					}
					
				}
			}
			
		}
		else if(SA[i]!=0 && (A[SA[i]-2]==-1)){
			printf("A is unclustered, B is zero\n");
			A[SA[i]-2]=numSupers;
			superclusters[numSupers]=numSupers; //referencing itself, meaning that it hasn't been linked to another cluster yet.
			superSize[numSupers]=1; //will get to recording proper sizes later in development
			numSupers++;
			//check for supercluster, add one if missing
		}
		else if((SB[i]!=0) && (B[SB[i]-2]==-1)){
			printf("B is unclustered, A is zero\n");
			B[SB[i]-2]=numSupers;
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
}