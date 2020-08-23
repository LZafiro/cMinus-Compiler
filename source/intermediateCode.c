/* For more explanations of all function and structures, check intermediateCode.h */

/* Already includes globals.h */
#include "intermediateCode.h"
/* Gives me access to the symbol table and all corresponding functions */
#include "symtab.h"

/* Declarations */
/////////////////////////////////////////////////////////////////////////////////
/* Variables for controlling the temporary values for the registers and the labels */
int cur_lab = 0, cur_reg = 0; 

char opNameList[28][7] = {"ADD", "SUB", "MUL", "DIV", "EQ", "DIF", "LT", "LET", "GT", "GET", "AND", "OR", 
"ASSIGN", "ALLOC", "IMME", "LOAD", "STORE", "ARR", "GOTO", "IFF", "RET", "FUNC",
"END", "PARAM", "CALL", "ARG", "LAB", "HALT"};
    /*
    * LT = Less than
    * LET = Less and Equal than
    * GT = Grather than
    * GET = Greather and equal than
    * IMME = Immidiate 
    * ARR = Array
    * IFF = If false
    * END = End of function
    * LAB = Labbel
    */

/* All functions related to the intermediate code generation */
/////////////////////////////////////////////////////////////////////////////////

void regIncrease(void){
    /* Because there are 16 temporary registers */ 
    if(cur_reg == 15)
        cur_reg = 0;
    
    else 
        cur_reg++;
}

Address newAddress(){

    Address tmp = (Address)malloc(sizeof(struct address));

    /* Allocation verification */
    if(tmp == NULL) return NULL;

    /* Just to future verifications and comparations */
    tmp->contents.var.name = NULL;
    tmp->contents.var.scope = NULL;

    return tmp;
}

Quadruple newQuad(){

    Quadruple tmp = (Quadruple)malloc(sizeof(struct quadruple));

    /* Allocation verification */
    if(tmp == NULL) return NULL;

    return tmp;
}

QuadrupleListElem newQuadListElem(){

    QuadrupleListElem tmp = (QuadrupleListElem)malloc(sizeof(struct QuadListElem));

    /* Allocation verification */
    if(tmp == NULL) return NULL;;

    /* Initialize with no next element */
    tmp->next = NULL;

    tmp->quadruple = newQuad();

    tmp->quadruple->addr1 = newAddress();
    tmp->quadruple->addr2 = newAddress();
    tmp->quadruple->addr3 = newAddress();

    return tmp;
}

QuadrupleListElem addElemToList(QuadrupleListElem listtop, QuadrupleListElem element){

    QuadrupleListElem aux;

    /* Empty list */
    if(listtop == NULL){
        listtop = element;
    }

    /* Only one element */
    else if(listtop->next == NULL){ 
        listtop->next = element;
    }

    else{
        aux = listtop->next;

        /* Runs until it finds the las element */
        while(aux->next != NULL)
            aux = aux->next;

        aux->next = element; 
    }

    return listtop;
}

char *addrPrintGenerator(Address addr){

    /* Selects based on the types of addresses */
    switch (addr->kind){

        case String:
            return(addr->contents.var.name); /* Verify */
        break;

        case IntConst:
            return(convertIntChar(addr->contents.val));
        break;

        case Empty:
            return("-");
        break;

        default:
        break;
    }
}

