# CMinus (C-) Compiler for ZAFx32 processor architecture

Project of a LALR(1) C- (C Minus) compiler designed for a specific architecture (MIPS based), using flex and bison as parse generator (analysis phase).

The synthesis phase (intermediate code, assembly and binary code generators), were implemented separately.

* The Processor project can be find [here](https://github.com/LZafiro/ECOMP---UNIFESP/tree/master/ECOMP%20-%20Full%20Computer%20Dev.%20Laboratories/mipsBasedProcessor).

The project was developed for some laboratories of the undergraduate course in Computer Engineering at the Federal University of São Paulo.

# Summary

1. [Hot To Use](#how_to_use);
2. [Specifications](#specifications);
3. [Tests](#tests);
4. [Credits](#credits);

# How To 

First of all you must install gcc, flex and bison:

```sh
sudo apt upadate
sudo apt install build-essential
sudo apt-get install flex
sudo apt-get install bison
```
In project directory. To compile/re-compile the compiler, type:

```sh
sh compileTheCompiler.sh
```
To compile, you must write yout programm to a .txt file (input_file.txt), and type:

```sh
./cmin input_file.txt
```

The compiler generates a few files in the 'outputs' directory:

- tokenList.txt
- symbolTable.txt
- syntaxTree.txt
- intermediateCode.txt
- assemblyCode.txt
- binaryCodeWithComments.txt
- rawBinaryCode.txt

# Specifications

## Lexycal Conventions for C-

### Key Words
    
    else, if, while, int, return, void

### Symbols
    
    '+' '-' '*' '/' '<' '<=' '>' '>=' '==' '!=' '=' ';' ',' '{' '}' '(' ')' '[' ']' '/*' '*/';

### Regular Expressions

    ID = letter letter*
    NUM = digit digit*
    leter = a|...|z|A|...|Z
    digit = 0|...|9

There are differences between upper and lower case. 

The comments are composed by:
    
    /* Comment here */

## Backus-Naur Form (BNF) Grammar for C-

    1.  programm -> declaration-list
    2.  declaration_list -> declaration_list declaration|declaration
    3.  declaration -> var_declaration|fun_declaration
    4.  var_declaration -> type_specifier ID ;|type_specifier [ NUM ]  ;
    5.  type_specifier -> INT | VOID
    6.  fun_declaration -> type_specifier ID ( params ) composed_decl
    7.  params-> param_list | VOID
    8.  param_list -> param_list , param | param
    9.  param -> type_especifier ID | type_especifier ID [ ]
    10. compost_decl -> { local_declarations statement_list }
    11. local_declarations -> local_declarations var_declaration | empty
    12. statement_list -> statement_list statement | empty
    13. statement -> exp_decl | compost_decl | selection_decl | iter_decl | return_decl
    14. exp_decl -> expression ; | ;
    15. selection_decl -> IF ( expression ) statement | IF ( expression ) statement ELSE statement
    16. iter_decl -> WHILE ( expression ) statement
    17. return_decl -> RETURN ; | RETURN expression ;
    18. expression -> var = epression | simple_expression
    19. var -> ID | ID [ expression ]
    20. simple_expressoin -> sum_expression relational sum_expression | sum_expression
    21. relational -> <= | < | > | >= | == | !=
    22. sum_expression -> sum_expression sum term | term
    23. sum -> + | -
    24. term -> term mult factor | factor
    25. mult -> * | /
    26. factor -> ( expression ) | var | activation | NUM4
    27. activation -> ID ( args )
    28. args -> arg_list | empty
    29. arg_list -> arg_list , expression | expression

Also there are two pre-defined global functions:

```C
int input(void){ ... }

void output(int x){ ... }
```
# Tests

Here are presented a test code, with the  intermediate code, assembly code and binaries code, the other outputs, as well as the ones that are presented next, can be found in the 'outputs' directory:

## Code

```C
/* Test code for C- compiler
 * For ZAFx32 Processor */

int vet[10];

int minloc(int a[], int low, int high){
    int i;
    int x;
    int k;
    
    k = low;
    x = a[low];
    i = low + 1;
    
    while(i < high){
        if(a[i] < x){
            x = a[i];
            k = i;
        }
        i = i + 1;
    }
    return k;
}

void sort(int a[], int low, int high){
    int i;
    int k;

    i = low;

    while(i < high - 1){
        int t;
        k = minloc(a, i, high);
        t = a[k];
        a[k] = a[i];
        a[i] = t;
        i = i + 1;
    }
}

void main(void){
    int i;

    i = 0;

    while(i < 10){
        vet[i] = input();
        i = i + 1;
    }
    sort(vet, 0, 10);
    i = 0;
    while(i < 10){
        output(vet[i]);
        i = i + 1;
    }
}
```

## Intermediate Code

```
..::Intermediate Code for ZAFx32 Processor::..

(ALLOC, vet, 10, global)
(FUNC, minloc, -, 3)
(PARAM, a, -, minloc)
(PARAM, low, -, minloc)
(PARAM, high, -, minloc)
(ALLOC, i, 1, minloc)
(ALLOC, x, 1, minloc)
(ALLOC, k, 1, minloc)
(LOAD, $t0, k, -)
(LOAD, $t1, low, -)
(ASSIGN, $t0, $t1, -)
(STORE, k, $t0, -)
(LOAD, $t2, x, -)
(LOAD, $t3, low, -)
(ARR, $t4, a, $t3)
(ASSIGN, $t2, $t4, -)
(STORE, x, $t2, -)
(LOAD, $t5, i, -)
(LOAD, $t6, low, -)
(IMME, $t7, 1, -)
(ADD, $t8, $t6, $t7)
(ASSIGN, $t5, $t8, -)
(STORE, i, $t5, -)
(LAB, L0, -, -)
(LOAD, $t9, i, -)
(LOAD, $t10, high, -)
(LT, $t11, $t9, $t10)
(IFF, $t11, L1, -)
(LOAD, $t12, i, -)
(ARR, $t13, a, $t12)
(LOAD, $t14, x, -)
(LT, $t15, $t13, $t14)
(IFF, $t15, L2, -)
(LOAD, $t0, x, -)
(LOAD, $t1, i, -)
(ARR, $t2, a, $t1)
(ASSIGN, $t0, $t2, -)
(STORE, x, $t0, -)
(LOAD, $t3, k, -)
(LOAD, $t4, i, -)
(ASSIGN, $t3, $t4, -)
(STORE, k, $t3, -)
(GOTO, L3, -, -)
(LAB, L2, -, -)
(LAB, L3, -, -)
(LOAD, $t5, i, -)
(LOAD, $t6, i, -)
(IMME, $t7, 1, -)
(ADD, $t8, $t6, $t7)
(ASSIGN, $t5, $t8, -)
(STORE, i, $t5, -)
(GOTO, L0, -, -)
(LAB, L1, -, -)
(LOAD, $t9, k, -)
(RET, $t9, -, -)
(END, minloc, -, -)
(FUNC, sort, -, 3)
(PARAM, a, -, sort)
(PARAM, low, -, sort)
(PARAM, high, -, sort)
(ALLOC, i, 1, sort)
(ALLOC, k, 1, sort)
(LOAD, $t10, i, -)
(LOAD, $t11, low, -)
(ASSIGN, $t10, $t11, -)
(STORE, i, $t10, -)
(LAB, L4, -, -)
(LOAD, $t12, i, -)
(LOAD, $t13, high, -)
(IMME, $t14, 1, -)
(SUB, $t15, $t13, $t14)
(LT, $t0, $t12, $t15)
(IFF, $t0, L5, -)
(ALLOC, t, 1, sort)
(LOAD, $t1, k, -)
(LOAD, $t2, a, -)
(ARG, $t2, -, -)
(LOAD, $t3, i, -)
(ARG, $t3, -, -)
(LOAD, $t4, high, -)
(ARG, $t4, -, -)
(CALL, $t5, minloc, 3)
(ASSIGN, $t1, $t5, -)
(STORE, k, $t1, -)
(LOAD, $t6, t, -)
(LOAD, $t7, k, -)
(ARR, $t8, a, $t7)
(ASSIGN, $t6, $t8, -)
(STORE, t, $t6, -)
(LOAD, $t9, k, -)
(ARR, $t10, a, $t9)
(LOAD, $t11, i, -)
(ARR, $t12, a, $t11)
(ASSIGN, $t10, $t12, -)
(STORE, a, $t10, -)
(LOAD, $t13, i, -)
(ARR, $t14, a, $t13)
(LOAD, $t15, t, -)
(ASSIGN, $t14, $t15, -)
(STORE, a, $t14, -)
(LOAD, $t0, i, -)
(LOAD, $t1, i, -)
(IMME, $t2, 1, -)
(ADD, $t3, $t1, $t2)
(ASSIGN, $t0, $t3, -)
(STORE, i, $t0, -)
(GOTO, L4, -, -)
(LAB, L5, -, -)
(END, sort, -, -)
(FUNC, main, -, 1)
(ALLOC, i, 1, main)
(LOAD, $t4, i, -)
(IMME, $t5, 0, -)
(ASSIGN, $t4, $t5, -)
(STORE, i, $t4, -)
(LAB, L6, -, -)
(LOAD, $t6, i, -)
(IMME, $t7, 10, -)
(LT, $t8, $t6, $t7)
(IFF, $t8, L7, -)
(LOAD, $t9, i, -)
(ARR, $t10, vet, $t9)
(CALL, $t11, input, 0)
(ASSIGN, $t10, $t11, -)
(STORE, vet, $t10, -)
(LOAD, $t12, i, -)
(LOAD, $t13, i, -)
(IMME, $t14, 1, -)
(ADD, $t15, $t13, $t14)
(ASSIGN, $t12, $t15, -)
(STORE, i, $t12, -)
(GOTO, L6, -, -)
(LAB, L7, -, -)
(LOAD, $t0, vet, -)
(ARG, $t0, -, -)
(IMME, $t1, 0, -)
(ARG, $t1, -, -)
(IMME, $t2, 10, -)
(ARG, $t2, -, -)
(CALL, $t3, sort, 3)
(LOAD, $t4, i, -)
(IMME, $t5, 0, -)
(ASSIGN, $t4, $t5, -)
(STORE, i, $t4, -)
(LAB, L8, -, -)
(LOAD, $t6, i, -)
(IMME, $t7, 10, -)
(LT, $t8, $t6, $t7)
(IFF, $t8, L9, -)
(LOAD, $t9, i, -)
(ARR, $t10, vet, $t9)
(ARG, $t10, -, -)
(CALL, $t11, output, 1)
(LOAD, $t12, i, -)
(LOAD, $t13, i, -)
(IMME, $t14, 1, -)
(ADD, $t15, $t13, $t14)
(ASSIGN, $t12, $t15, -)
(STORE, i, $t12, -)
(GOTO, L8, -, -)
(LAB, L9, -, -)
(END, main, -, -)
(HALT, -, -, -)
```

## Assembly Code (for ZAFx32 Processor)

```
/* Soon */
```

## Binary Code With Comments (for ZAFx32 Processor)

```
/* Soon */
```

## Raw Binary Code (for ZAFx32 Processor)

```
/* Soon */
```

# Credits

Developed by: Luiz Felipe Raveduti Zafiro
