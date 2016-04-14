all: program writer OSMP

program: main.o
	gcc -o program main.o

writer: writer.o
	gcc -o writer writer.o
OSMP: OSMP.o
	ar rs OSMP.a OSMP.o

	

%.o: %.c
	gcc -Wall -Wextra -Wconversion -m64 -g -c $<
