#ifndef _ASSEMBLYCODE_H_
#define _ASSEMBLYCODE_H_

#include "../source/intermediateCode.h"

/* All structures definitions for the assembly code generation */
/////////////////////////////////////////////////////////////////////////////////

/* All the instructions */
typedef enum{add, addi, sub, mul, div, mod, and, or, not, xor, slt, sgt, slet,
sget, lsh, rsh, mov, li, beq, bne, j, in, out, load, store, jr, jal, halt
} InstKind;

/* Instructions formats */
typedef enum{typeR, typeI, typeJ
} InstFormat;

/* Indicates if it is a instructon or a label */
typedef enum{intruction, label
} LineKind;

/* All registers defined in our assembly */
typedef enum{$zero, $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7, $t8, $t9, $t10, $t11,
$t12, $t13, $t14, $t15, $a0, $a1, $a2, $a3, $a4, $a5, $a6, $a7, $a8, $a9, $sp, 
$sp, $gp, $ra, $ret, $jmp
} AsmRegs;

/* Struct that defines how tha instruction is represented */
typedef struct instruction{
    InstFormat fomrat;
    InstKind kind;
    AsmRegs reg1;
    AsmRegs reg2;
    AsmRegs reg3;
    int im;         /* Verificar o que eh */
    char *imlabel;
} *Instruction;

/* Struct that defines a unit of the assembly code list structure */
typedef struct assemblyCodeList{
    int lineno;
    LineKind kind;
    union{
        Instruction instruction;
        char *label;
    } line;
    struct assemblyCodeList *next;
} *AssemblyCodeList;
/////////////////////////////////////////////////////////////////////////////////
/* End Structurtes */
 
/* All function declarations and labels for the assembly code generation */
/////////////////////////////////////////////////////////////////////////////////

/* Allocs a new instruction */
Instruction newInstruction();

/* Allocs a new element of the assembly code list 
 * Flag: 0 = Label instruction    1 = Normal intruction*/
AssemblyCodeList newAssemblyCodeListElem(int flag);

/* Function that prints the assembly list into the console and into a file (assemblyCode.txt) */
void printAssemblyCode(AssemblyCodeList list);

/* Adds a new element into the list, if the list is empty, sets as the first element */
AssemblyCodeList addElemToAsmList(AssemblyCodeList listtop, AssemblyCodeList element){

/* Generates the assembly code based on the current quadruple (top) */
AssemblyCodeList acGen(AssemblyCodeList list, QuadrupleListElem quad_list);

/* Principal function to generate the assembly code by the intermediate code list
 * Returns the list of the assemby code */
AssemblyCodeList assemblyCodeGenerator(QuadrupleListElem intermediateCodeList);
/////////////////////////////////////////////////////////////////////////////////
/* End Functions */

#endif