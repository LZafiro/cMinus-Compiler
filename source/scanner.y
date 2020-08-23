%{

#define YYPARSER
#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static TreeNode *savedTree;
static int yylex(void);

extern void abrirArq();
void yyerror(char *);

%}

%start programa
%token IF ELSE RETURN INT VOID WHILE
%token ID NUM
%token IGL PEV APR FPR MENOR MENORIGUAL MAIOR MAIORIGUAL IGUALIGUAL DIFERENTE VIRGULA 
%token COLCHETEABRE COLCHETEFECHA CHAVESABRE CHAVESFECHA SOM SUB MUL DIV
%token ERR FIM
%expect 1

%%


programa: 			declaracao_lista
					{						
						savedTree = $1;
					};

declaracao_lista:	declaracao_lista declaracao 
					{
						YYSTYPE t = $1;
						if(t != NULL){
							while(t -> sibling != NULL){
								t = t->sibling;
							}
							t->sibling = $2;
							$$ = $1;
						} else {
							$$ = $2;
						}
					}

				| 	declaracao
					{
						$$ = $1;
					} ;

declaracao: 		var_declaracao 
					{
						$$ = $1;
					}
				| 	fun_declaracao
					{
						$$ = $1;
					} ;

id:   			ID
                        {
                             $$ = newExpNode(idK);
                             $$->attr.name = copyString(tokenString);
						};

var_declaracao: 	INT id PEV
					{
						$$ = newExpNode(typeK);
						$$->type = integert;
						$$->attr.name = "integer";
						$$->child[0] = $2;
						$2->nodekind = statementK;
						$2->kind.stmt = varK;
						$2->type = integert;
						$2->attr.len = 1;
					} 

				| 	INT id COLCHETEABRE num COLCHETEFECHA PEV
					{	
						$$ = newExpNode(typeK);
						$$->type = integert;
						$$->attr.name = "integer";
						$$->child[0] = $2;
						$2->nodekind = statementK;
						$2->kind.stmt = varK;
						$2->type = integert;
						$2->attr.len = $4->attr.val; 	
					} 
				|
					VOID id PEV
					{
						$$ = newExpNode(typeK);
						$$->type = voidt;
						$$->attr.name = "void";
						$$->child[0] = $2;
						$2->nodekind = statementK;
						$2->kind.stmt = varK;
						$2->type = voidt;
						$2->attr.len = 1;
					} 
				|	
					VOID id COLCHETEABRE num COLCHETEFECHA PEV
					{	
						$$ = newExpNode(typeK);
						$$->type = voidt;
						$$->attr.name = "void";
						$$->child[0] = $2;
						$2->nodekind = statementK;
						$2->kind.stmt = varK;
						$2->type = voidt;
						$2->attr.len = $4->attr.val;
					} ;
				

num:				NUM{
						$$ = newExpNode(constK);
                        $$->attr.val = atoi(tokenString);
						$$->type = integert;
					};

fun_declaracao: 	INT id APR params FPR composto_decl
					{

						$$ = newExpNode(typeK);
                    	$$->type = integert;
                    	$$->attr.name = "integer";
                    	$$->child[0] = $2;
                    	$2->child[0] = $4;
                    	$2->child[1] = $6;
                    	$2->nodekind = statementK;
                    	$2->kind.stmt = funcK;
						$2->type = integert;
						$4->type = integert;
						aggScope($2->child[0], $2->attr.name);
						aggScope($2->child[1], $2->attr.name);

					} 
				| 	VOID id APR params FPR composto_decl
					{
						
						$$ = newExpNode(typeK);
                    	$$->type = voidt;
                    	$$->attr.name = "void";
                    	$$->child[0] = $2;
                    	$2->child[0] = $4;
                   	 	$2->child[1] = $6;
                    	$2->nodekind = statementK;
                    	$2->kind.stmt = funcK;
						aggScope($2->child[0], $2->attr.name);
						aggScope($2->child[1], $2->attr.name);

					};

params:	 			param_lista{
					
						$$ = $1;
					} 
				|	VOID{
						$$ = newExpNode(typeK);
           				$$->attr.name = "void";
					};

