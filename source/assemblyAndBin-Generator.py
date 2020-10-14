# Assembly and binary code generators for cMinus Compiler (ZAFx32 architecture)

# Imports
######################################################################
# import nltk
# nltk.download('punkt')
from nltk.tokenize import word_tokenize
######################################################################

# Global variables
######################################################################
MEM_SIZE = 300 # RAM size of processor
GLOBAL_SIZE = 50 # Size of global allocation space 
######################################################################


# Reads the intermediate code input from intermediateCode.txt
# And processes it as a data structure
def getIntermediateCode(filepath):
    # Stores the code in memory
    iCodeList = []
    # File oppening
    with open(filepath, 'r') as iCode:
        for line in iCode:
            l = word_tokenize(line)
            # The line is not empty
            if l != []:
                iCodeList.append(l)
        # Removes the headder of the intermediate Code
        iCodeList.pop(0)
    
    # Cleans all the list 
    for line in iCodeList:
        for i in reversed(range(len(line))):
            if line[i] in {'(', ')', ',', '$'}:
                del line[i]
            
    return iCodeList


# Make a dict with all scopes and all variables and arguments addresses
# from 0 to n (starts based in fp register)
def determineLocalAdddr(iCode):
    dic = {}
    # Controls the addr value, from fp
    pos = 0
    # Iterates thru all quadruples
    for quad in iCode:
        if quad[0] == 'PARAM':
            if quad[3] not in dic.keys():
                dic[quad[3]] = {}
                pos = 0
            # [position, size, type of access]
            dic[quad[3]][quad[1]] = [pos, 1, 'param']
            pos += 1
        
        elif quad[0] == 'ALLOC':
            if quad[3] not in dic.keys():
                dic[quad[3]] = {}
                pos = 0
            # [position, size, type of access]
            if quad[3] == 'global':
                dic[quad[3]][quad[1]] = [pos, int(quad[2]), 'global']
            else:
               dic[quad[3]][quad[1]] = [pos, int(quad[2]), 'local'] 
            pos += int(quad[2])

    return dic


# Generates list of asm code based on intermediate code
'''
    * The registers of processor are the following:

    $a0 - $a3 -> Arguments
    $ret -> Return value
    $fp -> Frame pointer
    $sp -> stack pointer
    $ra -> return address
    $gp -> global pointer
    $t0 - $t22 -> general propouse registers 

    * The supported instructions:

    add, addi, sub, mul, div, mod, and, or, xor, not, slt, sgt,
    slet, sget, lsh, rsh, mov, li, beq, bne, j, in, out, load,
    store, jr, jal, halt 
    '''
