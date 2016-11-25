MAIN_OBJS = process.o
GHOST_OBJS = process.o

all: main ghost

main: main.c $(MAIN_OBJS)
	gcc -Wall main.c $(MAIN_OBJS) -o main

ghost: ghost.c $(GHOST_OBJS)
	gcc -Wall ghost.c $(GHOST_OBJS) -o ghost

process.o: process.c process.h
	gcc -Wall -c process.c
