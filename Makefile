CFLAGS := -std=c99 -g -Wall
CC := gcc
EXEC = sh
BIN = $(filter-out $(EXEC).c, $(wildcard *.c))
BINFILES = $(BIN:.c=.o)
OBJDIR = build

$(OBJDIR)/%.o: %.c %.h
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

OBJEXEC = $(addprefix $(OBJDIR)/, $(EXEC))

all: $(OBJEXEC)

$(OBJEXEC): $(addprefix $(OBJDIR)/, $(BINFILES)) $(EXEC).c
	$(CC) $(CFLAGS) $^ -o $@

run: all
	./$(OBJDIR)/$(EXEC)

valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all ./$(OBJDIR)/$(EXEC)

code:
	./code.sh

clean:
	rm -rf $(OBJDIR)/ *.o *.asm

.PHONY: clean run
