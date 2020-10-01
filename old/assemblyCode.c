/* For all the atructures and more explanations of the functions,
 * consult "assemblyCode.h" */

#include "assemblyCode.h"

Instruction newInstruction(){

    /* Allocs a new intruction */
    Instruction tmp = (Instruction)malloc(sizeof(struct instruction));

    /* Allocation verification */ 
    if(tmp == NULL) exit(-1);

    return(tmp);
}

AssemblyCodeList newAssemblyCodeListElem(int flag){

    /* Alloc a new element to the list */
    AssemblyCodeList tmp = (AssemblyCodeList)malloc(sizeof(struct assemblyCodeList));

    /* Allocation verification */
    if(tmp == NULL) exit(-1);

    if(flag)
        /* Allocs a new instruction */
        tmp->line.instruction = newInstruction();

    return(tmp);
}

AssemblyCodeList addElemToAsmList(AssemblyCodeList listtop, AssemblyCodeList element){

    AssemblyCodeList aux;

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

AssemblyCodeList assemblyCodeGenerator(QuadrupleListElem intermediateCodeList){

    /* Pointer for the assembly code list root */
    AssemblyCodeList list = NULL;

    /* Stores the top of the list -> for the second pass */
    QuadrupleListElem top = intermediateCodeList;
 
    while(top != NULL){

        list = acGen(list, top);
        top = top->next;
    }
    
    return(list);
}