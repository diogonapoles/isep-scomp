# make VARIABLES
INCLUDES = 
OBJFILES = seller.o buyer.o
ASSFILE = 
PROGRAM = seller buyer
# suffix rules
.SUFFIXES : .c .o 
# .c to .o
.c.o:
	gcc -Wall -g -c $<
# .s to .o
.s.o:
	gcc -g -c $< 
main: ${OBJFILES} 
	gcc -Wall ${OBJFILES} -g -o ${PROGRAM} -lpthread
${OBJFILES}: ${SOURCES} ${INCLUDES}
build: main
clean:
	rm -f *.o seller buyer

	
