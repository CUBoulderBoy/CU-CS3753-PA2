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
	rm -f output.txt

clean-output:
	rm -f output.txt

run-multi-lookup: multi-lookup
		rm -f output.txt
		./multi-lookup input/names*.txt output.txt

test-multi-lookup: multi-lookup
		rm -f output.txt
		valgrind ./multi-lookup input/names*.txt output.txt
