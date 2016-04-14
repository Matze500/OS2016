all: program writer

program: main.o
	gcc -o program main.o

writer: writer.o
	gcc -o writer writer.o

%.o: %.c
	gcc -Wall -Wextra -Wconversion -m64 -g -c $<
