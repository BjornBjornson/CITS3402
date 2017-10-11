#include "uthash.h"
int main(int argc, char *argv[]){
	struct mine{
	int a;
	int b;
	UT_hash_handle hh;
	};
	struct mine *banana =NULL;
	struct mine *peel;
	peel=malloc(sizeof(struct mine));
	peel->a = 2;
	peel->b=3;
	HASH_ADD_INT(banana, a, peel);
	struct mine *steel;
	steel=malloc(sizeof(struct mine));
	steel->a = 2;
	steel->b =4;
	HASH_REPLACE_INT(banana, a, steel, peel);
	
	return 2;
}