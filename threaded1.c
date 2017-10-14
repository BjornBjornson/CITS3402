#include <stdio.h>
#include <stdlib.h>
#include "omp.h"
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>

//  compile:  gcc -fopen -o dfs dfs10.c
//  run:      ./dfs probability
// this version changes the Node map back to a non-pointer to enable new seeded maps

#define L 8             /* Linear edge dimension of map */
#define N L*L           // total number of nodes in a single map
#define NTHREADS 4

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
} perc[N];

struct Interface{
		char connect;
		int size;
} interface[8];

struct Combination{
	int a[L*4];
	int b[L*4];
	int c[L*4];
	int d[L*4];
	int aSize;
	int bSize;
	int cSize;
	int dSize;
} combination;

// initialise arrays to store whether cluster has spanned all rows and/or all columns
int rowPerc[L]={0};
int columnPerc[L]={0};

int top = 0;
int count = 0;
bool popped = false;


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
                        if (r1>probability) seeded1 = 2;
                        else seeded1 = 0;
                        if (r2>probability) seeded2 = 2;
                        else seeded2 = 0;
                        printf("pr1: %i\t pr2: %i\t", seeded1, seeded2);                                //check print of probability values

                        map[i][j].right = seeded1;
						if (j+1 < L) map[i][j+1].left = seeded1;
						if (map[i][j].right == 0) {
							map[i][j].flag = 1;
							if (j+1 < L) map[i][j+1].flag = 1;
						}

                        map[i][j].down = seeded2;
						if (i+1 < L) map[i+1][j].up = seeded2;
						if (map[i][j].down == 0) {
							map[i][j].flag = 1;
							if (i+1 < L) map[i+1][j].flag = 1;
						}

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

