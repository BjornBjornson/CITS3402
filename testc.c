#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define SIDE_LENGTH 10 // This will be the handy little value for incrementing the matrix size

/*
 needs to have a structure for holding cluster values
*/

struct cluster{ // can be replaced by sub-matrix level object if the size of clusters is going to be ignored.
	int size;
	int edge[4][SIDE_LENGTH]; //storing the links to the edge of the matrix	
};
//-1, 0, 1, 2
struct node{
	int parent[2];
	int left;
	int right;
	int up;
	int down;
	int clusterNum;
};
struct space{
	struct node map[SIDE_LENGTH][SIDE_LENGTH];
}	myspace;


void starmap(double down){
	//struct space myspace;
	for(int i=0; i<SIDE_LENGTH;i++){
		for(int j=0;j<SIDE_LENGTH;j++){
			double p =(double)(rand() %1000);
			p = p/1000.0;
			printf("(%f)",p);
			(myspace.map[i][j]).left=0;
			(myspace.map[i][j]).right=0;
			(myspace.map[i][j]).up=0;
			(myspace.map[i][j]).down=0;
			if(p<down){
				printf("1 ");
				(myspace.map[i][j]).clusterNum=0;
			}
			else{
				(myspace.map[i][j]).clusterNum =-1;
				printf("0 ");
			}
			if(i==0){ //these next two indicate that the top row need not search up, and the bottom not search down.
				(myspace.map[i][j]).up = 2;
			}
			else if(i== SIDE_LENGTH-1){
				(myspace.map[i][j]).down = 2;
			}
		}
		printf("\n");
	}
	printf("\n\n\n");
	//return myspace;
}

/*
possible helper function/s for the DFS
logic is similar to a recursive DFS, but due to the limitations of C it's not a real implementation of one.
Going to be using pointers to the map in order to have it work directly from the source, rather than have it pass it back and forth.
This should make parallelising it somewhat easier from a flow&conceptual level- 
don't have to worry about synchronising the various maps and segregating the work-load
but it be more temperamental when it comes to coding. 
Will have to put in handlers for the inevitable mem-access errors- when two threads try to access the same pointer at the same time.

*/
void nodeRead(int i, int j, int cluster){
	int notdone = 1;
	printf("entering while loop");
	while(notdone){
		/*
		progress logic:
		if unseen-child{
			parent.child-direction = 1
			store direction
			check child- keep i&j constant, use i+/- or j+/-1 to select child, or set up second variable set.
			if child.is_full{
				if(already grouped){
					child.prior-direction =2;
					active.child-direction=2;
					continue;
				}
				child.parent=[i,j]
				(determine parent-direction);
				child.parent-direction =2(?)
				update i&j;
				continue;
			}else{
				for all neighbour-nodes: node.this-direction =2;
				active-node=node.parent;
				check for un-processed child-
				
			}
			
		}
		*/
		struct node active= myspace.map[i][j];
		char childDir;
		int x,y; //the co-ordinates of the child-node (will be useful for parallelising, I think - something like: Here's your new start point, <x,y> come back when you're done.)
		//if direction supports searching:{
		//set new coordinates
		//set a direction variable char childDir= u/d/l/r;
		//active.direction=1;
		//}
		if(active.right==0){ //this gets to go first to get as much use out of the current array in memory as possible when the grids get big.
			x=(i+1)%SIDE_LENGTH;
			y=j;
			childDir = 'r';
			active.right=1;
		}else if(active.left==0){
			x=i-1;
			if(x<0){
				x=SIDE_LENGTH-1;
			}
			y=j;
			childDir = 'l';
			active.left=1;
		}else if(active.up==0){
			x=i;
			y=j-1;
			childDir = 'u';
			active.up=1;
		}else if(active.down==0){
			x=i;
			y=j+1;
			childDir = 'd';
			active.down=1;
		}else if(active.parent[0]==-1 && active.parent[1]==-1){
			printf("%i ,,, %i\n",i,j);
			printf("JOB DONE PLACEHOLDER \n");
			//store data and finish function
			for(int a =0; a<SIDE_LENGTH;a++){
				for(int b=0;b<SIDE_LENGTH;b++){
					printf("%i ",myspace.map[a][b].clusterNum);
				}
				printf("\n");
			}
			notdone=0;
		}else{
			int parent[]={active.parent[0], active.parent[1]}; //probably needs fixing
			active=myspace.map[parent[0]][parent[1]]; //to prevent any issue with defining using the outgoing variable
			if(parent[0]-i==0){
				if(parent[1]-j==1){
					active.down=2;
				}else{
					active.up=2;
				}
			}else if(parent[1]-j==0){
				if(parent[0]-i==1){
					active.right=2;
				}else{
					active.left=2;
				}
			}else{
				printf("Another useful error message\n");
			}
			i = parent[0];
			j=parent[1];
			myspace.map[i][j] = active;
			//go to parent and mark child as complete
			//then iterate loop
			continue;
		}
		myspace.map[i][j]=active;
		active = myspace.map[x][y];
		switch(active.clusterNum){
			case -1 :
				//do the empty-cluster fandango
				//should change this to handle all neighbours
				active=myspace.map[i][j];
				switch(childDir){
					case 'u':
						active.up=2;
						break;
					case 'd':
						active.down=2;
						break;
					case 'l':
						active.left=2;
						break;
					case 'r':
						active.right=2;
						break;
					default:
						printf("Some descriptive error here\n");
						break;
				}
				break;
			case 0 :
				active.parent[0] =i;
				active.parent[1] =j;
				active.clusterNum = cluster;
				switch(childDir){ //this next bit is inverted to the previous switch- this is because it's stating that the parent direction has been searched, rather than blocking a child direction.
					case 'u':
						active.down=2;
						break;
					case 'd':
						active.up=2;
						break;
					case 'r':
						active.left=2;
						break;
					case 'l':
						active.right=2;
						break;
					default:
						printf("Some third descriptive error here\n");
						break;
				}
				i=x;
				j=y;
				myspace.map[i][j]=active;
				break;
			case 1 : 
				switch(childDir){ //this next bit is inverted to the previous switch- this is because it's stating that the parent direction has been searched, rather than blocking a child direction.
					case 'u':
						active.down=2;
						break;
					case 'd':
						active.up=2;
						break;
					case 'r':
						active.left=2;
						break;
					case 'l':
						active.right=2;
						break;
					default:
						printf("Some third descriptive error here\n");
						break;
				}
				myspace.map[x][y]=active;
				active = myspace.map[i][j];
				switch(childDir){
					case 'u':
						active.up=2;
						break;
					case 'd':
						active.down=2;
						break;
					case 'l':
						active.left=2;
						break;
					case 'r':
						active.right=2;
						break;
					default:
						printf("ran into a 1 then exploded\n");
						break;
				}
				break;
				
		}
	}
	//return myspace;
}




