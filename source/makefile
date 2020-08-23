# MakeFile for the ZAFx32 CMinus Compiler
# By: Luiz Felipe Raveduti Zafiro

# If the lex.yy.o, lex.yy.c, scanner.tab.o, scanner.tab.c, scanner.tab.h were removed, run in terminal:
# 		flex scanner.l
#		bison -d -v scanner.y
#		gcc -c lex.yy.c scanner.tab.c
#
# This because those files are created in flex and bison execution

CC = gcc

FLEX = flex

BISON = bison -d -v

OBJS = main.o util.o symtab.o analyze.o globals.o lex.yy.o scanner.tab.o intermediateCode.o #Add o codigo intermediario dps 

CFLAGS = 

all: cmin end

cmin: $(OBJS)
	@echo '- Linking all object files'
	$(CC) $(CFLAGS) -o cmin $(OBJS)
	@echo '- All objects linked -> cmin(executable)'
	@echo ' '

main.o: main.c globals.h util.h parse.h scan.h symtab.h analyze.h
	$(CC) $(CFLAGS) -c main.c

# When bison runs, it generates scanner.tab.h in execution time, so it changes globals.h.
# Thats why we do a touch, to indicate the change in globals.h. Than we call make again.
# So everything that depends on globals.h (that was changed by scanner.tab.h) is recompiled.
scanner.tab.o: scanner.y
	@echo '- Rebuilding and recompiling the parser'
	$(BISON) scanner.y
	$(CC) -c scanner.tab.c
	@echo '- Done with the parser'
	@echo ' '
	touch globals.h 
	make
	@echo ' '

# Changes on the lexer, results in changes on the parser (.y). Because lex.yy.c is created in execution time
# We do a touch in .y and recompile it (recompiling as well all the dependencies of 
# globals.h that depends on the .y (scanner.tab.h))
lex.yy.o: scanner.l
	@echo '- Rebuilding and recompiling the lexer'
	$(FLEX) scanner.l
	$(CC) -c lex.yy.c
	@echo '- Done with the lexer'
	@echo ' '
	touch scanner.y
	make
	@echo ' '

globals.o: globals.c globals.h
	$(CC) $(CFLAGS) -c globals.c

util.o: util.c util.h globals.h
	$(CC) $(CFLAGS) -c util.c

symtab.o: symtab.c symtab.h
	$(CC) $(CFLAGS) -c symtab.c

analyze.o: analyze.c analyze.h globals.h symtab.h
	$(CC) $(CFLAGS) -c analyze.c

intermediateCode.o: intermediateCode.c intermediateCode.h globals.h
	$(CC) $(CFLAGS) -c intermediateCode.c

end: 
	@echo '- Moving cmin to the root directory'
	mv cmin ../
	@echo '- Finished moving cmin to the root directory'
	@echo ' '

clean:
	@echo 'Removing all .o files (except lex.yy.o and scanner.tab.o)'
	rm main.o util.o symtab.o analyze.o globals.o
	@echo 'Finished removing .o files'