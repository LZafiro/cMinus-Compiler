#ifndef _SYMTAB_H_
#define _SYMTAB_H_ 


/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( char * name, int lineno, int loc, char* scope, char* typeID, char* typeData, int paramQt);

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
int st_lookup (char * name, char* scope, char* typeID);

int st_lookup_memloc (char* name, char* Escopo); 

char* st_lookup_type(char* name, char* scope);

/* Looks up for the number of parameters of a function, used in function calls */
int st_lookup_paramQt(char *name, char *scope);

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing);

#endif