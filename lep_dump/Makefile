LDFLAG	= -lm
FFLAGS  = -g
CFLAGS  = -g
CSRC	= 
PROGRAM = lep_dump 
OBJ	= 
CC      = gcc

##########################################################
  all: 		$(PROGRAM)
  install:	
	mv $(PROGRAM) ../bin/
##########################################################

lep_dump:lep_dump.o
	gcc -g  -o lep_dump lep_dump.c $(LDFLAG) 

##########################################################
clean:
	rm -f *.o ${PROGRAM}
##########################################################



