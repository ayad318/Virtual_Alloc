CC=gcc
CFLAGS=-fsanitize=address -Wall -Werror -std=gnu11 -g -lm
TARGET=virtual_alloc

.PHONY: clean

tests: tests.c virtual_alloc.c
	$(CC) $(CFLAGS) $^ -o $@

virtual_alloc.o: virtual_alloc.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TARGET)
	rm -f *.o

run_tests:
	$(CC) $(CC) $(CFLAGS) $^ -o $@