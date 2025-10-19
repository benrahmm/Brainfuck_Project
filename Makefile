CC = gcc
CFLAGS = -Wall -Wextra -std=c99

SRCDIR = ~/brainfuck_src_etu

SOURCES = $(SRCDIR)/brainfuck_main.c $(SRCDIR)/brainfuck_helper.c
OBJECTS = $(SOURCES:%.c=%.o)

EXECUTABLE = brainfuck

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