void printSites(struct Node map[L][L]){

        int i,j;
        for (i = 0; i < (L); i++){
                for (j = 0; j < L; j++){
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
void printBonds(struct Node map[L][L]){

        int i,j;
        for (i = 0; i < (L); i++){
                for (j = 0; j < L; j++){
                        if (map[i][j].right==0){
                                printf("%i ----- ", map[i][j].flag);
						}
                                else printf("%i\t", map[i][j].flag);

                }
                printf("\n");
                for (j = 0; j < L; j++){
                        if (map[i][j].down==0) printf("|\t");
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

void deadEnd(int i, int j,  struct Node map[L][L]){   //function to change paths to level 3 . no more options

        map[i][j].right = 2;                   // change entire node to 2 if it's a dead end
        map[i][j].left = 2;
        map[i][j].down = 2;
        map[i][j].up = 2;
}

void displayNode(){
        printf("(%i, %i)\t", peek().y, peek().x);               //prints out map co-ordinate

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


        while(!isStackEmpty()){  

                //initialise temporary holding stack
                struct Node temp[1][1];
                temp[0][0].x = 0;
                temp[0][0].y = 0;

                cluster[top].x = map[i][j].x;      //store co-ordinates in the cluster stack
                cluster[top].y = map[i][j].y;

                rowPerc[i] = clusterID;            //store record of which row and column cluster reaches
                columnPerc[j] = clusterID;

                if(map[i][j].flag == 1){
                       map[i][j].flag = clusterID;
                //marking the node as both explored, and to which cluster of nodes it belongs- 
                        count++;         //will only update if there's a new filled node attached.

                }
                //displayNode();                              // print check of top node in the cluster
                push(cluster[top].y, cluster[top].x);                   // push node into the stack

                temp[0][0] = getNextNode(map);                  // allocate new node coords
                i = temp[0][0].y;
                j = temp[0][0].x;
                //printf("check3 (%i, %i)\n",i,j);

                if (!popped) ++top;            //want to go back to previous node if no connection made

        }
        /*int k;
        for (k = 0; k< count; k++){
                printf("cluster[%i] = (%i, %i, %c)\n", k, cluster[k].y, cluster[k].x, cluster[k].dir);
                                                                                                                //prints out map co-ordinate
        }*/
        return count;

}


/*
	0	1	2	3	4	5	6	7						0	1	2	3	4	5	6	7			
31									8				31									8		
30									9				30									9		
29									10				29									10		
28			combination.a	  		11				28			combination.b			11		
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
27			combination.c			12				27			combination.d			12		
26									13				26									13		
25									14				25									14		
24									15				24									15		
	23	22	21	20	19	18	17	16						23	22	21	20	19	18	17	16			
																								
																								
				C2														C3						
*/
void matchClusters(struct Combination combination){
	int n;
	int size; 
	int numPerc = 0;
	char percolated = 'y';

	for (n=0; n<8; n++){
		interface[n].connect = 'n';
	}
	for (n=0; n<L; n++){
		if (combination.a[n+L] > 0){					
			if (combination.a[n+L] == combination.b[L*4-n-1]) interface[0].connect = 'y';	//R0
		}
		if (combination.a[n+L*2] > 0){
			if (combination.a[n+L*2] == combination.c[L-n-1]) interface[1].connect = 'y';    //CO
		}
		if (combination.a[n+L*3] > 0){
			if (combination.a[n+L*3] == combination.b[L*2-n-1]) interface[2].connect = 'y';		//R1
		}
		if (combination.a[n] > 0){
			if (combination.a[n] == combination.c[L*3-n-1]) interface[3].connect = 'y';		//C2
		}
		if (combination.d[n] > 0){
			if (combination.d[n] == combination.b[L*3-n-1]) interface[4].connect = 'y';   //C1
		}
		if (combination.d[n+L] > 0){
			if (combination.d[n+L] == combination.c[L*4-n-1]) interface[5].connect = 'y';  //R3
		}		
		if (combination.d[n+L*2] > 0){
			if (combination.d[n+L*2] == combination.b[L-n-1]) interface[6].connect = 'y';  //C3
		}
		if (combination.d[n+L*3] > 0){
			if (combination.d[n+L*3] == combination.c[L*2-n-1]) interface[7].connect = 'y';	//R2
		}
	}

	printf("interface connection = ");
	for (n=0; n<8; n++){
		printf("%c \t",interface[n].connect);
		if (interface[n].connect == 'y') numPerc++;
	}
	if (numPerc > 6) {
		size = combination.aSize + combination.bSize + combination.cSize + combination.dSize;
		printf("\npercolated cluster size = %i \n", size);
	} else {
		printf("\ncluster did not percolate\n");
	}
	printf("\n");
}

void searchControl(int threadID, double probability, char seedType){

            int dfs, size;

			// Initialising the side arrays to 0-empty. Will be filled with clusterID numbers as they are found during the search/navigation.
			int upSide[L]={0};
			int downSide[L]={0};
			int leftSide[L]={0};
			int rightSide[L]={0};
			int upSideP[L]={0};
			int downSideP[L]={0};
			int leftSideP[L]={0};
			int rightSideP[L]={0};

			struct Node map[4][L][L];

			//struct Combination combination;

            int i, j, n, m, k, t;

            int clusterID = 2;
            char row = 'y';
            char column = 'y';
	
            //for (k = 0; k<NTHREADS; k++)				 
            //{
				if (seedType == 's') {
					seedSite(probability, map[threadID]);
					printSites(map[threadID]);
				}
				else {
					seedBond(probability, map[threadID]);
	               	printBonds(map[threadID]);
				}
                for(j =0; j<L; j++){
                        for(i =0; i<L; i++){
                                //printf("%i, %i\n", j,i);
                                //int thr = omp_get_thread_num();
                                //printf("thread num (%d)\n ", thr);

                                dfs = depthFirstSearch(i,j, clusterID, map[threadID]);
                                row = 'y';
                                column = 'y';
                                if(dfs>0){
     										// the following determines if the cluster has percolated over
 											//rows and/or columns and records details for each cluster 
                                        for (n=0; n<L; n++){
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
				i = 0;
				for (j = 0; j<L; j++){
					upSide[j] = map[threadID][i][j].flag;
				}
				i = L-1;
				for (j = 0; j<L; j++){
					downSide[j] = map[threadID][i][j].flag;
				}
				j = 0;
				for (i = 0; i<L; i++){
					leftSide[i] = map[threadID][i][j].flag;
				}
				j = L-1;
				for (i = 0; i<L; i++){
					rightSide[i] = map[threadID][i][j].flag;
				}
				for (m = 0; m<L; m++){
					upSideP[m] = 0;
					downSideP[m] = 0;
					rightSideP[m] = 0;
					leftSideP[m] = 0;
				}
				printf("clusterID %i\n", clusterID);
				for (n=0; n<clusterID-2; n++){
					for (m = 0; m<L; m++){
						//printf("a downSide[m] %i\n", downSide[m]);
						if (perc[n].rows == 'y' || perc[n].cols == 'y'){
							size = perc[n].size;
							if (upSide[m] == perc[n].ID){	//only include a value if clusterID is percolating
								upSideP[m] = 1;
							} else {
								upSideP[m] = 0;
							}
							if (downSide[m] == perc[n].ID){	
								downSideP[m] = 1;
							} else {
								downSideP[m] = 0;
							}
							if (rightSide[m] == perc[n].ID){
								rightSideP[m] = 1;
							} else {
								rightSideP[m] = 0;
							}
							if (leftSide[m] == perc[n].ID){	
								leftSideP[m] = 1;
							} else {
								leftSideP[m] = 0;
							}
						}
					}
				}
				
				switch(threadID){
				case (0):
                for (t = 0;t<L;t++){
                        combination.a[t] = upSideP[t];
						combination.a[t+L] = rightSideP[t];
						combination.a[t+L*2] = downSideP[L-t-1];
						combination.a[t+L*3] = leftSideP[L-t-1];
						combination.aSize = size;
                }
                break;
                case (1):
                for (t = 0;t<L;t++){
                        combination.b[t] = upSideP[t];
						combination.b[t+L] = rightSideP[t];
						combination.b[t+L*2] = downSideP[L-t-1];
						combination.b[t+L*3] = leftSideP[L-t-1];
						combination.bSize = size;
                }
                break;
                case (2):
                for (t = 0;t<L;t++){
                        combination.c[t] = upSideP[t];
						combination.c[t+L] = rightSideP[t];
						combination.c[t+L*2] = downSideP[L-t-1];
						combination.c[t+L*3] = leftSideP[L-t-1];
						combination.cSize = size;
                }
                break;
                case (3):
                for (t = 0;t<L;t++){
                        combination.d[t] = upSideP[t];
						combination.d[t+L] = rightSideP[t];
						combination.d[t+L*2] = downSideP[L-t-1];
						combination.d[t+L*3] = leftSideP[L-t-1];
						combination.dSize = size;
                }
                break;
				}
                printf("k = %i\n",k);
				if (seedType == 's') {
					printSites(map[threadID]);
				}
				else {
	               	printBonds(map[threadID]);
				}
				
                printf("\n\n\n");
                
                //}
                //}

                printf("1st GROUP: ");
                for(i=0;i<L*4;i++){
                        printf("%i  ", combination.a[i]);
                }
                printf("\n2ND GROUP: ");
                for(i=0;i<L*4;i++){
                        printf("%i  ", combination.b[i]);
                }
                printf("\n3RD GROUP: ");
                for(i=0;i<L*4;i++){
                        printf("%i  ", combination.c[i]);
                }
                printf("\n4TH GROUP: ");
                for(i=0;i<L*4;i++){
                        printf("%i  ", combination.d[i]);
                }
                printf("\n");

                for(i=0;i<(clusterID-2);i++){
                        printf("Cluster No. %i, size: %i, rows percolating: %c, columns percolating: %c \n",
                                perc[i].ID, perc[i].size, perc[i].rows, perc[i].cols);
                }
                printf("\n");
				//matchClusters(combination);

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
			char seedType = argv[1][0];			// enter 's' for site percolation, 'b' for bond percolation
			double p = atof(argv[2]);			// enter probability between 0 - 1, eg. 0.55

            double delta;
            int dfs, size;	
			int i;

		    srand(time(NULL));                              //should only be called once

            struct timeval start, end;
            gettimeofday(&start, NULL);

            omp_set_num_threads(4);

            #pragma omp parallel
			{
			int ID = omp_get_thread_num();
			searchControl(ID, p, seedType);
            }
			matchClusters(combination);
            gettimeofday(&end, NULL);
            delta = ((end.tv_sec  - start.tv_sec) * 1000000u +
                                end.tv_usec - start.tv_usec) / 1.e6;
            printf("time=%12.10f\n",delta);

            return 0;
    }
}

