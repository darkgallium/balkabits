CC=gcc

ALL=sender listener

all: sender listener

sender: sender.c utils.o
	$(CC) -o $@ $^


listener: listener.c utils.o
	$(CC) -o $@ $^

utils.o: utils.c
	$(CC) -o $@ -c $^

clean:
	rm -rf *.o $(ALL)