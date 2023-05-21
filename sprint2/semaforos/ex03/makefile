writer: writer.o
	gcc -g -Wall writer.o -o writer 

writer.o: writer.c
	gcc -g -Wall -c writer.c -o writer.o

reader: reader.o
	gcc -g -Wall reader.o -o reader 

reader.o: reader.c
	gcc -g -Wall -c reader.c -o reader.o

clean:
	rm -f *.o 
	rm -f reader
	rm -f writer

run: writer reader
	./writer
	./reader