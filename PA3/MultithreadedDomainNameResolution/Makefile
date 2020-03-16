CC = gcc

CFLAGS = -g -Wall -Wextra
LIBS = -lpthread

all: multi-lookup

multi-lookup: multi-lookup.o queue.o util.o
	$(CC) -o multi-lookup multi-lookup.c queue.c util.c $(CFLAGS) $(LIBS)

clean:
	rm -f multi-lookup
