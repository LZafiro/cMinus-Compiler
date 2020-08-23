%{
#include <stdlib.h>
#include <stdio.h>
#include "globals.h"
#include "util.h"
#include "scan.h"

char tokenString[MAXTOKENLEN+1];
static int yylex(void);

int yywrap ();

%}

DIGITO [0-9]
ESPACO " " 
LETRA [a-zA-Z]
ELSE else
IF if
INT int
RETURN return
VOID void 
WHILE while

%%

"/*" { 
	char c;
	do{ 
		c = input();
		if (c == EOF) 
			break;
		if (c == '\n') 
		lineno++;
		if (c == '*') {
			if(input() == '/')
				break;
		}		
	} 
	while (1);
	}

{ELSE}										return ELSE;
{IF}										return IF;
{INT}										return INT;
{RETURN}									return RETURN;
{VOID}										return VOID;
{WHILE}										return WHILE;
{DIGITO}+									return NUM;
{LETRA}({LETRA}|{DIGITO})*					return ID;
"+"											return SOM;
"-"											return SUB;
"*"											return MUL;
"/"											return DIV;	
"<"											return MENOR;
"<="										return MENORIGUAL;
">"											return MAIOR;
">="										return MAIORIGUAL;
"=="										return IGUALIGUAL;
"!="										return DIFERENTE;
,											return VIRGULA;
"{"											return CHAVESABRE;
"}"											return CHAVESFECHA;	
"["											return COLCHETEABRE;				
"]"											return COLCHETEFECHA;
=											return IGL;
;											return PEV;
"("											return APR;
")"											return FPR;
<<EOF>> 									return FIM;
\n              							lineno++;               
[ \t]+
.|({DIGITO}+{LETRA}+)						return ERR;

%%

void abrirArq(){
	yyin = fopen("entrada.txt", "r");
}

int yywrap () {  };

TokenType getToken(void){ 
	static int firstTime = TRUE;
 	TokenType currentToken;
 	if (firstTime){ 
		firstTime = FALSE;
   	lineno = lineno + 1;
  	yyin = source;
 	yyout = listing;
 	}
 	currentToken = yylex();
 	strncpy(tokenString,yytext,MAXTOKENLEN);
  	if (TraceScan) {
		  if(currentToken != FIM){ 
    		fprintf(tokenList,"\t%d: ",lineno);
    		printToken(tokenList, currentToken,tokenString);
		  }
	 }
	 
	if(currentToken == ERR){
		printf("\nERRO LÉXICO. Linha: %d, ", lineno);
		printToken(listing, currentToken,tokenString);
		Error = TRUE;
	}
	return currentToken;
}
