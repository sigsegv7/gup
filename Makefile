CFILES = $(shell find . -name "*.c")
OFILES = $(CFILES:.c=.o)
DFILES = $(CFILES:.c=.d)

CC = gcc
CFLAGS = -Wall -pedantic -MMD -Iinc/

.PHONY: all
all: $(OFILES)
	$(CC) $^ -o gup

%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@
