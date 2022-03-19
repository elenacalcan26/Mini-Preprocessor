CC = gcc
CFLAGS = -Wextra -Wall

build: main.o hashmap.o
	$(CC) -o so-cpp $^

main.o: main.c
	$(CC) -o $@ -c $^

hashmap.o: hashmap.c
	$(CC) -o $@ -c $^

run:
	./so-cpp

.PHONY: clean
clean:
	rm so-cpp *.o