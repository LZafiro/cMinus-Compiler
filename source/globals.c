#include "globals.h"

char *convertIntChar(int x){

    /* "NULL, 0" makes ir return the lenght needed */
    int lenght = snprintf(NULL, 0, "%d", x);

    /* Buffer allocation */
    char* tmp = (char *)malloc((lenght + 1) * sizeof(char));
    if(tmp == NULL) exit(-1);

    /* Creates the string into the tmp buffer */ 
    snprintf(tmp, lenght + 1, "%d", x);

    return(tmp);
}

int paramCounter(TreeNode *t){

	int tmp = 0;

	/* Selects the left child (list of parameters/arguments) */
	t = t->child[0];

	while(t != NULL){
		tmp++;
		t = t->sibling;
	}

	return tmp;
}