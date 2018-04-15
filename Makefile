CFLAGS := -std=c99 -g -Wall
CC := gcc
BIN := $(filter-out sh.c, $(wildcard *.c))
BINFILES := $(BIN:.c=.o)

all: sh

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

sh: $(BINFILES) sh.c
	$(CC) $(CFLAGS) $^ -o $@

run: all
	./sh

valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all ./sh

clean:
	rm -f sh *.o *.asm

.PHONY: clean run
