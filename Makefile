CC = gcc
CFLAGS = -c -g -Wall -Wextra
LFLAGS = -Wall -Wextra -pthread

.PHONY: all clean

all: multi-lookup

multi-lookup: multi-lookup.o queue.o util.o
	$(CC) $(LFLAGS) $^ -o $@

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) $<

util.o: util.c util.h
	$(CC) $(CFLAGS) $<

clean:
	rm -f multi-lookup lookup queueTest pthread-hello
	rm -f *.o
	rm -f *~
	rm -f results.txt

run-multi: multi-lookup
		./multi-lookup input/names*.txt results-ref.txt

test-multi: multi-lookup
		valgrind ./multi-lookup input/names*.txt results-ref.txt