param_lista: 		param_lista VIRGULA param{ 

						YYSTYPE t = $1;
                        	if(t != NULL){
                            	while(t->sibling != NULL)
                                	t = t->sibling;
                            	t->sibling = $3;
                            	$$ = $1;
                            }
                            else
                              $$ = $3;

					}
				| 	param{
			
						$$ = $1;
					};

param: 				INT id{
					
						$$ = newExpNode(typeK);
					    $2->nodekind = statementK;
                        $2->kind.stmt = paramK;
                        $$->type = integert;
						$2->type = integert; 	
                        $$->attr.name = "integer";
                        $$->child[0] = $2;

					} 
				| 	INT id COLCHETEABRE COLCHETEFECHA{
						
						$$ = newExpNode(typeK);
						$2->nodekind = statementK;
                        $2->kind.stmt = paramK;
                        $$->type = integert;
                        $$->attr.name = "integer";
                        $$->child[0] = $2;
						$2->type = integert;

					}
				|	VOID id{
						
						$$ = newExpNode(typeK);
					    $2->nodekind = statementK;
                        $2->kind.stmt = paramK;
                        $$->type = voidt;
						$2->type = voidt; 	
                        $$->attr.name = "integer";
                        $$->child[0] = $2;

					} 
				| 	VOID id COLCHETEABRE COLCHETEFECHA{
					
						$$ = newExpNode(typeK);
						$2->nodekind = statementK;
                        $2->kind.stmt = paramK;
                        $$->type = voidt;
                        $$->attr.name = "void";
                        $$->child[0] = $2;
						$2->type = voidt;

					};

composto_decl: 		CHAVESABRE local_declaracoes statement_lista CHAVESFECHA{

					
						YYSTYPE t = $2;
                        if(t != NULL){
							while(t->sibling != NULL)
                            t = t->sibling;
                            t->sibling = $3;
                            $$ = $2;
                        }
						else
                        	$$ = $3;

					}
				|   CHAVESABRE local_declaracoes CHAVESFECHA{
                            
							$$ = $2;
                    
					}
                |   CHAVESABRE statement_lista CHAVESFECHA{
                    
					        $$ = $2;
                     
					}
                |   CHAVESABRE CHAVESFECHA{
			   		};

local_declaracoes: 	local_declaracoes var_declaracao{

						
						YYSTYPE t = $1;
                        if(t != NULL){
                        	while(t->sibling != NULL)
                            	t = t->sibling;
                            t->sibling = $2;
                            $$ = $1;
                        }
                        else
                        	$$ = $2;

					} 
				| 	var_declaracao{

						$$ = $1;

					};

statement_lista: 	statement_lista statement{

					
                    	YYSTYPE t = $1;
                        if(t != NULL){
                        	while(t->sibling != NULL)
                            	t = t->sibling;
                            t->sibling = $2;
                            $$ = $1;
                        }
                    	else
                        	$$ = $2;
					
					} 
				| statement
                        {
                           $$ = $1;
                        };

statement: 			expressao_decl{
						
						$$ = $1;

					} 
				| 	composto_decl{
						
						$$ = $1;

					} 
				| 	selecao_decl{
						
						$$ = $1;

					} 
				| 	iteracao_decl{
						
						$$ = $1;

					} 
				| 	retorno_decl{

					
						$$ = $1;

					};

expressao_decl: 	expressao PEV{
						$$ = $1;
					} 
				| 	PEV{

					} ;

selecao_decl: 		IF APR expressao FPR statement{
						$$ = newStmtNode(ifK);
                    	$$->child[0] = $3;
                    	$$->child[1] = $5;
					} 
				| 	IF APR expressao FPR statement ELSE statement{
						$$ = newStmtNode(ifK);
                        $$->child[0] = $3;
                        $$->child[1] = $5;
                        $$->child[2] = $7;
					};

iteracao_decl: 		WHILE APR expressao FPR statement{
						$$ = newStmtNode(whileK);
                        $$->child[0] = $3;
                        $$->child[1] = $5;
					};

