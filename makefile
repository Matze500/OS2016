CC = gcc
CFLAGS = -Wall -Wextra -Wconversion
DEPS = main.h list.c list.h keys.c keys.h

all: osmprun OSMP  osmpexecutable

osmprun: main.o list.o keys.o
	$(CC) -o osmprun main.o list.o keys.o

osmpexecutable: writer.o OSMP.o keys.o
	$(CC) -o osmpexecutable writer.o OSMP.o keys.o

OSMP: OSMP.o
	ar rs OSMP.a OSMP.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -m64 -g -c -o $@ $<