void printIntermediateCode(QuadrupleListElem list){

    /* Buffer that will store the output string for the quadruple */
    char *buffer;
    const int bufferSize = 30;

    /* Temporary string */ 
    char *tmp;

    fprintf(listing, "Building Intermediate Code...\n");

    fprintf(intermediateCode, "..::Intermediate Code for ZAFx32 Processor::..\n\n");

    while(list != NULL){

        /* Buffer allocation */
        buffer = (char*)malloc(bufferSize * sizeof(char));
        if(buffer == NULL) exit(-1);

        /* Generates "(op, " */
        //////////////////////////////////////////////////////////////
        strcat(buffer, "(");

        /* Gets the name of the operator and puts into the buffer */
        strcat(buffer, opNameList[list->quadruple->op]);
        strcat(buffer, ", ");
        //////////////////////////////////////////////////////////////

        /* Generates "(op, addr1, " */
        //////////////////////////////////////////////////////////////
        tmp = addrPrintGenerator(list->quadruple->addr1);
        strcat(buffer, tmp);
        strcat(buffer, ", ");
        tmp = NULL;
        //////////////////////////////////////////////////////////////
    
        /* Generates "(op, addr1, addr2, " */
        //////////////////////////////////////////////////////////////
        tmp = addrPrintGenerator(list->quadruple->addr2);
        strcat(buffer, tmp);
        strcat(buffer, ", ");
        tmp = NULL;
        //////////////////////////////////////////////////////////////

        /* Generates "(op, addr1, addr2, addr3)" */
        //////////////////////////////////////////////////////////////
        tmp = addrPrintGenerator(list->quadruple->addr3);
        strcat(buffer, tmp);
        strcat(buffer, ")");
        tmp = NULL;
        //////////////////////////////////////////////////////////////

        /* Prints the quadruple into the file */
        fprintf(intermediateCode, "%s\n", buffer);

        /* Frees the buffer for future allocation */
        free(buffer);
        buffer = NULL;
        
        /* Next element */
        list = list->next;
    }
}

char *createsRegName(int x){
    
    /* Temorary buffer */
    char *buf;
    int buf_len = 4;

    buf = (char*)malloc(buf_len * sizeof(char));
    if(buf == NULL) exit(-1);

    strcat(buf, "$t");

    strcat(buf, convertIntChar(x));

    return(buf);
}

char *createsLabName(int x){
    
    /* Temorary buffer */
    char *buf;
    int buf_len = 3;

    buf = (char*)malloc(buf_len * sizeof(char));
    if(buf == NULL) exit(-1);

    strcat(buf, "L");

    strcat(buf, convertIntChar(x));

    return(buf);
}

QuadrupleListElem argGenerator(TreeNode *syntaxTree, QuadrupleListElem list){

    QuadrupleListElem quad;

    while(syntaxTree != NULL){

        /* Calls icGen but dont go automaticly to the sibling, because we want
         * To generate the arg quadruple everytime a new argument is found, if
         * we use icGen, only the first argumet will hava a coresponding quadruple
         * becouse there's not a argK defined */
        list = icGenArg(syntaxTree, list);

        /* Quadruple creation and setup (ARG) */
        //////////////////////////////////////////////////////////////

        quad = newQuadListElem();
        quad->quadruple->op = ARGc;
        quad->quadruple->addr1->kind = String;
        quad->quadruple->addr1->contents.var.name = createsRegName(syntaxTree->icTemp);
        quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
        quad->quadruple->addr2->kind = Empty;
        quad->quadruple->addr3->kind = Empty;
        
        /* Adds the quadruple to the list */
        list = addElemToList(list, quad);

        /* Ereases the quadruple created */
        
        quad = NULL;
        //////////////////////////////////////////////////////////////

        /* Next possible argument */
        syntaxTree = syntaxTree->sibling;
    }

    return(list);
}