retorno_decl: 		RETURN PEV{	
						$$ = newStmtNode(returnK);
						$$->type = voidt;
					} 
				| 	RETURN expressao PEV{	
						$$ = newStmtNode(returnK);
                        $$->child[0] = $2;
					};

expressao: 			var IGL expressao{
						$$ = newStmtNode(assignK);
                        $$->child[0] = $1;
                        $$->child[1] = $3;
					} 
				| 	simples_expressao{
						 $$ = $1;
					};

var: 				id{
						$$ = $1;
						//$$->type = integert;
					} 
				| 	id COLCHETEABRE expressao COLCHETEFECHA{
						$$ = $1;
                        $$->child[0] = $3;
                        $$->kind.exp = vectK;
						//$$->type = integert;
					};

simples_expressao:	soma_expressao relacional soma_expressao{
						$$ = $2;
                        $$->child[0] = $1;
                        $$->child[1] = $3;
					} 
				| 	soma_expressao{
						$$ = $1;
				};

relacional: 		MENORIGUAL{
						$$ = newExpNode(opK);
                        $$->attr.op = MENORIGUAL;                            
						$$->type = booleant;
					} 
				| 	MENOR{
						$$ = newExpNode(opK);
                        $$->attr.op = MENOR;                            
						$$->type = booleant;
					} 
				| 	MAIOR{
						$$ = newExpNode(opK);
                        $$->attr.op = MAIOR;                            
						$$->type = booleant;
					} 
				| 	MAIORIGUAL{
						$$ = newExpNode(opK);
                        $$->attr.op = MAIORIGUAL;                            
						$$->type = booleant;
					} 
				| 	IGUALIGUAL{
						$$ = newExpNode(opK);
                        $$->attr.op = IGUALIGUAL;
						$$->type = booleant;   
					} 
				| 	DIFERENTE{
						$$ = newExpNode(opK);
                        $$->attr.op = DIFERENTE;
						$$->type = booleant;  
					};

soma_expressao: 	soma_expressao soma termo{	
						$$ = $2;
                    	$$->child[0] = $1;
                    	$$->child[1] = $3;
					} 
				| 	termo{
					
						 $$ = $1;

					};

soma: 				SOM{
						$$ = newExpNode(opK);
                        $$->attr.op = SOM;     
					} 
				| 	SUB{
						$$ = newExpNode(opK);
                        $$->attr.op = SUB;     
					};

termo: 				termo mult fator{
						$$ = $2;
                        $$->child[0] = $1;
                        $$->child[1] = $3;
					} 
				| 	fator{			
						$$ = $1;
					};

mult: 				MUL{
						$$ = newExpNode(opK);
                        $$->attr.op = MUL; 
					} 
				| 	DIV{
						$$ = newExpNode(opK);
                    	$$->attr.op = DIV; 
					};

fator: 				APR expressao FPR{
						$$ = $2;
					} 
				| 	var{
						$$ = $1;
					} 
				| 	ativacao{
						$$ = $1;
					} 
				|   num{
						$$ = $1;

					};

ativacao: 			id APR arg_lista FPR{
						$$ = $1;
                        $$->child[0] = $3;
                        $$->nodekind = statementK;
                        $$->kind.stmt = callK;
					}
				|	id APR FPR{
						$$ = $1;
                        $$->nodekind = statementK;
                        $$->kind.stmt = callK;
					};

arg_lista: 			arg_lista VIRGULA expressao{
						YYSTYPE t = $1;
                        if(t != NULL){
                           while(t->sibling != NULL)
                            	t = t->sibling;
                            t->sibling = $3;
                            $$ = $1;
                        }
                        else
                            $$ = $3;
					} 
				| 	expressao{
						$$ = $1;
					};

%%

void yyerror(char* message){
	
	if(yychar != ERR){
		if(yychar != FIM){	
    		fprintf(listing,"Erro sintático na linha %d: %s\n",lineno,message);
    		fprintf(listing,"Token atual: ");
    		printToken(listing, yychar,tokenString);
    		Error = TRUE;
		}
	}
	else
		Error = TRUE;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void){
    return getToken();
}

TreeNode * parse(void){
    yyparse();
    return savedTree;
}