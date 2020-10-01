# Assembly code generator for cMinus Compiler (ZAFx32 architecture)

# Imports
######################################################################
# import nltk
# nltk.download('punkt')
from nltk.tokenize import word_tokenize
######################################################################

# Global variables
######################################################################
MEM_SIZE = 100 # RAM size of processor
######################################################################


# Reads the intermediate code input from intermediateCode.txt
# And processes it as a data structure
def getIntermediateCode(filepath):
    # Stores the code in memory
    iCodeList = []
    # File oppening
    with open(filepath) as iCode:
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
# from 0 to n (starts based in $fp register)
def determineLocalAdddr(iCode):
    dic = {}
    # Controls the addr value, from $fp
    pos = 0
    # Iterates thru all quadruples
    for quad in iCode:
        if quad[0] == 'PARAM':
            if quad[3] not in dic.keys():
                dic[quad[3]] = {}
                pos = 0
            # [position, size]
            dic[quad[3]][quad[1]] = [pos, 1]
            pos += 1
        
        elif quad[0] == 'ALLOC':
            if quad[3] not in dic.keys():
                dic[quad[3]] = {}
                pos = 0
            # [position, size]
            dic[quad[3]][quad[1]] = [pos, int(quad[2])]
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
    $r0 - $r23 -> general propouse registers 

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
    asmCode.append( ['li', '$sp', '0', '-'] )
    # Initialize $fp -> frame pointer
    asmCode.append( ['li', '$fp', '0', '-'] )
    # Initialize $zero 
    asmCode.append( ['li', '$zero', '0', '-'] )
    # Initialize $gp -> MEM_SIZE because the memory goes from 0 to MEM_SIZE -1
    # And by thar we are pointing outside the RAM (no values allocated)
    asmCode.append( ['li', '$gp', str(MEM_SIZE), '-'] )    
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
            It will only move $sp to allocate a space in the stack for the variable
            addi $sp, $sp, size_of_variable

            Special case when the allocation is global, we will use the top of the memory for that
            kind of allocation and access
            '''
            if quad[3] == 'global':
                pass
            else:
                asmCode.append( ['addi', '$sp', '$sp', quad[2]] )

        elif quad[0] == 'IMME':
            asmCode.append( ['li', quad[1], quad[2], '-'] )
        
        elif quad[0] == 'LOAD':
            '''
            load $tx, (memloc(var))$fp or $gp
            '''
            # If it is a global variable
            if quad[2] not in locAddr[curScope] and locAddr['global'][quad[2]][1] == 1:
                asmCode.append( ['load', quad[1], '$gp', '-{}'.format(str(locAddr['global'][quad[2]][0]))] )
            # If it is a global array
            elif quad[2] not in locAddr[curScope] and locAddr['global'][quad[2]][1] != 1:
                asmCode.append( ['addi', quad[1], '$gp', '-{}'.format(str(locAddr['global'][quad[2]][0]))] )
            else:
                asmCode.append( ['load', quad[1], '$fp', str(locAddr[curScope][quad[2]][0])] )

        elif quad[0] == 'STORE':
            '''
            store $tx, (memloc(var))$fp or $gp

            for vectors:
                store $tx, 0($ty)
            '''
            # It is a store of a non array
            if quad[3] != '-':
                asmCode.append( ['store', quad[2], quad[3], '0'] )
            else:   
                if quad[1] not in locAddr[curScope]:
                    asmCode.append( ['store', quad[2], '$gp', '-{}'.format(str(locAddr['global'][quad[1]][0]))] )
                else:
                    asmCode.append( ['store', quad[2], '$fp', str(locAddr[curScope][quad[1]][0])] )

        elif quad[0] == 'ARR':
            '''
            Global vec:
                sub $tx, $gp, $ty
                addi $tx, $tx, -(base_pos_vec)
                load $ty, 0($tx)
            Local vec:
                add $tx, $fp, $tx
                add $tx, $tx, (base_pos_vec)
                load $ty, 0($tx)
            '''
            # For global arrays
            if quad[2] in locAddr['global']:
                asmCode.append( ['sub', quad[3], '$gp', quad[3]] )
                asmCode.append( ['addi', quad[3], quad[3], '-{}'.format(locAddr['global'][quad[2]][0])] )
                asmCode.append( ['load', quad[1], quad[3], '0'] )

            # For local arrays - verify
            else: 
                asmCode.append( ['add', quad[3], '$fp', quad[3]] )
                asmCode.append( ['addi', quad[3], quad[3], '{}'.format(locAddr[curScope][quad[2]][0])] )
                asmCode.append( ['load', quad[1], quad[3], '0'] )

        elif quad[0] == 'GOTO':
            asmCode.append( ['j', quad[1], '-', '-'] )

        elif quad[0] == 'IFF':
            asmCode.append( ['beq', quad[1], '$zero', quad[2]] )

        elif quad[0] == 'RET':
            asmCode.append( ['mov', '$ret', quad[1], '-'] )

        elif quad[0] == 'FUNC':
            '''
            func:
                addi $sp, $sp, 2
                store $fp, -1($sp)
                mov $fp, $sp
                addi $fp, $fp, -1
                store $ra, 0($sp)
            '''
            paramCount = 0
            argCount = 0
            curScope = quad[1]
            asmCode.append( [quad[1].lower(), '-', '-', '-'] )
            if quad[1] != 'main':
                asmCode.append( ['addi', '$sp', '$sp', '2'] )
                asmCode.append( ['store', '$fp', '$sp', '-2'] )
                asmCode.append( ['mov', '$fp', '$sp', '-'] )
                asmCode.append( ['store', '$ra', '$sp', '-1'] )

        elif quad[0] == 'END':
            pass

        elif quad[0] == 'PARAM':
            '''
            addi $sp, $sp, 1
            store $ax, 0($sp)
            '''
            asmCode.append( ['addi', '$sp', '$sp', '1'] )
            asmCode.append( ['store', '$a{}'.format(paramCount), '$sp', '0'] )
            paramCount += 1

        elif quad[0] == 'CALL':
            argCount = 0
            if quad[2] == 'output':
                '''
                mov $tx, $a0
                out $tx
                '''
                # The parameter for output will always be unique and $a0
                asmCode.append( ['mov', quad[1], '$a0', '-'] )
                asmCode.append( ['out', quad[1], '-', '-'] )
            
            elif quad[2] == 'input':
                '''
                in $rx
                '''
                asmCode.append( ['in', quad[1], '-', '-'] )

            else:
                '''
                jal func_name
                mov $tx, $ret 
                '''
                asmCode.append( ['jal', quad[2], '-', '-'] )
                asmCode.append( ['mov', quad[1], '$ret', '-'] )

        elif quad[0] == 'ARG':
            asmCode.append( ['mov', '$a{}'.format(argCount), quad[1], '-'] )
            argCount += 1

        elif quad[0] == 'LAB':
            asmCode.append( [quad[1], '-', '-', '-'] )

        elif quad[0] == 'HALT':
            asmCode.append( ['halt', '-', '-', '-'] )

    return asmCode


def main():

    iCode = getIntermediateCode('../outputs/intermediateCode.txt')
    asmCode = generateAsmCode(iCode)
    
    for quad in iCode:
        print(quad)

    print()
    print()

    a = determineLocalAdddr(iCode)
    print(a)

    print()
    print()

    for asm in asmCode:
        print(asm)


if __name__ == '__main__':
    main()