#include "globals.h"
#include "symtab.h"
#include "analyze.h"

static int location = 0;

static void typeError(TreeNode * t, char * message)
{ fprintf(listing,"Erro de tipo em %s na linha %d: %s.\n",t->attr.name, t->lineno,message);
  Error = TRUE;
}

static void insertNode( TreeNode * t)
{ 
	int tmpParam;
	switch (t->nodekind)
    { 
		case statementK:
      	switch (t->kind.stmt)
        { 	case paramK:
				//Verificar se aqui deve ser substituido: variable -> parameter
          		if (st_lookup(t->attr.name, t->attr.scope,"variable") == -1 && st_lookup(t->attr.name, "global","variable") == -1)
            		st_insert(t->attr.name,t->lineno,location++, t->attr.scope, "variable", (t->type ==1) ? "integer":"void", 0);
          		else
            		typeError(t,"Declaração Inválida. Parâmetro já declarado.");	
            break;	
			case varK:
          		if (st_lookup(t->attr.name, t->attr.scope,"variable") == -1 && st_lookup(t->attr.name, "global","variable") == -1){
            		st_insert(t->attr.name,t->lineno,location++, t->attr.scope, "variable", (t->type ==1) ? "integer":"void", 0);

				  }
          		else
            		typeError(t,"Declaração Inválida. Variável já declarada.");	
				if(t->type == voidt)
					typeError(t, "Declaração Invalida. Variável do tipo Void");
            break;
			case funcK:

				if (st_lookup(t->attr.name, t->attr.scope,"function") == -1 && st_lookup(t->attr.name, "global","function") == -1)
				{
					/* Counts the number of parameters thar the function needs */
					tmpParam = paramCounter(t);
					if(t->type == integert){
                        st_insert(t->attr.name,t->lineno,location++, t->attr.scope,"function", "integer", tmpParam);
						
					}
					else{
                        st_insert(t->attr.name,t->lineno,location++, t->attr.scope,"function", "void", tmpParam);	
					}
				}
          		else
            		typeError(t,"Declaração Inválida. Função já declarada.");	
			break;
			case callK:
				if(strcmp(t -> attr.name, "input") != 0 && strcmp(t -> attr.name, "output") != 0 && strcmp(t -> attr.name, "main") != 0){
					if (st_lookup(t->attr.name, t->attr.scope,"function") == -1 && st_lookup(t->attr.name, "global","function") == -1)
						typeError(t,"Chamada inválida. Função não declarada.");	
					else{
						st_insert(t->attr.name,t->lineno,location++, t->attr.scope, "call", "-", paramCounter(t));
					}
				}
          	break;
			case returnK:
			break;
        	default:
          break;
      }
      break;
      case expressionK:
      switch (t->kind.exp)
      { 
		  case idK:
			if (st_lookup(t->attr.name, t->attr.scope,"variable") == -1 && st_lookup(t->attr.name, "global","variable") == -1)
				 typeError(t,"Erro 1: ainda não declarado.");
			else{
            	st_insert(t->attr.name,t->lineno,0, t->attr.scope, "variable", st_lookup_type(t->attr.name, t->attr.scope), 0);	
				
			}
		  break;
	      case vectK:
	      	if (st_lookup(t->attr.name, t->attr.scope,"vector") == -1 && st_lookup(t->attr.name, "global","vector") == -1)
				 typeError(t,"Erro 1: ainda não declarado.");
			else
            	st_insert(t->attr.name,t->lineno,0, t->attr.scope, "vector", st_lookup_type(t->attr.name, t->attr.scope), 0);	
		  break;
		  case vectIndexK:
		  	if (st_lookup(t->attr.name, t->attr.scope,"vector index") == -1 && st_lookup(t->attr.name, "global","vector index") == -1)
				 typeError(t,"Erro 1: ainda não declarado.");
			else
            	st_insert(t->attr.name,t->lineno,0, t->attr.scope, "vector index", "integer", 0);	
          break;
        default:
        break;
      }
      break;
    default:
      break;
  }
}

static void checkNode(TreeNode * t)
{ switch (t->nodekind)
  { 
   case expressionK:
      switch (t->kind.exp)
      { 
        case opK:
        break;
        default:
        break;
      }
      break;
    case statementK:
      switch (t->kind.stmt)
      { 
        case ifK:
          if (t->child[0]->type != booleant)
           typeError(t->child[0],"Argumento do IF não booleano.");
        break;

		case whileK:
          if (t->child[0]->type != booleant)
           typeError(t->child[0],"Argumento do WHILE não booleano.");
        break;

        case assignK:
          if (t->child[0]->type == voidt || t->child[1]->type == voidt)
            typeError(t->child[0],"Atribuição de um valor não inteiro");
		  
		  else if(t->child[1]->kind.stmt == callK)
		  {
		    if(strcmp(st_lookup_type(t->child[1]->attr.name, "global"), "integer") != 0 && strcmp(t->child[1]->attr.name, "input") != 0 ){
					typeError(t->child[1],"Atribuição de retorno vazio");
			}
		}
        break;

		case callK:
			/* Verifies if the parameters quantity match in declaration and call */
			if(st_lookup_paramQt(t->attr.name, t->attr.scope) != st_lookup_paramQt(t->attr.name, "global")){
				typeError(t, "Parameter quantity are not the same as in the declaration!");
			}
		break;
		
        default:
        break;
      }
    break;

    default:
    break;
  }
}

/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
static void traverse(TreeNode *t, void (*preProc) (TreeNode *), void (*postProc) (TreeNode *)){ 
	
	if (t != NULL){ 
		preProc(t);
    	{ 
	  		int i;
	        for (i=0; i < MAXCHILDREN; i++)
        		traverse(t->child[i],preProc,postProc);
    	}
    	postProc(t);
    	traverse(t->sibling,preProc,postProc);
  	}
}

/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t)
{ 
	if (t==NULL) 
		return;
  	else 
		return;
}

void buildSymtab(TreeNode * syntaxTree)
{ 
   
  traverse(syntaxTree,insertNode,nullProc); 
  if(st_lookup("main", "global","function") == -1)
   {
   		fprintf(listing,"Erro referência indefinida para main.\n");
   		Error = TRUE; 
   }
	
  if (TraceAnalyze && !Error)
  { 
    //fprintf(listing,"\nSymbol table:\n\n");
    printSymTab(symTab);
  }
}
 
void typeCheck(TreeNode * syntaxTree)
{ 
    traverse(syntaxTree,nullProc,checkNode);
}
