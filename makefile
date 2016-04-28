CC = gcc
CFLAGS = -Wall -Wextra -Wconversion
DEPS = main.h list.c list.h

all: program OSMP  writer

program: main.o list.o
	$(CC) -o program main.o list.o

writer: writer.o OSMP.o
	$(CC) -o writer writer.o OSMP.o

OSMP: OSMP.o
	ar rs OSMP.a OSMP.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -m64 -g -c -o $@ $<