QuadrupleListElem statementGenerator(TreeNode *syntaxTree, QuadrupleListElem list){
    
    /* Pointers for the 3 possible children */
    TreeNode *k1, *k2, *k3;

    /* Temporary pointer to a new quadruple */
    QuadrupleListElem quad;

    /* Temporary variables for the 'if' creation (local) */
    int tmp_iff1, tmp_iff2, tmp_lab1, tmp_lab2;

    //printf("\n--->Entrou no statementGenerator\n");
    //printf("---> %s\n", syntaxTree->attr.name);
    //printf("---> %d\n", syntaxTree->kind.stmt);

    /* Selects the type of statement that the node corresponds */
    switch(syntaxTree->kind.stmt){

        case ifK:
            /* Points to all the possible children */
            k1 = syntaxTree->child[0];
            k2 = syntaxTree->child[1];
            k3 = syntaxTree->child[2];         
            
            /* Calls to the condition child */
            list = icGen(k1, list);

            /* Quadruple creation and setup (IFF) */
            //////////////////////////////////////////////////////////////
            quad = newQuadListElem();
            quad->quadruple->op = IFFc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);;
            quad->quadruple->addr2->kind = String;
            /* This labble must be in the beguining of the 'else' block */
            quad->quadruple->addr2->contents.var.name = createsLabName(cur_lab);
            /* Stores the label related on thin IFF */
            tmp_lab1 = cur_lab;
            /* Increases the labbel count */ 
            cur_lab++;
            quad->quadruple->addr3->kind = Empty;

            /* Adds the quadruple to the list */
            list = addElemToList(list, quad);

            /* Ereases the quadruple created */
            quad = NULL;
            //////////////////////////////////////////////////////////////

            /* Calls to the 'then' child */
            list = icGen(k2, list);

            /* Quadruple creation and setup (GOTO) */
            //////////////////////////////////////////////////////////////
            quad = newQuadListElem();
            quad->quadruple->op = GOTOc;
            quad->quadruple->addr1->kind = String;
            /* This labbel must be in the end of the 'else' block */
            quad->quadruple->addr1->contents.var.name = createsLabName(cur_lab);
            /* Stores the label of the end of the 'else block' */
            tmp_lab2 = cur_lab;
            cur_lab++;
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = Empty;

            /* Adds the quadruple to the list */
            list = addElemToList(list, quad);

            /* Ereases the quadruple created */
            quad = NULL;
            //////////////////////////////////////////////////////////////

            /* Quadruple creation and setup (LAB) */
            //////////////////////////////////////////////////////////////
            quad = newQuadListElem();
            quad->quadruple->op = LABc;
            quad->quadruple->addr1->kind = String;
            /* This labbel must be the same as the IFF labbel -> tmp_lab1 */
            quad->quadruple->addr1->contents.var.name = createsLabName(tmp_lab1);
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = Empty;

            /* Adds the quadruple to the list */
            list = addElemToList(list, quad);

            /* Ereases the quadruple created */
            quad = NULL;
            //////////////////////////////////////////////////////////////

            /* Calls to the 'else' child */
            list = icGen(k3, list);

            /* Quadruple creation and setup (GOTO) */
            //////////////////////////////////////////////////////////////
            // quad = newQuadListElem();
            // quad->quadruple->op = GOTOc;
            // quad->quadruple->addr1->kind = String;
            // /* This labbel must be in the end of the 'else' block */
            // quad->quadruple->addr1->contents.var.name = createsLabName(tmp_lab2);
            // quad->quadruple->addr2->kind = Empty;
            // quad->quadruple->addr3->kind = Empty;

            // /* Adds the quadruple to the list */
            // list = addElemToList(list, quad);

            // /* Ereases the quadruple created */
            // quad = NULL;
            // //////////////////////////////////////////////////////////////

            /* Quadruple creation and setup (LAB) */
            //////////////////////////////////////////////////////////////
            quad = newQuadListElem();
            quad->quadruple->op = LABc;
            quad->quadruple->addr1->kind = String;
            /* This labbel is the end of the 'else' block */
            quad->quadruple->addr1->contents.var.name = createsLabName(tmp_lab2);
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = Empty;

            /* Adds the quadruple to the list */
            list = addElemToList(list, quad);

            /* Ereases the quadruple created */
            quad = NULL;
            //////////////////////////////////////////////////////////////
        break;

        case whileK:
            /* Points to the children */
            k1 = syntaxTree->child[0];
            k2 = syntaxTree->child[1];

            /* Quadruple creation and setup (LAB) */
            //////////////////////////////////////////////////////////////
            quad = newQuadListElem();
            quad->quadruple->op = LABc;
            quad->quadruple->addr1->kind = String;
            /* This labbel is in the top of the while */
            quad->quadruple->addr1->contents.var.name = createsLabName(cur_lab);
            tmp_lab1 = cur_lab;
            cur_lab++;
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = Empty;

            /* Adds the quadruple to the list */
            list = addElemToList(list, quad);

            /* Ereases the quadruple created */
            quad = NULL;
            //////////////////////////////////////////////////////////////

            /* Calls to the condition block */
            list = icGen(k1, list);

            /* Quadruple creation and setup (IFF) */
            //////////////////////////////////////////////////////////////
            quad = newQuadListElem();
            quad->quadruple->op = IFFc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
            quad->quadruple->addr2->kind = String;
            /* This labble must be the same as the end of the while block */
            quad->quadruple->addr2->contents.var.name = createsLabName(cur_lab);
            tmp_lab2 = cur_lab;
            cur_lab++;
            quad->quadruple->addr3->kind = Empty;

            /* Adds the quadruple to the list */
            list = addElemToList(list, quad);

            /* Ereases the quadruple created */   
            quad = NULL;
            //////////////////////////////////////////////////////////////

            /* Calls to the body of the while block */
            list = icGen(k2, list);

            /* Quadruple creation and setup (GOTO) */
            //////////////////////////////////////////////////////////////
            quad = newQuadListElem();
            quad->quadruple->op = GOTOc;
            quad->quadruple->addr1->kind = String;
            /* This labbel must be the same as the one into the top os the while block */
            quad->quadruple->addr1->contents.var.name = createsLabName(tmp_lab1);
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = Empty;

            /* Adds the quadruple to the list */
            list = addElemToList(list, quad);

            /* Ereases the quadruple created */           
            quad = NULL;
            //////////////////////////////////////////////////////////////            

            /* Quadruple creation and setup (LAB) */
            //////////////////////////////////////////////////////////////
            quad = newQuadListElem();
            quad->quadruple->op = LABc;
            quad->quadruple->addr1->kind = String;
            /* This labbel is the end of the while block */
            quad->quadruple->addr1->contents.var.name = createsLabName(tmp_lab2);
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = Empty;

            /* Adds the quadruple to the list */
            list = addElemToList(list, quad);

            /* Ereases the quadruple created */        
            quad = NULL;
            //////////////////////////////////////////////////////////////
        break;

        case assignK:
            /* Points to the children */
            k1 = syntaxTree->child[0];
            k2 = syntaxTree->child[1];

            /* Calls to the left side of equalty */
            list = icGen(k1, list);

            /* Calls to the right side of the equalty */
            list = icGen(k2, list);

            /* Quadruple creation and setup (ASSIGN) */
            //////////////////////////////////////////////////////////////
            quad = newQuadListElem();
            quad->quadruple->op = ASSIGNc;
            quad->quadruple->addr1->kind = String;
            /* Uses the temposrary value present into the tree node */
            quad->quadruple->addr1->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
            quad->quadruple->addr2->kind = String;
            quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);
            quad->quadruple->addr3->kind = Empty;

            /* Adds the quadruple to the list */
            list = addElemToList(list, quad);

            /* Ereases the quadruple created */
            quad = NULL;
            //////////////////////////////////////////////////////////////
     
            /* Quadruple creation and setup (STORE) */
            //////////////////////////////////////////////////////////////
            quad = newQuadListElem();
            quad->quadruple->op = STOREc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = syntaxTree->child[0]->attr.name;
            quad->quadruple->addr2->kind = String;
            quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
            quad->quadruple->addr3->kind = Empty;

            /* Adds the quadruple to the list */
            list = addElemToList(list, quad);

            /* Ereases the quadruple created */
            quad = NULL;
            //////////////////////////////////////////////////////////////
        break;

        case varK:
            /* Quadruple creation and setup (ALLOC) */
            //////////////////////////////////////////////////////////////
            quad = newQuadListElem();
            quad->quadruple->op = ALLOCc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = syntaxTree->attr.name;
            quad->quadruple->addr2->kind = IntConst;
            quad->quadruple->addr2->contents.val = syntaxTree->attr.len;
            quad->quadruple->addr3->kind = String;
            quad->quadruple->addr3->contents.var.name = quad->quadruple->addr3->contents.var.scope =  syntaxTree->attr.scope;
            
            /* Adds the quadruple to the list */
            list = addElemToList(list, quad);

            /* Ereases the quadruple created */  
            quad = NULL;
            //////////////////////////////////////////////////////////////
        break;

        case funcK:

            /* Points to the children */
            k1 = syntaxTree->child[0];
            k2 = syntaxTree->child[1];

            /* Quadruple creation and setup (FUNC) */
            //////////////////////////////////////////////////////////////

            quad = newQuadListElem();
            /* segmentation fault */
            quad->quadruple->op = FUNCc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = syntaxTree->attr.name;
            quad->quadruple->addr1->contents.var.scope =  syntaxTree->attr.scope;
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = IntConst;
            quad->quadruple->addr3->contents.val = st_lookup_paramQt(syntaxTree->attr.name, syntaxTree->attr.scope);

            /* Adds the quadruple to the list */
            list = addElemToList(list, quad);

            /* Ereases the quadruple created */
            quad = NULL;
            //////////////////////////////////////////////////////////////

            /* Calls to the parameters block */
            list = icGen(k1, list);

            /* Calls to the code block */
            list = icGen(k2, list);

            /* Quadruple creation and setup (END) */
            //////////////////////////////////////////////////////////////

            quad = newQuadListElem();
            quad->quadruple->op = ENDc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = syntaxTree->attr.name;
            quad->quadruple->addr1->contents.var.scope =  syntaxTree->attr.scope;
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = Empty;
            
            /* Adds the quadruple to the list */
            list = addElemToList(list, quad);

            /* Ereases the quadruple created */
            quad = NULL;
            //////////////////////////////////////////////////////////////
        break;

        case callK:
            /* Points to the child */
            k1 = syntaxTree->child[0];

            /* Calls to the arguments block */
            //list = icGen(k1, list);

            /* Calls a special routine to generate arguments quadruples */
            list = argGenerator(k1, list);

            /* Quadruple creation and setup (CALL) */
            //////////////////////////////////////////////////////////////
            quad = newQuadListElem();
            quad->quadruple->op = CALLc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
            syntaxTree->icTemp = cur_reg;
            regIncrease();
            quad->quadruple->addr2->kind = String;
            quad->quadruple->addr2->contents.var.name =  syntaxTree->attr.name;
            quad->quadruple->addr2->contents.var.scope = syntaxTree->attr.scope;
            quad->quadruple->addr3->kind = IntConst;
            /* The input and output functions does not have a reference in the symble table
             * By that, their param number are counted here */
            if(strcmp(syntaxTree->attr.name, "input") == 0 || strcmp(syntaxTree->attr.name, "output") == 0)
                quad->quadruple->addr3->contents.val = paramCounter(syntaxTree);
            else 
                quad->quadruple->addr3->contents.val = st_lookup_paramQt(syntaxTree->attr.name, syntaxTree->attr.scope);
            
            /* Adds the quadruple to the list */
            list = addElemToList(list, quad);

            /* Ereases the quadruple created */
            quad = NULL;
            //////////////////////////////////////////////////////////////
        break;

        case returnK:
            /* Points to the return parameter */
            k1 = syntaxTree->child[0];

            /* Calls for the child */
            list = icGen(k1, list);

            /* Quadruple creation and setup (END) */
            //////////////////////////////////////////////////////////////

            quad = newQuadListElem();
            quad->quadruple->op = RETc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
            quad->quadruple->addr1->contents.var.scope =  syntaxTree->attr.scope;
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = Empty;
            
            /* Adds the quadruple to the list */
            list = addElemToList(list, quad);

            /* Ereases the quadruple created */
            
            quad = NULL;
            //////////////////////////////////////////////////////////////
        break;

        case paramK:
            /* Quadruple creation and setup (PARAM) */
            //////////////////////////////////////////////////////////////

            quad = newQuadListElem();
            quad->quadruple->op = PARAMc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = syntaxTree->attr.name;
            quad->quadruple->addr1->contents.var.scope =  syntaxTree->attr.scope;
            quad->quadruple->addr2->kind = Empty;
            quad->quadruple->addr3->kind = String;
            quad->quadruple->addr3->contents.var.name = syntaxTree->attr.scope;
            
            /* Adds the quadruple to the list */
            list = addElemToList(list, quad);

            /* Ereases the quadruple created */
            
            quad = NULL;
            //////////////////////////////////////////////////////////////
        break;

        default:
        break;
    }
}