def generateAsmCode(iCode):

    # Count control variables
    argCount = 0
    paramCount = 0
    curScope = ''

    # Determines all local addresses for all variables in all scopes (dict)
    locAddr = determineLocalAdddr(iCode)

    # Assembly code list
    asmCode = []
    
    # Code initialization
    ##########################################################
    # Initialize $sp -> stack pointer
    asmCode.append( ['li', 'sp', '-1', '-'] )
    # Initialize fp -> frame pointer
    asmCode.append( ['li', 'fp', '0', '-'] )
    # Initialize zero 
    asmCode.append( ['li', 'zero', '0', '-'] )
    # Initialize gp -> Allocates 30 memory spaces for global declatarions
    asmCode.append( ['li', 'gp', str(MEM_SIZE - GLOBAL_SIZE - 1), '-'] )    
    # After initialization, go to main
    asmCode.append( ['j', 'main', '-', '-'] )
    ##########################################################

    # Iterates thru all quadruples from iCode
    for quad in iCode:
        # All cases
        if quad[0] == 'ADD': 
            asmCode.append( ['add', quad[1], quad[2], quad[3]] )
           
        elif quad[0] == 'SUB':
            asmCode.append( ['sub', quad[1], quad[2], quad[3]] )

        elif quad[0] == 'MUL':
            asmCode.append( ['mul', quad[1], quad[2], quad[3]] )

        elif quad[0] == 'DIV':
            asmCode.append( ['div', quad[1], quad[2], quad[3]] )

        elif quad[0] == 'EQ':
            asmCode.append( ['eq', quad[1], quad[2], quad[3]] )

        elif quad[0] == 'DIF':
           asmCode.append( ['neq', quad[1], quad[2], quad[3]] )

        elif quad[0] == 'LT':
            asmCode.append( ['slt', quad[1], quad[2], quad[3]] )

        elif quad[0] == 'LET':
            asmCode.append( ['slet', quad[1], quad[2], quad[3]] )

        elif quad[0] == 'GT':
            asmCode.append( ['sgt', quad[1], quad[2], quad[3]] )

        elif quad[0] == 'GET':
            asmCode.append( ['sget', quad[1], quad[2], quad[3]] )

        elif quad[0] == 'AND':
            asmCode.append( ['and', quad[1], quad[2], quad[3]] )

        elif quad[0] == 'OR':
            asmCode.append( ['or', quad[1], quad[2], quad[3]] )

        elif quad[0] == 'ASSIGN':
            asmCode.append( ['mov', quad[1], quad[2], '-'] )

        elif quad[0] == 'ALLOC':
            '''
            It will only move sp to allocate a space in the stack for the variable
            addi sp, sp, size_of_variable

            Special case when the allocation is global, we will use the top of the memory for that
            kind of allocation and access
            '''
            if quad[3] == 'global':
                # Not needed because we already have the gp (reference) and the memory position
                # Of global declariations. Because the global space size can't be changed, we don't have
                # To store the top position of the allocated block, we access it by the memory positon
                # Defined on the auxiliary table (determinaLocalAddr)
                pass
            else:
                asmCode.append( ['addi', 'sp', 'sp', quad[2]] )

        elif quad[0] == 'IMME':
            asmCode.append( ['li', quad[1], quad[2], '-'] )
        
        elif quad[0] == 'LOAD':
            '''
            load tx, (memloc(var))fp or gp
            or
            addi tx, fp or gp, memloc(var)
            '''
            # If it is a global variable
            if quad[2] not in locAddr[curScope] and locAddr['global'][quad[2]][1] == 1:
                asmCode.append( ['load', quad[1], 'gp', '{}'.format(str(locAddr['global'][quad[2]][0]))] )
            # If it is a global array
            elif quad[2] not in locAddr[curScope] and locAddr['global'][quad[2]][1] != 1:
                asmCode.append( ['addi', quad[1], 'gp', '{}'.format(str(locAddr['global'][quad[2]][0]))] )

            # if it is a local variable
            elif quad[2] in locAddr[curScope] and locAddr[curScope][quad[2]][1] == 1:
                asmCode.append( ['load', quad[1], 'fp', str(locAddr[curScope][quad[2]][0])] )
            # if it is a local array
            elif quad[2] in locAddr[curScope] and locAddr[curScope][quad[2]][1] != 1:
                asmCode.append( ['addi', quad[1], 'fp', str(locAddr[curScope][quad[2]][0])] )

        elif quad[0] == 'STORE':
            '''
            store tx, (memloc(var))fp or gp

            for vectors:
                store tx, 0(ty)
            '''
            # It is a store of a array
            if quad[3] != '-':
                asmCode.append( ['store', quad[2], quad[3], '0'] )
            else:   
                if quad[1] not in locAddr[curScope]:
                    asmCode.append( ['store', quad[2], 'gp', '{}'.format(str(locAddr['global'][quad[1]][0]))] )
                else:
                    asmCode.append( ['store', quad[2], 'fp', str(locAddr[curScope][quad[1]][0])] )

        elif quad[0] == 'ARR':
            '''
                (ARR, t0, x, t1)

                - For direct access to global arrays
                add t1, gp, t1
                addi t1, t1, mem_loc
                load t0, 0(t1)

                - For passed by argument arrays
                load t0, mem_loc(fp)
                add t1, t0, t1
                load t0, 0(t1)

                - For arrays declared on the current scope (local)
                add t1, fp, t1
                addi t1, t1, memloc
                load t0, 0(t1)
            '''
            # For global arrays
            if quad[2] in locAddr['global']:
                asmCode.append( ['add', quad[3], 'gp', quad[3]] )
                asmCode.append( ['addi', quad[3], quad[3], '{}'.format(locAddr['global'][quad[2]][0])] )
                asmCode.append( ['load', quad[1], quad[3], '0'] )

            # Non global 
            elif quad[2] in locAddr[curScope]:

                # For local arrays
                if locAddr[curScope][quad[2]][2] == 'local': 
                    asmCode.append( ['add', quad[3], 'fp', quad[3]] )
                    asmCode.append( ['addi', quad[3], quad[3], '{}'.format(locAddr[curScope][quad[2]][0])] )
                    asmCode.append( ['load', quad[1], quad[3], '0'] )

                # For passed by argument arrays (reference)
                elif locAddr[curScope][quad[2]][2] == 'param':
                    asmCode.append( ['load', quad[1], 'fp', '{}'.format(locAddr[curScope][quad[2]][0])] )
                    asmCode.append( ['add', quad[3], quad[1], quad[3]] )
                    asmCode.append( ['load', quad[1], quad[3], '0'] )


        elif quad[0] == 'GOTO':
            asmCode.append( ['j', quad[1], '-', '-'] )

        elif quad[0] == 'IFF':
            asmCode.append( ['beq', quad[1], 'zero', quad[2]] )

        elif quad[0] == 'RET':
            '''
            mov ret, tx
            addi fp, fp, -2
            load ra, (1)fp
            mov sp, fp
            addi sp, sp, -1
            load fp, 0(fp)
            jr ra
            '''
            asmCode.append( ['mov', 'ret', quad[1], '-'] )
            asmCode.append( ['addi', 'fp', 'fp', '-2'] )
            asmCode.append( ['load', 'ra', 'fp', '1'] )
            asmCode.append( ['mov', 'sp', 'fp', '-'] )
            asmCode.append( ['addi', 'sp', 'sp', '-1'] )
            asmCode.append( ['load', 'fp', 'fp', '0'] )
            asmCode.append( ['jr', 'ra', '-', '-'] )

        elif quad[0] == 'FUNC':
            '''
            func:
                addi sp, sp, 2
                store ra, 0(sp)
                store fp, -1(sp)
                mov fp, sp
                addi fp, fp, 1 
            '''
            paramCount = 0
            argCount = 0
            curScope = quad[1]
            asmCode.append( [quad[1].lower(), '-', '-', '-'] )
            if quad[1] != 'main':
                asmCode.append( ['addi', 'sp', 'sp', '2'] )
                asmCode.append( ['store', 'ra', 'sp', '0'] )
                asmCode.append( ['store', 'fp', 'sp', '-1'] )
                asmCode.append( ['mov', 'fp', 'sp', '-'] )
                asmCode.append( ['addi', 'fp', 'fp', '1'] )

        elif quad[0] == 'END':
            '''
            If not main:

            addi fp, fp, -2
            load ra, (1)fp
            mov sp, fp
            addi sp, sp, -1
            load fp, 0(fp)
            jr ra

            If main:
            j end
            '''
            if quad[1] == 'main':
                asmCode.append( ['j', 'end', '-', '-'] )

            else:
                asmCode.append( ['addi', 'fp', 'fp', '-2'] )
                asmCode.append( ['load', 'ra', 'fp', '1'] )
                asmCode.append( ['mov', 'sp', 'fp', '-'] )
                asmCode.append( ['addi', 'sp', 'sp', '-1'] )
                asmCode.append( ['load', 'fp', 'fp', '0'] )
                asmCode.append( ['jr', 'ra', '-', '-'] )

        elif quad[0] == 'PARAM':
            '''
            addi sp, sp, 1
            store ax, 0(sp)
            '''
            asmCode.append( ['addi', 'sp', 'sp', '1'] )
            asmCode.append( ['store', 'a{}'.format(paramCount), 'sp', '0'] )
            paramCount += 1

        elif quad[0] == 'CALL':
            argCount = 0
            if quad[2] == 'output':
                '''
                mov tx, a0
                out tx
                '''
                # The parameter for output will always be unique and a0
                asmCode.append( ['mov', quad[1], 'a0', '-'] )
                asmCode.append( ['out', quad[1], '-', '-'] )
            
            elif quad[2] == 'input':
                '''
                in rx
                '''
                asmCode.append( ['in', quad[1], '-', '-'] )

            else:
                '''
                jal func_name
                mov tx, ret 
                '''
                asmCode.append( ['jal', quad[2], '-', '-'] )
                asmCode.append( ['mov', quad[1], 'ret', '-'] )

        elif quad[0] == 'ARG':
            '''
            When we have a array, we must pass the memory address of the base
            When we have a commun variable, we must pass the actual value
            '''
            asmCode.append( ['mov', 'a{}'.format(argCount), quad[1], '-'] )
            argCount += 1

        elif quad[0] == 'LAB':
            asmCode.append( [quad[1], '-', '-', '-'] )

        elif quad[0] == 'HALT':
            asmCode.append( ['end', '-', '-', '-'] )
            asmCode.append( ['halt', '-', '-', '-'] )

        #asmCode.append( [] )

    return asmCode


