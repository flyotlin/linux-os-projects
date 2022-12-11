.PHONY: all clean

CC=gcc
CFLAG_STATIC=-static

all: static

static:
	$(CC) -pthread -o thread.o thread.c $(CFLAG_STATIC)

dynamic:
	$(CC) -pthread -o thread.o thread.c

run:
	./thread.o

clean:
	$(RM) *.o