QuadrupleListElem expressionGenerator(TreeNode *syntaxTree, QuadrupleListElem list){

    /* Pointers for the 2 possible children */
    TreeNode *k1, *k2;

    /* Temporary pointer to a new quadruple */
    QuadrupleListElem quad;

    //printf("\n--->Entrou no expressionGenerator\n");
    //printf("---> %s\n", syntaxTree->attr.name);
    //printf("---> %d\n", syntaxTree->kind.exp);

    /* Selects the type of statement that the node corresponds */
    switch(syntaxTree->kind.exp){
    
        case opK:
            /* Points to the children */
            k1 = syntaxTree->child[0];
            k2 = syntaxTree->child[1];

            /* Calls to the first operand */
            list = icGen(k1, list);

            /* Calls to the second operand */
            list = icGen(k2, list);

            /* Selects by the operation */
            switch(syntaxTree->attr.op){
                
                case SOM:
                    /* Quadruple creation and setup (SOM) */
                    //////////////////////////////////////////////////////////////
                    quad = newQuadListElem();
                    quad->quadruple->op = ADDc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();
                    /* Stores the scope of the operation just in case we need it after */
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    /* Uses the temposrary value present into the tree node */
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);

                    /* Adds the quadruple to the list */
                    list =addElemToList(list, quad);

                    /* Ereases the quadruple created */
                    
                    quad = NULL;
                    //////////////////////////////////////////////////////////////
                break;

                case SUB:
                    /* Quadruple creation and setup (SUB) */
                    //////////////////////////////////////////////////////////////
                    quad = newQuadListElem();
                    quad->quadruple->op = SUBc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();
                    /* Stores the scope of the operation just in case we need it after */
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    /* Uses the temposrary value present into the tree node */
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);

                    /* Adds the quadruple to the list */
                    list =addElemToList(list, quad);

                    /* Ereases the quadruple created */
                    
                    quad = NULL;
                    //////////////////////////////////////////////////////////////
                break;

                case MUL:
                    /* Quadruple creation and setup (MUL) */
                    //////////////////////////////////////////////////////////////
                    quad = newQuadListElem();
                    quad->quadruple->op = MULc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();
                    /* Stores the scope of the operation just in case we need it after */
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    /* Uses the temposrary value present into the tree node */
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);

                    /* Adds the quadruple to the list */
                    list =addElemToList(list, quad);

                    /* Ereases the quadruple created */
                    
                    quad = NULL;
                    //////////////////////////////////////////////////////////////
                break;

                case DIV:
                    /* Quadruple creation and setup (DIV) */
                    //////////////////////////////////////////////////////////////
                    quad = newQuadListElem();
                    quad->quadruple->op = DIVc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();
                    /* Stores the scope of the operation just in case we need it after */
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    /* Uses the temposrary value present into the tree node */
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);

                    /* Adds the quadruple to the list */
                    list =addElemToList(list, quad);

                    /* Ereases the quadruple created */
                    
                    quad = NULL;
                    //////////////////////////////////////////////////////////////
                break;

                case IGUALIGUAL:
                    /* Quadruple creation and setup (I) */
                    //////////////////////////////////////////////////////////////
                    quad = newQuadListElem();
                    quad->quadruple->op = EQc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();
                    /* Stores the scope of the operation just in case we need it after */
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    /* Uses the temposrary value present into the tree node */
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);

                    /* Adds the quadruple to the list */
                    list =addElemToList(list, quad);

                    /* Ereases the quadruple created */
                    
                    quad = NULL;
                    //////////////////////////////////////////////////////////////
                break;

                case DIFERENTE:
                    /* Quadruple creation and setup (SOM) */
                    //////////////////////////////////////////////////////////////
                    quad = newQuadListElem();
                    quad->quadruple->op = DIFc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();
                    /* Stores the scope of the operation just in case we need it after */
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    /* Uses the temposrary value present into the tree node */
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);

                    /* Adds the quadruple to the list */
                    list =addElemToList(list, quad);

                    /* Ereases the quadruple created */
                    
                    quad = NULL;
                    //////////////////////////////////////////////////////////////
                break;

                case MENOR:
                    /* Quadruple creation and setup (SOM) */
                    //////////////////////////////////////////////////////////////
                    quad = newQuadListElem();
                    quad->quadruple->op = LTc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();
                    /* Stores the scope of the operation just in case we need it after */
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    /* Uses the temposrary value present into the tree node */
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);

                    /* Adds the quadruple to the list */
                    list =addElemToList(list, quad);

                    /* Ereases the quadruple created */
                    
                    quad = NULL;
                    //////////////////////////////////////////////////////////////
                break;

                case MENORIGUAL:
                    /* Quadruple creation and setup (SOM) */
                    //////////////////////////////////////////////////////////////
                    quad = newQuadListElem();
                    quad->quadruple->op = LETc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();
                    /* Stores the scope of the operation just in case we need it after */
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    /* Uses the temposrary value present into the tree node */
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);

                    /* Adds the quadruple to the list */
                    list =addElemToList(list, quad);

                    /* Ereases the quadruple created */
                    
                    quad = NULL;
                    //////////////////////////////////////////////////////////////
                break;

                case MAIOR:
                    /* Quadruple creation and setup (SOM) */
                    //////////////////////////////////////////////////////////////
                    quad = newQuadListElem();
                    quad->quadruple->op = GTc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();
                    /* Stores the scope of the operation just in case we need it after */
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    /* Uses the temposrary value present into the tree node */
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);

                    /* Adds the quadruple to the list */
                    list =addElemToList(list, quad);

                    /* Ereases the quadruple created */
                    
                    quad = NULL;
                    //////////////////////////////////////////////////////////////
                break;

                case MAIORIGUAL:
                    /* Quadruple creation and setup (SOM) */
                    //////////////////////////////////////////////////////////////
                    quad = newQuadListElem();
                    quad->quadruple->op = GETc;
                    quad->quadruple->addr1->kind = String;
                    quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
                    syntaxTree->icTemp = cur_reg;
                    regIncrease();
                    /* Stores the scope of the operation just in case we need it after */
                    quad->quadruple->addr1->contents.var.scope = syntaxTree->attr.scope;
                    quad->quadruple->addr2->kind = String;
                    /* Uses the temposrary value present into the tree node */
                    quad->quadruple->addr2->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);
                    quad->quadruple->addr3->kind = String;
                    quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[1]->icTemp);

                    /* Adds the quadruple to the list */
                    list =addElemToList(list, quad);

                    /* Ereases the quadruple created */
                    
                    quad = NULL;
                    //////////////////////////////////////////////////////////////
                break;
            }
        break;

        case idK:
            /* Quadruple creation and setup (LOAD) */
            //////////////////////////////////////////////////////////////
            quad = newQuadListElem();
            quad->quadruple->op = LOADc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
            syntaxTree->icTemp = cur_reg;
            regIncrease();
            quad->quadruple->addr2->kind = String;
            quad->quadruple->addr2->contents.var.name = syntaxTree->attr.name;
            quad->quadruple->addr3->kind = Empty;

            /* Adds the quadruple to the list */
            list =addElemToList(list, quad);

            /* Ereases the quadruple created */
            
            quad = NULL;
            //////////////////////////////////////////////////////////////
        break;

        case constK:
            /* Quadruple creation and setup (IMME) */
            //////////////////////////////////////////////////////////////
            quad = newQuadListElem();
            quad->quadruple->op = IMMEc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
            syntaxTree->icTemp = cur_reg;
            regIncrease();
            quad->quadruple->addr2->kind = IntConst;
            quad->quadruple->addr2->contents.val = syntaxTree->attr.val;
            quad->quadruple->addr3->kind = Empty;

            /* Adds the quadruple to the list */
            list =addElemToList(list, quad);

            /* Ereases the quadruple created */
            
            quad = NULL;
            //////////////////////////////////////////////////////////////
        break;

        case vectK:
            /* Points to the index block */
            k1 = syntaxTree->child[0];

            /* Calls to the index block */
            list = icGen(k1, list);

            /* Quadruple creation and setup (ARR) */
            //////////////////////////////////////////////////////////////
            quad = newQuadListElem();
            quad->quadruple->op = ARRc;
            quad->quadruple->addr1->kind = String;
            quad->quadruple->addr1->contents.var.name = createsRegName(cur_reg);
            syntaxTree->icTemp = cur_reg;
            regIncrease();
            quad->quadruple->addr2->kind = String;
            quad->quadruple->addr2->contents.var.name = syntaxTree->attr.name;
            quad->quadruple->addr3->kind = String;
            quad->quadruple->addr3->contents.var.name = createsRegName(syntaxTree->child[0]->icTemp);

            /* Adds the quadruple to the list */
            list =addElemToList(list, quad);

            /* Ereases the quadruple created */
            
            quad = NULL;
            //////////////////////////////////////////////////////////////
        break;

        default:
        break;
    }
}

