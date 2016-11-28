MAIN_OBJS = http.o ip.o process.o
GHOST_OBJS = process.o

all: main ghost

main: main.c $(MAIN_OBJS)
	gcc -Wall main.c $(MAIN_OBJS) -o main -lpthread -lpcap

ghost: ghost.c $(GHOST_OBJS)
	gcc -Wall ghost.c $(GHOST_OBJS) -o ghost -lpthread

process.o: process.c process.h
	gcc -Wall -c process.c

http.o: http.h http.c
	gcc -Wall -c http.c

ip.o: ip.h ip.c
	gcc -Wall -c ip.c
