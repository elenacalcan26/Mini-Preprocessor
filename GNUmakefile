CC = gcc
CFLAGS = -Wextra -Wall

all: so-cpp

so-cpp: main.o hashmap.o
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c
	$(CC) -o $@ -c $^

hashmap.o: hashmap.c 
	$(CC) -o $@ -c $^

clean:
	rm so-cpp *.o