QuadrupleListElem icGen(TreeNode *syntaxTree, QuadrupleListElem list){

    //printf("\n--->Entrou no icGen\n");

    if(syntaxTree != NULL){

        /* Selects the node based on the type */
        switch (syntaxTree->nodekind){

            case statementK:
                list = statementGenerator(syntaxTree, list);
            break;
 
            case expressionK:
                //(strcmp(syntaxTree->attr.name, "integer") == 0 || strcmp(syntaxTree->attr.name, "void") == 0) && syntaxTree->kind.exp != constK
                if(syntaxTree->kind.exp == typeK)
                    list = icGen(syntaxTree->child[0], list);
                else 
                    list = expressionGenerator(syntaxTree, list);
            break;
        
            default:
            break;
        }
        /* Calls for the sibling in the end -> next function (if exists) */
        list = icGen(syntaxTree->sibling, list);
    }

    return(list);
}

QuadrupleListElem icGenArg(TreeNode *syntaxTree, QuadrupleListElem list){

    if(syntaxTree != NULL){

        /* Selects the node based on the type */
        switch (syntaxTree->nodekind){

            case statementK:
                list = statementGenerator(syntaxTree, list);
            break;
 
            case expressionK:
                list = expressionGenerator(syntaxTree, list);
            break;
        
            default:
            break;
        };
    }

    return(list);
}

QuadrupleListElem intermediateCodeGenerator(TreeNode *syntaxTree){

    QuadrupleListElem list = NULL, quad;

    /* Fazer as devidas inicializações da geração do código */

    /* Calls the code generator function */
    list = icGen(syntaxTree, list);

    /* Quadruple creation and setup (LOAD) */
    //////////////////////////////////////////////////////////////
    quad = newQuadListElem();
    quad->quadruple->op = HALTc;
    quad->quadruple->addr1->kind = Empty;
    quad->quadruple->addr2->kind = Empty;
    quad->quadruple->addr3->kind = Empty;

    /* Adds the quadruple to the list */
    list = addElemToList(list, quad);

    /* Ereases the quadruple created */
    quad = NULL;
    //////////////////////////////////////////////////////////////

    return(list);
}

/////////////////////////////////////////////////////////////////////////////////