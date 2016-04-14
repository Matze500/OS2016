all: program writer

program: main.o
		gcc -o program main.o

main.o: main.c
		gcc -c main.c

writer: writer.o
		gcc -o writer writer.o

writer.o: writer.c
		gcc -c writer.c
