#Jonathon Schnell
#cpre308 lab1

DEPS = Queue.h Bank.h

makefile: appserver_coarse.o Bank.o Queue.o
	gcc -o appserver_coarse Bank.o appserver_coarse.o Queue.o -lpthread

clean:
	rm *.o
