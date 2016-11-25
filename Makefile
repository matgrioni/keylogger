OBJS = http.o ip.o

main: main.c $(OBJS)
	gcc main.c ip.o http.o -lpcap -o main

http.o: http.h http.c
	gcc -Wall -c http.c

ip.o: ip.h ip.c
	gcc -Wall -c ip.c

