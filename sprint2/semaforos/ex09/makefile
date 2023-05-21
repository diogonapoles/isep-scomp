# make VARIABLES
INCLUDES = 
OBJFILES = testpar.o
ASSFILE = 
PROGRAM = testpar
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
run: main
	./testpar 1 100 us
	./testpar 1 500 us
	./testpar 1 1 ms
	./testpar 1 10 ms
	./testpar 1 50 ms
	./testpar 1 500 ms
	./testpar 1 1 s
	./testpar 2 100 us
	./testpar 2 500 us
	./testpar 2 1 ms
	./testpar 2 10 ms
	./testpar 2 50 ms
	./testpar 2 500 ms
	./testpar 2 1 s 
	./testpar 4 100 us
	./testpar 4 500 us
	./testpar 4 1 ms
	./testpar 4 10 ms
	./testpar 4 50 ms
	./testpar 4 500 ms
	./testpar 4 1 s 
	./testpar 8 100 us
	./testpar 8 500 us
	./testpar 8 1 ms
	./testpar 8 10 ms
	./testpar 8 50 ms
	./testpar 8 500 ms
	./testpar 8 1 s 
	./testpar 16 100 us
	./testpar 16 500 us
	./testpar 16 1 ms
	./testpar 16 10 ms
	./testpar 16 50 ms
	./testpar 16 500 ms
	./testpar 16 1 s
clean:
	rm -f *.o main

	
