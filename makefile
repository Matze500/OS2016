CC = gcc
CFLAGS = -Wall -Wextra -Wconversion -std=c11
DEPS = main.h list.c list.h keys.c keys.h structs.h

all: osmprun OSMP  osmpexecutable

osmprun: main.o list.o keys.o
	$(CC) -o osmprun main.o list.o keys.o

osmpexecutable: writer.o OSMP.o
	$(CC) -o osmpexecutable writer.o OSMP.o

OSMP: OSMP.o keys.o
	ar rs OSMP.a OSMP.o keys.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -m64 -g -c -o $@ $<
