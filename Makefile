OBJS = process.o

main: main.c $(OBJS)
	gcc -Wall main.c $(OBJS) -o main

process.o: process.c process.h
	gcc -Wall -c process.c
