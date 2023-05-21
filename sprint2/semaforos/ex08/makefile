insert: insert.o
	gcc -g -Wall insert.o -o insert -lrt -lpthread

insert.o: insert.c
	gcc -g -Wall -c insert.c -o insert.o -lrt -lpthread

consult: consult.o
	gcc -g -Wall consult.o -o consult -lrt -lpthread

consult.o: consult.c
	gcc -g -Wall -c consult.c -o consult.o -lrt -lpthread

consult_all: consult_all.o
	gcc -g -Wall consult_all.o -o consult_all -lrt -lpthread

consult_all.o: consult_all.c
	gcc -g -Wall -c consult_all.c -o consult_all.o -lrt -lpthread

clean:
	rm -f *.o 
	rm -f consult_all
	rm -f insert
	rm -f consult

insertdata:
	make insert
	make consult
	make consult_all

run: insert consult consult_all
	./insert
	./consult
	./consult_all