# Creates file for the assembly code and returns a dict of all labels lines
def generateAsmCodeFile(asmCode):

    # Line cont
    cont = 0
    # Dict for all labels lines
    labs = {}

    # Open file
    with open('./outputs/assemblyCode.txt', 'w') as asmFile:
        # Initial info
        asmFile.write("%s\n\n" % '..::Assembly Code for ZAFx32 Processor::..')

        # Iterates thru all asm code list
        for line in asmCode:
            # Removes all '-'
            line = list(filter(lambda a: a != '-', line))
            
            # All cases
            # Labbels 
            if len(line) == 1:
                if line[0] == 'halt':
                    asmFile.write("%s\n" % '{:<12}{}'.format('{}:'.format(cont), line[0]))
                    cont += 1
                else:
                    asmFile.write("%s\n" % '.{}'.format(line[0]))
                    labs[line[0]] = cont

            elif len(line) == 2:
                asmFile.write("%s\n" % '{:<12}{} {}'.format('{}:'.format(cont), line[0], line[1]))
                cont += 1

            elif len(line) == 3:
                asmFile.write("%s\n" % '{:<12}{} {}, {}'.format('{}:'.format(cont), line[0], line[1], line[2]))
                cont += 1

            elif len(line) == 4:
                asmFile.write("%s\n" % '{:<12}{} {}, {}, {}'.format('{}:'.format(cont), line[0], line[1], line[2], line[3]))
                cont += 1
    
    return labs