/*
this will do a Depth first navigation of the matrix
*/
void navman(){
	int clusters =1;
	for(int i=0; i<SIDE_LENGTH; i++){
		printf("Cluster %d \n\n",clusters);
		if(myspace.map[i][0].clusterNum==0){ //a full, but not yet processed node
			myspace.map[i][0].clusterNum=clusters;
			myspace.map[i][0].parent[0]=-1; //To indicate this is the core node of the search pattern
			myspace.map[i][0].parent[1]=-1; //To indicate this is the core node of the search pattern
			nodeRead(i,0,clusters);
			printf("still alive"); //this will need to be replaced. need to work out the pointers for the above functions to do so though.
			clusters++;
		}
	}
}

/*
This is a test version that just sequentially analyses the grid and puts in connecting lines.
Will get more involved in time.
*/
void display(struct space star){
	//int cluster = 0;
	for(int i = 0; i<SIDE_LENGTH; i++){
		char upper[20];
		char lower[20];
		for(int j =0;j<SIDE_LENGTH;j++){
			upper[(j*2)+1]=' ';
			if(star.map[i][j].clusterNum==0){
				lower[j*2] = '1';
				if((i!=0)&&star.map[(i+9)%SIDE_LENGTH][j].clusterNum==0){
					upper[j*2]= '|';
				}
				else{
					upper[j*2]= ' ';
				}
				
				if(star.map[i][(j+1)%SIDE_LENGTH].clusterNum==0){
					lower[(j*2)+1]='-';
				}
				else{
					lower[(j*2)+1]=' ';
				}
			}
			else{
				upper[j*2]= ' ';
				lower[(j*2)] = '0';
				lower[(j*2)+1] = ' ';
			}
		}
		upper[20] = '\0';
		lower[20] = '\0';
		printf("%s\n", upper);
		printf("%s\n", lower);
	}
}


int main(int argc, char *argv[]){
	double a = 0.5;
	
	starmap(a);
	display(myspace);
	printf("Startign");
	navman();
	/*
	for(int i=0; i<10;i++){
		for(int j=0;j<10;j++){
			if(strstr((star.map[i][j]).full,"T")){
				printf("1 ");
			}else{
				printf("0 ");
			}
		}
		printf("\n");
	}
	*/
}