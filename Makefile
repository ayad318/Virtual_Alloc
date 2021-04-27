CC=gcc
CFLAGS=-fsanitize=address -Wall -Werror -std=gnu11 -g -lm -Wno-deprecated-declarations
TARGET=virtual_alloc

.PHONY: clean

tests: tests.c virtual_alloc.c
	$(CC) $(CFLAGS) $^ -o $@

virtual_alloc.o: virtual_alloc.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TARGET)
	rm -f *.o
	rm -f tests
	rm -f tests_file/*.out


run_tests:
	./tests
	@chmod +x test.sh
	./test.sh

git:
	git add .
	git commit -m "submit"
	git push