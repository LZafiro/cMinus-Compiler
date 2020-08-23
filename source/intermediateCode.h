#ifndef _INTERMEDIATECODE_H_
#define _INTERMEDIATECODE_H_

/* Quando finalizado alterar caminho apenas para globals.h -> estará na pasta source */
#include "globals.h"
 
/* All structures definitions for the intermidiate code generation - Quadruples */
/////////////////////////////////////////////////////////////////////////////////

/* Structure that defines the type of the operator for each quadruple */
typedef enum{ADDc = 0, SUBc, MULc, DIVc, EQc, DIFc, LTc, LETc, GTc, GETc, ANDc, ORc, 
ASSIGNc, ALLOCc, IMMEc, LOADc, STOREc, ARRc, GOTOc, IFFc, RETc, FUNCc,
ENDc, PARAMc, CALLc, ARGc, LABc, HALTc} OpKind;

/* Structure that defines the kind of each address */
typedef enum{String, IntConst, Empty} AddrKind;
    /*
    * String = Variables, temporaries, labbels 
    * IntConst = Immidiates
    * Empty = for an ampty address
    */

/* Structure that defines and describes all relevant info present 
in the address*/
typedef struct address{
    AddrKind kind;
    union{
        int val;
        struct{
            char *name;
            char *scope; /* Verify */
        } var;
    } contents;
} *Address;

/* Structure thar defines a quadruple (op, addr, addr, addr) */
typedef struct quadruple{
    OpKind op;
    Address addr1, addr2, addr3;
} *Quadruple; 

/* Structure thar defines a unidiractional list of quadruples */
typedef struct QuadListElem{
    int location; /* Verify */
    Quadruple quadruple;
    struct QuadListElem *next;
} *QuadrupleListElem;
/////////////////////////////////////////////////////////////////////////////////

/* All function declarations and labbels for the intermediate code generation */
/////////////////////////////////////////////////////////////////////////////////

/* Function to create a new address element (returns a pointer for it) ok */
Address newAddress(); /* Remove */

/* Function to create a new quadruple (returns a pointer to a quadruple) ok */
Quadruple newQuad(); /* Remove */

/* Function to create a new quadruple list elem(returns a pointer to a quadruple list elem) ok */
QuadrupleListElem newQubashadListElem();

/* Function that add the element to the list of quadruple element */
QuadrupleListElem addElemToList(QuadrupleListElem listtop, QuadrupleListElem element);

/* Function that prints the quadruple list into the console and into a file (intermediateCode.txt) */
void printIntermediateCode(QuadrupleListElem list);

/* Function that generates a string based in the address of the quadruple (for printing) */
char *addrPrintGenerator(Address addr);

/* Main function of the intermediate code generator (returns the list of quadruples elements) */
QuadrupleListElem intermediateCodeGenerator(TreeNode *syntaxTree);

/* Auxialiar function (internal) for the code generation (recursive) */
QuadrupleListElem icGen(TreeNode *syntaxTree, QuadrupleListElem list);

/* icMethod that does not cll the sibling node */
QuadrupleListElem icGenArg(TreeNode *syntaxTree, QuadrupleListElem list);

/* Generates the arguments quadruples */
QuadrupleListElem argGenerator(TreeNode *synTree, QuadrupleListElem list);

/* Function that evaluate a statement node kind (can be called for any node) */
QuadrupleListElem statementGenerator(TreeNode *syntaxTree, QuadrupleListElem list);

/* Function that evaluate a statement node kind (can be called for any node) */
QuadrupleListElem expressionGenerator(TreeNode *syntaxTree, QuadrupleListElem list);

/* Creates a string to the register name: $tx */
char *createsRegName(int x);

/* Creates a string to the label name: Lx */
char *createsLabName(int x);

/* Verifies if the global var (cur_reg) reaches the maximum. If so, resets
 * otherwise, increases */
void regIncrease(void); /* Maybe will be deleted, confirm in the assembly generation! */
/////////////////////////////////////////////////////////////////////////////////

#endif