def generateBinCodeFile(asmCode, labels):

    '''
    Following the instruction types of the processor

    R = [opcode(6), rs(5), rt(5), rd(5), shamt(11)]
    I = [opcode(6), rs(5), rt(5), immidiate(16)]
    J = [opcode(6), immidiate(26)]
    '''

    # Line cont
    cont = 0

    opCode = ['add', 'addi', 'sub', 'mul', 'div', 'mod', 'and', 'or', 'not', 'xor', 'slt', 'sgt', 'slet',
              'sget', 'lsh', 'rsh', 'mov', 'li', 'beq', 'bne', 'j', 'in', 'out', 'load', 'store', 'jr',
              'jal', 'halt', 'eq', 'neq'
            ]

    registers = ['zero', 'sp', 'fp', 'gp', 'ra', 'ret', 'a0', 'a1', 'a2', 'a3', 't0', 't1',
                 't2', 't3', 't4', 't5', 't6', 't7', 't8', 't9', 't10', 't11', 't12', 't13',
                 't14', 't15', 't16', 't17', 't18', 't19', 't20', 't21'
                ]

    # Open file
    with open('./outputs/binaryCode.txt', 'w') as binFile:

        # Initial info
        binFile.write("%s\n\n" % '..::Binary Code for ZAFx32 Processor::..')

        # Iterates thru all asm code list
        for line in asmCode:
            # Removes all '-'
            line = list(filter(lambda a: a != '-', line))
            
            # All cases
            # Labbels and halt
            if len(line) == 1:
                if line[0] == 'halt':
                    binFile.write("%s\n" % 'MemInst[{}] = {{ 6\'d{}, 26\'d{} }}{:>20}'.format(cont, 
                                                                                   opCode.index('halt'), 
                                                                                   0, 
                                                                                   '//{}'.format('halt')))
                    cont += 1
                else:
                    binFile.write("%s\n" % '//{}'.format(line[0]))

            elif len(line) == 2:
                # Jumps
                if line[0] in {'j', 'jal'}:
                    binFile.write("%s\n" % 'MemInst[{}] = {{ 6\'d{}, 26\'d{} }}{:>20}'.format(cont, 
                                                                                   opCode.index(line[0]), 
                                                                                   labels[line[1]], 
                                                                                   '//{}'.format(line[0])))
                    cont += 1
                else:
                    binFile.write("%s\n" % 'MemInst[{}] = {{ 6\'d{}, 26\'d{} }}{:>20}'.format(cont, 
                                                                                   opCode.index(line[0]), 
                                                                                   registers.index(line[1]), 
                                                                                   '//{}'.format(line[0])))
                    cont += 1

            elif len(line) == 3:
                if line[0] == 'li':
                    binFile.write("%s\n" % 'MemInst[{}] = {{ 6\'d{}, 5\'d{}, 5\'d{}, 16\'d{} }}{:>10}'.format(cont,
                                                                                   opCode.index(line[0]), 
                                                                                   0,
                                                                                   registers.index(line[1]),
                                                                                   int(line[2]), 
                                                                                   '//{}'.format(line[0])))
                    cont += 1

                elif line[0] == 'mov':
                    binFile.write("%s\n" % 'MemInst[{}] = {{ 6\'d{}, 5\'d{}, 5\'d{}, 16\'d{} }}{:>10}'.format(cont,
                                                                                   opCode.index(line[0]), 
                                                                                   registers.index(line[2]),
                                                                                   registers.index(line[1]),
                                                                                   0, 
                                                                                   '//{}'.format(line[0])))
                    cont += 1

            elif len(line) == 4:
                # If the last value is a register (R type)
                if line[3][0] == 't':
                    # Commun R type instruction 
                    binFile.write("%s\n" % 'MemInst[{}] = {{ 6\'d{}, 5\'d{}, 5\'d{}, 5\'d{}, 11\'d{} }}{:>10}'.format(cont,
                                                                                   opCode.index(line[0]), 
                                                                                   registers.index(line[1]),
                                                                                   registers.index(line[2]),
                                                                                   registers.index(line[3]),
                                                                                   0, 
                                                                                   '//{}'.format(line[0])))
                    cont += 1

                elif line[3][0] == 'L':
                    binFile.write("%s\n" % 'MemInst[{}] = {{ 6\'d{}, 5\'d{}, 5\'d{}, 16\'d{} }}{:>10}'.format(cont,
                                                                                   opCode.index(line[0]), 
                                                                                   registers.index(line[1]),
                                                                                   registers.index(line[2]),
                                                                                   labels[line[3]], 
                                                                                   '//{}'.format(line[0])))
                    cont += 1

                else:
                    if int(line[3]) < 0:
                        binFile.write("%s\n" % 'MemInst[{}] = {{ 6\'d{}, 5\'d{}, 5\'d{}, -16\'d{} }}{:>10}'.format(cont,
                                                                                    opCode.index(line[0]), 
                                                                                    registers.index(line[2]),
                                                                                    registers.index(line[1]),
                                                                                    abs(int(line[3])), 
                                                                                    '//{}'.format(line[0])))
                        cont += 1

                    else:
                        binFile.write("%s\n" % 'MemInst[{}] = {{ 6\'d{}, 5\'d{}, 5\'d{}, 16\'d{} }}{:>10}'.format(cont,
                                                                                    opCode.index(line[0]), 
                                                                                    registers.index(line[2]),
                                                                                    registers.index(line[1]),
                                                                                    int(line[3]), 
                                                                                    '//{}'.format(line[0])))
                        cont += 1
                        

def main():
    iCode = getIntermediateCode('./outputs/intermediateCode.txt')
    asmCode = generateAsmCode(iCode)
    labels = generateAsmCodeFile(asmCode)
    generateBinCodeFile(asmCode, labels)


if __name__ == '__main__':
    main()