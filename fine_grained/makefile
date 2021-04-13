#Jonathon Schnell
#cpre308 lab1

DEPS = Queue.h Bank.h

makefile: appserver.o Bank.o Queue.o
	gcc -o appserver Bank.o appserver.o Queue.o -lpthread

clean:
	rm *.o
