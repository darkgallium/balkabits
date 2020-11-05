CC=gcc

ALL=sender listener

all: sender listener

sender: sender.c utils.o hamming.o

listener: listener.c utils.o

utils.o: utils.c utils.h

hamming.o: hamming.c hamming.h

clean:
	rm -rf *.o $(ALL)
