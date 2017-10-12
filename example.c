/*
Example 1:
Global variable
*/

int DATA[12]={0};

void function(/* no input variables*/){
	for(int i; i<12; i++){
		printf("%i\n",DATA[i]);
	}
}



/*
Example 2:
Instance/local variable
*/

void function2(int *DATA2){//notice: the array gets automatically retyped as a pointer
	for(int i; i<12; i++){
		printf("%i\n",DATA2[i]);//functionally very similar
	}
}



int main(int argc, char *argv[]){
	function(); //just calling it from here
	int example2[12]={0};
	function2(example2);
}