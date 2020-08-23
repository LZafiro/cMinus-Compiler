#include "globals.h"
#include "util.h"
#include "parse.h"
#include "scan.h"
#include "symtab.h"
#include "analyze.h"
#include "intermediateCode.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE FALSE

/* allocate global variables */
int  lineno = 0;
FILE *source;
FILE *listing;
FILE *tokenList;
FILE *symTab;
FILE *synTree;
FILE *intermediateCode;

/* allocate and set tracing flags */
int TraceScan = TRUE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int Error = FALSE;


int main( int argc, char * argv[] ){ 

    TreeNode * syntaxTree;
    QuadrupleListElem interCodeList;

    char pgm[120]; /* source code file name */
    
    if (argc != 2){ 
        fprintf(stderr,"Arquivo: %s <filename>\n",argv[0]);
        exit(1);
    }
    
    strcpy(pgm,argv[1]) ;
    
    source = fopen(pgm,"r");
    
    if (source==NULL){ 
        fprintf(stderr,"Arquivo %s não encontrado!\n",pgm);
        exit(1);
    }
 
    listing = stdout; /* send listing to screen */

    tokenList = fopen("./outputs/tokenList.txt","w");
    symTab = fopen("./outputs/symbolTable.txt","w");
    synTree = fopen("./outputs/syntaxTree.txt","w"); 
    intermediateCode = fopen("./outputs/intermediateCode.txt","w");

    fprintf(listing, "###################################################");
    fprintf(listing,"\nZAFx32 Compilation... File: %s\n\n",pgm);
    
    if(NO_PARSE){
        while (getToken()!=ENDFILE);
    }
    else{
        
	//printf("Erro: %d\n\n",Error);

    fprintf(listing, "###################################################\n");

	if(!Error){
		syntaxTree = parse();

        	if (TraceParse && !Error) {
            		fprintf(listing,"Building Syntax Tree...\n");
            		printTree(syntaxTree);
        	}
    	
        if(!NO_ANALYZE){

            if (! Error){ 
                if (TraceAnalyze){
                    fprintf(listing,"Building Symbol Table...\n");
                    buildSymtab(syntaxTree);
                }

                if (TraceAnalyze){ 
                    fprintf(listing,"Type Checking...\n");
                    typeCheck(syntaxTree);
                }

                //if (TraceAnalyze && !Error) 
                    //fprintf(listing,"Type Checking Finished!\n");
                }
            }
        }
    }

    /* ..::Intermediate Code Generation::.. */
    //////////////////////////////////////////////////////////////

    /* Creates the Intermediate Code by traverse the Syntax Tree
     * Returns the list os quadruples (interCodeList) */
    interCodeList = intermediateCodeGenerator(syntaxTree);
    printIntermediateCode(interCodeList);
    //////////////////////////////////////////////////////////////

    fprintf(listing, "###################################################\n");

    if(!Error){
        fprintf(listing,"\nCompilation Concluded.\n");
        fprintf(listing, "Check 'outputs' directory to view the results.\n");
        fprintf(listing, "###################################################\n");
    }
    
    fclose(source);
    fclose(tokenList);
    fclose(symTab);
    fclose(synTree);
    fclose(intermediateCode);
   
    return 0;

}
